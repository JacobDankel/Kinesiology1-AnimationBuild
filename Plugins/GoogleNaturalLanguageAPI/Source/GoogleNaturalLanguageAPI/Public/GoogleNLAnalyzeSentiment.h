// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GoogleNLDefinitions.h"
#include "HttpModule.h"
#include "GoogleNLAnalyzeSentiment.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnSentimentAnalizedPin, const FNLSentiment&, documentSentiment, const ENLLanguage&, language, const TArray<FNLSentence>&, sentences, const FString&, errorMessage,bool, Success);

/**
 * 
 */
UCLASS()
class GOOGLENATURALLANGUAGEAPI_API UGoogleNLAnalyzeSentiment : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
    UGoogleNLAnalyzeSentiment();

    FNLDocument doc;
    ENLEncodingType encoding = ENLEncodingType::NONE;

    UPROPERTY(BlueprintAssignable, Category = "GoogleNaturalLanguage")
        FOnSentimentAnalizedPin Finished;
private:
    GoogleNaturalLanguageValueMapping mapping;

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "GoogleNaturalLanguage")
        static UGoogleNLAnalyzeSentiment* GoogleNaturalLanguageAnalyzeSentiment(FNLDocument document, ENLEncodingType encoding);
    
    virtual void Activate() override;
    void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

};
