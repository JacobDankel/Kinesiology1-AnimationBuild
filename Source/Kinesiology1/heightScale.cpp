// Fill out your copyright notice in the Description page of Project Settings.


#include "heightScale.h"

// Sets default values
AheightScale::AheightScale()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AheightScale::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AheightScale::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
/*
void AheightScale::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	InputComponent->BindAction("RecordSTT", IE_Pressed, this, &AheightScale::CallStartRecord); //handle input maybe???
	InputComponent->BindAction("RecordSTT", IE_Released, this, &AheightScale::CallFinishRecord);

	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("binding input called"));
}

void AheightScale::CallStartRecord() {
	ACMetaHumans::StartRecord();
}

void AheightScale::CallFinishRecord() {
	ACMetaHumans::FinishRecord();
}
*/