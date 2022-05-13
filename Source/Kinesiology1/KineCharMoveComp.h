// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CMetaHumans.h"
#include "VRBaseCharacterMovementComponent.h"
#include "KineCharMoveComp.generated.h"

/**
 * 
 */
UCLASS()
class KINESIOLOGY1_API UKineCharMoveComp : public UVRBaseCharacterMovementComponent
{
	GENERATED_BODY()
	
public:

	//UFUNCTION()
	void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	//ACMetaHumans* HuRef;

};
