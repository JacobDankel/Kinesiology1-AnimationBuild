// Copyright 2020 Ilgar Lunin. All Rights Reserved.


#include "GoogleNaturalLanguageUtils.h"
#include "GoogleNLDefinitions.h"
#include "GoogleNaturalLanguageAPI.h"
#include "Modules/ModuleManager.h"


bool UGoogleNaturalLanguageUtils::isLanguageValid(ENLAPIMethod apiMethod, ENLLanguage language)
{
    bool _result = false;

    const TMap<ENLAPIMethod, TArray<ENLLanguage>> apiMethodsLanguageSupport = {
        { ENLAPIMethod::CONTENT_CLASSIFICATION, { ENLLanguage::ENGLISH }},
        { ENLAPIMethod::SYNTACTIC_ANALISIS, {
            ENLLanguage::CHINISE_SIMPLIFIED, ENLLanguage::CHINISE_TRADITIONAL,
            ENLLanguage::ENGLISH, ENLLanguage::FRENCH, ENLLanguage::GERMAN,
            ENLLanguage::ITALIAN, ENLLanguage::JAPANESE, ENLLanguage::KOREAN,
            ENLLanguage::PORTUGUESE, ENLLanguage::RUSSIAN, ENLLanguage::SPANISH
        }},
        { ENLAPIMethod::ENTITY_ANALYSIS, {
            ENLLanguage::CHINISE_SIMPLIFIED, ENLLanguage::CHINISE_TRADITIONAL,
            ENLLanguage::ENGLISH, ENLLanguage::FRENCH, ENLLanguage::GERMAN,
            ENLLanguage::ITALIAN, ENLLanguage::JAPANESE, ENLLanguage::KOREAN,
            ENLLanguage::PORTUGUESE, ENLLanguage::RUSSIAN, ENLLanguage::SPANISH
        }},
        { ENLAPIMethod::SENTIMENT_ANALYSIS, {
            ENLLanguage::ARABIC, ENLLanguage::CHINISE_SIMPLIFIED, ENLLanguage::CHINISE_TRADITIONAL,
            ENLLanguage::DUTCH, ENLLanguage::ENGLISH, ENLLanguage::FRENCH, ENLLanguage::GERMAN, ENLLanguage::INDONESIAN,
            ENLLanguage::ITALIAN, ENLLanguage::JAPANESE, ENLLanguage::KOREAN, ENLLanguage::POLISH, ENLLanguage::PORTUGUESE,
            ENLLanguage::SPANISH, ENLLanguage::THAI, ENLLanguage::TURKISH, ENLLanguage::VIETNAMESE
        }},
        { ENLAPIMethod::ENTITY_SENTIMENT_ANALYSIS, {
            ENLLanguage::ENGLISH, ENLLanguage::JAPANESE, ENLLanguage::SPANISH
        }}
    };

    // Treat language valid since annotate text is all methods all together
    if (apiMethod == ENLAPIMethod::ANNOTATE_TEXT)
        _result = true;
    else
        _result = apiMethodsLanguageSupport[apiMethod].Contains(language);

    return _result;
}

void UGoogleNaturalLanguageUtils::setGoogleNaturalLanguageApiKey(FString apiKey)
{
    FGoogleNaturalLanguageAPIModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleNaturalLanguageAPIModule>("GoogleNaturalLanguageAPI");
    mod._apiKey = apiKey;
}

FString UGoogleNaturalLanguageUtils::getApiKey()
{
    FGoogleNaturalLanguageAPIModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleNaturalLanguageAPIModule>("GoogleNaturalLanguageAPI");
    return mod._apiKey;
}

void UGoogleNaturalLanguageUtils::setUseNaturalLanguageApiKeyFromEnvironmentVars(bool bUseEnvVariable)
{
    FGoogleNaturalLanguageAPIModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleNaturalLanguageAPIModule>("GoogleNaturalLanguageAPI");
    mod._useApiKeyFromEnvVariable = bUseEnvVariable;
}

bool UGoogleNaturalLanguageUtils::getUseApiKeyFromEnvironmentVars()
{
    FGoogleNaturalLanguageAPIModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleNaturalLanguageAPIModule>("GoogleNaturalLanguageAPI");
    return mod._useApiKeyFromEnvVariable;
}


FString UGoogleNaturalLanguageUtils::GetEnvironmentVariable(FString key)
{
    FString result;
#if PLATFORM_WINDOWS
    result = FWindowsPlatformMisc::GetEnvironmentVariable(*key);
#endif

#if PLATFORM_MAC
    result = FApplePlatformMisc::GetEnvironmentVariable(*key);
#endif

#if PLATFORM_LINUX
    result = FLinuxPlatformMisc::GetEnvironmentVariable(*key);
#endif

    return result;
}


FNLEntity UGoogleNaturalLanguageUtils::parseNLEntity(FJsonObject json)
{
    GoogleNaturalLanguageValueMapping mapping;

    FNLEntity res;

    // name
    res.name = json.GetStringField(TEXT("name"));

    // type
    FString eType = json.GetStringField("type");
    const ENLEntityType* entityType = mapping.entityTypes.FindKey(eType);
    if (entityType != nullptr)
        res.type = *entityType;

    // meta
    auto metaObject = json.GetObjectField(TEXT("meta"));
    for (auto& elem : metaObject->Values)
        res.metaData.Add(elem.Key, elem.Value->AsString());

    // salience
    res.salience = json.GetNumberField("salience");

    return res;
}
