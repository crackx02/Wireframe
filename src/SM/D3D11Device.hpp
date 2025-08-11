#pragma once

#include "d3d11.h"
#include "Util.hpp"

namespace SM {
	class D3D11Device {
		public:
			static D3D11Device** _selfPtr;
			inline static D3D11Device* Get() {return *_selfPtr;};

			inline ID3D11Device* getDevice() const {return m_pDevice;};
			inline ID3D11DeviceContext* getContext() const { return m_pDeviceContext; };

		private:
			char _pad0[0x8];
			ID3D11Device* m_pDevice;
			ID3D11DeviceContext* m_pDeviceContext;
			char _pad1[0x222];
	};
	ASSERT_SIZE(D3D11Device, 0x240);
}
