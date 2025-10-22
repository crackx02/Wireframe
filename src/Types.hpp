#pragma once

#include <cstdint>

#include "glm/glm.hpp"
#include "LinearMath/btVector3.h"

#include "Util.hpp"

using Vec3 = glm::vec3;
using Vec2 = glm::vec2;
using u8Vec3 = glm::u8vec3;
using u8Vec4 = glm::u8vec4;
using i32Vec3 = glm::i32vec3;
using i32Vec2 = glm::i32vec2;

using int8 = int8_t;
using uint8 = uint8_t;
using int16 = int16_t;
using uint16 = uint16_t;
using int32 = int32_t;
using uint32 = uint32_t;
using int64= int64_t;
using uint64 = uint64_t;
using uintptr = uintptr_t;

inline static constexpr Vec3 ToGLM(const btVector3& vec) {
	return {vec.x(), vec.y(), vec.z()};
};

inline static btVector3 ToBT(const Vec3& vec) {
	return {vec.x, vec.y, vec.z};
};

// btVector3 doesn't have a comparison operator, but implicitly converts
// itself to float and compares only the X value (wtf)
inline static bool GT(const btVector3& a, const btVector3& b) {
	return Vec3CMP(a, >= , b);
}
inline static bool LT(const btVector3& a, const btVector3& b) {
	return Vec3CMP(a, <= , b);
}
