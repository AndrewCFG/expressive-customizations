#include "Actors/ExpressiveTextActorShadowed.h"

#include "Engine/TextureRenderTarget2D.h"

void AExpressiveTextActorShadowed::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Perform a one time setup of the shadow proxy once the render target is available
	if (!ShadowProxy && ExpressiveTextComponent && ExpressiveTextComponent->GetRenderTarget())
	{
		SetupShadowProxy();
		SetActorTickEnabled(false);
	}

#if WITH_EDITOR
	// Editing the actor instance ExprText Component ends up re-creating it; this keeps the shadow in sync in the editor
	// We also need to re-enable tick in-editor for the parent class to reflect changes to the widget
	if (const UWorld* World = GetWorld())
	{
		if (World->WorldType == EWorldType::Editor && ShadowProxy)
		{
			SetActorTickEnabled(true);
			UpdateShadowTexture();
		}
	}
#endif
}

void AExpressiveTextActorShadowed::SetShadowOffset(const FVector& NewOffset)
{
	ShadowOffset = NewOffset;
	if (ShadowProxy)
	{
		ShadowProxy->SetRelativeLocation(ShadowOffset);
	}
}

void AExpressiveTextActorShadowed::SetShadowCacheInvalidationBehavior(const EShadowCacheInvalidationBehavior NewBehavior)
{
	ShadowCacheInvalidationBehavior = NewBehavior;
	if (ShadowProxy)
	{
		ShadowProxy->ShadowCacheInvalidationBehavior = ShadowCacheInvalidationBehavior;
	}
}

void AExpressiveTextActorShadowed::SetupShadowProxy()
{
	if (UStaticMesh* PlaneMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane")))
	{
		if (UMaterialInterface* WidgetMatInterface = LoadObject<UMaterialInterface>(
			nullptr, TEXT(
				"/ExpressiveText/Core/Materials/ActorWidgetMaterials/MAT_WidgetShadowProxy.MAT_WidgetShadowProxy")))
		{
			ShadowProxy = NewObject<UStaticMeshComponent>(this, TEXT("ShadowProxy"),RF_Transient | RF_DuplicateTransient);
			ShadowProxy->SetStaticMesh(PlaneMesh);

			// Ensure correct orientation to match the underlying widget
			ShadowProxy->SetRelativeRotation(FRotator(0.f, -90.f, 90.f));
			ShadowProxy->SetRelativeLocation(ShadowOffset);

			// Scale to the same size as the Expressive Text Component
			UpdateShadowScale();

			// Invisible but casts shadows
			ShadowProxy->SetVisibility(false);
			ShadowProxy->CastShadow = true;
			ShadowProxy->bCastHiddenShadow = true;
			ShadowProxy->ShadowCacheInvalidationBehavior = ShadowCacheInvalidationBehavior;
			ShadowProxy->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			ShadowMID = ShadowProxy->CreateDynamicMaterialInstance(0, WidgetMatInterface);
			UpdateShadowTexture();

			ShadowProxy->RegisterComponent();
			ShadowProxy->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

void AExpressiveTextActorShadowed::UpdateShadowTexture() const
{
	ShadowMID->SetTextureParameterValue(TEXT("RenderTarget"), ExpressiveTextComponent->GetRenderTarget());
}

void AExpressiveTextActorShadowed::UpdateShadowScale() const
{
	const FIntPoint Resolution = ExpressiveTextComponent->Resolution;
	ShadowProxy->SetRelativeScale3D(FVector(Resolution.X / 100.f, Resolution.Y / 100.f, 1.0f));
}

#if WITH_EDITOR
void AExpressiveTextActorShadowed::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	
	// Ensure visual update in editor when properties change
	for (auto* Node = PropertyChangedEvent.PropertyChain.GetHead(); Node; Node = Node->GetNextNode())
	{
		const FName PropertyName = Node->GetValue()->GetFName();
		if (PropertyName == GET_MEMBER_NAME_CHECKED(AExpressiveTextActorShadowed, ShadowCacheInvalidationBehavior))
		{
			SetShadowCacheInvalidationBehavior(ShadowCacheInvalidationBehavior);
			return;
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(AExpressiveTextActorShadowed, ShadowOffset))
		{
			SetShadowOffset(ShadowOffset);
			return;
		}
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UExpressiveTextComponent, Resolution))
		{
			UpdateShadowScale();
			return;
		}
	}
}
#endif
