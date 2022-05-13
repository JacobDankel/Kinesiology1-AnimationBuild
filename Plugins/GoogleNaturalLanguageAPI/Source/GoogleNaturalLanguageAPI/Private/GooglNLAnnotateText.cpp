// Copyright 2020 Ilgar Lunin. All Rights Reserved.


#include "GooglNLAnnotateText.h"

UGooglNLAnnotateText::UGooglNLAnnotateText()
{

}

UGooglNLAnnotateText* UGooglNLAnnotateText::GoogleNaturalLanguageAnnotateText(FNLDocument document, FNLFeatures features, ENLEncodingType encoding)
{
    UGooglNLAnnotateText* BPNode = NewObject<UGooglNLAnnotateText>();

    BPNode->doc = document;
    BPNode->features = features;
    BPNode->encoding = encoding;

    return BPNode;
}

void UGooglNLAnnotateText::Activate()
{
    FString _apiKey;
    if (UGoogleNaturalLanguageUtils::getUseApiKeyFromEnvironmentVars())
        _apiKey = UGoogleNaturalLanguageUtils::GetEnvironmentVariable(TEXT("GOOGLE_API_KEY"));
    else
        _apiKey = UGoogleNaturalLanguageUtils::getApiKey();

    if (_apiKey.IsEmpty())
    {
        Finished.Broadcast({}, {}, {}, {}, doc.language, {}, TEXT("Api key is not set"), false);
        return;
    }

    auto HttpRequest = FHttpModule::Get().CreateRequest();
    FString url = FString::Printf(TEXT("https://language.googleapis.com/v1beta2/documents:annotateText?key=%s"), *_apiKey);
    HttpRequest->SetURL(url);
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json; charset=utf-8"));

    // build payload
    TSharedPtr<FJsonObject> _payloadJson = MakeShareable(new FJsonObject());

    // document
    TSharedPtr<FJsonObject> _documentObject = MakeShareable(new FJsonObject());
    _documentObject->SetStringField(TEXT("type"), mapping.documentType[doc.type]);
    _documentObject->SetStringField(TEXT("language"), mapping.languagCodes[doc.language]);
    if (!doc.content.IsEmpty())
        _documentObject->SetStringField(TEXT("content"), doc.content);
    else if (!doc.gcsContentUri.IsEmpty())
        _documentObject->SetStringField(TEXT("gcsContentUri"), doc.gcsContentUri);

    _payloadJson->SetObjectField(TEXT("document"), _documentObject);

    // features
    TSharedPtr<FJsonObject> _featuresObject = MakeShareable(new FJsonObject());
    _featuresObject->SetBoolField(TEXT("extractSyntax"), features.extractSyntax);
    _featuresObject->SetBoolField(TEXT("extractEntities"), features.extractEntities);
    _featuresObject->SetBoolField(TEXT("extractDocumentSentiment"), features.extractDocumentSentiment);
    _featuresObject->SetBoolField(TEXT("extractEntitySentiment"), features.extractEntitySentiment);
    _featuresObject->SetBoolField(TEXT("classifyText"), features.classifyText);
    _payloadJson->SetObjectField(TEXT("features"), _featuresObject);

    // encoding
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
        HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGooglNLAnnotateText::OnResponse);
    }
    else
    {
        Finished.Broadcast({}, {}, {}, {}, doc.language, {}, TEXT("Error sending request"), false);
    }
}

void UGooglNLAnnotateText::OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful)
{
    if (!WasSuccessful)
    {
        UE_LOG(LogTemp, Warning, TEXT("Error processing request.\n%s\n%s"), *Response->GetContentAsString(), *Response->GetURL());
        if (Finished.IsBound())
        {
            Finished.Broadcast({}, {}, {}, {}, doc.language, {}, *Response->GetContentAsString(), false);
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
            Finished.Broadcast({}, {}, {}, {}, doc.language, {}, TEXT("Api error"), false);
            return;
        }


        GoogleNLParserV1beta2 parser;

        // sentences
        TArray<FNLSentence> _sentences = {};
        auto sentencesObject = responseObject->GetArrayField(TEXT("sentences"));
        for (auto& elem : sentencesObject)
        {
            _sentences.Add(parser.parseSentense(*elem->AsObject()));
        }

        // tokens
        TArray<FNLToken> _tokens = {};
        auto tokensObject = responseObject->GetArrayField(TEXT("tokens"));
        for (auto& elem : tokensObject)
        {
            _tokens.Add(parser.parseToken(*elem->AsObject()));
        }

        // entities
        TArray<FNLEntity> _entities = {};
        auto entitiesObject = responseObject->GetArrayField(TEXT("entities"));
        for (auto& elem : entitiesObject)
        {
            _entities.Add(parser.parseEntity(*elem->AsObject()));
        }

        // sentiment
        auto docSentimentObject = responseObject->GetObjectField(TEXT("documentSentiment"));
        FNLSentiment _sentiment = parser.parseSentiment(*docSentimentObject);

        // language
        FString lang = responseObject->GetStringField(TEXT("language"));
        const ENLLanguage* langType = mapping.languagCodes.FindKey(lang);

        // categories
        TArray<FNLClassificationCategory> _categories = {};
        auto categoriesObject = responseObject->GetArrayField(TEXT("categories"));
        for (auto& elem : categoriesObject)
        {
            _categories.Add(parser.parseClassificationCategory(*elem->AsObject()));
        }


        Finished.Broadcast(_sentences, _tokens, _entities, _sentiment, *langType, _categories, TEXT(""), true);

    }
}
