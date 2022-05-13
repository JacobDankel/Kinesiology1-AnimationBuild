// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#include "GoogleNLAnalyzeSentiment.h"
#include "Http.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "GoogleNLParserV1beta2.h"


UGoogleNLAnalyzeSentiment::UGoogleNLAnalyzeSentiment()
{

}

UGoogleNLAnalyzeSentiment* UGoogleNLAnalyzeSentiment::GoogleNaturalLanguageAnalyzeSentiment(FNLDocument document, ENLEncodingType encoding)
{
    UGoogleNLAnalyzeSentiment* BPNode = NewObject<UGoogleNLAnalyzeSentiment>();
    BPNode->doc = document;
    BPNode->encoding = encoding;

    return BPNode;
}

void UGoogleNLAnalyzeSentiment::Activate()
{
    if (!UGoogleNaturalLanguageUtils::isLanguageValid(ENLAPIMethod::SENTIMENT_ANALYSIS, doc.language))
    {
        FNLSentiment res = {};
        Finished.Broadcast(res, doc.language, {}, TEXT(""), false);
        return;
    }

    FString _apiKey;
    if (UGoogleNaturalLanguageUtils::getUseApiKeyFromEnvironmentVars())
        _apiKey = UGoogleNaturalLanguageUtils::GetEnvironmentVariable(TEXT("GOOGLE_API_KEY"));
    else
        _apiKey = UGoogleNaturalLanguageUtils::getApiKey();

    if (_apiKey.IsEmpty())
    {
        FNLSentiment res = {};
        Finished.Broadcast(res, doc.language, {}, TEXT(""), false);
        return;
    }

    auto HttpRequest = FHttpModule::Get().CreateRequest();
    FString url = FString::Printf(TEXT("https://language.googleapis.com/v1beta2/documents:analyzeSentiment?key=%s"), *_apiKey);
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
        HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGoogleNLAnalyzeSentiment::OnResponse);
    }
    else
    {
        FNLSentiment res = {};
        Finished.Broadcast(res, doc.language, {}, TEXT(""), false);
    }

}

void UGoogleNLAnalyzeSentiment::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
    if (!WasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("Error processing request.\n%s\n%s"), *Response->GetContentAsString(), *Response->GetURL());
        if (Finished.IsBound())
        {
            FNLSentiment res = {};
            Finished.Broadcast(res, doc.language, {}, *Response->GetContentAsString(), false);
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
            Finished.Broadcast({}, doc.language, {}, TEXT("Api error"), false);
            return;
        }

        GoogleNLParserV1beta2 parser;

        // documentSentiment
        auto docSentimentObject = responseObject->GetObjectField(TEXT("documentSentiment"));
        FNLSentiment sentimnent = parser.parseSentiment(*docSentimentObject);

        // language
        FString lang = responseObject->GetStringField(TEXT("language"));
        const ENLLanguage* langType = mapping.languagCodes.FindKey(lang);

        // sentences
        TArray<FNLSentence> sentences = {};
        auto sentencesObject = responseObject->GetArrayField(TEXT("sentences"));
        for (auto& elem : sentencesObject)
        {
            sentences.Add(parser.parseSentense(*elem->AsObject()));
        }

        if (langType != nullptr)
        {
            Finished.Broadcast(sentimnent, *langType, sentences, TEXT(""), true);
        }
    }
}
