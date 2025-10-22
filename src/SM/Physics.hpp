#pragma once

#include "PhysicsBase.hpp"
#include "BulletCollision/BroadphaseCollision/btDbvtBroadphase.h"

namespace SM {
	// Notice: Incomplete structure!
	class Physics {
		public:
			inline PhysicsBase* getPhysicsBase() const {return m_pPhysicsBase;};
			inline btCollisionWorld* getCollisionWorld() const {return m_pCollisionWorld;};

		private:
			PhysicsBase* m_pPhysicsBase;
			char _pad0[0x20];
			btCollisionWorld* m_pCollisionWorld;
	};
	ASSERT_SIZE(Physics, 0x30);
}
