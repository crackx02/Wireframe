
#include <thread>
#include <set>
#include <mutex>

#define WIN32_LEAN_AND_MEAN
#include "Windows.h"
#include "Intrin.h"
#include "MinHook.h"
#include "xxhash.h"
#include "QuickHull.hpp"
#include "BulletCollision/CollisionShapes/btConvexHullShape.h"

#include "Util.hpp"
#include "Types.hpp"
#include "NullHash.hpp"
#include "BulletDebugDraw.hpp"
#include "IndexPair.hpp"
#include "SM/D3D11Device.hpp"
#include "SM/ChatCommandManager.hpp"
#include "SM/ClientWorld.hpp"
#include "SM/DebugDrawer.hpp"
#include "SM/MyPlayer.hpp"
#include "SM/Console.hpp"

using namespace SM;

constexpr uintptr Offset_InitializeConsole = 0x02d7a80;
constexpr uintptr Offset_InitializeDXDevice = 0x090d140;
constexpr uintptr Offset_PlayState_Cleanup = 0x042dab0;
constexpr uintptr Offset_ChatCommandManager = 0x06c5d10;
constexpr uintptr Offset_DebugDrawObject = 0x0ca7020;
constexpr uintptr Offset_DebugDrawer_Render = 0x09ef890;
constexpr uintptr Offset_ClientWorldUpdate = 0x02abec0;

constexpr uintptr Index_D3D11DeviceContext_RSSetState = 43;

constexpr uintptr RetAddr_DrawMyGUI = 0x0d1d674;
constexpr uintptr RetAddr_DrawFSTri = 0x07ddc1d;

constexpr uint16 NoWorldID = 65534;

// I resized QuickHull's Vector3 to avoid some unnecessary data copies
static_assert(sizeof(btVector3) == sizeof(quickhull::Vector3<float>));



// State //

static struct {
	bool bMhInitialized = false;
	void* hExeModule = nullptr;
	bool bWireframeEnabled = false;
	NullHashMap<ID3D11RasterizerState*, ID3D11RasterizerState*> mapRasterStateToWireframeState;
	BulletDebugDraw btDebugDraw;
	NullHashMap<uint32, std::vector<uint32>> mapHullHashToLineIndices;
} g_State;



// Callbacks

static std::string OnChatCommand(const ChatCommand::VecParams& vecParams) {
	SM_ASSERT(vecParams.size() != 0);

	std::string_view sCommand = vecParams[0].stringValue;

	if ( sCommand == "/polygonmode" || sCommand == "/wireframe" )
		return std::format("Global wireframe rendering {}", (g_State.bWireframeEnabled = !g_State.bWireframeEnabled) ? "enabled" : "disabled");

	auto& btd = g_State.btDebugDraw;
	if ( sCommand == "/wf_phys_all" ) {
		if ( btd.getEnableAll() )
			btd.disableAll();
		else
			btd.enableAll();
		return std::format("All physics debug drawing {}", btd.getEnableAll() ? "enabled" : "disabled");
	}

	if ( sCommand == "/wf_phys_wireframe" ) {
		btd.setWireframe(!btd.getWireframe());
		return std::format("Collision mesh wireframe {}", btd.getWireframe() ? "enabled" : "disabled");
	} else if ( sCommand == "/wf_phys_aabb" ) {
		btd.setAABB(!btd.getAABB());
		return std::format("AABB rendering {}", btd.getAABB() ? "enabled" : "disabled");
	} else if ( sCommand == "/wf_phys_contacts" ) {
		btd.setContactPoints(!btd.getContactPoints());
		return std::format("Collision contact point rendering {}", btd.getContactPoints() ? "enabled" : "disabled");
	} else if ( sCommand == "/wf_phys_constraints" ) {
		btd.setConstraints(!btd.getConstraints());
		return std::format("Physics constraint (joint) debug rendering {}", btd.getConstraints() ? "enabled" : "disabled");
	} else if ( sCommand == "/wf_phys_constraintlimits" ) {
		btd.setConstraintLimits(!btd.getConstraintLimits());
		return std::format("Physics constraint (joint) limit rendering {}", btd.getConstraintLimits() ? "enabled" : "disabled");
	} else if ( sCommand == "/wf_phys_normals" ) {
		btd.setNormals(!btd.getNormals());
		return std::format("Collision mesh normals rendering {}", btd.getNormals() ? "enabled" : "disabled");
	} else if ( sCommand == "/wf_phys_transforms" ) {
		btd.setFrames(!btd.getFrames());
		return std::format("Physics object transform rendering {}", btd.getFrames() ? "enabled" : "disabled");
	} else if ( sCommand == "/wf_phys_capsules" ) {
		btd.setCapsules(!btd.getCapsules());
		return std::format("Character capsule rendering {}", btd.getCapsules() ? "enabled" : "disabled");
	} else if ( sCommand == "/wf_phys_renderdistance" ) {
		SM_ASSERT(vecParams.size() == 2);
		const auto& p = vecParams[1];
		SM_ASSERT(p.type == ChatCommand::Param::Float);
		btd.setCullingBoxRadius(p.floatValue);
		return std::format("Physics debug render distance set to {} meters", p.floatValue);
	} else if ( sCommand == "/wf_phys_contactcount" ) {
		return std::format("Collision contact counts:\nTotal: {}\nRendered: {}", btd.getTotalContacts(), btd.getRenderContacts());
	} else
		return "Unknown command received";
}



// Hooks //

static void(*O_DebugDrawObject)(TickDynamicsWorld*, const btTransform&, const btCollisionShape*, const btVector3&) = nullptr;
static void H_DebugDrawObject(
	TickDynamicsWorld* self,
	const btTransform& transform,
	const btCollisionShape* pShape,
	const btVector3& color
) {
	BulletDebugDraw* pDrawer = (BulletDebugDraw*)self->getDebugDrawer();
	if ( !pDrawer )
		return;

	int shapeType = pShape->getShapeType();
	if ( shapeType == CAPSULE_SHAPE_PROXYTYPE && !pDrawer->getCapsules() )
		return;

	if ( shapeType == CONVEX_HULL_SHAPE_PROXYTYPE ) {
		// Draw a small simplex at the center of the object
		if ( pDrawer->getDebugMode() & btIDebugDraw::DBG_DrawFrames )
			pDrawer->drawTransform(transform, 0.2f);

		btConvexHullShape* pHull = (btConvexHullShape*)pShape;

		const btVector3* pArrPoints = pHull->getUnscaledPoints();
		int numPoints = pHull->getNumPoints();
		// Most collision meshes don't have many vertices so this should be ok
		uint32 hash = XXH32(pArrPoints, numPoints * sizeof(btVector3), 0);

		auto& mapLineIndices = g_State.mapHullHashToLineIndices;
		auto it = mapLineIndices.find(hash);
		if ( it == mapLineIndices.end() ) {
			/*
				We have to generate mesh indices ourselves as bullet doesn't provide them for hull
				shapes and drawing without indices just results in spaghetti instead of a mesh.
			*/
			it = mapLineIndices.emplace(hash, std::vector<uint32>()).first;
			auto& vecLineIndices = it->second;

			quickhull::QuickHull<float> qh;
			auto hull = qh.getConvexHull((const quickhull::Vector3<float>*)pArrPoints, numPoints, false, true);
			const auto& indices = hull.getIndexBuffer();

			std::set<IndexPair> setUniquePairs;
			for ( auto it = indices.begin(); it < indices.end(); it += 3 ) {
				uint32 i0 = uint32(*(it + 0));
				uint32 i1 = uint32(*(it + 1));
				uint32 i2 = uint32 (*(it + 2));
				setUniquePairs.emplace(i0, i1);
				setUniquePairs.emplace(i1, i2);
				setUniquePairs.emplace(i2, i0);
			}

			vecLineIndices.reserve(setUniquePairs.size() * 2);
			for ( const auto& pair : setUniquePairs )
				vecLineIndices.insert(vecLineIndices.end(), {pair.a, pair.b});
			
			SM_LOG(
				"Generated {} line indices with {} failed edges for hull shape with {} vertices",
				vecLineIndices.size(), qh.getDiagnostics().m_failedHorizonEdges, numPoints
			);
		}

		auto& vecLineIndices = it->second;
		for ( auto it = vecLineIndices.begin(); it < vecLineIndices.end(); it += 2 )
			pDrawer->drawLine(transform * pHull->getScaledPoint(*it), transform * pHull->getScaledPoint(*(it + 1)), color);

		return;
	}
	return O_DebugDrawObject(self, transform, pShape, color);
}

static void(*O_DebugDrawer_Render)(DebugDrawer*, float, void*, void*) = nullptr;
static void H_DebugDrawer_Render(DebugDrawer* self, float p2, void* p3, void* p4) {
	// Debug drawing needs to happen here, other callbacks risk threading errors
	{
		std::scoped_lock lock(g_State.btDebugDraw.getMutex());
		g_State.btDebugDraw.render();
	}
	return O_DebugDrawer_Render(self, p2, p3, p4);
}

static void(*O_ClientWorldUpdate)(ClientWorld*, float) = nullptr;
static void H_ClientWorldUpdate(ClientWorld* self, float delta) {
	O_ClientWorldUpdate(self, delta);
	if ( self->getWorldID() == NoWorldID )
		return;

	auto pPhysics = self->getPhysics();
	if ( !pPhysics )
		return;

	PhysicsBase* pPhysicsBase = pPhysics->getPhysicsBase();
	if ( !pPhysicsBase )
		return;

	TickDynamicsWorld* pTickWorld = pPhysicsBase->getTickWorld();
	if ( !pTickWorld )
		return;

	if ( !pTickWorld->getDebugDrawer() )
		pTickWorld->setDebugDrawer(&g_State.btDebugDraw);

	if ( MyPlayer* mp = MyPlayer::Get(); mp != nullptr )
		g_State.btDebugDraw.setCullingBoxPosition(ToBT(mp->getCameraPosition()));

	std::scoped_lock lock(g_State.btDebugDraw.getMutex());
	g_State.btDebugDraw.resetTickData();
	pTickWorld->debugDrawWorld();
}

static ChatCommandManager*(*O_ChatCommandManager)(ChatCommandManager*) = nullptr;
static ChatCommandManager* H_ChatCommandManager(ChatCommandManager* self) {
	O_ChatCommandManager(self);
	self->registerCommand("/polygonmode", {}, OnChatCommand, "Switches global rendering between normal and wireframe render modes.");
	self->registerCommand("/wf_phys_all", {}, OnChatCommand, "Enables/disables all available physics debug rendering features at once.");
	self->registerCommand("/wf_phys_wireframe", {}, OnChatCommand, "Toggles rendering of physics object's collision mesh.");
	self->registerCommand("/wf_phys_aabb", {}, OnChatCommand, "Toggles rendering of physics object's AABB (bounding box).");
	self->registerCommand("/wf_phys_contacts", {}, OnChatCommand, "Toggles rendering of physics object's collision contact points.");
	self->registerCommand("/wf_phys_constraints", {}, OnChatCommand, "Toggles debug rendering of physics constraint (joint) origin transforms.");
	self->registerCommand("/wf_phys_constraintlimits", {}, OnChatCommand, "Toggles rendering of physics constraint (joint) limits.");
	self->registerCommand("/wf_phys_normals", {}, OnChatCommand, "Toggles rendering of physics collision mesh normals.");
	self->registerCommand("/wf_phys_transforms", {}, OnChatCommand, "Toggles rendering of physics object's transform axes.\n#FFFF00Note: Requires /wf_phys_wireframe!");
	self->registerCommand("/wf_phys_capsules", {}, OnChatCommand, "Toggles rendering of character capsule shapes.\n#FFFF00Note: Requires /wf_phys_wireframe!");
	self->registerCommand(
		"/wf_phys_renderdistance",
		{{ChatCommand::Param::Float, "distance", false}},
		OnChatCommand,
		"Sets the render distance for physics debug drawing, in meters, around the camera position (default: 32)."
	);
	self->registerCommand("/wf_phys_contactcount", {}, OnChatCommand, "Returns the number of debug collision contact points that were registered in the last frame.");
	SM_LOG("Registered chat commands");
	return self;
}

static void(*O_PlayState_Cleanup)(void*) = nullptr;
static void H_PlayState_Cleanup(void* self) {
	g_State.bWireframeEnabled = false;
	for ( auto& [_, pState] : g_State.mapRasterStateToWireframeState )
		pState->Release();
	g_State.mapRasterStateToWireframeState.clear();
	g_State.btDebugDraw.disableAll();
	g_State.btDebugDraw.setCullingBoxRadius(32.0f);
	g_State.btDebugDraw.resetTickData();
	g_State.mapHullHashToLineIndices.clear();
	O_PlayState_Cleanup(self);
}

static void(*O_RSSetState)(ID3D11DeviceContext*, ID3D11RasterizerState*) = nullptr;
static void H_RSSetState(ID3D11DeviceContext* self, ID3D11RasterizerState* pState) {
	uintptr retAddr = uintptr(_ReturnAddress()) - uintptr(g_State.hExeModule);

	if ( retAddr == RetAddr_DrawMyGUI || retAddr == RetAddr_DrawFSTri )
		return O_RSSetState(self, pState);

	if ( g_State.bWireframeEnabled ) {
		auto it = g_State.mapRasterStateToWireframeState.find(pState);
		if ( it == g_State.mapRasterStateToWireframeState.end() ) {
			ID3D11Device* pDevice;
			self->GetDevice(&pDevice);

			D3D11_RASTERIZER_DESC desc;
			pState->GetDesc(&desc);

			desc.FillMode = D3D11_FILL_WIREFRAME;

			ID3D11RasterizerState* pWireframeState = nullptr;
			if ( pDevice->CreateRasterizerState(&desc, &pWireframeState) != S_OK ) {
				SM_ERROR("Failed to create wireframe rasterizer state ({})", GetLastError());
				pWireframeState = pState;
			}

			it = g_State.mapRasterStateToWireframeState.emplace(pState, pWireframeState).first;

			if ( pWireframeState != pState )
				SM_LOG("Created wireframe rasterizer state for {}", (void*)pState);
		}
		pState = it->second;
	}
	return O_RSSetState(self, pState);
}

static void HookRSSetState(ID3D11DeviceContext* pDeviceContext) {
	using fptr = decltype(O_RSSetState);
	fptr* pVT = *(fptr**)pDeviceContext;
	fptr* ppRSSetState = pVT + Index_D3D11DeviceContext_RSSetState;
	DWORD oldProtect;
	if ( !VirtualProtect(ppRSSetState, 8, PAGE_READWRITE, &oldProtect) )
		return SM_ERROR("Failed to change memory protection for ID3D11DeviceContext::RSSetState");
	O_RSSetState = *ppRSSetState;
	*ppRSSetState = &H_RSSetState;
	VirtualProtect(ppRSSetState, 8, oldProtect, &oldProtect);
}

static HRESULT(*O_InitializeDXDevice)(D3D11Device*, HWND) = nullptr;
static HRESULT H_InitializeDXDevice(D3D11Device* self, HWND hwnd) {
	HRESULT res = O_InitializeDXDevice(self, hwnd);
	if ( res == 1 )
		HookRSSetState(self->getContext());
	return res;
}

static void(*O_InitializeConsole)(void*, void*) = nullptr;
static void H_InitializeConsole(void* pContraption, void* ptr) {
	if ( pContraption != nullptr )
		O_InitializeConsole(pContraption, ptr);
	
	SM_LOG("Initializing");

	g_State.hExeModule = GetModuleHandle(0);

	ResolveClassOffset(D3D11Device);
	ResolveClassOffset(ChatCommandManager);
	ResolveClassOffset(RenderStateManager);
	ResolveClassOffset(MyPlayer);

	if ( MakeHook(DebugDrawObject) != MH_OK )
		SM_ERROR("Failed to hook DebugDrawObject!");

	if ( MakeHook(ClientWorldUpdate) != MH_OK )
		SM_ERROR("Failed to hook ClientWorld::update!");

	if ( MakeHook(DebugDrawer_Render) != MH_OK )
		SM_ERROR("Failed to hook DebugDrawer::render!");

	if ( MakeHook(ChatCommandManager) != MH_OK )
		SM_ERROR("Failed to hook ChatCommandManager!");

	if ( MakeHook(PlayState_Cleanup) != MH_OK )
		SM_ERROR("Failed to hook PlayState::cleanup!");

	if ( D3D11Device::Get() == nullptr ) {
		if ( MakeHook(InitializeDXDevice) != MH_OK )
			SM_ERROR("Failed to hook InitializeDXDevice!");
	} else
		HookRSSetState(D3D11Device::Get()->getContext());

	if ( MH_EnableHook(MH_ALL_HOOKS) != MH_OK ) {
		SM_ERROR("Failed to enable hooks!");
		if ( pContraption == nullptr )
			*(bool*)ptr = false;
		return;
	}

	SM_LOG("Initialized");
}



// DLL Process //

static bool Attach() {
	if ( MH_Initialize() != MH_OK ) {
		MessageBoxA(nullptr, "Failed to initialize MinHook", "Wireframe ERROR", MB_OK);
		return false;
	}
	g_State.bMhInitialized = true;
	
	ResolveClassOffset(Console);

	// Initialize only once the console exists, that way we can properly log stuff
	if ( Console::Get() == nullptr ) {
		if ( MakeHook(InitializeConsole) != MH_OK || EnableHook(InitializeConsole) != MH_OK ) {
			MessageBoxA(nullptr, "Failed to hook InitializeConsole", "Wireframe ERROR", MB_OK);
			return false;
		}
	} else {
		SM_WARN("Wireframe mod was injected after game startup! This is experimental and may not work properly.");
		bool res = true;
		H_InitializeConsole(nullptr, &res);
		return res;
	}

	return true;
}

static void Detach() {
	if ( g_State.bMhInitialized ) {
		g_State.bMhInitialized = false;
		MH_Uninitialize();
	}
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
		case DLL_PROCESS_ATTACH: {
			return Attach();
			break;
		}
		case DLL_PROCESS_DETACH:
			Detach();
	}
	return TRUE;
}
