#pragma once

#include "PhysicsBase.hpp"

namespace SM {
	// Notice: Incomplete structure!
	class Physics {
		public:
			inline PhysicsBase* getPhysicsBase() const {return m_pPhysicsBase;};

		private:
			PhysicsBase* m_pPhysicsBase;
	};
}
