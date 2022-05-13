// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FGoogleNaturalLanguageAPIModule : public IModuleInterface
{
	friend class UGoogleNaturalLanguageUtils;
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	FString _apiKey = "";
	bool _useApiKeyFromEnvVariable = false;
};
