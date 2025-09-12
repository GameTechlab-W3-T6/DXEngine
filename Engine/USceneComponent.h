#pragma once
#include "Matrix.h"
#include "UObject.h"
#include "Vector.h"
#include "Quaternion.h"

class USceneComponent : public UObject
{
	DECLARE_UCLASS(USceneComponent, UObject)
public:
	FVector RelativeLocation;
	FVector RelativeScale3D;
	FQuaternion RelativeQuaternion;
	USceneComponent(FVector pos = { 0,0,0 }, FVector rot = { 0,0,0 }, FVector scl = { 1,1,1 })
		: RelativeLocation(pos), //RelativeRotation(rot), 
		RelativeScale3D(scl), RelativeQuaternion(FQuaternion::FromEulerXYZDeg(rot))
	{
	}

	virtual FMatrix GetWorldTransform();

	virtual bool IsManageable() { return false; }
	virtual void Update(float deltaTime) {}
	virtual void OnShutdown(){}

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