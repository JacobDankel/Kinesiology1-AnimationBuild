// Copyright 2020 Ilgar Lunin. All Rights Reserved.


#include "GoogleNLAnalyzeEntities.h"
#include "GoogleNaturalLanguageUtils.h"
#include "Http.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "GoogleNLParserV1beta2.h"


UGoogleNLAnalyzeEntities::UGoogleNLAnalyzeEntities()
{

}

UGoogleNLAnalyzeEntities* UGoogleNLAnalyzeEntities::GoogleNaturalLanguageAnalyzeEntitites(FNLDocument document, ENLEncodingType encoding, bool analyzeEntitySentiment)
{
    UGoogleNLAnalyzeEntities* BPNode = NewObject<UGoogleNLAnalyzeEntities>();
    BPNode->mAnalyzeEntitySentiment = analyzeEntitySentiment;
    BPNode->doc = document;
    BPNode->encoding = encoding;

    return BPNode;
}

void UGoogleNLAnalyzeEntities::Activate()
{
    ENLAPIMethod selectedApiMethod = mAnalyzeEntitySentiment ? ENLAPIMethod::ENTITY_SENTIMENT_ANALYSIS : ENLAPIMethod::ENTITY_ANALYSIS;
    if (!UGoogleNaturalLanguageUtils::isLanguageValid(selectedApiMethod, doc.language))
    {
        Finished.Broadcast({}, doc.language, TEXT("Language is not supported"), false);
        return;
    }

    FString _apiKey;
    if (UGoogleNaturalLanguageUtils::getUseApiKeyFromEnvironmentVars())
        _apiKey = UGoogleNaturalLanguageUtils::GetEnvironmentVariable(TEXT("GOOGLE_API_KEY"));
    else
        _apiKey = UGoogleNaturalLanguageUtils::getApiKey();

    if (_apiKey.IsEmpty())
    {
        Finished.Broadcast({}, doc.language, TEXT("Api key is not set"), false);
        return;
    }

    auto HttpRequest = FHttpModule::Get().CreateRequest();
    FString apiMethod = mAnalyzeEntitySentiment ? FString("analyzeEntitySentiment") : FString("analyzeEntities");
    FString url = FString::Printf(TEXT("https://language.googleapis.com/v1beta2/documents:%s?key=%s"), *apiMethod, *_apiKey);
    HttpRequest->SetURL(url);
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json; charset=utf-8"));

    // build payload
    TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());

    TSharedPtr<FJsonObject> _documentObject = MakeShareable(new FJsonObject());
    _documentObject->SetStringField(TEXT("type"), mapping.documentType[doc.type]);
    _documentObject->SetStringField(TEXT("language"), mapping.languagCodes[doc.language]);
    if (!doc.content.IsEmpty())
        _documentObject->SetStringField(TEXT("content"), doc.content);
    else if (!doc.gcsContentUri.IsEmpty())
        _documentObject->SetStringField(TEXT("gcsContentUri"), doc.gcsContentUri);

    _payloadJson->SetObjectField(TEXT("document"), _documentObject);
    _payloadJson->SetStringField(TEXT("encodingType"), mapping.encodingTypes[encoding]);

    // convert payload to string
    FString _payload;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
    FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);

    // commit request
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetContentAsString(_payload);

    if (HttpRequest->ProcessRequest())
    {
        HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGoogleNLAnalyzeEntities::OnResponse);
    }
    else
    {
        Finished.Broadcast({}, doc.language, ("Error sending request"), false);
    }
}


void UGoogleNLAnalyzeEntities::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
    if (!WasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("Error processing request.\n%s\n%s"), *Response->GetContentAsString(), *Response->GetURL());
        if (Finished.IsBound())
        {
            Finished.Broadcast({}, doc.language, *Response->GetContentAsString(), false);
        }

        return;
    }

    TSharedPtr<FJsonObject> responseObject;
    TSharedRef<TJsonReader<>> reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
    if (FJsonSerializer::Deserialize(reader, responseObject))
    {
        bool err = responseObject->HasField("error");

        if (err)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s"), *Response->GetContentAsString());
            Finished.Broadcast({}, doc.language, TEXT("Api error"), false);
            return;
        }

        FString lang = responseObject->GetStringField(TEXT("language"));

        GoogleNLParserV1beta2 parser;
        TArray<FNLEntity> _out;

        auto entitiesObject = responseObject->GetArrayField(TEXT("entities"));
        for (auto &elem : entitiesObject)
        {
            _out.Add(parser.parseEntity(*elem->AsObject()));
        }

        const ENLLanguage* langType = mapping.languagCodes.FindKey(lang);
        if (langType != nullptr)
            Finished.Broadcast(_out, *langType, "", true);

    }

}
