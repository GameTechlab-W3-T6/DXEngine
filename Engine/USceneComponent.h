#pragma once
#include "Matrix.h"
#include "UActorComponent.h"
#include "Vector.h"
#include "Quaternion.h"
#include "TArray.h"

/**
 * @brief Base component for objects with transform in 3D space
 */
class USceneComponent : public UActorComponent
{
	DECLARE_UCLASS(USceneComponent, UActorComponent)
public:
	FVector RelativeLocation;
	FVector RelativeScale3D;
	FQuaternion RelativeQuaternion;
	bool bIsSelected = false;
	bool markedAsDestroyed = false;

protected:
	USceneComponent* AttachParent = nullptr;
	TArray<USceneComponent*> AttachChildren;

public:
	USceneComponent(FVector pos = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: UActorComponent(), RelativeLocation(pos), //RelativeRotation(rot),
		RelativeScale3D(scl), RelativeQuaternion(FQuaternion::FromEulerXYZDeg(rot))
	{
		UUID = UEngineStatics::GenUUID();
	}

	virtual FMatrix GetWorldTransform() const;
	virtual FMatrix GetRelativeTransform() const;

	// Attachment functions
	void AttachToComponent(USceneComponent* Parent);
	void AttachChild(USceneComponent* Child);
	void DetachFromComponent();
	void DetachAllChildren();

	USceneComponent* GetAttachParent() const { return AttachParent; }
	const TArray<USceneComponent*>& GetAttachChildren() const { return AttachChildren; }

	// Hierarchy query functions
	bool IsAttachedTo(USceneComponent* Component) const;
	USceneComponent* GetRootComponent() const;
	void GetAllChildren(TArray<USceneComponent*>& OutChildren, bool bIncludeNestedChildren = true) const;

	// Transform functions for hierarchy
	FVector GetWorldLocation() const;
	FQuaternion GetWorldRotation() const;
	FVector GetWorldScale() const;
	void SetWorldLocation(const FVector& NewLocation);
	void SetWorldRotation(const FQuaternion& NewRotation);
	void SetWorldScale(const FVector& NewScale);

	virtual bool IsManageable() { return false; }

	// Lifecycle methods (using existing names)
	virtual bool Initialize() override;     // TODO: Rename to InitializeComponent() later
	virtual void Update(float deltaTime) override;  // TODO: Rename to TickComponent() later
	virtual void OnShutdown() override;     // TODO: Rename to EndPlay() later

	virtual void HandleInput(int32 keyCode);

	// TODO: Add these methods later for full Unreal-style lifecycle
	// virtual void BeginPlay();
	// virtual void EndPlay();

	// 위치와 스케일 설정 함수들
	void SetPosition(const FVector& pos) { RelativeLocation = pos; }
	void SetScale(const FVector& scl) { RelativeScale3D = scl; }
	void SetRotation(const FVector& rot) { RelativeQuaternion = FQuaternion::FromEulerXYZDeg(rot); }
	void AddQuaternion(const FVector& axis, const float deg, const bool isWorldAxis = false)
	{
		if (isWorldAxis)
		{
			RelativeQuaternion.RotateWorldAxisAngle(axis, deg);
		}
		else
		{
			RelativeQuaternion.RotateLocalAxisAngle(axis, deg);
		}
	}
	void SetQuaternion(const FQuaternion quat) { RelativeQuaternion = quat; }
	void ResetQuaternion()
	{
		RelativeQuaternion.X = 0;
		RelativeQuaternion.Y = 0;
		RelativeQuaternion.Z = 0;
		RelativeQuaternion.W = 1;
	}
	FVector GetPosition() const
	{
		return RelativeLocation;
	}
	FVector GetScale() const
	{
		return RelativeScale3D;
	}
	FVector GetRotation() const
	{
		return RelativeQuaternion.GetEulerXYZDeg();
	}
	FQuaternion GetQuaternion() const
	{
		return RelativeQuaternion;
	}

	json::JSON Serialize() const override;
	bool Deserialize(const json::JSON& data) override;
};