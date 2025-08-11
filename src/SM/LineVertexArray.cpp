
#include "LineVertexArray.hpp"
#include "Console.hpp"

using namespace SM;

#pragma warning(disable: 6001)
#pragma warning(disable: 6386)

void LineVertexArray::push(const Vec3& point, u8Vec3 color) {
	if ( m_pArrVertices == nullptr ) {
		m_pArrVertices = new LineVertex[10];
		m_capacity = 10;
		SM_ASSERT(m_pArrVertices);
	}
	if ( m_size >= m_capacity ) {
		m_capacity *= 1.5;
		LineVertex* pArrNewVertices = new LineVertex[m_capacity];
		SM_ASSERT(pArrNewVertices);
		memcpy(pArrNewVertices, m_pArrVertices, (m_size - 1) * sizeof(LineVertex));
		delete[] m_pArrVertices;
		m_pArrVertices = pArrNewVertices;
	}
	m_pArrVertices[m_size++] = {point, {0xFF, color.b, color.g, color.r}};
}
