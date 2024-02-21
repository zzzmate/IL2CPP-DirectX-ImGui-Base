#include "../../Utils.h"

#ifndef VECTOR2_H
#define VECTOR2_H

class Vector2 {
public:
    float x, y;

    // Constructors
    Vector2();
    Vector2(float x, float y);

    // Basic arithmetic operations
    Vector2 operator+(const Vector2& other) const;
    Vector2 operator-(const Vector2& other) const;
    Vector2 operator*(float scalar) const;
    Vector2 operator/(float scalar) const;

    // Comparison operators
    bool operator==(const Vector2& other) const;
    bool operator!=(const Vector2& other) const;

    // Vector operations
    float dot(const Vector2& other) const;
    float magnitude() const;
    Vector2 normalized() const;

    // Utility functions
    static Vector2 zero();
    static Vector2 one();

    // Additional functions (example: clamp)
    static Vector2 clamp(const Vector2& value, const Vector2& min, const Vector2& max);
};

// Inline function implementations
inline Vector2::Vector2() : x(0.0f), y(0.0f) {}
inline Vector2::Vector2(float x, float y) : x(x), y(y) {}

inline Vector2 Vector2::operator+(const Vector2& other) const {
    return Vector2(x + other.x, y + other.y);
}

inline Vector2 Vector2::operator-(const Vector2& other) const {
    return Vector2(x - other.x, y - other.y);
}

inline Vector2 Vector2::operator*(float scalar) const {
    return Vector2(x * scalar, y * scalar);
}

inline Vector2 Vector2::operator/(float scalar) const {
    return Vector2(x / scalar, y / scalar);
}

inline bool Vector2::operator==(const Vector2& other) const {
    return (x == other.x) && (y == other.y);
}

inline bool Vector2::operator!=(const Vector2& other) const {
    return !(*this == other);
}

inline float Vector2::dot(const Vector2& other) const {
    return x * other.x + y * other.y;
}

inline float Vector2::magnitude() const {
    return sqrt(x * x + y * y);
}

inline Vector2 Vector2::normalized() const {
    float mag = magnitude();
    if (mag != 0.0f) {
        return *this / mag;
    }
    return *this;
}

inline Vector2 Vector2::zero() {
    return Vector2(0.0f, 0.0f);
}

inline Vector2 Vector2::one() {
    return Vector2(1.0f, 1.0f);
}

inline Vector2 Vector2::clamp(const Vector2& value, const Vector2& min, const Vector2& max) {
    return Vector2(
        std::clamp(value.x, min.x, max.x),
        std::clamp(value.y, min.y, max.y)
    );
}

#endif // VECTOR2_H