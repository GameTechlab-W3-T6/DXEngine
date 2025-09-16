#pragma once
#include "FVertexPosColor.h"
#include "TArray.h"

// LH Z-up 좌표계 기준 XY 평면 정사각형 (CW 와인딩)
//inline TArray<FVertexPosColor> plane_vertices = {
//	// 첫 번째 삼각형 (LH CW: 좌상 -> 좌하 -> 우상)
//	{ -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f}, // 좌상 - Red
//	{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}, // 좌하 - Blue
//	{  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f}, // 우상 - Green
//
//	// 두 번째 삼각형 (LH CW: 우상 -> 좌하 -> 우하)
//	{  0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f}, // 우상 - Green
//	{ -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f}, // 좌하 - Blue
//	{  0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f}  // 우하 - Yellow
//};

inline TArray<FVertexPosColorUV> plane_vertices = {
	// 첫 번째 삼각형 (CW: 좌상 -> 우상 -> 좌하)
	{ -0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f/*1.0f, 0.0f, 0.0f, 1.0f */}, // 좌상 - Red
	{  0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f/*0.0f, 1.0f, 0.0f, 1.0f */}, // 우상 - Green
	{ -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f/*0.0f, 0.0f, 1.0f, 1.0f */}, // 좌하 - Blue

	// 두 번째 삼각형 (CW: 우상 -> 우하 -> 좌하)
	{  0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f/*0.0f, 1.0f, 0.0f, 1.0f*/}, // 우상 - Green
	{  0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f/*1.0f, 1.0f, 0.0f, 1.0f*/}, // 우하 - Yellow
	{ -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f/*0.0f, 0.0f, 1.0f, 1.0f*/}  // 좌하 - Blue
};