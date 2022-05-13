// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#include "GoogleNLDefinitions.h"

#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"


class IGoogleNLParser
{
public:
    virtual FNLClassificationCategory parseClassificationCategory(FJsonObject) = 0;
    virtual ENLEncodingType parseEncodingType(FString) = 0;
    virtual FNLEntity parseEntity(FJsonObject) = 0;
    virtual FNLSentence parseSentense(FJsonObject) = 0;
    virtual FNLSentiment parseSentiment(FJsonObject) = 0;
    virtual FNLTextSpan parseTextSpan(FJsonObject) = 0;
    virtual FNLToken parseToken(FJsonObject) = 0;
};
