#pragma once

#include "Util.hpp"

namespace SM {
	class DebugDrawer;
	class RenderStateManager {
		public:
			static RenderStateManager** _selfPtr;
			inline static RenderStateManager* Get() {return *_selfPtr;};

			inline DebugDrawer* getDebugDrawer() const {return m_pDebugDrawer;}

		private:
			char _pad0[0x260];
			DebugDrawer* m_pDebugDrawer;
			char _pad1[0x978D8];
	};
	ASSERT_SIZE(RenderStateManager, 0x97B40);
}
