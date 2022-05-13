// Copyright 2019 Ilgar Lunin. All Rights Reserved.


#include "GoogleSpeechFunctionLibrary.h"
#include "Modules/ModuleManager.h"
#include "GoogleSpeechKit.h"
#include "GoogleSpeechUtils.h"

#include <vector>
#include <string>
#include <algorithm>

#include "Serialization/BufferArchive.h"


uint32 ComputeLevenshteinDistance(const std::string& s1, const std::string& s2)
{
    const std::size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<uint32>> d(len1 + 1, std::vector<uint32>(len2 + 1));

    d[0][0] = 0;
    for (uint32 i = 1; i <= len1; ++i) d[i][0] = i;
    for (uint32 i = 1; i <= len2; ++i) d[0][i] = i;

    for (uint32 i = 1; i <= len1; ++i)
        for (uint32 j = 1; j <= len2; ++j)
            d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1) });
    return d[len1][len2];
}

float UGoogleSpeechFunctionLibrary::CompareStrings(const FString& source, const FString& target)
{
    if ((source.Len() == 0) || (target.Len() == 0)) return 0.0;
    if (source == target) return 1.0;

    std::string s1 = TCHAR_TO_UTF8(*source);
    std::string s2 = TCHAR_TO_UTF8(*target);

    int stepsToSame = ComputeLevenshteinDistance(s1, s2);
    return (1.0 - ((double)stepsToSame / (double)FMath::Max(source.Len(), target.Len())));
}

void UGoogleSpeechFunctionLibrary::SetUseApiKeyFromEnvironmentVars(bool bUseEnvVariable)
{
    FGoogleSpeechKitModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleSpeechKitModule>("GoogleSpeechKit");
    mod.bUseApiKeyFromEnvironment = bUseEnvVariable;
}

void UGoogleSpeechFunctionLibrary::SetTTSCachingEnabled(bool bEnabled)
{
    FGoogleSpeechKitModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleSpeechKitModule>("GoogleSpeechKit");
    mod.setTTSCachingEnabled(bEnabled);
}

bool UGoogleSpeechFunctionLibrary::isTTSCachingEnabled()
{
    FGoogleSpeechKitModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleSpeechKitModule>("GoogleSpeechKit");
    return mod.isTTSCachingEnabled();
}

bool UGoogleSpeechFunctionLibrary::wipeTTSCache()
{
    FGoogleSpeechKitModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleSpeechKitModule>("GoogleSpeechKit");
    return mod.wipeCache();
}

void UGoogleSpeechFunctionLibrary::resetTTSCacheRootDirectory()
{
    FGoogleSpeechKitModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleSpeechKitModule>("GoogleSpeechKit");
    mod.resetTTSCacheRootDirectory();
}

FString UGoogleSpeechFunctionLibrary::getTTSCacheRootDirectory()
{
    FGoogleSpeechKitModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleSpeechKitModule>("GoogleSpeechKit");
    return mod.getTTSCacheRootDirectory();
}

void UGoogleSpeechFunctionLibrary::SetTTSCacheRootDirectory(const FString& path)
{
    FGoogleSpeechKitModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleSpeechKitModule>("GoogleSpeechKit");
    mod.setTTSCacheRootDirectory(path);
}

bool UGoogleSpeechFunctionLibrary::GetUseApiKeyFromEnvironmentVars()
{
    FGoogleSpeechKitModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleSpeechKitModule>("GoogleSpeechKit");
    return mod.bUseApiKeyFromEnvironment;
}

void UGoogleSpeechFunctionLibrary::SetApiKey(const FString& apiKey)
{
    FGoogleSpeechKitModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleSpeechKitModule>("GoogleSpeechKit");
    mod._apiKey = apiKey;
}

bool UGoogleSpeechFunctionLibrary::CompareSynthesysParameters(const FGoogleSpeechSynthesisParams& lhs, const FGoogleSpeechSynthesisParams& rhs)
{
    return lhs == rhs;
}

FString UGoogleSpeechFunctionLibrary::GetApiKey()
{
    FGoogleSpeechKitModule& mod = FModuleManager::Get().LoadModuleChecked<FGoogleSpeechKitModule>("GoogleSpeechKit");
    return mod._apiKey;
}

USoundWave* UGoogleSpeechFunctionLibrary::SoundWaveFromRawSamples(const TArray<uint8>& rawSamples, int32 sampleRate, int32 numChannels, bool generateHeader)
{
    USoundWave* result = nullptr;

    if (generateHeader)
        result = GoogleSpeechKitUtils::CreateSoundFromWaveDataWithoutHeader(rawSamples, sampleRate, numChannels);
    else
        result = GoogleSpeechKitUtils::CreateSoundFromWaveDataWithHeader(rawSamples);

    return result;
}

UGoogleTTS* UGoogleSpeechFunctionLibrary::CallGoogleTTS(const FString& speech, FString lang) {
	
	UGoogleTTS* instance = UGoogleTTS::CallTTS(speech, lang);
	instance->Activate();
	return instance;
}

UGoogleSTT* UGoogleSpeechFunctionLibrary::CallGoogleSTT(TArray<uint8> CaptureData, FString lang) {

	UGoogleSTT* instance = UGoogleSTT::CallSTT(CaptureData, lang);
	instance->Activate();
	return instance;
}
