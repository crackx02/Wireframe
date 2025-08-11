#pragma once

#include "Types.hpp"

namespace SM {
	struct LineVertex {
		Vec3 point;
		u8Vec4 color;
	};
	class LineVertexArray {
		public:

			void push(const Vec3& point, u8Vec3 color);

		private:
			LineVertex* m_pArrVertices = nullptr;
			uint32 m_capacity = 0;
			uint32 m_size = 0;
	};
}
