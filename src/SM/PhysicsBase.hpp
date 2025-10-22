#pragma once

#include "TickDynamicsWorld.hpp"

namespace SM {
	// Notice: Incomplete structure!
	class PhysicsBase {
		public:
			virtual ~PhysicsBase() {};
			
			inline TickDynamicsWorld* getTickDynamicsWorld() {return m_pTickDynamicsWorld;}
			inline btCollisionWorld* getTickRaycastWorld() {return m_pTickRaycastWorld;}
			inline btCollisionWorld* getInterpolatedRaycastWorld() {return m_pInterpolatedRaycastWorld;}

		private:
			char _pad0[0x80];
			TickDynamicsWorld* m_pTickDynamicsWorld;
			char _pad1[0xE0];
			btCollisionWorld* m_pTickRaycastWorld;
			char _pad2[0x18];
			btCollisionWorld* m_pInterpolatedRaycastWorld;
	};
}
