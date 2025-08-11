#pragma once

#include "RenderStateManager.hpp"
#include "SRWLock.hpp"
#include "LineVertexArray.hpp"
#include "Types.hpp"

namespace SM {
	class DebugDrawer {
		public:
			inline static DebugDrawer* Get() {return RenderStateManager::Get()->getDebugDrawer();}

			inline SRWLock& getLock() {return m_lock;};

			inline void drawLine(const Vec3& begin, const Vec3& end, u8Vec3 color) {
				m_lineVertices.push(begin, color);
				m_lineVertices.push(end, color);
			};

		private:
			char _pad0[0x148];
			SRWLock m_lock;
			LineVertexArray m_lineVertices;
	};
}
