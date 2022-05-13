// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GoogleNLDefinitions.h"
#include "HttpModule.h"
#include "GoogleNLAnalyzeEntities.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnEntityAnalizedPin, const TArray<FNLEntity>&, entitites, const ENLLanguage&, language, const FString&, errorMessage, bool, Success);

/**
 * 
 */
UCLASS()
class GOOGLENATURALLANGUAGEAPI_API UGoogleNLAnalyzeEntities : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
    UGoogleNLAnalyzeEntities();

    bool mAnalyzeEntitySentiment = false;
    FNLDocument doc;
    ENLEncodingType encoding = ENLEncodingType::NONE;

    UPROPERTY(BlueprintAssignable, Category = "GoogleNaturalLanguage")
        FOnEntityAnalizedPin Finished;
private:
    GoogleNaturalLanguageValueMapping mapping;

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "GoogleNaturalLanguage")
        static UGoogleNLAnalyzeEntities* GoogleNaturalLanguageAnalyzeEntitites(FNLDocument document, ENLEncodingType encoding, bool analyzeEntitySentiment=false);

    virtual void Activate() override;
    void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};
