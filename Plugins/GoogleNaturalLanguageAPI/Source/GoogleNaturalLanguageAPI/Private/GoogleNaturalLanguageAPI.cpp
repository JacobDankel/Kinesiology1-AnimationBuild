// Copyright 2020 Ilgar Lunin. All Rights Reserved.

#include "GoogleNaturalLanguageAPI.h"

#define LOCTEXT_NAMESPACE "FGoogleNaturalLanguageAPIModule"

void FGoogleNaturalLanguageAPIModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FGoogleNaturalLanguageAPIModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FGoogleNaturalLanguageAPIModule, GoogleNaturalLanguageAPI)