// Fill out your copyright notice in the Description page of Project Settings.


#include "KineCharMoveComp.h"

void UKineCharMoveComp::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) {
	Super::OnMoveCompleted(RequestID, Result);
	
	//HuRef->LocationReached();
}