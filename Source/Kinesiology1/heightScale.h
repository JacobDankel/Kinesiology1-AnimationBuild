// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
//#include "CMetaHumans.h"
#include "Components/InputComponent.h"
#include "GameFramework/Actor.h"
#include "heightScale.generated.h"

UCLASS()
class KINESIOLOGY1_API AheightScale : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AheightScale();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	/*
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	void CallStartRecord();

	void CallFinishRecord();
	*/
};
