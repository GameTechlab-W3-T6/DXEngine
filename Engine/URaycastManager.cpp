#include "stdafx.h"
#include "URaycastManager.h"
#include "ImguiConsole.h"
#include "Vector.h"
#include "URenderer.h"
#include "UScene.h"


IMPLEMENT_UCLASS(URaycastManager, UEngineSubsystem)
URaycastManager::URaycastManager()
	: Renderer(nullptr),
	InputManager(nullptr),
	RayOrigin(FVector(0, 0, 0)),
	RayDirection(FVector(0, 0, 0)),
	MouseX(0),
	MouseY(0)
{
}

URaycastManager::URaycastManager(URenderer* renderer, UCamera* camera, UInputManager* inputManager)
	: Renderer(renderer),
	InputManager(inputManager),
	RayOrigin(FVector(0, 0, 0)),
	RayDirection(FVector(0, 0, 0)),
	MouseX(0),
	MouseY(0)
{
}

URaycastManager::~URaycastManager()
{
	Renderer = nullptr;
	InputManager = nullptr;
}

FVector URaycastManager::GetRaycastOrigin(UCamera* camera)
{
	return camera->GetLocation();
}

FVector URaycastManager::GetRaycastDirection(UCamera* camera)
{
	float CameraFOV = camera->GetFOV();

	// convert the mouse coords to Normalized Device Coordinates (NDC)
	int32 width = 0, height = 0;
	Renderer->GetBackBufferSize(width, height);
	float ndcX = (2.0f * MouseX) / static_cast<float>(width) - 1.0f;
	float ndcY = 1.0f - (2.0f * MouseY) / static_cast<float>(height);

	// get the camera-space ray direction
	// assume the camera is 1.0f units of distance away from the screen
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	FVector rayViewDir;
	rayViewDir.X = ndcX * tan(CameraFOV / 2.0f) * aspect;
	rayViewDir.Y = ndcY * tan(CameraFOV / 2.0f);
	rayViewDir.Z = -1.0f; // points forward in Camera space
	rayViewDir.Normalize();

	// convert the camera-space ray direction to world direction
	FMatrix V = FMatrix::LookAtLH(camera->GetLocation(), camera->GetLocation() + camera->GetForward(), camera->GetUp());
	V = FMatrix::Inverse(V);
	FVector4 rayDirection = FMatrix::MultiplyVector(V, FVector4(rayViewDir.X, rayViewDir.Y, rayViewDir.Z, 0.0f));

	return { rayDirection.X, rayDirection.Y, rayDirection.Z };
}

template <typename T>
bool URaycastManager::RayIntersectsMeshes(UCamera* camera, TArray<T*>& components, T*& hitComponent, FVector& outImpactPoint)
{
	MouseX = static_cast<float>(InputManager->GetMouseX());
	MouseY = static_cast<float>(InputManager->GetMouseY());

	FRay ray = CreateRayFromScreenPosition(camera);

	RayOrigin = ray.Origin;
	RayDirection = ray.Direction;

	// std::cout << "Ray Origin: (" 
	// 	  << RayOrigin.X << ", " << RayOrigin.Y << ", " << RayOrigin.Z 
	// 	  << ") | Ray Dir: (" 
	// 	  << RayDirection.X << ", " << RayDirection.Y << ", " << RayDirection.Z 
	// 	  << ")\n";

	bool hit = false;
	float closestHit = FLT_MAX;
	T* closestComponent = nullptr;
	 
	for (T* component : components)
	{
		UMesh* mesh = component->GetMesh();
		FMatrix worldTransform = component->GetWorldTransform();

		if (mesh->NumVertices < 3) continue;

		for (int32 i = 0; i + 2 < mesh->NumVertices; i += 3)
		{ 
			FVector triangleVertices[3];
			if (mesh->Vertices.size() > 0)
			{
				triangleVertices[0] = TransformVertexToWorld(mesh->Vertices[i], worldTransform);
				triangleVertices[1] = TransformVertexToWorld(mesh->Vertices[i + 1], worldTransform);
				triangleVertices[2] = TransformVertexToWorld(mesh->Vertices[i + 2], worldTransform);  
			}
			else
			{
				triangleVertices[0] = TransformVertexToWorld(mesh->Vertices2[i], worldTransform);
				triangleVertices[1] = TransformVertexToWorld(mesh->Vertices2[i + 1], worldTransform);
				triangleVertices[2] = TransformVertexToWorld(mesh->Vertices2[i + 2], worldTransform);
			}
			
			// std::cout << "Triangle: V0(" 
		 //  << triangleVertices[0].X << ", " << triangleVertices[0].Y << ", " << triangleVertices[0].Z 
		 //  << ") V1(" 
		 //  << triangleVertices[1].X << ", " << triangleVertices[1].Y << ", " << triangleVertices[1].Z 
		 //  << ") V2(" 
		 //  << triangleVertices[2].X << ", " << triangleVertices[2].Y << ", " << triangleVertices[2].Z 
		 //  << ")\n";

			auto result = RayIntersectsTriangle(triangleVertices);
			if (result.has_value())
			{
				float t = (*result - RayOrigin).Length(); // distance along ray
				if (t < closestHit)
				{  					
					closestHit = t;
					hit = true;
					closestComponent = component;
					outImpactPoint = *result; 

					// AABB 구하기 위한 min, max Pos 
					// 여기서는 model space이기 때문에 world space로 재계산 
					//MakeAABBInfo(mesh, outMinPos, outMaxPos);
					//outMinPos = TransformVertexToWorld(outMinPos, worldTransform);
					//outMaxPos = TransformVertexToWorld(outMaxPos, worldTransform); 
				}
			}
		}
	}
	if (hit)
	{
		hitComponent = closestComponent;
		  

	}
	return hit;
}

template bool URaycastManager::RayIntersectsMeshes<UGizmoComponent>(UCamera*, TArray<UGizmoComponent*>&, UGizmoComponent*&, FVector& );
template bool URaycastManager::RayIntersectsMeshes<UPrimitiveComponent>(UCamera*, TArray<UPrimitiveComponent*>&, UPrimitiveComponent*&, FVector&);


TOptional<FVector> URaycastManager::RayIntersectsTriangle(FVector triangleVertices[3])
{
	constexpr float epsilon = std::numeric_limits<float>::epsilon();

	FVector edge1 = triangleVertices[1] - triangleVertices[0];
	FVector edge2 = triangleVertices[2] - triangleVertices[0];
	FVector ray_cross_e2 = RayDirection.Cross(edge2);
	float det = edge1.Dot(ray_cross_e2);

	if (det > -epsilon && det < epsilon)
		return {};    // This ray is parallel to this triangle.

	float inv_det = 1.0f / det;
	FVector s = RayOrigin - triangleVertices[0];
	float u = inv_det * s.Dot(ray_cross_e2);

	if ((u < 0 && abs(u) > epsilon) || (u > 1 && abs(u - 1) > epsilon))
		return {};

	FVector s_cross_e1 = s.Cross(edge1);
	float v = inv_det * RayDirection.Dot(s_cross_e1);

	if ((v < 0 && abs(v) > epsilon) || (u + v > 1 && abs(u + v - 1) > epsilon))
		return {};

	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = inv_det * edge2.Dot(s_cross_e1);

	if (t > epsilon) // ray intersection
	{
		return FVector(RayOrigin + RayDirection * t);
	}
	else // This means that there is a line intersection but not a ray intersection.
		return {};
}

FVector URaycastManager::TransformVertexToWorld(const FVertexPosColor4& vertex, const FMatrix& world)
{
	FVector4 pos4(vertex.x, vertex.y, vertex.z, vertex.w);
	FVector4 worldPos4 = FMatrix::MultiplyVectorRow(pos4, world);
	return FVector(worldPos4.X, worldPos4.Y, worldPos4.Z);
}

FVector URaycastManager::TransformVertexToWorld(const FVertexPosUV4& vertex, const FMatrix& world)
{
	FVector4 pos4(vertex.x, vertex.y, vertex.z, vertex.w);
	FVector4 worldPos4 = FMatrix::MultiplyVectorRow(pos4, world);
	return FVector(worldPos4.X, worldPos4.Y, worldPos4.Z);
}

FVector URaycastManager::TransformVertexToWorld(const FVector& vertex, const FMatrix& world)
{
	FVector4 pos4(vertex.X, vertex.Y, vertex.Z, 1.0f);
	FVector4 worldPos4 = FMatrix::MultiplyVectorRow(pos4, world);
	return FVector(worldPos4.X, worldPos4.Y, worldPos4.Z);
}

// bool RaycastManager::RayIntersectsSphere(FVector& rayOrigin, FVector& rayDirection, USphereComp& sphere, float& tHit)
// {
//     // R(t) = O + tD
//     // where O = rayOrigin, D = rayDirection
//     // C = intersection of circle
//     
//     // to check intersection:
//     // || R(t) - C ||^2 = r^2
//     // because || R(t) - C || is the distance from point to center. squared for easier calculations
//     // || O + tD - C ||^2 = r^2
//     // Let L = O - C
//     // || L + tD ||^2 = r^2
//     // (L + tD) . (L + tD) = r^2
//     // t^2 (D . D) + 2(D . L)t + (L . L - r^2) = 0
//     // note D . D = 1 because D is normalized
//     // use the quadratic equation
//     // t = - (D . L) +- sqrt((D . L)^2 - (L . L - r^2))
//
//     FVector L = rayOrigin - sphere.GetPosition();
//     float b = L.Dot(rayDirection);
//     float c = L.Dot(L) - sphere.GetRadius() * sphere.GetRadius();
//
//     float determinant = b * b - c;
//     if (determinant < 0.0f) return false;
//
//     tHit = - b - sqrtf(determinant);
//     // may be negative, which will be the intersection behind the ray origin.
//     // doesn't matter for now
//     return true;
// }

// 현재 클릭한 마우스 위치와 카메라를 기준으로 ray 생성
FRay URaycastManager::CreateRayFromScreenPosition(UCamera* camera)
{
	MouseX = static_cast<float>(InputManager->GetMouseX());
	MouseY = static_cast<float>(InputManager->GetMouseY());

	int32 viewportWidth = 0, viewportHeight = 0;
	Renderer->GetBackBufferSize(viewportWidth, viewportHeight);

	// 1단계: Screen -> NDC
	FVector ndcPos;
	ndcPos.X = (MouseX / viewportWidth) * 2.0f - 1.0f;
	ndcPos.Y = 1.0f - (MouseY / viewportHeight) * 2.0f;
	ndcPos.Z = 0.0f; // Near Plane

	FMatrix invProjectionMatrix = FMatrix::Inverse(camera->GetProj());
	FVector viewPos = invProjectionMatrix.TransformPointRow(ndcPos);

	// 3단계: View -> World
	FMatrix invViewMatrix = FMatrix::Inverse(camera->GetView());
	FVector worldPos = invViewMatrix.TransformPointRow(viewPos);

	if (camera->IsOrtho())
	{
		FRay resultRay;
		resultRay.Origin = worldPos;
		resultRay.Direction = camera->GetForward();
		return resultRay;
	}

	// 4단계: 방향 벡터 계산
	FRay resultRay;
	resultRay.Origin = camera->GetLocation(); // 카메라의 월드 위치
	resultRay.Direction = (worldPos - resultRay.Origin).Normalized();
	resultRay.MousePos = { MouseX, MouseY };
	//UE_LOG("%f %f %f / %f %f %f", resultRay.Origin.X, resultRay.Origin.Y, resultRay.Origin.Z, resultRay.Direction.X, resultRay.Direction.Y, resultRay.Direction.Z);

	return resultRay;
}

bool URaycastManager::MakeAABBInfo(UMesh* mesh, FVector& outMin, FVector& outMax)
{
	if (mesh->Vertices.size() == 0) return false;

	float maxPos[3] = { INT_MIN, INT_MIN, INT_MIN };
	float minPos[3] = { INT_MAX, INT_MAX,INT_MAX };

	auto UpdateMinMax = [&](const FVector& p)
		{
			minPos[0] = (p.X < minPos[0]) ? p.X : minPos[0];
			minPos[1] = (p.Y < minPos[1]) ? p.Y : minPos[1];
			minPos[2] = (p.Z < minPos[2]) ? p.Z : minPos[2];

			// max 갱신: 더 크면 교체
			maxPos[0] = (p.X > maxPos[0]) ? p.X : maxPos[0];
			maxPos[1] = (p.Y > maxPos[1]) ? p.Y : maxPos[1];
			maxPos[2] = (p.Z > maxPos[2]) ? p.Z : maxPos[2];
		};

	for (auto v : mesh->Vertices)
	{
		UpdateMinMax({v.x, v.y, v.z});
	}
	outMin.X = minPos[0];
	outMin.Y = minPos[1];
	outMin.Z = minPos[2];

	outMax.X = maxPos[0];
	outMax.Y = maxPos[1];
	outMax.Z = maxPos[2];   
}


void URaycastManager::ComputeWorldAABB_BruteForce(const FMatrix& M, const FVector& localMin, const FVector& localMax, FVector& worldMin, FVector& worldMax)
{
	FVector corners[8] = {
		{localMin.X, localMin.Y, localMin.Z},
		{localMax.X, localMin.Y, localMin.Z},
		{localMin.X, localMax.Y, localMin.Z},
		{localMax.X, localMax.Y, localMin.Z},
		{localMin.X, localMin.Y, localMax.Z},
		{localMax.X, localMin.Y, localMax.Z},
		{localMin.X, localMax.Y, localMax.Z},
		{localMax.X, localMax.Y, localMax.Z}
	};

	worldMin = { FLT_MAX,  FLT_MAX,  FLT_MAX };
	worldMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (int i = 0; i < 8; ++i)
	{ 
		FVector w = TransformVertexToWorld(corners[i], M); // 반드시 Point 변환(w=1)이어야 함
		worldMin.X = min(worldMin.X, w.X);
		worldMin.Y = min(worldMin.Y, w.Y);
		worldMin.Z = min(worldMin.Z, w.Z);
		worldMax.X = max(worldMax.X, w.X);
		worldMax.Y = max(worldMax.Y, w.Y);
		worldMax.Z = max(worldMax.Z, w.Z);
	} 
}