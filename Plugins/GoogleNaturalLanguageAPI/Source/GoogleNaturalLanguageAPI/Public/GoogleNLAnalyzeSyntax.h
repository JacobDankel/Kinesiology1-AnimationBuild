// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GoogleNLDefinitions.h"
#include "HttpModule.h"
#include "GoogleNLAnalyzeSyntax.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnSyntaxAnalizedPin, const TArray<FNLSentence>&, sentences, const TArray<FNLToken>&, tokens, const ENLLanguage&, language, const FString&, errorMessage, bool, Success);

/**
 * 
 */
UCLASS()
class GOOGLENATURALLANGUAGEAPI_API UGoogleNLAnalyzeSyntax : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
    UGoogleNLAnalyzeSyntax();

    FNLDocument doc;
    ENLEncodingType encoding = ENLEncodingType::NONE;

    UPROPERTY(BlueprintAssignable, Category = "GoogleNaturalLanguage")
        FOnSyntaxAnalizedPin Finished;

private:
    GoogleNaturalLanguageValueMapping mapping;

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "GoogleNaturalLanguage")
        static UGoogleNLAnalyzeSyntax* GoogleNaturalLanguageAnalyzeSyntax(FNLDocument document, ENLEncodingType encoding);

    virtual void Activate() override;
    void OnResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool WasSuccessful);
};
