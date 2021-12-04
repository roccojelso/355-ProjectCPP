// Fill out your copyright notice in the Description page of Project Settings. 

#include "Door.h" 

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	TheRoot = CreateDefaultSubobject<USceneComponent>(FName("TheRoot"));

	RootComponent = TheRoot;


	TheHinge = CreateDefaultSubobject<USceneComponent>(FName("TheHinge"));
	TheHinge->SetupAttachment(TheRoot);

	Collider = CreateDefaultSubobject<UBoxComponent>(FName("box collider"));
	Collider->SetupAttachment(TheRoot);

	TheMeshDoor = CreateDefaultSubobject<UStaticMeshComponent>(FName("TheMeshDoor"));
	TheMeshDoor->SetupAttachment(TheHinge);

	TheMeshFrame = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("TheMeshFrame"));
	TheMeshFrame->SetupAttachment(TheRoot);


	ConstructorHelpers::FObjectFinder<UStaticMesh> TheCubeMesh(TEXT("/Game/Art/Meshes/1M_Cube"));

	if (TheCubeMesh.Object)
	{
		TheMeshDoor->SetStaticMesh(TheCubeMesh.Object);
		TheMeshFrame->SetStaticMesh(TheCubeMesh.Object);
	}

	DoorAnim = CreateDefaultSubobject<UTimelineComponent>(FName("Door open timeline"));

}
void ADoor::OnAnimUpdate(float val)
{
	FRotator rot(0, val * 90, 0);

	if (IsDoorFlipped) rot.Yaw *= -1;

	TheHinge->SetRelativeRotation(rot);
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	if (doorOpenCurve)
	{

		FOnTimelineFloat eventHandler;
		eventHandler.BindUFunction(this, TEXT("OnAnimUpdate"));
		//eventHandler.BindDynamic(this, &ADoor::OnAnimUpdate);
		DoorAnim->AddInterpFloat(doorOpenCurve, eventHandler, TEXT("Handle curve func"));
		DoorAnim->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame); // set to use last keyframe as time of timeline
	}
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADoor::OnConstruction(const FTransform & xform)
{
	Super::OnConstruction(xform);

	TheMeshDoor->SetWorldScale3D(FVector(WidthOfDoor, DepthOfDoor, HeightOfDoor) / 100);

	TheMeshDoor->SetRelativeLocation(FVector(WidthOfDoor / 2, 0, HeightOfDoor / 2));

	TheHinge->SetRelativeLocation(FVector(-WidthOfDoor / 2, 0, 0));

	TheMeshFrame->ClearInstances();

	TheMeshFrame->AddInstance(FTransform(FRotator(0, 0, 0), FVector(WidthOfDoor / 2 + DepthOfDoor / 2, 0, HeightOfDoor / 2), FVector(DepthOfDoor, DepthOfDoor, HeightOfDoor) / 100));
	TheMeshFrame->AddInstance(FTransform(FRotator(0, 0, 0), FVector(-WidthOfDoor / 2 + -DepthOfDoor / 2, 0, HeightOfDoor / 2), FVector(DepthOfDoor, DepthOfDoor, HeightOfDoor) / 100));


	FVector doorSize = FVector(WidthOfDoor, DepthOfDoor, HeightOfDoor) / 2;

	doorSize.X += 50;
	doorSize.Y += 50;

	Collider->SetBoxExtent(doorSize);
	Collider->SetRelativeLocation(FVector(0, 0, HeightOfDoor / 2));
	//Collider->collision
}

void ADoor::Interact()
{
	if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Green, "Player interacted with door");



	UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	ACharacter* player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	FVector vecToDoor = GetActorLocation() - player->GetActorLocation();
	vecToDoor.Normalize();

	GetActorRightVector();

	float align = FVector::DotProduct(vecToDoor, GetActorRightVector());

	IsDoorFlipped = (align < 0);

	DoorAnim->Play();
}



