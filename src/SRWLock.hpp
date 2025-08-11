#pragma once

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"

#pragma warning(disable: 26110)	// Caller failing to hold lock before calling ReleaseSRWLockShared

namespace SM {
	class SRWLock {
		public:
			void lock() {
				AcquireSRWLockShared(&m_lock);
			}
			void unlock() {
				ReleaseSRWLockShared(&m_lock);
			}

		private:
			SRWLOCK m_lock;
	};
}

#pragma warning(default: 26110)
