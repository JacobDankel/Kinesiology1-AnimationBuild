// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#pragma once

#include "GoogleNaturalLanguageInterfaces.h"
#include "GoogleNLDefinitions.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"


class GoogleNLParserV1beta2 : public IGoogleNLParser
{
public:
    GoogleNLParserV1beta2();
    virtual ~GoogleNLParserV1beta2();
    virtual FNLClassificationCategory parseClassificationCategory(FJsonObject) override;
    virtual ENLEncodingType parseEncodingType(FString) override;
    virtual FNLEntity parseEntity(FJsonObject) override;
    virtual FNLSentence parseSentense(FJsonObject) override;
    virtual FNLSentiment parseSentiment(FJsonObject) override;
    virtual FNLTextSpan parseTextSpan(FJsonObject) override;
    virtual FNLToken parseToken(FJsonObject) override;
    
    FNLEntityMention parseEntityMention(const FJsonObject&);
    ENLEntityType parseEntityType(FString);
    ENLMentionType parseMentionType(FString);
    FNLPartOfSpeech parsePartOfSpeech(const FJsonObject&);
    FNLDependencyEdge parseDependencyEdge(const FJsonObject&);

private:
    const GoogleNaturalLanguageValueMapping mapping;

};
