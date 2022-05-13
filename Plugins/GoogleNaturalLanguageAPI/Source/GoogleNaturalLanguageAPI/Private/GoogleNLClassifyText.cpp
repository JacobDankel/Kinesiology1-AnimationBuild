// Copyright 2020 Ilgar Lunin. All Rights Reserved.


#include "GoogleNLClassifyText.h"

UGoogleNLClassifyText::UGoogleNLClassifyText()
{

}

UGoogleNLClassifyText* UGoogleNLClassifyText::GoogleNaturalLanguageClassifyText(FNLDocument document)
{
    UGoogleNLClassifyText* BPNode = NewObject<UGoogleNLClassifyText>();

    BPNode->doc = document;

    return BPNode;
}

void UGoogleNLClassifyText::Activate()
{
    if (!UGoogleNaturalLanguageUtils::isLanguageValid(ENLAPIMethod::CONTENT_CLASSIFICATION, doc.language))
    {
        FNLSentiment res = {};
        Finished.Broadcast({}, TEXT(""), false);
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
        Finished.Broadcast({}, TEXT(""), false);
        return;
    }

    auto HttpRequest = FHttpModule::Get().CreateRequest();
    FString url = FString::Printf(TEXT("https://language.googleapis.com/v1beta2/documents:classifyText?key=%s"), *_apiKey);
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

    // convert payload to string
    FString _payload;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&_payload);
    FJsonSerializer::Serialize(_payloadJson.ToSharedRef(), Writer);

    // commit request
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetContentAsString(_payload);

    if (HttpRequest->ProcessRequest())
    {
        HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGoogleNLClassifyText::OnResponse);
    }
    else
    {
        Finished.Broadcast({}, TEXT(""), false);
    }
}

void UGoogleNLClassifyText::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
    if (!WasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("Error processing request.\n%s\n%s"), *Response->GetContentAsString(), *Response->GetURL());
        if (Finished.IsBound())
        {
            Finished.Broadcast({}, *Response->GetContentAsString(), false);
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
            Finished.Broadcast({}, TEXT("Api error"), false);
            return;
        }

        GoogleNLParserV1beta2 parser;

        TArray<FNLClassificationCategory> _out = {};

        auto categoriesObject = responseObject->GetArrayField(TEXT("categories"));
        for (auto &elem : categoriesObject)
        {
            _out.Add(parser.parseClassificationCategory(*elem->AsObject()));
        }

        Finished.Broadcast(_out, TEXT(""), true);
    }
}
