// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GoogleNLDefinitions.h"

#include "Dom/JsonObject.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"

#if PLATFORM_WINDOWS
#include "Runtime/Core/Public/Windows/WindowsPlatformMisc.h"
#endif

#if PLATFORM_MAC
#include "Runtime/Core/Public/Apple/ApplePlatformMisc.h"
#endif

#if PLATFORM_LINUX
#include "Runtime/Core/Public/Linux/LinuxPlatformMisc.h"
#endif

#include "GoogleNaturalLanguageUtils.generated.h"

/**
 * 
 */
UCLASS()
class GOOGLENATURALLANGUAGEAPI_API UGoogleNaturalLanguageUtils : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "GoogleNaturalLanguage")
    static bool isLanguageValid(ENLAPIMethod apiMethod, ENLLanguage language);

    UFUNCTION(BlueprintCallable, Category = "GoogleNaturalLanguage")
    static void setGoogleNaturalLanguageApiKey(FString apiKey);

    static FString getApiKey();

    UFUNCTION(BlueprintCallable, Category = "GoogleNaturalLanguage")
    static void setUseNaturalLanguageApiKeyFromEnvironmentVars(bool bUseEnvVariable);

    static bool getUseApiKeyFromEnvironmentVars();

    static FString GetEnvironmentVariable(FString key);

    static FNLEntity parseNLEntity(FJsonObject json);
};
