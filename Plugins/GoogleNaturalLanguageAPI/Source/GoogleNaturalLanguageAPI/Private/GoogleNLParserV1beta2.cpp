// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#include "GoogleNLParserV1beta2.h"


GoogleNLParserV1beta2::GoogleNLParserV1beta2()
{

}

GoogleNLParserV1beta2::~GoogleNLParserV1beta2()
{

}

FNLClassificationCategory GoogleNLParserV1beta2::parseClassificationCategory(FJsonObject json)
{
    FNLClassificationCategory res = {};

    res.name = json.GetStringField(TEXT("name"));
    res.confidence = json.GetNumberField(TEXT("confidence"));

    return res;
}

ENLEncodingType GoogleNLParserV1beta2::parseEncodingType(FString)
{
    ENLEncodingType res = ENLEncodingType::NONE;

    return res;
}

FNLEntity GoogleNLParserV1beta2::parseEntity(FJsonObject json)
{
    FNLEntity res = {};

    res.name = json.GetStringField(TEXT("name"));

    res.type = parseEntityType(json.GetStringField(TEXT("type")));
    
    auto metaObject = json.GetObjectField(TEXT("metadata"));
    for (auto &elem : metaObject->Values)
        res.metaData.Add(elem.Key, elem.Value->AsString());

    res.salience = json.GetNumberField(TEXT("salience"));
    
    auto mentionsObject = json.GetArrayField(TEXT("mentions"));
    for (TSharedPtr<FJsonValue> &elem : mentionsObject)
    {
        FNLEntityMention mention = parseEntityMention(*elem->AsObject());
        res.mentions.Add(mention);
    }

    if (json.HasField(TEXT("sentiment")))
        res.sentiment = parseSentiment(*json.GetObjectField(TEXT("sentiment")));

    return res;
}

FNLSentence GoogleNLParserV1beta2::parseSentense(FJsonObject json)
{
    FNLSentence res = {};

    res.text = parseTextSpan(*json.GetObjectField(TEXT("text")));
    if (json.HasField(TEXT("sentiment")))
    {
        res.sentiment = parseSentiment(*json.GetObjectField(TEXT("sentiment")));
    }
    return res;
}

FNLEntityMention GoogleNLParserV1beta2::parseEntityMention(const FJsonObject& json)
{
    FNLEntityMention res = {};

    res.text = parseTextSpan(*json.GetObjectField(TEXT("text")));
    res.type = parseMentionType(json.GetStringField(TEXT("type")));
    if (json.HasField(TEXT("sentiment")))
        res.sentiment = parseSentiment(*json.GetObjectField(TEXT("sentiment")));

    return res;
}

FNLSentiment GoogleNLParserV1beta2::parseSentiment(FJsonObject json)
{
    FNLSentiment res = {};

    res.magnitude = json.GetNumberField(TEXT("magnitude"));
    res.score = json.GetNumberField(TEXT("score"));

    return res;
}

FNLTextSpan GoogleNLParserV1beta2::parseTextSpan(FJsonObject json)
{
    FNLTextSpan res = {};

    res.content = json.GetStringField(TEXT("content"));
    res.beginOffset = json.GetNumberField(TEXT("beginOffset"));

    return res;
}

FNLToken GoogleNLParserV1beta2::parseToken(FJsonObject json)
{
    FNLToken res = {};

    res.text = parseTextSpan(*json.GetObjectField(TEXT("text")));
    res.partOfSpeech = parsePartOfSpeech(*json.GetObjectField(TEXT("partOfSpeech")));
    res.dependencyEdge = parseDependencyEdge(*json.GetObjectField(TEXT("dependencyEdge")));
    res.lemma = json.GetStringField(TEXT("lemma"));

    return res;
}

ENLEntityType GoogleNLParserV1beta2::parseEntityType(FString typeString)
{
    ENLEntityType res = ENLEntityType::UNKNOWN;

    const ENLEntityType* type = mapping.entityTypes.FindKey(typeString);
    if (type != nullptr)
    {
        res = *type;
    }

    return res;
}

ENLMentionType GoogleNLParserV1beta2::parseMentionType(FString type)
{
    ENLMentionType res = ENLMentionType::TYPE_UNKNOWN;

    const ENLMentionType* mentionType = mapping.mentionTypes.FindKey(type);
    if (mentionType != nullptr)
    {
        res = *mentionType;
    }

    return res;
}

FNLPartOfSpeech GoogleNLParserV1beta2::parsePartOfSpeech(const FJsonObject& json)
{
    FNLPartOfSpeech res = {};

    res.tag = *mapping.tagTypes.FindKey(json.GetStringField(TEXT("tag")));
    res.aspect = *mapping.aspectTypes.FindKey(json.GetStringField(TEXT("aspect")));
    res.Case = *mapping.caseTypes.FindKey(json.GetStringField(TEXT("case")));
    res.form = *mapping.formTypes.FindKey(json.GetStringField(TEXT("form")));
    res.gender = *mapping.genderTypes.FindKey(json.GetStringField(TEXT("gender")));
    res.mood = *mapping.moodTypes.FindKey(json.GetStringField(TEXT("mood")));
    res.number = *mapping.numberTypes.FindKey(json.GetStringField(TEXT("number")));
    res.person = *mapping.personTypes.FindKey(json.GetStringField(TEXT("person")));
    res.proper = *mapping.properTypes.FindKey(json.GetStringField(TEXT("proper")));
    res.reciprocity = *mapping.reciprocityTypes.FindKey(json.GetStringField(TEXT("reciprocity")));
    res.tense = *mapping.tenseTypes.FindKey(json.GetStringField(TEXT("tense")));
    res.voice = *mapping.voiceTypes.FindKey(json.GetStringField(TEXT("voice")));

    return res;
}

FNLDependencyEdge GoogleNLParserV1beta2::parseDependencyEdge(const FJsonObject& json)
{
    FNLDependencyEdge res = {};

    res.headTokenIndex = int32(json.GetNumberField(TEXT("headTokenIndex")));
    res.label = *mapping.labelTypes.FindKey(json.GetStringField(TEXT("label")));;

    return res;
}
