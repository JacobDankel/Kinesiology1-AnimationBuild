// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GoogleNLDefinitions.h"
#include "HttpModule.h"
#include "GooglNLAnnotateText.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_EightParams(FOnTextAnnotatedPin, const TArray<FNLSentence>&, sentences, const TArray<FNLToken>&, tokens, const TArray<FNLEntity>&, entities, FNLSentiment, documentSentiment, ENLLanguage, language, const TArray<FNLClassificationCategory>&, categories, FString, errorMessage, bool, Success);


/**
 * 
 */
UCLASS()
class GOOGLENATURALLANGUAGEAPI_API UGooglNLAnnotateText : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
    UGooglNLAnnotateText();

    FNLDocument doc;
    FNLFeatures features;
    ENLEncodingType encoding = ENLEncodingType::NONE;
	
    UPROPERTY(BlueprintAssignable, Category = "GoogleNaturalLanguage")
        FOnTextAnnotatedPin Finished;
private:
    GoogleNaturalLanguageValueMapping mapping;

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "GoogleNaturalLanguage")
        static UGooglNLAnnotateText* GoogleNaturalLanguageAnnotateText(FNLDocument document, FNLFeatures features, ENLEncodingType encoding);

    virtual void Activate() override;
    void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};
