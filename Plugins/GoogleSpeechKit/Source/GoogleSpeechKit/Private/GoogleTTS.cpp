// Copyright 2019 Ilgar Lunin. All Rights Reserved.

#include "GoogleTTS.h"
#include "Http.h"
#include "GoogleSpeechUtils.h"
#include "GoogleSpeechFunctionLibrary.h"
#include "GenericPlatform/GenericPlatformHttp.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/Base64.h"
#include "GoogleSpeechKit.h"


DEFINE_LOG_CATEGORY(LogGoogleTTS)


UGoogleTTS* UGoogleTTS::GoogleTTS(FGoogleSpeechSynthesisParams params)
{
    UGoogleTTS* BPNode = NewObject<UGoogleTTS>();

    // according to the api https://cloud.google.com/text-to-speech/docs/reference/rest/v1beta1/text/synthesize#AudioConfig
    params.Pitch = FMath::Clamp<float>(params.Pitch, -20.f, 20.f);
    params.SpeakingRate = FMath::Clamp<float>(params.SpeakingRate, 0.25f, 4.f);

    BPNode->params = params;

    return BPNode;
}

UGoogleTTS* UGoogleTTS::CallTTS(const FString& speech, FString language) {
	
	FGoogleSpeechSynthesisParams newParams;
	newParams.Text = FText::FromString(speech);
	newParams.Language = language;
	
	return GoogleTTS(newParams);
	
}

void UGoogleTTS::Activate()
{
    if (params.Text.IsEmpty())
    {
        Finished.Broadcast(nullptr, TArray<uint8>(), false);
        return;
    }

    // check cache
    FGoogleSpeechKitModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleSpeechKitModule>("GoogleSpeechKit");

    bool cacheFound = false;
    TArray<uint8> rawData = mod.findTTSCache(params, cacheFound);
    if (cacheFound)
    {
        USoundWave* cachedSound = GoogleSpeechKitUtils::CreateSoundFromWaveDataWithHeader(rawData);
        Finished.Broadcast(cachedSound, rawData, true);
        UE_LOG(LogTemp, Warning, TEXT("Cache was found"));
        return;
    }

    FString _apiKey;
	if (UGoogleSpeechFunctionLibrary::GetUseApiKeyFromEnvironmentVars()) {
		//_apiKey = GoogleSpeechKitUtils::GetEnvironmentVariable(TEXT("GOOGLE_API_KEY"));
		UE_LOG(LogGoogleTTS, Error, TEXT("Using Environment Variable"));
	}
    else
        _apiKey = UGoogleSpeechFunctionLibrary::GetApiKey();

    if (_apiKey.IsEmpty())
    {
        UE_LOG(LogGoogleTTS, Error, TEXT("Api key is not set"));
        Finished.Broadcast(nullptr, TArray<uint8>(), false);
        return;
    }

    auto HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(TEXT("https://texttospeech.googleapis.com/v1/text:synthesize"));
    HttpRequest->SetHeader(TEXT("X-Goog-Api-Key"), _apiKey);
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json; charset=utf-8"));

    TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());
    TSharedPtr<FJsonObject> _input = MakeShareable(new FJsonObject());
    if (params.bUseSsml)
        _input->SetStringField("ssml", params.Text.ToString());
    else
        _input->SetStringField("text", params.Text.ToString());

    TSharedPtr<FJsonObject> _voice = MakeShareable(new FJsonObject());
    FString _voiceName = params.Language;
    FString _voiceCode = _voiceName.Mid(0, 5);
    _voice->SetStringField("languageCode", _voiceCode);
    _voice->SetStringField("name", _voiceName);

    TSharedPtr<FJsonObject> _audioConfig = MakeShareable(new FJsonObject());
    _audioConfig->SetStringField("audioEncoding", "LINEAR16");
    _audioConfig->SetNumberField("pitch", params.Pitch);
    _audioConfig->SetNumberField("speakingRate", params.SpeakingRate);
    _audioConfig->SetNumberField("sampleRateHertz", params.SampleRateHertz);

    TArray<TSharedPtr<FJsonValue>> audioEffects;
    for (EAudioEffectsProfile effect : params.AudioEffectsStack)
    {
        FString profileString = _audioProfilesValues[effect];
        audioEffects.Add(MakeShareable(new FJsonValueString(profileString)));
    }
    _audioConfig->SetArrayField("effectsProfileId", audioEffects);

    _payloadJson->SetObjectField("input", _input);
    _payloadJson->SetObjectField("voice", _voice);
    _payloadJson->SetObjectField("audioConfig", _audioConfig);

    FString _payload;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
    FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);

    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetContentAsString(_payload);

    if (HttpRequest->ProcessRequest())
    {
        HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGoogleTTS::OnResponse);
    }
    else
    {
        UE_LOG(LogGoogleTTS, Warning, TEXT("Error sending request"));
        Finished.Broadcast(nullptr, TArray<uint8>(), false);
    }
}


void UGoogleTTS::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
    if (!WasSuccessful)
    {
        UE_LOG(LogGoogleTTS, Warning, TEXT("Error processing request.\n%s\n%s"), *Response->GetContentAsString(), *Response->GetURL());
        if (Finished.IsBound())
        {
            Finished.Broadcast(nullptr, TArray<uint8>(), false);
        }

        return;
    }

    // check error message. If not enough rights, generate new IAM token
    // or return false
    TSharedPtr<FJsonObject> response;
    TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
    if (FJsonSerializer::Deserialize(reader, response))
    {
        bool err = response->HasField("error");

        if (err)
        {
            UE_LOG(LogGoogleTTS, Warning, TEXT("api error: %s"), *Response->GetContentAsString());
            return;
        }

        FString audioContentb64 = response->GetStringField("audioContent");
        TArray<uint8> rawData;

        // according to docs (https://cloud.google.com/text-to-speech/docs/reference/rest/v1beta1/text/synthesize#response-body)
        // wave already contains header info and it is encoded to base64
        FBase64::Decode(audioContentb64, rawData);
        sWave = GoogleSpeechKitUtils::CreateSoundFromWaveDataWithHeader(rawData);
        if (sWave != nullptr)
        {
            if (Finished.IsBound())
            {
                FGoogleSpeechKitModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleSpeechKitModule>("GoogleSpeechKit");
                if (mod.isTTSCachingEnabled())
                {
                    FString relativeFilePath = FString::Printf(TEXT("%s.wav"), *FGuid().NewGuid().ToString());
                    if (mod.cacheTTS(relativeFilePath, params, rawData))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Cache been saved to %s"), *relativeFilePath);
                    }
                }
                Finished.Broadcast(sWave, rawData, WasSuccessful);
            }
        }
    }
}


UGoogleTTS::UGoogleTTS()
{
    _languageValues.Add(TEXT("ro-RO-Wavenet-A"));
    _languageValues.Add(TEXT("ar-XA-Wavenet-A"));
    _languageValues.Add(TEXT("ar-XA-Wavenet-B"));
    _languageValues.Add(TEXT("ar-XA-Wavenet-C"));
    _languageValues.Add(TEXT("ar-XA-Wavenet-D"));
    _languageValues.Add(TEXT("bn-IN-Wavenet-A"));
    _languageValues.Add(TEXT("bn-IN-Wavenet-B"));
    _languageValues.Add(TEXT("en-GB-Wavenet-A"));
    _languageValues.Add(TEXT("en-GB-Wavenet-B"));
    _languageValues.Add(TEXT("en-GB-Wavenet-C"));
    _languageValues.Add(TEXT("en-GB-Wavenet-D"));
    _languageValues.Add(TEXT("en-GB-Wavenet-F"));
    _languageValues.Add(TEXT("fr-CA-Wavenet-A"));
    _languageValues.Add(TEXT("fr-CA-Wavenet-B"));
    _languageValues.Add(TEXT("fr-CA-Wavenet-C"));
    _languageValues.Add(TEXT("fr-CA-Wavenet-D"));
    _languageValues.Add(TEXT("en-US-Wavenet-G"));
    _languageValues.Add(TEXT("en-US-Wavenet-H"));
    _languageValues.Add(TEXT("en-US-Wavenet-I"));
    _languageValues.Add(TEXT("en-US-Wavenet-J"));
    _languageValues.Add(TEXT("en-US-Wavenet-A"));
    _languageValues.Add(TEXT("en-US-Wavenet-B"));
    _languageValues.Add(TEXT("en-US-Wavenet-C"));
    _languageValues.Add(TEXT("en-US-Wavenet-D"));
    _languageValues.Add(TEXT("en-US-Wavenet-E"));
    _languageValues.Add(TEXT("en-US-Wavenet-F"));
    _languageValues.Add(TEXT("es-ES-Wavenet-C"));
    _languageValues.Add(TEXT("es-ES-Wavenet-D"));
    _languageValues.Add(TEXT("es-ES-Wavenet-B"));
    _languageValues.Add(TEXT("fi-FI-Wavenet-A"));
    _languageValues.Add(TEXT("gu-IN-Wavenet-A"));
    _languageValues.Add(TEXT("gu-IN-Wavenet-B"));
    _languageValues.Add(TEXT("ja-JP-Wavenet-B"));
    _languageValues.Add(TEXT("ja-JP-Wavenet-C"));
    _languageValues.Add(TEXT("ja-JP-Wavenet-D"));
    _languageValues.Add(TEXT("ja-JP-Wavenet-A"));
    _languageValues.Add(TEXT("kn-IN-Wavenet-A"));
    _languageValues.Add(TEXT("kn-IN-Wavenet-B"));
    _languageValues.Add(TEXT("ml-IN-Wavenet-A"));
    _languageValues.Add(TEXT("ml-IN-Wavenet-B"));
    _languageValues.Add(TEXT("sv-SE-Wavenet-A"));
    _languageValues.Add(TEXT("ta-IN-Wavenet-A"));
    _languageValues.Add(TEXT("ta-IN-Wavenet-B"));
    _languageValues.Add(TEXT("tr-TR-Wavenet-B"));
    _languageValues.Add(TEXT("tr-TR-Wavenet-C"));
    _languageValues.Add(TEXT("tr-TR-Wavenet-D"));
    _languageValues.Add(TEXT("tr-TR-Wavenet-E"));
    _languageValues.Add(TEXT("tr-TR-Wavenet-A"));
    _languageValues.Add(TEXT("ms-MY-Wavenet-A"));
    _languageValues.Add(TEXT("ms-MY-Wavenet-B"));
    _languageValues.Add(TEXT("ms-MY-Wavenet-C"));
    _languageValues.Add(TEXT("ms-MY-Wavenet-D"));
    _languageValues.Add(TEXT("pa-IN-Wavenet-A"));
    _languageValues.Add(TEXT("pa-IN-Wavenet-B"));
    _languageValues.Add(TEXT("pa-IN-Wavenet-C"));
    _languageValues.Add(TEXT("pa-IN-Wavenet-D"));
    _languageValues.Add(TEXT("cs-CZ-Wavenet-A"));
    _languageValues.Add(TEXT("de-DE-Wavenet-F"));
    _languageValues.Add(TEXT("de-DE-Wavenet-A"));
    _languageValues.Add(TEXT("de-DE-Wavenet-B"));
    _languageValues.Add(TEXT("de-DE-Wavenet-C"));
    _languageValues.Add(TEXT("de-DE-Wavenet-D"));
    _languageValues.Add(TEXT("de-DE-Wavenet-E"));
    _languageValues.Add(TEXT("en-AU-Wavenet-A"));
    _languageValues.Add(TEXT("en-AU-Wavenet-B"));
    _languageValues.Add(TEXT("en-AU-Wavenet-C"));
    _languageValues.Add(TEXT("en-AU-Wavenet-D"));
    _languageValues.Add(TEXT("en-IN-Wavenet-D"));
    _languageValues.Add(TEXT("en-IN-Wavenet-A"));
    _languageValues.Add(TEXT("en-IN-Wavenet-B"));
    _languageValues.Add(TEXT("en-IN-Wavenet-C"));
    _languageValues.Add(TEXT("es-US-Wavenet-A"));
    _languageValues.Add(TEXT("es-US-Wavenet-B"));
    _languageValues.Add(TEXT("es-US-Wavenet-C"));
    _languageValues.Add(TEXT("fr-FR-Wavenet-E"));
    _languageValues.Add(TEXT("fr-FR-Wavenet-A"));
    _languageValues.Add(TEXT("fr-FR-Wavenet-B"));
    _languageValues.Add(TEXT("fr-FR-Wavenet-C"));
    _languageValues.Add(TEXT("fr-FR-Wavenet-D"));
    _languageValues.Add(TEXT("hi-IN-Wavenet-D"));
    _languageValues.Add(TEXT("hi-IN-Wavenet-A"));
    _languageValues.Add(TEXT("hi-IN-Wavenet-B"));
    _languageValues.Add(TEXT("hi-IN-Wavenet-C"));
    _languageValues.Add(TEXT("id-ID-Wavenet-D"));
    _languageValues.Add(TEXT("id-ID-Wavenet-A"));
    _languageValues.Add(TEXT("id-ID-Wavenet-B"));
    _languageValues.Add(TEXT("id-ID-Wavenet-C"));
    _languageValues.Add(TEXT("it-IT-Wavenet-A"));
    _languageValues.Add(TEXT("it-IT-Wavenet-B"));
    _languageValues.Add(TEXT("it-IT-Wavenet-C"));
    _languageValues.Add(TEXT("it-IT-Wavenet-D"));
    _languageValues.Add(TEXT("ko-KR-Wavenet-A"));
    _languageValues.Add(TEXT("ko-KR-Wavenet-B"));
    _languageValues.Add(TEXT("ko-KR-Wavenet-C"));
    _languageValues.Add(TEXT("ko-KR-Wavenet-D"));
    _languageValues.Add(TEXT("ru-RU-Wavenet-E"));
    _languageValues.Add(TEXT("ru-RU-Wavenet-A"));
    _languageValues.Add(TEXT("ru-RU-Wavenet-B"));
    _languageValues.Add(TEXT("ru-RU-Wavenet-C"));
    _languageValues.Add(TEXT("ru-RU-Wavenet-D"));
    _languageValues.Add(TEXT("uk-UA-Wavenet-A"));
    _languageValues.Add(TEXT("cmn-CN-Wavenet-A"));
    _languageValues.Add(TEXT("cmn-CN-Wavenet-B"));
    _languageValues.Add(TEXT("cmn-CN-Wavenet-C"));
    _languageValues.Add(TEXT("cmn-CN-Wavenet-D"));
    _languageValues.Add(TEXT("cmn-TW-Wavenet-A"));
    _languageValues.Add(TEXT("cmn-TW-Wavenet-B"));
    _languageValues.Add(TEXT("cmn-TW-Wavenet-C"));
    _languageValues.Add(TEXT("da-DK-Wavenet-C"));
    _languageValues.Add(TEXT("da-DK-Wavenet-D"));
    _languageValues.Add(TEXT("da-DK-Wavenet-E"));
    _languageValues.Add(TEXT("da-DK-Wavenet-A"));
    _languageValues.Add(TEXT("el-GR-Wavenet-A"));
    _languageValues.Add(TEXT("fil-PH-Wavenet-A"));
    _languageValues.Add(TEXT("fil-PH-Wavenet-B"));
    _languageValues.Add(TEXT("fil-PH-Wavenet-C"));
    _languageValues.Add(TEXT("fil-PH-Wavenet-D"));
    _languageValues.Add(TEXT("hu-HU-Wavenet-A"));
    _languageValues.Add(TEXT("nb-NO-Wavenet-A"));
    _languageValues.Add(TEXT("nb-NO-Wavenet-B"));
    _languageValues.Add(TEXT("nb-NO-Wavenet-C"));
    _languageValues.Add(TEXT("nb-NO-Wavenet-D"));
    _languageValues.Add(TEXT("nb-no-Wavenet-E"));
    _languageValues.Add(TEXT("nl-BE-Wavenet-A"));
    _languageValues.Add(TEXT("nl-BE-Wavenet-B"));
    _languageValues.Add(TEXT("nl-NL-Wavenet-B"));
    _languageValues.Add(TEXT("nl-NL-Wavenet-C"));
    _languageValues.Add(TEXT("nl-NL-Wavenet-D"));
    _languageValues.Add(TEXT("nl-NL-Wavenet-A"));
    _languageValues.Add(TEXT("nl-NL-Wavenet-E"));
    _languageValues.Add(TEXT("pt-PT-Wavenet-A"));
    _languageValues.Add(TEXT("pt-PT-Wavenet-B"));
    _languageValues.Add(TEXT("pt-PT-Wavenet-C"));
    _languageValues.Add(TEXT("pt-PT-Wavenet-D"));
    _languageValues.Add(TEXT("sk-SK-Wavenet-A"));
    _languageValues.Add(TEXT("vi-VN-Wavenet-A"));
    _languageValues.Add(TEXT("vi-VN-Wavenet-B"));
    _languageValues.Add(TEXT("vi-VN-Wavenet-C"));
    _languageValues.Add(TEXT("vi-VN-Wavenet-D"));
    _languageValues.Add(TEXT("pl-PL-Wavenet-A"));
    _languageValues.Add(TEXT("pl-PL-Wavenet-B"));
    _languageValues.Add(TEXT("pl-PL-Wavenet-C"));
    _languageValues.Add(TEXT("pl-PL-Wavenet-E"));
    _languageValues.Add(TEXT("pl-PL-Wavenet-D"));
    _languageValues.Add(TEXT("pt-BR-Wavenet-A"));
    _languageValues.Add(TEXT("cs-CZ-Standard-A"));
    _languageValues.Add(TEXT("el-GR-Standard-A"));
    _languageValues.Add(TEXT("pt-BR-Standard-A"));
    _languageValues.Add(TEXT("es-US-Standard-A"));
    _languageValues.Add(TEXT("es-US-Standard-B"));
    _languageValues.Add(TEXT("es-US-Standard-C"));
    _languageValues.Add(TEXT("ar-XA-Standard-A"));
    _languageValues.Add(TEXT("ar-XA-Standard-B"));
    _languageValues.Add(TEXT("ar-XA-Standard-C"));
    _languageValues.Add(TEXT("ar-XA-Standard-D"));
    _languageValues.Add(TEXT("fr-FR-Standard-E"));
    _languageValues.Add(TEXT("it-IT-Standard-A"));
    _languageValues.Add(TEXT("ru-RU-Standard-E"));
    _languageValues.Add(TEXT("ru-RU-Standard-A"));
    _languageValues.Add(TEXT("ru-RU-Standard-B"));
    _languageValues.Add(TEXT("ru-RU-Standard-C"));
    _languageValues.Add(TEXT("ru-RU-Standard-D"));
    _languageValues.Add(TEXT("da-DK-Standard-A"));
    _languageValues.Add(TEXT("da-DK-Standard-C"));
    _languageValues.Add(TEXT("da-DK-Standard-D"));
    _languageValues.Add(TEXT("da-DK-Standard-E"));
    _languageValues.Add(TEXT("fi-FI-Standard-A"));
    _languageValues.Add(TEXT("is-is-Standard-A"));
    _languageValues.Add(TEXT("sv-SE-Standard-A"));
    _languageValues.Add(TEXT("nb-NO-Standard-A"));
    _languageValues.Add(TEXT("nb-NO-Standard-B"));
    _languageValues.Add(TEXT("nb-NO-Standard-C"));
    _languageValues.Add(TEXT("nb-NO-Standard-D"));
    _languageValues.Add(TEXT("nb-no-Standard-E"));
    _languageValues.Add(TEXT("nb-no-Standard-E"));
    _languageValues.Add(TEXT("pt-PT-Standard-A"));
    _languageValues.Add(TEXT("pt-PT-Standard-B"));
    _languageValues.Add(TEXT("pt-PT-Standard-C"));
    _languageValues.Add(TEXT("pt-PT-Standard-D"));
    _languageValues.Add(TEXT("fr-FR-Standard-A"));
    _languageValues.Add(TEXT("fr-FR-Standard-B"));
    _languageValues.Add(TEXT("fr-FR-Standard-C"));
    _languageValues.Add(TEXT("fr-FR-Standard-D"));
    _languageValues.Add(TEXT("de-DE-Standard-E"));
    _languageValues.Add(TEXT("de-DE-Standard-A"));
    _languageValues.Add(TEXT("de-DE-Standard-B"));
    _languageValues.Add(TEXT("de-DE-Standard-C"));
    _languageValues.Add(TEXT("de-DE-Standard-D"));
    _languageValues.Add(TEXT("de-DE-Standard-F"));
    _languageValues.Add(TEXT("fr-CA-Standard-A"));
    _languageValues.Add(TEXT("fr-CA-Standard-B"));
    _languageValues.Add(TEXT("fr-CA-Standard-C"));
    _languageValues.Add(TEXT("fr-CA-Standard-D"));
    _languageValues.Add(TEXT("it-IT-Standard-B"));
    _languageValues.Add(TEXT("it-IT-Standard-C"));
    _languageValues.Add(TEXT("it-IT-Standard-D"));
    _languageValues.Add(TEXT("en-AU-Standard-A"));
    _languageValues.Add(TEXT("en-AU-Standard-B"));
    _languageValues.Add(TEXT("en-AU-Standard-C"));
    _languageValues.Add(TEXT("en-AU-Standard-D"));
    _languageValues.Add(TEXT("en-GB-Standard-A"));
    _languageValues.Add(TEXT("en-GB-Standard-B"));
    _languageValues.Add(TEXT("en-GB-Standard-C"));
    _languageValues.Add(TEXT("en-GB-Standard-D"));
    _languageValues.Add(TEXT("en-GB-Standard-F"));
    _languageValues.Add(TEXT("cmn-CN-Standard-C"));
    _languageValues.Add(TEXT("cmn-CN-Standard-B"));
    _languageValues.Add(TEXT("cmn-CN-Standard-A"));
    _languageValues.Add(TEXT("cmn-CN-Standard-D"));
    _languageValues.Add(TEXT("ja-JP-Standard-A"));
    _languageValues.Add(TEXT("ja-JP-Standard-B"));
    _languageValues.Add(TEXT("ja-JP-Standard-C"));
    _languageValues.Add(TEXT("ja-JP-Standard-D"));
    _languageValues.Add(TEXT("cmn-TW-Standard-A"));
    _languageValues.Add(TEXT("cmn-TW-Standard-B"));
    _languageValues.Add(TEXT("cmn-TW-Standard-C"));
    _languageValues.Add(TEXT("ko-KR-Standard-B"));
    _languageValues.Add(TEXT("ko-KR-Standard-C"));
    _languageValues.Add(TEXT("ko-KR-Standard-D"));
    _languageValues.Add(TEXT("ko-KR-Standard-A"));
    _languageValues.Add(TEXT("vi-VN-Standard-A"));
    _languageValues.Add(TEXT("vi-VN-Standard-B"));
    _languageValues.Add(TEXT("vi-VN-Standard-C"));
    _languageValues.Add(TEXT("vi-VN-Standard-D"));
    _languageValues.Add(TEXT("en-US-Standard-A"));
    _languageValues.Add(TEXT("en-US-Standard-B"));
    _languageValues.Add(TEXT("en-US-Standard-C"));
    _languageValues.Add(TEXT("en-US-Standard-D"));
    _languageValues.Add(TEXT("en-US-Standard-E"));
    _languageValues.Add(TEXT("en-US-Standard-F"));
    _languageValues.Add(TEXT("en-US-Standard-G"));
    _languageValues.Add(TEXT("en-US-Standard-H"));
    _languageValues.Add(TEXT("en-US-Standard-I"));
    _languageValues.Add(TEXT("en-US-Standard-J"));
    _languageValues.Add(TEXT("th-TH-Standard-A"));
    _languageValues.Add(TEXT("bn-IN-Standard-A"));
    _languageValues.Add(TEXT("bn-IN-Standard-B"));
    _languageValues.Add(TEXT("en-IN-Standard-D"));
    _languageValues.Add(TEXT("en-IN-Standard-A"));
    _languageValues.Add(TEXT("en-IN-Standard-B"));
    _languageValues.Add(TEXT("en-IN-Standard-C"));
    _languageValues.Add(TEXT("gu-IN-Standard-A"));
    _languageValues.Add(TEXT("gu-IN-Standard-B"));
    _languageValues.Add(TEXT("hi-IN-Standard-D"));
    _languageValues.Add(TEXT("hi-IN-Standard-A"));
    _languageValues.Add(TEXT("hi-IN-Standard-B"));
    _languageValues.Add(TEXT("hi-IN-Standard-C"));
    _languageValues.Add(TEXT("kn-IN-Standard-A"));
    _languageValues.Add(TEXT("kn-IN-Standard-B"));
    _languageValues.Add(TEXT("ml-IN-Standard-A"));
    _languageValues.Add(TEXT("ml-IN-Standard-B"));
    _languageValues.Add(TEXT("ta-IN-Standard-A"));
    _languageValues.Add(TEXT("ta-IN-Standard-B"));
    _languageValues.Add(TEXT("te-IN-Standard-A"));
    _languageValues.Add(TEXT("te-IN-Standard-B"));
    _languageValues.Add(TEXT("pa-IN-Standard-A"));
    _languageValues.Add(TEXT("pa-IN-Standard-B"));
    _languageValues.Add(TEXT("pa-IN-Standard-C"));
    _languageValues.Add(TEXT("pa-IN-Standard-D"));
    _languageValues.Add(TEXT("ms-MY-Standard-A"));
    _languageValues.Add(TEXT("ms-MY-Standard-B"));
    _languageValues.Add(TEXT("ms-MY-Standard-C"));
    _languageValues.Add(TEXT("ms-MY-Standard-D"));
    _languageValues.Add(TEXT("id-ID-Standard-A"));
    _languageValues.Add(TEXT("id-ID-Standard-B"));
    _languageValues.Add(TEXT("id-ID-Standard-C"));
    _languageValues.Add(TEXT("id-ID-Standard-D"));
    _languageValues.Add(TEXT("nl-BE-Standard-A"));
    _languageValues.Add(TEXT("nl-BE-Standard-B"));
    _languageValues.Add(TEXT("nl-NL-Standard-B"));
    _languageValues.Add(TEXT("nl-NL-Standard-C"));
    _languageValues.Add(TEXT("nl-NL-Standard-D"));
    _languageValues.Add(TEXT("nl-NL-Standard-E"));
    _languageValues.Add(TEXT("nl-NL-Standard-A"));
    _languageValues.Add(TEXT("fil-PH-Standard-B"));
    _languageValues.Add(TEXT("fil-PH-Standard-C"));
    _languageValues.Add(TEXT("fil-PH-Standard-D"));
    _languageValues.Add(TEXT("fil-PH-Standard-A"));
    _languageValues.Add(TEXT("yue-HK-Standard-A"));
    _languageValues.Add(TEXT("yue-HK-Standard-B"));
    _languageValues.Add(TEXT("yue-HK-Standard-C"));
    _languageValues.Add(TEXT("yue-HK-Standard-D"));
    _languageValues.Add(TEXT("af-ZA-Standard-A"));
    _languageValues.Add(TEXT("bg-bg-Standard-A"));
    _languageValues.Add(TEXT("hu-HU-Standard-A"));
    _languageValues.Add(TEXT("lv-lv-Standard-A"));
    _languageValues.Add(TEXT("pl-PL-Standard-E"));
    _languageValues.Add(TEXT("ro-RO-Standard-A"));
    _languageValues.Add(TEXT("sk-SK-Standard-A"));
    _languageValues.Add(TEXT("sr-rs-Standard-A"));
    _languageValues.Add(TEXT("uk-UA-Standard-A"));
    _languageValues.Add(TEXT("pl-PL-Standard-A"));
    _languageValues.Add(TEXT("pl-PL-Standard-B"));
    _languageValues.Add(TEXT("pl-PL-Standard-C"));
    _languageValues.Add(TEXT("pl-PL-Standard-D"));
    _languageValues.Add(TEXT("tr-TR-Standard-B"));
    _languageValues.Add(TEXT("tr-TR-Standard-C"));
    _languageValues.Add(TEXT("tr-TR-Standard-D"));
    _languageValues.Add(TEXT("tr-TR-Standard-E"));
    _languageValues.Add(TEXT("tr-TR-Standard-A"));
    _languageValues.Add(TEXT("ca-es-Standard-A"));
    _languageValues.Add(TEXT("es-ES-Standard-C"));
    _languageValues.Add(TEXT("es-ES-Standard-D"));
    _languageValues.Add(TEXT("es-ES-Standard-B"));
    _languageValues.Add(TEXT("es-ES-Standard-A"));

    _audioProfilesValues.Add(EAudioEffectsProfile::Wearable, TEXT("wearable-class-device"));
    _audioProfilesValues.Add(EAudioEffectsProfile::Handset, TEXT("handset-class-device"));
    _audioProfilesValues.Add(EAudioEffectsProfile::Headphone, TEXT("headphone-class-device"));
    _audioProfilesValues.Add(EAudioEffectsProfile::SmallBluetoothSpeaker, TEXT("small-bluetooth-speaker-class-device"));
    _audioProfilesValues.Add(EAudioEffectsProfile::MediumBluetoothSpeaker, TEXT("medium-bluetooth-speaker-class-device"));
    _audioProfilesValues.Add(EAudioEffectsProfile::LargeHomeEntertainment, TEXT("large-home-entertainment-class-device"));
    _audioProfilesValues.Add(EAudioEffectsProfile::LargeAutomotive, TEXT("large-automotive-class-device"));
    _audioProfilesValues.Add(EAudioEffectsProfile::Telephony, TEXT("telephony-class-application"));
}

