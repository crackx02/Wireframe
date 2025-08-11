#pragma once

#include "TickDynamicsWorld.hpp"

namespace SM {
	// Notice: Incomplete structure!
	class PhysicsBase {
		public:
			virtual ~PhysicsBase() {};
			
			inline TickDynamicsWorld* getTickWorld() {return m_pTickDynamicsWorld;}

		private:
			char _pad0[0x80];
			TickDynamicsWorld* m_pTickDynamicsWorld;
	};
}
