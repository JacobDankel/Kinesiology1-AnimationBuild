// Fill out your copyright notice in the Description page of Project Settings.

#include "CMetaHumans.h" 
#include "EngineUtils.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

// Sets default values
ACMetaHumans::ACMetaHumans()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	checkBoxWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Human Interface"));
	checkBoxWidget->SetRelativeLocation(FVector(23, -85, 27));

	cAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("cAudio"));
	cOVR = CreateDefaultSubobject<UOVRLipSyncPlaybackActorComponent>(TEXT("cOVR"));

	micCap = CreateDefaultSubobject<UMicrophoneCapture>(TEXT("micCap"));

}

//void ACMetaHumans::PostInitializeComponents() {
//	Super::PostInitializeComponents();
//	GetCharacterMovement()->SetFixedBrakingDistance(100.0f);
//
//}

// Called when the game starts or when spawned
void ACMetaHumans::BeginPlay()
{
	Super::BeginPlay();

	gsfl->wipeTTSCache();
	gsfl->SetUseApiKeyFromEnvironmentVars(false);

	// read from text file and get the API keys from there so they arent hanging around on github
	FString CompleteFilePath = FPaths::GameSourceDir() + "gsflAPIKey.txt";
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*CompleteFilePath)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Could not Find speech function File"));
	}
	FString FileData = "Placeholder";
	FFileHelper::LoadFileToString(FileData, *CompleteFilePath);

	gsfl->SetApiKey(FileData);

	//now we do it again for the second plugin
	CompleteFilePath = FPaths::GameSourceDir() + "gnluAPIKey.txt";
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*CompleteFilePath)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Could not Find natural language File"));
	}
	FileData = "Placeholder";
	FFileHelper::LoadFileToString(FileData, *CompleteFilePath);

	gnlu->setGoogleNaturalLanguageApiKey(FileData);

	TArray<AActor*> heightScaleActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AheightScale::StaticClass(), heightScaleActors);
	scaleRef = Cast<AheightScale>(heightScaleActors[0]);

	FTimerHandle TimerHandle; //this part is how we deal with vr character being annoying and not initializing properly so we have a short delay
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			TArray<AActor*> vrCharacterActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AVRCharacter::StaticClass(), vrCharacterActors);
			vrCharacterActors.Num() > 0 ? playerRef = Cast<AVRCharacter>(vrCharacterActors[0]) : GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("No VR Character Found!"));
		}, 1, false);

	//testing entire routine but without me having to talk
	
	//FinishSTT("Hello", true, "This should not be printing");
	//
	//FTimerHandle timer1; 
	//GetWorld()->GetTimerManager().SetTimer(timer1, [&]()
	//	{
	//		FinishSTT("Remove your hat", true, "This should not be printing");
	//	}, 5, false);
	//
	//FTimerHandle timer2; 
	//GetWorld()->GetTimerManager().SetTimer(timer2, [&]()
	//	{
	//		FinishSTT("Remove your shoes", true, "This should not be printing");
	//	}, 10, false);
	//
	//FTimerHandle timer3;
	//GetWorld()->GetTimerManager().SetTimer(timer3, [&]()
	//	{
	//		FinishSTT("Walk to the scale", true, "This should not be printing");
	//	}, 15, false);
	//
	//FTimerHandle timer4;
	//GetWorld()->GetTimerManager().SetTimer(timer4, [&]()
	//	{
	//		FinishSTT("Take breath", true, "This should not be printing");
	//	}, 20, false);

	//FTimerHandle timer5;
	//GetWorld()->GetTimerManager().SetTimer(timer5, [&]()
	//	{
	//		FinishSTT("Raise head", true, "This should not be printing");
	//	}, 25, false);

	//FTimerHandle timer6;
	//GetWorld()->GetTimerManager().SetTimer(timer6, [&]()
	//	{
	//		FinishSTT("Stand staight", true, "This should not be printing");
	//	}, 30, false);

	//FTimerHandle timer7;
	//GetWorld()->GetTimerManager().SetTimer(timer7, [&]()
	//	{
	//		FinishSTT("Turn your back to the pole", true, "This should not be printing");
	//	}, 35, false);
}

// Called every frame
void ACMetaHumans::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//removed facing away from scale for now, uncomment the below to put it back
	//if (startMove && this->GetActorLocation().Equals(scaleRef->GetActorLocation() + (20,20,0), 100.0f)) {	//the worst way i could check for matching scale location and i hate it but i cant find another way :(
	//	startMove = false;

	//	FTimerHandle bruhTimer;
	//	GetWorld()->GetTimerManager().SetTimer(bruhTimer, [&]()
	//		{
	//			FaceAwayFromScale();
	//		}, 1, false);
	//	
	//}
	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("L" + (scaleRef->GetActorLocation() + (20, 20, 0)).ToString()));
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("L" + GetActorLocation().ToString()));*/
}

// Called to bind functionality to input
void ACMetaHumans::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ACMetaHumans::AddPoint(float point) {
	scoreNum += point;
	score = FText::FromString(TEXT("Score: " + FString::SanitizeFloat(scoreNum) + "/5"));
}

void ACMetaHumans::FinishTTS(USoundWave * Sound, const TArray<uint8>& RawSamples, bool Success)
{
	if (Success) {
		cAudio->SetSound(Sound);

		//this is where Cook Frame Sequence goes
		UCookFrameSequenceAsync* instance = UCookFrameSequenceAsync::CookFrameSequence(RawSamples);
		instance->Activate();

		instance->onFrameSequenceCooked.AddDynamic(this, &ACMetaHumans::FinishLipSync);
	}
}

void ACMetaHumans::FinishLipSync(UOVRLipSyncFrameSequence* FrameSequence, bool Success) {
	if (Success) {
		cOVR->Start(cAudio, FrameSequence);
	}

}

void ACMetaHumans::Start_TTS(const FString& text) {
	UGoogleSpeechFunctionLibrary::CallGoogleTTS(text, lang)->Finished.AddDynamic(this, &ACMetaHumans::FinishTTS);
}

//both StartRecord and FinishRecord are called from menuPopup due to input setup not being called for whatever reason
void ACMetaHumans::StartRecord() {
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Recording STT..."));
	micCap->BeginCapture();
}

void ACMetaHumans::FinishRecord() {
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Saving STT..."));
	micCap->FinishCapture(CaptureData, SamplesRecorded);
	UGoogleSpeechFunctionLibrary::CallGoogleSTT(CaptureData, lang)->Finished.AddDynamic(this, &ACMetaHumans::FinishSTT);
}

void ACMetaHumans::FinishSTT(FString phrase, bool success, FString errorMessage) {
	if (success) { //i have || true here for testing purposes, once this is working it should be removed
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Blue, TEXT("Successful call: " + phrase));
		
		FLatentActionInfo LatentInfo(0, 1, TEXT("FinishApplyURL"), this);	// linkage, UUID, callback function, callback target

		VSubsystem = GEngine->GetEngineSubsystem<UVaRestSubsystem>();
		RequestBP = VSubsystem->ConstructVaRestRequestExt(EVaRestRequestVerb::POST, EVaRestRequestContentType::json);
		RequestObjectBP = VSubsystem->ConstructVaRestJsonObject();
		question = phrase;
		RequestBP->GetRequestObject()->SetStringField("input", phrase); //if second argument is phrase then we are using STT input, if not then it is a string used for testing purposes
		RequestBP->GetRequestObject()->SetStringField("lang", lang);
		RequestBP->ApplyURL("http://127.0.0.1:5000/", RequestObjectBP, GetWorld(), LatentInfo);
		
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, TEXT("Error: " + errorMessage));
		Start_TTS("I'm sorry, I didn't quite catch that.");
	}
}

void ACMetaHumans::FinishApplyURL() {
	FString r = RequestObjectBP->GetStringField("response");
	//this is where i will attempt to get the question and answer to a text file
	//try this: put question in a string, then get intent and response here and write to text file

	FString FilePath = FPaths::GameSourceDir() + "QAdata.txt";
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*FilePath)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Could not Find data File"));
	}
	FString FileData = "Placeholder";
	FFileHelper::LoadFileToString(FileData, *FilePath);
	FString SaveData = FileData + "\nQuestion: " + question + "\nResponse: " + r + "\nIntent: " + RequestObjectBP->GetStringField("intent") + "\n\n";

	if (FFileHelper::SaveStringToFile(SaveData, *FilePath)) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Operation successful"));
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Operation not successful"));
	}

	if (RequestObjectBP->GetStringField("intent") != "Walk To Scale") {	//I check this here so that the standard TTS and the helping TTS dont collide with each other and get 2 speeches at the same time
		Start_TTS(r);
		UseIntent(RequestObjectBP->GetStringField("intent"));
	}
	else if (RequestObjectBP->GetStringField("intent") == "Walk To Scale") {
		if (!(isHatOn && isShoesOn)) {
			Start_TTS(r);
			UseIntent(RequestObjectBP->GetStringField("intent"));
		}
		else {
			Start_TTS("I'm not quite ready to be measured yet, isn't there something I should do before that?");
		}
	}
}

void ACMetaHumans::UseIntent(const FString& intent) {
	// well i wanted to use a switch statement here to be consistent with BP but for some reason FString cant do that so heres a long if/else thingy
	if (intent == "Walk To Scale") {
		WalkScale();
	}
	else if (intent == "RotateRight") {
		RotatePatient(90);
	}
	else if (intent == "RotateLeft") {
		RotatePatient(-90);
	}
	else if (intent == "BackPost") {
		FaceAwayFromScale();
	}
	else if (intent == "RemoveHat") {
		RemoveHat();
	}
	else if (intent == "RemoveShoes") {
		RemoveShoes();
	}
	else if (intent == "StandStraight") {
		StandStraight();
	}
	else if (intent == "DeepBreath") {
		TakeBreath();
	}
	else if (intent == "RaiseHead") {
		RaiseHead();
	}
	else if (intent == "SpanishMode") {
		SwitchToSpanish();
	}
	else if (intent == "EnglishMode") {
		SwitchToEnglish();
	}
}

void ACMetaHumans::WalkScale() {
	FVector loc = scaleRef->GetActorLocation();
	loc += FVector(0, 20, 0);

	UNavigationPath* path = UNavigationSystemV1::FindPathToLocationSynchronously(this, this->GetActorLocation(), loc, this, NULL);
	if (path && path->IsValid()) {
		FAIMoveRequest req;
		req.SetAcceptanceRadius(1);
		req.SetUsePathfinding(true);
		
		if (AAIController* ai = Cast<AAIController>(this->GetController())) {
			ai->RequestMove(req, path->GetPath());
			//maybe instead of this use an inherited class from VRBaseCharacterMovementComponent and use Super() to find when OnMoveCompleted is called?
			//this->GetController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &ACMetaHumans::LocationReached);
			/*FPathFollowingResult thing;
			ai->OnMoveCompleted(ai->GetCurrentMoveRequestID(), thing);
			LookAtPlayer();*/

			startMove = true;
		}
	}

	if (scoreArr[5]) {
		AddPoint(1);
		scoreArr[5] = false;
	}
}

//void ACMetaHumans::LocationReached() {
//	LookAtPlayer();
//}

void ACMetaHumans::RotatePatient(float direction) {
	FRotator rot = this->GetActorRotation();
	rot += FRotator(0, direction, 0);

	this->SetActorRotation(rot);
}

void ACMetaHumans::FaceAwayFromScale() {
	//this is old code that i used to make the avatar look at the player, but they wanted it changed so that it faced away from scale now

//	FRotator rota = this->GetActorRotation();
//	FVector ploc = playerRef->GetActorLocation();
//	FVector mloc = this->GetActorLocation();
//
//	FRotator newrot = UKismetMathLibrary::FindLookAtRotation(mloc, ploc); //for some reason using this->GetActorLocation() doesnt work instead of mloc
//	/*
//	float x = rota.GetComponentForAxis(EAxis::X);
//	float y = rota.GetComponentForAxis(EAxis::Y);
//	float z = newrot.GetComponentForAxis(EAxis::Z);
//	*/
//	float x = rota.GetComponentForAxis(EAxis::X);
//	float y = rota.GetComponentForAxis(EAxis::Y);
//	float z = newrot.GetComponentForAxis(EAxis::Z) + 90; //the + 90 accounts for the offset that the character has in terms of rotation
//
//	this->SetActorRotation(FRotator(x, z, y)); //not sure why the variables have to be swapped here, but it only works in this configurations
//
//	if (scoreArr[0]) {
//		AddPoint(1);
//		scoreArr[0] = false;
//	}

	//new code for making the avatar have back to the post
	FRotator rota = this->GetActorRotation();

	FRotator scaleRot = scaleRef->GetActorRotation(); //we want the z rotation of this

	float x = rota.GetComponentForAxis(EAxis::X);
	float y = rota.GetComponentForAxis(EAxis::Y);
	float z = scaleRot.GetComponentForAxis(EAxis::Z);

	this->SetActorRotation(FRotator(x, z, y)); //not sure why the variables have to be swapped here, but it only works in this configurations

	backToPole = true;
	if (scoreArr[0]) {
		AddPoint(1);
		scoreArr[0] = false;
	}
	//backToPole = false;
	FTimerHandle backTimer; //timer is temp solution to finding when animation ends, might change later if needed
	GetWorld()->GetTimerManager().SetTimer(backTimer, [&]()
		{
			backToPole = false;
		}, 1, false);
}

void ACMetaHumans::RemoveHat() {
	isHatOn = false;
	removeHat = true;
	if (scoreArr[1]) {
		AddPoint(0.5);
		scoreArr[1] = false;
	}
	//removeHat = true;
	FTimerHandle hatTimer; //timer is temp solution to finding when animation ends, might change later if needed
	GetWorld()->GetTimerManager().SetTimer(hatTimer, [&]()
		{
			removeHat = false;
		}, 1, false);
	FTimerHandle hatVisiblityTimer;
	GetWorld()->GetTimerManager().SetTimer(hatVisiblityTimer, [&]()
		{
			cHat->SetVisibility(false);
		}, 1.58f, false);
}

void ACMetaHumans::RemoveShoes() {

	isShoesOn = false;
	removeShoes = true;
	if (scoreArr[2]) {
		AddPoint(0.5);
		scoreArr[2] = false;
	}
	//removeShoes = false;
	FTimerHandle shoeTimer; //timer is temp solution to finding when animation ends, might change later if needed
	GetWorld()->GetTimerManager().SetTimer(shoeTimer, [&]()
		{
			removeShoes = false;
		}, 1, false);
	FTimerHandle shoeVisibilityTimer;
	GetWorld()->GetTimerManager().SetTimer(shoeVisibilityTimer, [&]()
		{
			cFeet->HideBoneByName("foot_l", EPhysBodyOp::PBO_None);
		}, 1.595, false);
	FTimerHandle shoeVisibilityTimer2;
	GetWorld()->GetTimerManager().SetTimer(shoeVisibilityTimer2, [&]()
		{
			cFeet->HideBoneByName("foot_r", EPhysBodyOp::PBO_None);
		}, 2.6, false);
}

void ACMetaHumans::StandStraight() {
	standStill = true;
	if (scoreArr[4]) {
		AddPoint(1);
		scoreArr[4] = false;
	}
	//standStill = false;
	FTimerHandle standTimer; //timer is temp solution to finding when animation ends, might change later if needed
	GetWorld()->GetTimerManager().SetTimer(standTimer, [&]()
		{
			standStill = false;
		}, 1, false);
}

void ACMetaHumans::TakeBreath() {
	breathIn = true;
	if (scoreArr[3]) {
		AddPoint(1);
		scoreArr[3] = false;
	}
	//breathIn = false;
	FTimerHandle breathTimer; //timer is temp solution to finding when animation ends, might change later if needed
	GetWorld()->GetTimerManager().SetTimer(breathTimer, [&]()
		{
			breathIn= false;
		}, 1, false);
}

void ACMetaHumans::RaiseHead() {
	raiseHead = true;
	if (scoreArr[6]) {
		AddPoint(1);
		scoreArr[6] = false;
	}
	//raiseHead = false;
	FTimerHandle headTimer; //timer is temp solution to finding when animation ends, might change later if needed
	GetWorld()->GetTimerManager().SetTimer(headTimer, [&]()
		{
			raiseHead = false;
		}, 1, false);
}

void ACMetaHumans::SwitchToSpanish() {
	lang = "es-MX";
}

void ACMetaHumans::SwitchToEnglish() {
	lang = "en-US";
}