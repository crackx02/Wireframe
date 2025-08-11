#pragma once

#define ASSERT_SIZE(type, sz) static_assert(sizeof(type) == sz, #type " has wrong size! Expected " #sz " bytes" )

#define ResolveClassOffset(obj) obj::_selfPtr = decltype(obj::_selfPtr)(uintptr_t(GetModuleHandle(0)) + uintptr_t(obj::_selfPtr))
#define ResolveOffset(off) (uintptr_t(GetModuleHandle(0)) + off)
#define ResolveGlobal(name) if (!g_##name) g_##name = (decltype(g_##name))(ResolveOffset(Offset_##name))
#define MakeHook(name) MH_CreateHook((LPVOID)ResolveOffset(Offset_##name), (LPVOID)&H_##name, (LPVOID*)&O_##name)
#define EnableHook(name) MH_EnableHook((LPVOID)ResolveOffset(Offset_##name))

#define Vec3CMP(v1, cmp, v2) (v1.x() cmp v2.x() && v1.y() cmp v2.y() && v1.z() cmp v2.z())
