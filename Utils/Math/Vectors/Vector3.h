#pragma once

#include "../../Utils.h"

#ifndef VECTOR3_H
#define VECTOR3_H

struct Vector3
{
    float x;
    float y;
    float z;

public:
    inline Vector3(void)
    {
        x = 0;
        y = 0;
        z = 0;
    }
    inline Vector3(float val)
    {
        x = val;
        y = val;
        z = val;
    }
    inline Vector3(float X, float Y, float Z)
    {
        x = X;
        y = Y;
        z = Z;
    }
    inline Vector3(const float* clr)
    {
        x = clr[0];
        y = clr[1];
        z = clr[2];
    }

    inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f)
    {
        x = ix; y = iy; z = iz;
    }
    inline bool IsValid() const
    {
        return true;
    }

    inline float& operator[](int i)
    {
        return ((float*)this)[i];
    }
    inline float operator[](int i) const
    {
        return ((float*)this)[i];
    }

    inline Vector3 Zero()
    {
        return Vector3(0.0f, 0.0f, 0.0f);
    }

    inline bool IsZero()
    {
        return (x == 0 && y == 0 && z == 0);
    }

    inline bool operator==(const Vector3& src) const
    {
        return (src.x == x) && (src.y == y) && (src.z == z);
    }
    inline bool operator!=(const Vector3& src) const
    {
        return (src.x != x) || (src.y != y) || (src.z != z);
    }
    inline float Distance(const Vector3& vOther) const
    {
        return sqrt(
            (x - vOther.x) * (x - vOther.x) +
            (y - vOther.y) * (y - vOther.y) +
            (z - vOther.z) * (z - vOther.z));
    }
    inline Vector3& operator+=(const Vector3& v)
    {
        x += v.x; y += v.y; z += v.z;
        return *this;
    }
    inline Vector3& operator-=(const Vector3& v)
    {
        x -= v.x; y -= v.y; z -= v.z;
        return *this;
    }
    inline Vector3& operator*=(float fl)
    {
        x *= fl;
        y *= fl;
        z *= fl;
        return *this;
    }
    inline Vector3& operator*=(const Vector3& v)
    {
        x *= v.x;
        y *= v.y;
        z *= v.z;
        return *this;
    }
    inline Vector3& operator/=(const Vector3& v)
    {
        x /= v.x;
        y /= v.y;
        z /= v.z;
        return *this;
    }
    inline Vector3& operator+=(float fl)
    {
        x += fl;
        y += fl;
        z += fl;
        return *this;
    }
    inline Vector3& operator/=(float fl)
    {
        x /= fl;
        y /= fl;
        z /= fl;
        return *this;
    }
    inline Vector3& operator-=(float fl)
    {
        x -= fl;
        y -= fl;
        z -= fl;
        return *this;
    }

    inline void Clamp()
    {
        if (this->x > 180.f)
            this->x -= 360.f;

        else if (this->x < -180.f)
            this->x += 360.f;

        if (this->z > 180.f)
            this->z -= 360.f;

        else if (this->z < -180.f)
            this->z += 360.f;

        if (this->x < -89.f)
            this->x = -89.f;

        if (this->x > 89.f)
            this->x = 89.f;

        while (this->z < -180.0f)
            this->z += 360.0f;

        while (this->z > 180.0f)
            this->z -= 360.0f;
    }

    inline void NormalizeInPlace()
    {
        *this = Normalized();
    }
    inline Vector3 Normalized() const
    {
        Vector3 res = *this;
        float l = res.Length();
        if (l != 0.0f) {
            res /= l;
        }
        else {
            res.x = res.y = res.z = 0.0f;
        }
        return res;
    }

    inline float DistTo(const Vector3& vOther) const
    {
        Vector3 delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.Length();
    }
    inline float DistToSqr(const Vector3& vOther) const
    {
        Vector3 delta;

        delta.x = x - vOther.x;
        delta.y = y - vOther.y;
        delta.z = z - vOther.z;

        return delta.LengthSqr();
    }
    inline float Dot(const Vector3& vOther) const
    {
        return (x * vOther.x + y * vOther.y + z * vOther.z);
    }
    inline float Length() const
    {
        return sqrt(x * x + y * y + z * z);
    }
    inline float LengthSqr(void) const
    {
        return (x * x + y * y + z * z);
    }
    inline float Length2D() const
    {
        return sqrt(x * x + y * y);
    }
    inline float deltaDistance(Vector3 target_position) const
    {
        return sqrt(
            pow(x - target_position.x, 2) +
            pow(y - target_position.y, 2) +
            pow(z - target_position.z, 2));
    }

    inline Vector3& operator=(const Vector3& vOther)
    {
        x = vOther.x; y = vOther.y; z = vOther.z;
        return *this;
    }

    inline Vector3 operator-(void) const
    {
        return Vector3(-x, -y, -z);
    }
    inline Vector3 operator+(const Vector3& v) const
    {
        return Vector3(x + v.x, y + v.y, z + v.z);
    }
    inline Vector3 operator-(const Vector3& v) const
    {
        return Vector3(x - v.x, y - v.y, z - v.z);
    }
    inline Vector3 operator*(float fl) const
    {
        return Vector3(x * fl, y * fl, z * fl);
    }
    inline Vector3 operator*(const Vector3& v) const
    {
        return Vector3(x * v.x, y * v.y, z * v.z);
    }
    inline Vector3 operator/(float fl) const
    {
        return Vector3(x / fl, y / fl, z / fl);
    }
    inline Vector3 operator/(const Vector3& v) const
    {
        return Vector3(x / v.x, y / v.y, z / v.z);
    }
    inline Vector3 Normalize() const
    {
        Vector3 normalized;
        float length = this->Length();

        if (length != 0) {
            normalized.x = x / length;
            normalized.y = y / length;
            normalized.z = z / length;
        }
        else {
            normalized.x = normalized.y = 0.0f; normalized.z = 1.0f;
        }

        return normalized;
    }
    inline float Normalizes() const
    {
        Vector3 res = *this;
        float length = res.Length();
        if (length != 0.0f) {
            res /= length;
        }
        else {
            res.x = res.y = res.z = 0.0f;
        }
        return length;
    }
};

inline Vector3 operator*(float lhs, const Vector3& rhs)
{
    return rhs * lhs;
}

inline Vector3 operator/(float lhs, const Vector3& rhs)
{
    return rhs / lhs;
}

class __declspec(align(16)) Vector3Aligned : public Vector3
{
public:
    inline Vector3Aligned(void) {};
    inline Vector3Aligned(float X, float Y, float Z)
    {
        Init(X, Y, Z);
    }

public:
    inline explicit Vector3Aligned(const Vector3& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
    }

    inline Vector3Aligned& operator=(const Vector3& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    inline Vector3Aligned& operator=(const Vector3Aligned& vOther)
    {
        Init(vOther.x, vOther.y, vOther.z);
        return *this;
    }

    float w;
};

struct Vector4
{
public:
    float x;
    float y;
    float z;
    float w;
    Vector4 Zero();
};

struct Matrix3x4
{
    float m[3][4];
};

struct Matrix4x3
{
    float m[4][3];
};

struct Matrix4x4
{
    union { Vector4 v[4]; float m[4][4]; struct { Vector4 right; Vector4 up; Vector4 forward; Vector4 trans; }; };
};

#endif