
#include "BulletDebugDraw.hpp"
#include "SM/DebugDrawer.hpp"
#include "SM/Console.hpp"

using namespace SM;

constexpr uint64 MaxContactLifetimeMs = 2000;
constexpr Vec3 ContactCountLimitColor = {1.0f, 0.0f, 0.0f};

// btVector3 doesn't have a comparison operator, but implicitly converts
// itself to float and compares only the X value (wtf)
inline static bool GT(const btVector3& a, const btVector3& b) {
	return Vec3CMP(a, >= , b);
}
inline static bool LT(const btVector3& a, const btVector3& b) {
	return Vec3CMP(a, <=, b);
}

void BulletDebugDraw::drawLine(const btVector3& from, const btVector3& to, const btVector3& color) {
	if ( GT(from, m_vCullingBoxMin) && LT(from, m_vCullingBoxMax) )
		m_vecLines.emplace_back(ToGLM(from), ToGLM(to), ToGLM(color * 255.0f));
};

void BulletDebugDraw::drawContactPoint(
	const btVector3& PointOnB,
	const btVector3& normalOnB,
	btScalar distance,
	int lifeTime,
	const btVector3& color
) {
	++m_totalContacts;
	if ( GT(PointOnB, m_vCullingBoxMin) && LT(PointOnB, m_vCullingBoxMax) ) {
		++m_renderContacts;
		m_vecLines.emplace_back(ToGLM(PointOnB), ToGLM(PointOnB + (normalOnB * max(distance, 0.05f))), ToGLM(color * 255.0f));
	}
};

void BulletDebugDraw::reportErrorWarning(const char* warningString) {
	SM_WARN("Bullet3: {}", warningString);
};

void BulletDebugDraw::resetTickData() {
	m_totalContacts = 0;
	m_renderContacts = 0;
	m_vecLines.clear();
}

void BulletDebugDraw::render() {
	DebugDrawer* pDrawer = DebugDrawer::Get();
	std::scoped_lock lock(pDrawer->getLock());
	for ( const Line& line : m_vecLines )
		pDrawer->drawLine(line.begin, line.end, line.color);
}
