// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "heightScale.h"
#include "VRCharacter.h"
#include "scorePlacement.h"
#include "GoogleSpeechFunctionLibrary.h"
#include "MicrophoneCapture.h"
#include "GoogleNaturalLanguageUtils.h"
#include "C_UI.h"
#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"
#include "Components/InputComponent.h"
#include "CookFrameSequenceAsync.h"
#include "OVRLipSyncPlaybackActorComponent.h"
#include "VaRest.h"
#include "VaRestRequestJSON.h"
#include "VaRestJsonObject.h"
#include "VaRestSubsystem.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "AIController.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/LatentActionManager.h"
#include "Misc/OutputDeviceNull.h"
#include "CMetaHumans.generated.h"

UCLASS()
class KINESIOLOGY1_API ACMetaHumans : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACMetaHumans();

	ACMetaHumans * self;

	UGoogleSpeechFunctionLibrary* gsfl;
	UGoogleNaturalLanguageUtils* gnlu;
	AheightScale* scaleRef;
	AVRCharacter* playerRef;
	TArray<bool> scoreArr = { true, true, true, true, true, true, true };
	float scoreNum = 0;
	UGoogleTTS* tts;
	UMicrophoneCapture* micCap;
	TArray<uint8> CaptureData;
	int32 SamplesRecorded;
	FString question;
	bool startMove = false;
	FString lang = "en-US";

	UPROPERTY(EditAnywhere)
	UWidgetComponent* checkBoxWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isHatOn =  true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isShoesOn = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool walkReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool standStill = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool breathIn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool removeHat = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool removeShoes = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool raiseHead = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool backToPole = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText score = FText::FromString(TEXT("Score: 0/5"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* cHat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* cFeet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAudioComponent* cAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UOVRLipSyncPlaybackActorComponent* cOVR;

	UPROPERTY()
	UVaRestSubsystem* VSubsystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Database)
	UVaRestRequestJSON* RequestBP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Database)
	UVaRestJsonObject* RequestObjectBP;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	//virtual void PostInitializeComponents() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable)
	void AddPoint(float point);

	UFUNCTION(BlueprintCallable)
	void Start_TTS(const FString& text);

	UFUNCTION(BlueprintCallable, Category = "Custom")
	void FinishSTT(FString phrase, bool success, FString errorMessage);

	UFUNCTION(BlueprintCallable)
	void FinishApplyURL();

	UFUNCTION()
	void FinishTTS(USoundWave* Sound, const TArray<uint8>& RawSamples, bool Success);

	UFUNCTION()
	void FinishLipSync(UOVRLipSyncFrameSequence* FrameSequence, bool Success);

	UFUNCTION(BlueprintCallable)
	void StartRecord();

	UFUNCTION(BlueprintCallable)
	void FinishRecord();

	UFUNCTION(BlueprintCallable)
	void UseIntent(const FString& intent);

	UFUNCTION(BlueprintCallable)
	void WalkScale();

	UFUNCTION(BlueprintCallable)
	void RotatePatient(float direction);

	UFUNCTION(BlueprintCallable)
	void FaceAwayFromScale();

	UFUNCTION(BlueprintCallable)
	void RemoveHat();

	UFUNCTION(BlueprintCallable)
	void RemoveShoes();

	UFUNCTION(BlueprintCallable)
	void StandStraight();

	UFUNCTION(BlueprintCallable)
	void TakeBreath();

	UFUNCTION(BlueprintCallable)
	void RaiseHead();

	UFUNCTION(BlueprintCallable)
	void SwitchToSpanish();

	UFUNCTION(BlueprintCallable)
	void SwitchToEnglish();
};
