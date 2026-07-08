#pragma once

#include "CoreMinimal.h"
#include "ExpressiveTextActor.h"
#include "ExpressiveTextActorShadowed.generated.h"

UCLASS()
class EXPRESSIVETEXT_API AExpressiveTextActorShadowed : public AExpressiveTextActor
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaTime) override;
	
#if WITH_EDITOR
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

protected:
	/** Offset of the shadow from the text to give finer positioning control **/
	UPROPERTY(BlueprintReadWrite, BlueprintSetter = SetShadowOffset, EditAnywhere, Category = "ExpressiveText")
	FVector ShadowOffset = FVector(0.f, 0.f, 0.f);
	
	UFUNCTION(BlueprintSetter)
	void SetShadowOffset(const FVector& NewOffset);

	/** Shadow invalidation behavior: keep on Always if moving/animating/altering text to prevent VSM shadow artifacts. Can be set back to auto/stricter for static text. **/
	UPROPERTY(BlueprintReadWrite, BlueprintSetter = SetShadowCacheInvalidationBehavior, EditAnywhere, Category = "ExpressiveText")
	EShadowCacheInvalidationBehavior ShadowCacheInvalidationBehavior = EShadowCacheInvalidationBehavior::Always;
	
	UFUNCTION(BlueprintSetter)
	void SetShadowCacheInvalidationBehavior(EShadowCacheInvalidationBehavior NewBehavior);

private:
	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UStaticMeshComponent> ShadowProxy;

	UPROPERTY(Transient, DuplicateTransient)
	TObjectPtr<UMaterialInstanceDynamic> ShadowMID;

	void UpdateShadowTexture() const;
	
	void UpdateShadowScale() const;

	void SetupShadowProxy();
};
