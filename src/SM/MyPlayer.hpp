#pragma once

#include "Types.hpp"
#include "Util.hpp"

namespace SM {
	class MyPlayer {
		public:
			static MyPlayer** _selfPtr;
			inline static MyPlayer* Get() {return *_selfPtr;};

			inline const Vec3& getCameraPosition() const {return m_vCameraPosition;};

		private:
			char _pad0[0xA8];
			Vec3 m_vCameraPosition;
			char _pad1[0x2BC];
	};
	ASSERT_SIZE(MyPlayer, 0x370);
}
