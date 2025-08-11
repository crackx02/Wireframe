#pragma once

#include <memory>

#include "Physics.hpp"
#include "Util.hpp"
#include "Types.hpp"

namespace SM {
	class ClientWorld {
		public:
			inline uint16 getWorldID() const {return m_worldID;};
			inline std::shared_ptr<Physics> getPhysics() const {return m_pPhysics;};

		private:
			char _pad0[0x20];
			std::shared_ptr<Physics> m_pPhysics;
			char _pad1[0xC8];
			uint16 m_worldID;
			char _pad2[0x36];
	};
	ASSERT_SIZE(ClientWorld, 0x130);
}
