// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GoogleNLDefinitions.h"
#include "HttpModule.h"
#include "GoogleNLClassifyText.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTextClassifiedPin, const TArray<FNLClassificationCategory>&, categories, const FString&, errorMessage, bool, Success);

/**
 * 
 */
UCLASS()
class GOOGLENATURALLANGUAGEAPI_API UGoogleNLClassifyText : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
    UGoogleNLClassifyText();

    FNLDocument doc;

    UPROPERTY(BlueprintAssignable, Category = "GoogleNaturalLanguage")
        FOnTextClassifiedPin Finished;

private:
    GoogleNaturalLanguageValueMapping mapping;

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "GoogleNaturalLanguage")
        static UGoogleNLClassifyText* GoogleNaturalLanguageClassifyText(FNLDocument document);

    virtual void Activate() override;
    void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);

};
