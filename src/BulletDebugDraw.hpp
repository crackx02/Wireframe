#pragma once

#include <vector>
#include <mutex>

#include "LinearMath/btIDebugDraw.h"

#include "Types.hpp"

class BulletDebugDraw : public btIDebugDraw {
	public:
		BulletDebugDraw() {m_updateDebugMode();}

		void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;

		void drawContactPoint(
			const btVector3& PointOnB,
			const btVector3& normalOnB,
			btScalar distance,
			int lifeTime,
			const btVector3& color
		) override;

		void reportErrorWarning(const char* warningString) override;

		void draw3dText(const btVector3& location, const char* textString) override {};

		void setDebugMode(int debugMode) override {
			m_debugMode = debugMode;
		};

		int getDebugMode() const override {
			return m_debugMode;
		};

		inline void setWireframe(bool state) {m_bDrawWireframe = state; m_updateDebugMode();};
		inline void setAABB(bool state) {m_bDrawAABB = state; m_updateDebugMode();};
		inline void setContactPoints(bool state) {m_bDrawContactPoints = state; m_updateDebugMode();};
		inline void setConstraints(bool state) {m_bDrawConstraints = state; m_updateDebugMode();};
		inline void setConstraintLimits(bool state) {m_bDrawConstraintLimits = state; m_updateDebugMode();};
		inline void setNormals(bool state) {m_bDrawNormals = state; m_updateDebugMode();};
		inline void setFrames(bool state) {m_bDrawFrames = state; m_updateDebugMode();};
		inline void setCapsules(bool state) {m_bDrawCapsules = state;};

		inline bool getWireframe() const {return m_bDrawWireframe;};
		inline bool getAABB() const {return m_bDrawAABB;};
		inline bool getContactPoints() const {return m_bDrawContactPoints;};
		inline bool getConstraints() const {return m_bDrawConstraints;};
		inline bool getConstraintLimits() const {return m_bDrawConstraintLimits;};
		inline bool getNormals() const {return m_bDrawNormals;};
		inline bool getFrames() const {return m_bDrawFrames;};
		inline bool getCapsules() const {return m_bDrawCapsules;};

		inline void enableAll() {
			for ( bool* p = &m_bDrawWireframe; p <= &m_bEnableAll; ++p )
				*p = true;
			m_updateDebugMode();
		}

		inline void disableAll() {
			for ( bool* p = &m_bDrawWireframe; p <= &m_bEnableAll; ++p )
				*p = false;
			m_updateDebugMode();
		}

		inline bool getEnableAll() const {return m_bEnableAll;}

		inline void setCullingBoxPosition(const btVector3& pos) {m_vCullingBoxPosition = pos; m_updateCullingBox();};
		inline void setCullingBoxRadius(float radius) {m_vCullingBoxRadius = {radius, radius, radius}; m_updateCullingBox();}
		inline void getCullingBox(btVector3& min, btVector3& max) const {min = m_vCullingBoxMin; max = m_vCullingBoxMax;};

		inline uint32 getTotalContacts() const {return m_totalContacts;};
		inline uint32 getRenderContacts() const { return m_renderContacts; };

		inline std::mutex& getMutex() {return m_mutex;};

		void resetTickData();
		void render();

	private:
		struct Line {
			Vec3 begin;
			Vec3 end;
			u8Vec3 color;
		};
		struct ContactPoint {
			Vec3 point;
			Vec3 normal;
			float distance = 1.0f;
			int lifetime = 500;
			int remainingFrames = 0;
		};
		std::mutex m_mutex;
		std::vector<Line> m_vecLines;
		btVector3 m_vCullingBoxPosition = {0.0f, 0.0f, 0.0f};
		btVector3 m_vCullingBoxRadius = {82.0f, 82.0f, 82.0f};
		btVector3 m_vCullingBoxMin = {-82.0f, -82.0f, -82.0f};
		btVector3 m_vCullingBoxMax = {82.0f, 82.0f, 82.0f};
		int m_totalContacts = 0;
		int m_renderContacts = 0;
		int m_debugMode = DBG_NoDebug;
		bool m_bDrawWireframe = false;
		bool m_bDrawAABB = false;
		bool m_bDrawContactPoints = false;
		bool m_bDrawConstraints = false;
		bool m_bDrawConstraintLimits = false;
		bool m_bDrawNormals = false;
		bool m_bDrawFrames = false;
		bool m_bDrawCapsules = false;
		bool m_bEnableAll = false;

		void m_updateDebugMode() {
			m_debugMode = DBG_NoDebug;
			m_debugMode |= (m_bDrawWireframe ? DBG_DrawWireframe : 0);
			m_debugMode |= (m_bDrawAABB ? DBG_DrawAabb : 0);
			m_debugMode |= (m_bDrawContactPoints ? DBG_DrawContactPoints : 0);
			m_debugMode |= (m_bDrawConstraints ? DBG_DrawConstraints : 0);
			m_debugMode |= (m_bDrawConstraintLimits ? DBG_DrawConstraintLimits : 0);
			m_debugMode |= (m_bDrawNormals ? DBG_DrawNormals : 0);
			m_debugMode |= (m_bDrawFrames ? DBG_DrawFrames : 0);
		};
		void m_updateCullingBox() {
			m_vCullingBoxMin = m_vCullingBoxPosition - m_vCullingBoxRadius;
			m_vCullingBoxMax = m_vCullingBoxPosition + m_vCullingBoxRadius;
		}
};
