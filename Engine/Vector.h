#pragma once
#include "stdafx.h"

/**
 * @brief 2D vector structure for float2 support
 */
struct FVector2
{
    float X, Y;
};

/**
 * @brief 3D vector structure with mathematical operations
 */
struct FVector
{
    float X, Y, Z;

    FVector(float _x = 0, float _y = 0, float _z = 0) : X(_x), Y(_y), Z(_z) {}

    // ----- 산술 연산 -----
    FVector operator+(const FVector& o) const { return FVector(X + o.X, Y + o.Y, Z + o.Z); }
    FVector operator-(const FVector& o) const { return FVector(X - o.X, Y - o.Y, Z - o.Z); }
    FVector operator*(float s) const { return FVector(X * s, Y * s, Z * s); }
    FVector operator/(float s) const { return FVector(X / s, Y / s, Z / s); }

    FVector& operator+=(const FVector& o) { X += o.X; Y += o.Y; Z += o.Z; return *this; }
    FVector& operator-=(const FVector& o) { X -= o.X; Y -= o.Y; Z -= o.Z; return *this; }
    FVector& operator*=(float s) { X *= s; Y *= s; Z *= s; return *this; }
    FVector& operator/=(float s) { X /= s; Y /= s; Z /= s; return *this; }

    FVector operator-() const { return FVector(-X, -Y, -Z); }

    // ----- 기하 연산 -----
    float Dot(const FVector& o) const { return X * o.X + Y * o.Y + Z * o.Z; }

    // LH 좌표계 기준 
    FVector Cross(const FVector& o) const {
        return FVector(
            Y * o.Z - Z * o.Y,
            Z * o.X - X * o.Z,
            X * o.Y - Y * o.X
        );
    }

    float LengthSquared() const { return X * X + Y * Y + Z * Z; }
    float Length() const { return sqrtf(LengthSquared()); }

    void Normalize() {
        float ls = LengthSquared();
        if (ls > 0.0f) {
            float inv = 1.0f / sqrtf(ls);
            X *= inv; Y *= inv; Z *= inv;
        }
        else { X = Y = Z = 0.0f; }
    }

    FVector Normalized() const {
        float ls = LengthSquared();
        if (ls > 0.0f) {
            float inv = 1.0f / sqrtf(ls);
            return FVector(X * inv, Y * inv, Z * inv);
        }
        return FVector(0, 0, 0);
    }

    // 투영/리젝션 편의
    FVector ProjectOn(const FVector& n) const {
        float d = n.LengthSquared();
        if (d == 0.0f) return FVector();
        return n * (Dot(n) / d);
    }
    FVector RejectFrom(const FVector& n) const { return *this - ProjectOn(n); }

    friend inline FVector operator*(float s, const FVector& v) {
        // return v * s; 로 멤버 연산자 재사용해도 됨
        return FVector(s * v.X, s * v.Y, s * v.Z);
    }
};
