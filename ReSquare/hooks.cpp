#include "hooks.hpp"
#include <intrin.h>  

#include "render.hpp"
#include "menu.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "hacks.h"
#include "options.hpp"
#pragma intrinsic(_ReturnAddress)  
#define TICKS_TO_TIME(t) ( 	g_GlobalVars->interval_per_tick* (t))
#define clamp(v, min, max) (v > max ? max : (v < min ? min : v))

namespace Hooks {
	class c_detour {
		void* m_addr = nullptr, * m_dst = nullptr, * m_trampoline = nullptr;
		std::size_t m_len;
		std::vector< std::uint8_t > m_bytes{ };

	public:
		c_detour(void* addr, void* dst, std::size_t len) {
			this->m_addr = addr;
			this->m_dst = dst;
			this->m_len = len;
			this->m_trampoline = VirtualAlloc(nullptr, this->m_len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
		};
		void* hook(bool trampoline = true) {
			if (this->m_len < 5)
				return nullptr;

			unsigned long prot = 0;
			VirtualProtect(this->m_addr, this->m_len, PAGE_EXECUTE_READWRITE, &prot);

			for (auto i = 0; i < this->m_len; i++) {
				auto& byte = *(std::uint8_t*)((std::uintptr_t)this->m_addr + i);
				*(std::uint8_t*)((std::uintptr_t)this->m_trampoline + i) = byte;
				this->m_bytes.push_back(byte);
				byte = 144;
			}

			auto rel32 = (std::uintptr_t)this->m_dst - (std::uint32_t)this->m_addr - 5;

			*(std::uint8_t*)this->m_addr = 233;
			*(std::uint32_t*)((std::uintptr_t)this->m_addr + 1) = rel32;

			VirtualProtect(this->m_addr, this->m_len, prot, &prot);

			auto target = (std::uintptr_t)this->m_trampoline + this->m_len;
			auto new_src = (std::uintptr_t)this->m_addr + this->m_len;
			auto trampoline_rel32 = (std::uintptr_t)new_src - (std::uint32_t)target - 5;

			*(std::uint8_t*)target = 233;
			*(std::uint32_t*)(target + 1) = trampoline_rel32;

			if (trampoline)
				return this->m_trampoline;

			return (void*)((std::uintptr_t)this->m_addr + this->m_len);
		};
		bool unhook() {
			if (!this->m_bytes.size())
				return false;

			unsigned long prot = 0;
			VirtualProtect(this->m_addr, this->m_len, PAGE_EXECUTE_READWRITE, &prot);

			std::uintptr_t i = 0;

			for (auto& byte : this->m_bytes) {
				*(std::uint8_t*)((std::uintptr_t)this->m_addr + i) = byte;
				i++;
			}

			VirtualProtect(this->m_addr, this->m_len, prot, &prot);

			VirtualFree(this->m_trampoline, this->m_len + 5, MEM_RELEASE);

			return true;
		};
	};

	using doextraboneprocessing_fn = void(__thiscall*)(void*, int, int, int, int, int, int);
	std::unique_ptr< c_detour > doextraboneprocessing_hk{ };
	static doextraboneprocessing_fn o_doextraboneprocessing = nullptr;

	using updateanims_fn = void(__thiscall*)(C_BaseEntity*);
	std::unique_ptr< c_detour > updateanims_hk{ };
	static updateanims_fn o_updateanims = nullptr;		

	using senddatagram_fn = int(__thiscall*)(INetChannel*, void*);
	std::unique_ptr< c_detour > senddatagram_hk{ };
	static senddatagram_fn o_senddatagram = nullptr;		
	
	std::unique_ptr< c_detour > shouldskip_hk{ };
	

	/*
	roses are red,
	the only data type i know is string,
	even tough we just met,
	i would like to ask just one thing,
	what fucking calling convention does clmove use,
	im currently sipping some juice,	
	i would love to make a truce,
	when i play on asia servers i have 1000 ping,	
	my toaster goes cling,
	goa lan ma bing chilling 
	*/
	std::unique_ptr< c_detour > cl_move_hk{ };
	static void* o_cl_move;
	//--------------------------------------------------------------------------------
	int m_nTickbaseShift = 0;
	int m_nTickbaseShift2 = 0;
	int doubletapCharge = 0;
	float lastshiftspeedx = 0;
	float lastshiftspeedy = 0;
	int lastDoubleTapInTickcount = 0;
	bool isShifting = false;
	int lastbt = -1;
	class EventListener : IGameEventListener2
	{
	public:
		void start()
		{
			if (!g_GameEvents->AddListener(this, "round_start", false) || !g_GameEvents->AddListener(this, "enter_buyzone", false) || !g_GameEvents->AddListener(this, "player_hurt", false) || !g_GameEvents->AddListener(this, "round_end", false) || !g_GameEvents->AddListener(this, "round_freeze_end", false) || !g_GameEvents->AddListener(this, "player_death", false) || !g_GameEvents->AddListener(this, "bullet_impact", false) || !g_GameEvents->AddListener(this, "player_disconnect", false)) {
				throw std::exception("Failed to register the event");
			}
		}
		void stop()
		{
			g_GameEvents->RemoveListener(this);
		}
		void FireGameEvent(IGameEvent* event) override
		{
			g_Misc.AutoBuy(event);
			g_Resolver.OnFireEvent(event, lastbt);
			g_Misc.Hitmarker(event);
			g_Walkbot.Reset(event);
			g_Misc.BulletTracersRegister(event);
			if (!strcmp(event->GetName(), "round_start"))
			{
				lastDoubleTapInTickcount = g_GlobalVars->tickcount;
			}
		}
		int GetEventDebugID(void) override
		{
			return EVENT_DEBUG_ID_INIT;
		}
	};
	EventListener _listener;

	bool __fastcall shouldSkipHK()
	{
		return false;
	}

	void Initialize()
	{
		renderview_hook.setup(g_RenderView);
		hlclient_hook.setup(g_CHLClient);
		direct3d_hook.setup(g_D3DDevice9);
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		sound_hook.setup(g_EngineSound);
		mdlrender_hook.setup(g_MdlRender);
		clientmode_hook.setup(g_ClientMode);
		engine_hook.setup(g_EngineClient);
		bsp_query.setup(g_EngineClient->GetBSPTreeQuery());
		ConVar* sv_cheats_con = g_CVar->FindVar("sv_cheats");
		sv_cheats.setup(sv_cheats_con);
		//renderview_hook.hook_index(index::SceneEnd, hkSceneEnd);
		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);
		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);
		hlclient_hook.hook_index(24, WriteUsercmdDeltaToBuffer);
		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);
		sound_hook.hook_index(index::EmitSound1, hkEmitSound1);
		vguisurf_hook.hook_index(index::LockCursor, hkLockCursor);
		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);
		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(index::OverrideView, hkOverrideView);
		clientmode_hook.hook_index(index::GetViewModelFov, hkGetViewModelFov);
		engine_hook.hook_index(101, getScreenAspectRatio);
		sv_cheats.hook_index(index::SvCheatsGetBool, hkSvCheatsGetBool);
		bsp_query.hook_index(6, hklistLeavesInBox);
		auto c_csplayer__doextraboneprocessing = (void*)Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 56 8B F1 57 89 74 24 1C 80");
		doextraboneprocessing_hk = std::make_unique<c_detour>(c_csplayer__doextraboneprocessing, Hooks::DoExtraBonesProcessing, 6);
		o_doextraboneprocessing = (doextraboneprocessing_fn)doextraboneprocessing_hk->hook();

		auto c_csplayer__updateanims = (void*)Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74");
		updateanims_hk = std::make_unique<c_detour>(c_csplayer__updateanims, Hooks::UpdateClientSideAnimHook, 7);
		o_updateanims = (updateanims_fn)updateanims_hk->hook();


		auto cl_move = (void*)Utils::PatternScan(GetModuleHandleA("engine.dll"), "55 8B EC 81 EC ? ? ? ? 53 56 8A");
		cl_move_hk = std::make_unique<c_detour>(cl_move, Hooks::hookCLMove, 9);
		o_cl_move = cl_move_hk->hook();		
		
		auto shouldskip = (void*)Utils::PatternScan(GetModuleHandleA("client.dll"), "57 8B F9 8B 07 8B ? ? ? ? ? FF D0 84 C0 75");
		shouldskip_hk = std::make_unique<c_detour>(shouldskip, shouldSkipHK, 5);
		shouldskip_hk->hook();

		
		/*std::stringstream farto;
		for (int hashbrowns = 0; hashbrowns < 100; hashbrowns++)
		{
			BYTE canigetanuhhhh = ((BYTE*)senddatagram)[hashbrowns];
			farto << std::hex << std::setw(2) << std::setfill('0') << (int)canigetanuhhhh << ' ';
		}
		g_CVar->ConsolePrintf(farto.str().c_str());*/
		auto senddatagram = (void*)Utils::PatternScan(GetModuleHandleA("engine.dll"), "55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B F9 89 7C 24 18");
		senddatagram_hk = std::make_unique<c_detour>(senddatagram, Hooks::SendDatagram, 6);
		o_senddatagram = (senddatagram_fn)senddatagram_hk->hook();
		
		/*
		auto retardedaddress = (void*)Utils::PatternScan(GetModuleHandleA("engine.dll"), "55 8B EC 56 8B F1 33 C0 57");
		retardaddress_hk = std::make_unique<c_detour>(retardedaddress, RetardedValveFix, 6);
		retardaddress_hk->hook();
				
		auto retarded1address = (void*)Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 56 8B F1 33 C0 57");
		retardaddress1_hk = std::make_unique<c_detour>(retarded1address, RetardedValveFix, 6);
		retardaddress1_hk->hook();
				
		auto retarded2address = (void*)Utils::PatternScan(GetModuleHandleA("studiorender.dll"), "55 8B EC 56 8B F1 33 C0 57");
		retardaddress2_hk = std::make_unique<c_detour>(retarded2address, RetardedValveFix, 6);
		retardaddress2_hk->hook();

		auto retarded3address = (void*)Utils::PatternScan(GetModuleHandleA("materialsystem.dll"), "55 8B EC 56 8B F1 33 C0 57");
		retardaddress3_hk = std::make_unique<c_detour>(retarded3address, RetardedValveFix, 6);
		retardaddress3_hk->hook();*/


		char* fart[]{ "client.dll", "engine.dll", "server.dll", "studiorender.dll", "materialsystem.dll" };
		WORD amongus = 0x01B0;
		for (auto sex : fart) {
			void* ptr = (void*)((DWORD)Utils::PatternScan(GetModuleHandleA(sex), "3B C1 7C EE 5F 32 C0")+5);
			WriteProcessMemory(GetCurrentProcess(), (void*)ptr, &amongus, 2, 0);
		}
		_listener.start();
	}
	//--------------------------------------------------------------------------------
	void Shutdown()
	{
		renderview_hook.unhook_all();
		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();
		sound_hook.unhook_all();
		sv_cheats.unhook_all();
		bsp_query.unhook_all();
		//send_datagram.unhook_all();
		doextraboneprocessing_hk->unhook();
		updateanims_hk->unhook();
		cl_move_hk->unhook();
		shouldskip_hk->unhook();
		senddatagram_hk->unhook();
		/*
		retardaddress_hk->unhook();
		retardaddress1_hk->unhook();
		retardaddress2_hk->unhook();
		retardaddress3_hk->unhook();
		*/
		_listener.stop();
	}
	//--------------------------------------------------------------------------------
	struct IncomingSequence
	{
		int inreliablestate;
		int outreliablestate;
		int sequencenr;
		float servertime;
	};
	static std::deque<IncomingSequence>sequences;
	//--------------------------------------------------------------------------------
	void AddLatencyToNetchan(INetChannel* netchan, float Latency)
	{
		for (auto& seq : sequences)
		{
			if (g_GlobalVars->realtime - seq.servertime >= Latency)
			{
				netchan->m_nInReliableState = seq.inreliablestate;
				netchan->m_nInSequenceNr = seq.sequencenr;
				break;
			}
		}
	}
	//--------------------------------------------------------------------------------
	void UpdateIncomingSequences()
	{
		static int32_t lastIncomingSequenceNumber = 0;
		if (g_ClientState)
		{
			INetChannel* netchan = g_ClientState->m_NetChannel;

			if (netchan)
			{
				if (netchan->m_nInSequenceNr > lastIncomingSequenceNumber)
				{
					lastIncomingSequenceNumber = netchan->m_nInSequenceNr;
					auto newincomingsequence = IncomingSequence();
					newincomingsequence.inreliablestate = netchan->m_nInReliableState;
					newincomingsequence.outreliablestate = netchan->m_nOutReliableState;
					newincomingsequence.sequencenr = netchan->m_nInSequenceNr;
					newincomingsequence.servertime = g_GlobalVars->realtime;
					sequences.push_front(newincomingsequence);
				}

				if (sequences.size() > 2048)
					sequences.pop_back();
			}
		}
	}
	//--------------------------------------------------------------------------------
	int __fastcall SendDatagram(INetChannel* netchan, void* buff, void* datagram)
	{
		if (((g_Options.backtrack_settings.time<=12||!g_Options.backtrack_settings.enabled)&&g_Options.misc_settings.fakelatency<=0) || datagram)
		{
		return o_senddatagram(netchan, datagram);//o_senddatagram(netchan, datagram);
		}
		int instate = netchan->m_nInReliableState;
		int insequencenr = netchan->m_nInSequenceNr;
		auto lag_s = std::max(g_Options.misc_settings.fakelatency/ 1000.f, (g_Options.backtrack_settings.enabled&&g_Options.backtrack_settings.time>12)?TICKS_TO_TIME(g_Options.backtrack_settings.time-11):0) ;
		auto lag_delta = lag_s - g_EngineClient->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING);
		AddLatencyToNetchan(netchan, lag_delta);

		int ret = o_senddatagram(netchan, datagram);

		netchan->m_nInReliableState = instate;
		netchan->m_nInSequenceNr = insequencenr;

		return ret;
	}
	//--------------------------------------------------------------------------------
	void __fastcall UpdateClientSideAnimHook(C_BaseEntity* entity, void* edx) {
		if (false){//entity && entity->IsPlayer()) {
			auto player = (C_BasePlayer*)entity;

			player->GetPlayerAnimState()->m_iLastClientSideAnimationUpdateFramecount = g_GlobalVars->framecount;
			player->GetPlayerAnimState()->m_flLastClientSideAnimationUpdateTime = g_GlobalVars->curtime;
		}
		//o_updateanims(entity);
	}
	//--------------------------------------------------------------------------------
	void __fastcall DoExtraBonesProcessing(void* ecx, void* edx, int a2, int a3, int a4, int a5, int a6, int a7)
	{
		C_BasePlayer* player = (C_BasePlayer*)ecx;
		if (player->m_fEffects() & 8)
			return;

		const auto state = player->GetPlayerAnimState();

		if (!state || !state->pBaseEntity)
			return o_doextraboneprocessing(ecx, a2, a3, a4, a5, a6, a7);

		auto o_on_ground = state->m_bOnGround;
		state->m_bOnGround = false;
		o_doextraboneprocessing(ecx, a2, a3, a4, a5, a6, a7);
		state->m_bOnGround = o_on_ground;
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkEndScene(IDirect3DDevice9* pDevice)
	{
		static auto oEndScene = direct3d_hook.get_original<decltype(&hkEndScene)>(index::EndScene);

		DWORD colorwrite, srgbwrite;
		IDirect3DVertexDeclaration9* vert_dec = nullptr;
		IDirect3DVertexShader9* vert_shader = nullptr;
		DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);	

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		//removes the source engine color correction
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->GetVertexDeclaration(&vert_dec);
		pDevice->GetVertexShader(&vert_shader);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		static bool menupress = false;
		if (g_Options.keys.MenuKey==0)
			Menu::Get()._visible = true;
		if (InputSys::Get().IsKeyDown(g_Options.keys.MenuKey) && !menupress)
		{
			Menu::Get().Toggle();
			menupress = true;
		}
		else if (!InputSys::Get().IsKeyDown(g_Options.keys.MenuKey) && menupress)
		{
			menupress = false;
		}
		//Render::Get().Background();
		auto esp_drawlist = Render::Get().RenderScene();
		Menu::Get().menurender = (Menu::Get().IsVisible()? (clock() - Menu::Get().lastTime) :(200-(clock()- Menu::Get().lastTime)));
		Menu::Get().Render();
		g_Misc.KeybindList();
		g_Misc.SpectatorList();
		g_Misc.SpeedGraph();
		ImGui::Render(Render::Get().RenderScene());

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
		pDevice->SetVertexDeclaration(vert_dec);
		pDevice->SetVertexShader(vert_shader);

		return oEndScene(pDevice);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkSceneEnd(void* thisptr, void* edx)
	{
		static auto ofunc = renderview_hook.get_original<decltype(&hkSceneEnd)>(index::SceneEnd);
		ofunc(thisptr, edx);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto oReset = direct3d_hook.get_original<decltype(&hkReset)>(index::Reset);

		Menu::Get().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0)
			Menu::Get().OnDeviceReset();

		return hr;
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		static auto oCreateMove = hlclient_hook.get_original<decltype(&hkCreateMove_Proxy)>(index::CreateMove);

		oCreateMove(g_CHLClient, 0, sequence_number, input_sample_frametime, active);

		auto cmd = g_Input->GetUserCmd(sequence_number);
		auto verified = g_Input->GetVerifiedCmd(sequence_number);

		if (!cmd || !cmd->command_number)
			return;		



		if (g_EngineClient->IsInGame() && g_LocalPlayer&&g_LocalPlayer->IsAlive())
		{
			g_ESP.ShouldBreakLBY();		
			UpdateIncomingSequences();
			//g_CVar->ConsolePrintf(std::string(std::to_string(g_LocalPlayer->GetAnimOverlay(6)->m_flPlaybackRate)).c_str());
			//g_CVar->ConsolePrintf("\n");
			auto backupcmd = *cmd;
			static bool sidePress = false;	
			static bool legitsidePress = false;
			static bool thirdpersonPress = false;
			if (InputSys::Get().IsKeyDown(g_Options.antiaim_settings.tankAntiaimKey) && !sidePress)
			{
				g_Options.antiaim_settings.aaSide = !g_Options.antiaim_settings.aaSide;
				sidePress = true;
			}
			else if (!InputSys::Get().IsKeyDown(g_Options.antiaim_settings.tankAntiaimKey) && sidePress)
			{
				sidePress = false;
			}
			if (InputSys::Get().IsKeyDown(g_Options.antiaim_settings.legitAntiaimKey) && !legitsidePress)
			{
				g_Options.antiaim_settings.legitaaflip = !g_Options.antiaim_settings.legitaaflip;
				legitsidePress = true;
			}
			else if (!InputSys::Get().IsKeyDown(g_Options.antiaim_settings.legitAntiaimKey) && legitsidePress)
			{
				legitsidePress = false;
			}
			if (InputSys::Get().IsKeyDown(g_Options.visual_settings.thirdpersonKey) && !thirdpersonPress)
			{
				g_Options.visual_settings.thirdperson = !g_Options.visual_settings.thirdperson;
				thirdpersonPress = true;
			}
			else if (!InputSys::Get().IsKeyDown(g_Options.visual_settings.thirdpersonKey) && thirdpersonPress)
			{
				thirdpersonPress = false;
			}
			g_Misc.AutoStrafe(cmd, backupcmd.viewangles);
			if (g_Options.legit)
			{
				g_AntiAim.LegitAA(cmd, bSendPacket);
				if(InputSys::Get().IsKeyDown(g_Options.keys.AimbotKey)|| g_Options.keys.AimbotKey==0)
				g_Aimbot.Aim(cmd, bSendPacket,backupcmd.viewangles);
				g_Aimbot.RemoveRecoil(cmd);
				if (InputSys::Get().IsKeyDown(g_Options.keys.TriggerbotKey) || g_Options.keys.TriggerbotKey == 0)
				g_Triggerbot.Trigger(cmd);
			}
			//g_Ragebot.bestindex = -1;
			if (g_Options.rage&&!InputSys::Get().IsKeyDown(g_Options.misc_settings.invalidatecmdkey))
			{
				if(g_Options.antiaim_settings.enabled)
				g_AntiAim.Run(cmd, bSendPacket);
				if (InputSys::Get().IsKeyDown(g_Options.keys.RagebotKey) || g_Options.keys.RagebotKey == 0)
				//CPredictionSystem::Get().Start(cmd, g_LocalPlayer);
				g_Ragebot.Aim(cmd, lastDoubleTapInTickcount);
				//CPredictionSystem::Get().End(g_LocalPlayer);

			}
			lastbt = -1;
			if ((g_Options.rage || g_Options.legit) && g_Options.backtrack_settings.enabled)
				g_Backtrack.CreateMoveRecord(cmd);
			if ((g_Options.rage || g_Options.legit)&&g_Options.backtrack_settings.enabled)
				g_Backtrack.CreateMoveSet(cmd, lastbt);
			g_Misc.NadePredict();
			g_Misc.ViewModelCM();
			g_Misc.ForceCrosshairCM();
			g_Misc.FogCM();
			g_Misc.AmbientCM();
			g_Misc.Clantag();
			g_Misc.Bhop(cmd, bSendPacket);
			g_Misc.SlowWalk(cmd);
			g_Misc.Radar();
			g_Misc.FakeDuck(cmd, bSendPacket);
			g_Misc.SpeedGraphRecord();
			int localPlayerFlags = g_LocalPlayer->m_fFlags();
			CPredictionSystem::Get().Start(cmd, g_LocalPlayer);
			g_Misc.Jumpbug(cmd);
			CPredictionSystem::Get().End(g_LocalPlayer);			
			g_Misc.EdgeJump(cmd, localPlayerFlags);
			if(g_Options.misc_settings.fastduck)
				cmd->buttons |= IN_BULLRUSH;

			auto weapon = g_LocalPlayer->m_hActiveWeapon();
			int doubletapTickcountDelta = g_GlobalVars->tickcount - lastDoubleTapInTickcount;
			int weaponIndex = weapon->getWeaponStandalone(weapon->m_Item().m_iItemDefinitionIndex());
			if (!g_Options.ragebot_settings[weaponIndex].enabled) {
				weaponIndex = weapon->getWeaponClass(weapon->m_Item().m_iItemDefinitionIndex());
				if (!g_Options.ragebot_settings[weaponIndex].enabled) {
					weaponIndex = 0;
				}
			}
			float time = 0;
			static bool teleport = false;
			static bool ideal = false;
			switch (g_Options.ragebot_settings[weaponIndex].dt)
			{
			default:
				time = 0.f;
				ideal = false;
				teleport = false;
				break;
			case 1:
				time = 0.184375f;
				ideal = false;
				teleport = false;
				break;
			case 2:
				time = 0.2f;
				ideal = false;
				teleport = false;
				break;
			case 3:
				time = 0.16875;
				ideal = false;
				teleport = false;
				break;
			case 4:
				time = 0.21875f;
				ideal = false;
				teleport = true;
				break;
			case 5:
				time = 0.21875f;
				ideal = true;
				teleport = false;
				break;
			case 6:
				time = std::max(weapon->GetFireRate(), 0.f);
				ideal = false;
				teleport = true;
				break;			
			case 7:
				time = 0.09375f;
				ideal = false;
				teleport = true;
				break;
			}
			if (g_Options.misc_settings.autopistol && weapon->m_flNextPrimaryAttack() > g_GlobalVars->curtime && !weapon->GetCSWeaponData()->bFullAuto)
				cmd->buttons &= ~IN_ATTACK;
			if (((g_Options.misc_settings.dtkey==0||InputSys::Get().IsKeyDown(g_Options.misc_settings.dtkey))&&cmd->buttons & IN_ATTACK && doubletapTickcountDelta >= TIME_TO_TICKS(time) && ((g_Options.ragebot_settings[weaponIndex].dt != 0) && g_Options.ragebot_settings[weaponIndex].enabled)&&g_Options.rage && weapon->m_flNextPrimaryAttack() <= g_GlobalVars->curtime && weapon->m_iClip1()) && !InputSys::Get().IsKeyDown(g_Options.misc_settings.invalidatecmdkey))
			{
				lastDoubleTapInTickcount = g_GlobalVars->tickcount;
				if (teleport || ideal)
					m_nTickbaseShift = TIME_TO_TICKS(time);
				else
					m_nTickbaseShift2 = TIME_TO_TICKS(time);
				lastshiftspeedx = cmd->forwardmove;
				lastshiftspeedy = cmd->sidemove;
			}
			g_AntiAim.CorrectMovement(backupcmd.viewangles, cmd, cmd->forwardmove, cmd->sidemove);
			g_Misc.FastStop(cmd);
			g_Misc.AutoPeak(cmd, bSendPacket);
			g_Walkbot.Run(cmd);
			g_Misc.RunMovement(cmd);
			g_Misc.Exploits(cmd, bSendPacket, &m_nTickbaseShift, doubletapCharge, lastDoubleTapInTickcount);
			if (m_nTickbaseShift > 0 && !teleport && ideal)
			{
				cmd->forwardmove = -lastshiftspeedx;
				cmd->sidemove = -lastshiftspeedy;
			}
			if ((cmd->buttons & IN_ATTACK) || m_nTickbaseShift > 0|| m_nTickbaseShift2 > 0)
			{
				bSendPacket = true;
			}
			if (bSendPacket) {
				g_LocalAnimations.local_data.fake_angles = cmd->viewangles;
				g_LocalAnimations.local_data.real_angles = g_LocalAnimations.local_data.stored_real_angles;
				g_AntiAim.Desync.simtime = g_LocalPlayer->m_flSimulationTime();
			}
			else
			{
				g_LocalAnimations.local_data.stored_real_angles = cmd->viewangles;
				g_AntiAim.Desync.simtimereal = g_LocalPlayer->m_flSimulationTime();
			}
			if (g_Options.antiaim_settings.antiuntrusted)
			{
				Math::NormalizeAngles(cmd->viewangles);
				cmd->forwardmove = clamp(cmd->forwardmove, -450, 450);
				cmd->sidemove = clamp(cmd->sidemove, -450, 450);
				m_nTickbaseShift = 0;
				m_nTickbaseShift2 = 0;
			}
		}
		if (Menu::Get().IsVisible()&&g_Options.legit)
			cmd->buttons &= ~IN_ATTACK;
			
		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum();
	}
	//--------------------------------------------------------------------------------
	__declspec(naked) void __fastcall hkCreateMove_Proxy(void* _this, int, int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx; not sure if we need this
			push esp
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}
	//--------------------------------------------------------------------------------
	static float actualfov = 0;
	void renderAimbotFov()
	{
		auto weapon = g_LocalPlayer->m_hActiveWeapon();
		if (!weapon || !weapon->IsGun())
			return;
		int weaponIndex = weapon->getWeaponStandalone(weapon->m_Item().m_iItemDefinitionIndex());
		if (!g_Options.aimbot_settings[weaponIndex].enabled) {
			weaponIndex = weapon->getWeaponClass(weapon->m_Item().m_iItemDefinitionIndex());
			if (!g_Options.aimbot_settings[weaponIndex].enabled) {
				weaponIndex = 0;
				if (!g_Options.aimbot_settings[0].enabled)
					return;
			}
		}
		int w, h;
		if (g_Options.aimbot_settings[weaponIndex].fov > 80.f)
			return;
		g_EngineClient->GetScreenSize(w, h);
		if (actualfov == 0)
			return;
		int cx = w / 2;	
		int cy = h / 2;
		float ratio = w/h;
		float screenFov = atanf((ratio) * (0.75f) * tan(DEG2RAD(actualfov * 0.5f)));;
		float radius = tanf(DEG2RAD(g_Options.aimbot_settings[weaponIndex].fov)) / tanf(screenFov) * cx;
		g_VGuiSurface->DrawSetColor(Color(g_Options.misc_settings.aimbotfov.x, g_Options.misc_settings.aimbotfov.y, g_Options.misc_settings.aimbotfov.z , g_Options.misc_settings.aimbotfov.w));
		g_VGuiSurface->DrawOutlinedCircle(cx, cy, radius/2.f, 50);

	}
	void renderAimbotFovRage()
	{
		auto weapon = g_LocalPlayer->m_hActiveWeapon();
		if (!weapon || !weapon->IsGun())
			return;
		int weaponIndex = weapon->getWeaponStandalone(weapon->m_Item().m_iItemDefinitionIndex());
		if (!g_Options.ragebot_settings[weaponIndex].enabled) {
			weaponIndex = weapon->getWeaponClass(weapon->m_Item().m_iItemDefinitionIndex());
			if (!g_Options.ragebot_settings[weaponIndex].enabled) {
				weaponIndex = 0;
				if (!g_Options.ragebot_settings[0].enabled)
					return;
			}
		}
		int w, h;
		if (g_Options.ragebot_settings[weaponIndex].fov > 80.f)
			return;
		g_EngineClient->GetScreenSize(w, h);
		if (actualfov == 0)
			return;
		int cx = w / 2;
		int cy = h / 2;
		float ratio = w / h;
		float screenFov = atanf((ratio) * (0.75f) * tan(DEG2RAD(actualfov * 0.5f)));;
		float radius = tanf(DEG2RAD(g_Options.ragebot_settings[weaponIndex].fov)) / tanf(screenFov) * cx;
		g_VGuiSurface->DrawSetColor(Color(g_Options.misc_settings.aimbotfov.x, g_Options.misc_settings.aimbotfov.y, g_Options.misc_settings.aimbotfov.z, g_Options.misc_settings.aimbotfov.w));
		g_VGuiSurface->DrawOutlinedCircle(cx, cy, radius / 2.f, 50);

	}
	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<decltype(&hkPaintTraverse)>(index::PaintTraverse);
		if (g_Options.visual_settings.noscope && !strcmp("HudZoom", g_VGuiPanel->GetName(panel)))
			return;
		oPaintTraverse(g_VGuiPanel, edx, panel, forceRepaint, allowForce);

		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (panelId == panel) 
		{
			static int bSkip = 0;
			bSkip++;

			if (bSkip%g_Options.misc_settings.limiter!=0)
				return;
			if (g_EngineClient->IsInGame() && g_LocalPlayer) {
				g_Misc.MotionBlur();
				g_Glow.ExecDlight();
				if(g_LocalPlayer->IsAlive())
				if (g_LocalPlayer->m_bIsScoped()&&g_Options.visual_settings.noscope) {
					int w, h;

					g_EngineClient->GetScreenSize(w, h);

					g_VGuiSurface->DrawSetColor(Color(0, 0, 0, 255));

					int cx = w / 2;
					int cy = h / 2;

					g_VGuiSurface->DrawLine(0, cy, w, cy);
					g_VGuiSurface->DrawLine(cx, 0, cx, h);
				}
				if (g_Options.misc_settings.aimbotfov.w!=0)
				{
					if (g_Options.legit)
						renderAimbotFov();
					else if (g_Options.rage)
						renderAimbotFovRage();
				}
			}
			Render::Get().BeginScene(doubletapCharge, (std::min(g_GlobalVars->tickcount - lastDoubleTapInTickcount, 64)/64.f));
		}
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkEmitSound1(void* _this, int edx, IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk) {
		static auto ofunc = sound_hook.get_original<decltype(&hkEmitSound1)>(index::EmitSound1);


		if (!strcmp(pSoundEntry, "UIPanorama.popup_accept_match_beep")) {
			static auto fnAccept = reinterpret_cast<bool(__stdcall*)(const char*)>(Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"));

			if (fnAccept) {

				fnAccept("");

				//This will flash the CSGO window on the taskbar
				//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
				FLASHWINFO fi;
				fi.cbSize = sizeof(FLASHWINFO);
				fi.hwnd = InputSys::Get().GetMainWindow();
				fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
				fi.uCount = 0;
				fi.dwTimeout = 0;
				FlashWindowEx(&fi);
			}
		}

		ofunc(g_EngineSound, edx, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk);

	}
	//--------------------------------------------------------------------------------
	void __stdcall render2dEffectsPreHud(CViewSetup* setup)
	{
		g_Misc.MotionBlur();
	}
	//--------------------------------------------------------------------------------
	int __fastcall hkDoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmode_hook.get_original<decltype(&hkDoPostScreenEffects)>(index::DoPostScreenSpaceEffects);
		if (g_LocalPlayer)
			g_Glow.Exec();
		g_Misc.BulletTracers();
		return oDoPostScreenEffects(g_ClientMode, edx, a1);
	}
	//--------------------------------------------------------------------------------
	void __cdecl hookCLMove_Proxy(float AES, bool BFT)
	{
		__asm
		{
			ENTER 2, 0
			//MOV PTR [ESP + 4], CL
			MOVSS DWORD PTR [ESP], XMM0
			call Hooks::hookCLMove
			LEAVE
			RET
		}
	}
	//--------------------------------------------------------------------------------
	void __cdecl hookCLMove(int pad1, int pad2, int pad3, float pad4) {
		double accumulated_extra_samples = 0;
		int bFinalTick = false; // this is not an int actually a bool but shh
		__asm { // plz gib
			mov bFinalTick, ecx
			movq accumulated_extra_samples, xmm0
		}
		if ((g_GlobalVars->tickcount % (g_Options.misc_settings.chargespeed))==0 && doubletapCharge < 14) {
			doubletapCharge++;
			// Task for the reader: Tickbase still increases when recharging, but the client doesn't know that...
			return;
		}
		DWORD oclmovef = (DWORD)o_cl_move;
		__asm {
			push ecx
			mov ecx, bFinalTick
			movq xmm0, accumulated_extra_samples
			call oclmovef
			pop ecx
		}
		if (m_nTickbaseShift > 0)
		{
			for (m_nTickbaseShift = std::min(doubletapCharge, m_nTickbaseShift); m_nTickbaseShift > 0; m_nTickbaseShift--, doubletapCharge--) {
				__asm {
					push ecx
					mov ecx, bFinalTick
					movq xmm0, accumulated_extra_samples
					call oclmovef
					pop ecx
				}
			}
		}
	}
	//--------------------------------------------------------------------------------
	void WriteUsercmd_t(void* buf, CUserCmd* in, CUserCmd* out)
	{
		static DWORD WriteUsercmdF = (DWORD)Utils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 83 E4 F8 51 53 56 8B D9");;

		__asm
		{
			mov ecx, buf
			mov edx, in
			push out
			call WriteUsercmdF
			add esp, 4
		}
	}
	//--------------------------------------------------------------------------------
	bool __fastcall WriteUsercmdDeltaToBuffer(void* ecx, void* edx, int slot, bf_write* buf, int from, int to, bool isnewcommand)
	{
		auto original = hlclient_hook.get_original<decltype(&WriteUsercmdDeltaToBuffer)>(24);
		if (m_nTickbaseShift2 <= 0)
			return original(ecx, edx, slot, buf, from, to, isnewcommand);
		if (from != -1)
			return true;
		auto WriteUsercmd = [](bf_write* buf, CUserCmd* in, CUserCmd* out) {
			using WriteUsercmd_t = void(__fastcall*)(bf_write*, CUserCmd*, CUserCmd*);
			static WriteUsercmd_t WriteUsercmdF = (WriteUsercmd_t)Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "55 8B EC 83 E4 F8 51 53 56 8B D9 8B 0D");
			WriteUsercmdF(buf, in, out);
		};
		int* pNumBackupCommands = (int*)(reinterpret_cast <uintptr_t> (buf) - 0x30);
		int* pNumNewCommands = (int*)(reinterpret_cast <uintptr_t> (buf) - 0x2C);
		int32_t new_commands = *pNumNewCommands;
		int32_t next_cmdnr = g_ClientState->m_nLastOutgoingCommand + g_ClientState->m_nChokedCommands + 1;
		int32_t total_new_commands = std::min(m_nTickbaseShift2, 16);
		m_nTickbaseShift2 -= total_new_commands;
		from = -1;
		*pNumNewCommands = total_new_commands;
		*pNumBackupCommands = 0;



		for (to = next_cmdnr - new_commands + 1; to <= next_cmdnr; to++) {
			if (!original(ecx, edx, slot, buf, from, to, isnewcommand))
				return false;

			from = to;
		}

		CUserCmd* last_realCmd = g_Input->GetUserCmd(slot, from);
		CUserCmd fromCmd;

		if (last_realCmd)
			fromCmd = *last_realCmd;

		CUserCmd toCmd = fromCmd;
		toCmd.command_number++;
		toCmd.tick_count += 200;

		for (int i = new_commands; i <= total_new_commands; i++) {
			WriteUsercmd_t(buf, &toCmd, &fromCmd);
			fromCmd = toCmd;
			toCmd.command_number++;
			toCmd.tick_count++;
		}

		return true;
	}
	//--------------------------------------------------------------------------------
	struct CRenderableInfo {
		IClientRenderable* renderable;
		std::byte pad[18];
		uint16_t flags;
		uint16_t flags2;
	};
	//--------------------------------------------------------------------------------
	int __stdcall hklistLeavesInBox(const Vector& mins, const Vector& maxs, unsigned short* list, int listMax)
	{
		static auto ofunc = bsp_query.get_original<decltype(&hklistLeavesInBox)>(6);
		const static auto insertintotree = ((uintptr_t)Utils::PatternScan(GetModuleHandleA("client.dll"), "56 52 FF 50 18"))+5;
		if (g_Options.misc_settings.listleaves && std::uintptr_t(_ReturnAddress()) == insertintotree) {
			if (const auto info = *reinterpret_cast<CRenderableInfo**>(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) + 0x14); info && info->renderable) {
				if (const auto ent = info->renderable->GetIClientUnknown()->GetBaseEntity(); ent && ent->GetClientClass()->m_ClassID==ClassId::ClassId_CCSPlayer) {
					static const Vector min{ -16384.0f, -16384.0f, -16384.0f };
					static const Vector max{ 16384.0f, 16384.0f, 16384.0f };
					info->flags &= ~0x100;
					info->flags2 |= 0xC0;
					return ofunc(min, max, list, listMax);
				}
			}
		}
		return ofunc(mins, maxs, list, listMax);
	}
	//--------------------------------------------------------------------------------
	float __stdcall getScreenAspectRatio(int width, int height)
	{
		auto ofunc = engine_hook.get_original<decltype(&getScreenAspectRatio)>(101);
		if (g_Options.visual_settings.aspectratio != 0.0f)
			return g_Options.visual_settings.aspectratio;
		return ofunc(width, height);
	}
	//--------------------------------------------------------------------------------
	std::string skyboxList[] = { "Default", "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam", "sky_lunacy", "sky_hr_aztec" };
	void __fastcall hkFrameStageNotify(void* _this, int edx, ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<decltype(&hkFrameStageNotify)>(index::FrameStageNotify);
		if (g_EngineClient->IsInGame())
		{
			if (g_Options.visual_settings.bloomenabled)
			{
				for (int i = 0; i < 2048; i++)
				{
					C_BaseToneMapController* ent = (C_BaseToneMapController*)g_EntityList->GetClientEntity(i);

					if (!ent)
						continue;
					if(ent->GetClientClass()->m_ClassID!=ClassId::ClassId_CEnvTonemapController)
						continue;
					ent->m_bUseCustomBloomScale() = true;
					ent->m_flCustomBloomScale() = g_Options.visual_settings.bloomScale / 10;
					//ent->m_bUseCustomBloomScale() = 1;
					//ent->m_flCustomAutoExposureMin() = g_Options.visual_settings.bloomworldExposure / 10;
					//ent->m_flCustomAutoExposureMax() = g_Options.visual_settings.bloomworldExposure / 10;
					//ent->m_flCustomBloomScale() = g_Options.visual_settings.bloomScale / 10;
					g_CVar->FindVar("r_modelAmbientMin")->SetValue(g_Options.visual_settings.bloomModelAmbient);
					g_CVar->FindVar("mat_force_tonemap_scale")->SetValue(g_Options.visual_settings.bloomworldExposure/10);
				}
			}
			static auto loadSky = reinterpret_cast<void(__fastcall*)(const char*)>(Utils::PatternScan(GetModuleHandleW(L"engine.dll"), "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45"));
			if (g_Options.visual_settings.skybox > 0) {
				loadSky(skyboxList[g_Options.visual_settings.skybox].c_str());
				g_CVar->FindVar("r_3dsky")->SetValue(0);
			}
			else {
				static const auto sv_skyname = g_CVar->FindVar("sv_skyname");
				loadSky(sv_skyname->GetString());
				g_CVar->FindVar("r_3dsky")->SetValue(1);
			}
		}
		auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
		if (!pLocal) return;
		if (g_EngineClient->IsInGame()) {
			if (pLocal->IsAlive()&&(g_AntiAim.Desync.simtime > pLocal->m_flSimulationTime() - 1) && (g_AntiAim.Desync.simtimereal > pLocal->m_flSimulationTime() - 1))
			{
				if (!g_Options.misc_settings.disableAnim && !g_LocalAnimations.local_data.real_angles.IsZero() && !g_LocalAnimations.local_data.fake_angles.IsZero())
				{
					g_AntiAim.Desync.simtimeupdate = pLocal->m_flSimulationTime();
					g_LocalAnimations.AnimationFix(stage);
				}
			}
			if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
				if(g_Options.misc_settings.weather!=0)
				g_Misc.Rain();
				g_Skins.ExecuteCheck();
				for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
				{
					auto entity = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(i));

					if (!entity || !entity->IsPlayer() || entity->IsDormant() || !entity->IsAlive() || entity==pLocal)
						continue;
					if (entity->GetPlayerAnimState())
						o_updateanims(entity);
				}
				if (g_Options.rage && g_Options.resolver) {
					g_Resolver.OnFramestageNotify();
				}
			}			
			if (stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_END) {
				if (pLocal->GetPlayerAnimState())
					o_updateanims(pLocal);
			}
			if (stage == ClientFrameStage_t::FRAME_RENDER_START)
			{
				for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
				{
					auto entity = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(i));

					if (!entity || !entity->IsPlayer() || entity->IsDormant() || !entity->IsAlive())
						continue;
					if (entity->GetPlayerAnimState())
						entity->SetAbsAngles(QAngle(0.f, entity->GetPlayerAnimState()->m_flGoalFeetYaw, 0.f));//if u not doin dis it f*cks up the model lol
				}

				//g_LocalAnimations.Sync();
				if(g_Options.visual_settings.worldenabled|| g_Options.visual_settings.skyenabled)
				for (MaterialHandle_t imathandle = g_MatSystem->FirstMaterial(); imathandle != g_MatSystem->InvalidMaterial(); imathandle = g_MatSystem->NextMaterial(imathandle))
				{
					IMaterial* pMaterial = g_MatSystem->GetMaterial(imathandle);
					if (!pMaterial || pMaterial->IsErrorMaterial())
						continue;
					if ((strstr(pMaterial->GetTextureGroupName(), "World") || strstr(pMaterial->GetTextureGroupName(), "StaticProp"))&&g_Options.visual_settings.worldenabled) //$envmapmask
					{
						pMaterial->ColorModulate(g_Options.visual_settings.worldcolor.x, g_Options.visual_settings.worldcolor.y, g_Options.visual_settings.worldcolor.z );
						pMaterial->AlphaModulate(g_Options.visual_settings.worldcolor.w);
						//pMaterial = g_Chams.materiallist.at(2);
						/*
						auto ShaderParams = pMaterial->GetShaderParams();
						if ((!std::strstr(pMaterial->GetShaderName(), "VertexLitGeneric")))
							continue;
						std::string vars;
						std::vector<std::string> varnames;
						std::vector<std::string> varvalues;
						for (size_t i = 0; i < pMaterial->ShaderParamCount(); i++)
						{
							if (std::strstr(ShaderParams[i]->GetName(), "flags"))
								continue;
							vars += ShaderParams[i]->GetName();
							vars += " ";
							if (strcmp(ShaderParams[i]->GetName(), "$decalblendmode") == 0) {
								vars += "1";
								g_CVar->ConsolePrintf("fuak");
							}
							else
							vars += ShaderParams[i]->GetValue();
							vars += " ";
							if (strstr(ShaderParams[i]->GetValue(), "["))
							{
								varnames.push_back(ShaderParams[i]->GetName());
								varvalues.push_back(ShaderParams[i]->GetValue());
							}
						}
						KeyValues Generator;
						auto GeneratedValues = Generator.CreateFromString(pMaterial->GetShaderName(), vars.c_str(), g_Chams.patternchams);
						for (int iname = 0; iname < varnames.size(); iname++)
						{
							GeneratedValues->setString(varnames.at(iname).c_str(), varvalues.at(iname).c_str(), g_Chams.patternsetkv, g_Chams.patternfindkv);
						}
						pMaterial->SetShaderAndParams(GeneratedValues);
						*/
						
					}
					if (strstr(pMaterial->GetTextureGroupName(), "SkyBox") && g_Options.visual_settings.skyenabled)
					{
						pMaterial->ColorModulate(g_Options.visual_settings.skycolor.x, g_Options.visual_settings.skycolor.y, g_Options.visual_settings.skycolor.z);
					}
				}
				//if (g_Options.visual_settings.worldenabled)
				//	g_Options.visual_settings.worldenabled = false;
				if(g_Options.rage)
				{ 
				for (int i = 1; i <= g_GlobalVars->maxClients; i++)
				{
					if (i == g_EngineClient->GetLocalPlayer()) continue;

					IClientEntity* pCurEntity = g_EntityList->GetClientEntity(i);
					if (!pCurEntity) continue;

					*(int*)((uintptr_t)pCurEntity + 0xA30) = g_GlobalVars->framecount;
					*(int*)((uintptr_t)pCurEntity + 0xA28) = 0;
				}
				}
				constexpr std::array smokeMaterials{ "particle/vistasmokev1/vistasmokev1_emods","particle/vistasmokev1/vistasmokev1_emods_impactdust","particle/vistasmokev1/vistasmokev1_fire","particle/vistasmokev1/vistasmokev1_smokegrenade" };

				for (const auto mat : smokeMaterials) {
					const auto material = g_MatSystem->FindMaterial(mat);
					material->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, g_Options.visual_settings.nosmoke == 1);
					material->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, g_Options.visual_settings.nosmoke == 2);
				}
				static auto smokecount = *(DWORD*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 83 EC 08 8B 15 ? ? ? ? 0F 57 C0") + 0x8);
				if (g_Options.visual_settings.nosmoke)
					*(int*)smokecount = 0;
			}
		}
		ofunc(g_CHLClient, edx, stage);
	}
	//--------------------------------------------------------------------------------
	bool init = true;
	void __fastcall hkOverrideView(void* _this, int edx, CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<decltype(&hkOverrideView)>(index::OverrideView);
		if(g_EngineClient->IsInGame() && g_LocalPlayer)
		{ 
			g_Input->m_fCameraInThirdPerson = g_Options.visual_settings.thirdperson;
			if (g_Options.visual_settings.thirdperson)
			{
				if (!g_Input->m_fCameraInThirdPerson)
				{
					g_Input->m_fCameraInThirdPerson = true;
				}
				float dist = g_Options.visual_settings.thirdpersondist;
				QAngle* view = g_LocalPlayer->GetVAngles();
				trace_t tr;
				Ray_t ray;
				Vector desiredCamOffset = Vector(cos(DEG2RAD(view->yaw)) * dist,
					sin(DEG2RAD(view->yaw)) * dist,
					sin(DEG2RAD(-view->pitch)) * dist
				);
				ray.Init(g_LocalPlayer->GetEyePos(), (g_LocalPlayer->GetEyePos() - desiredCamOffset));
				CTraceFilter traceFilter;
				traceFilter.pSkip = g_LocalPlayer;
				g_EngineTrace->TraceRay(ray, MASK_SHOT, &traceFilter, &tr);
				Vector diff = g_LocalPlayer->GetEyePos() - tr.endpos;
				float distance2D = sqrt(abs(diff.x * diff.x) + abs(diff.y * diff.y));
				bool horOK = distance2D > (dist - 2.0f);
				bool vertOK = (abs(diff.z) - abs(desiredCamOffset.z) < 3.0f);
				float cameraDistance;
				if (horOK && vertOK) 
					cameraDistance = dist;
				else
				{
					if (vertOK) 
						cameraDistance = distance2D * 0.95f;
					else
						cameraDistance = abs(diff.z) * 0.95f;
				}
				g_Input->m_fCameraInThirdPerson = true;
				g_Input->m_vecCameraOffset.z = cameraDistance;
			}
			else
				g_Input->m_fCameraInThirdPerson = false;
			if (!g_LocalPlayer->m_bIsScoped())
				vsView->fov += g_Options.visual_settings.fov;
			else if (g_LocalPlayer->m_bIsScoped())
				vsView->fov += g_Options.visual_settings.scopedfov + 50;
			if (g_Options.visual_settings.noflash){
				g_LocalPlayer->m_flFlashDuration() = 0.f;
				g_LocalPlayer->m_flFlashMaxAlpha() = 0.f;
			}
			actualfov = vsView->fov;
		}
		ofunc(g_ClientMode, edx, vsView);
	}
	//--------------------------------------------------------------------------------
	float __stdcall hkGetViewModelFov() {
	return  60 + g_Options.visual_settings.viewmodelfov;
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkLockCursor(void* _this)
	{
		static auto ofunc = vguisurf_hook.get_original<decltype(&hkLockCursor)>(index::LockCursor);

		if (Menu::Get().IsVisible()) {
			g_VGuiSurface->UnlockCursor();
			g_InputSystem->ResetInputState();
			return;
		}
		ofunc(g_VGuiSurface);

	}
	//--------------------------------------------------------------------------------
	void __fastcall hkDrawModelExecute(void* _this, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<decltype(&hkDrawModelExecute)>(index::DrawModelExecute);
		if (g_MdlRender->IsForcedMaterialOverride() &&
			!strstr(pInfo.pModel->szName, "sleeve") &&
			!strstr(pInfo.pModel->szName, "arms") &&
			!strstr(pInfo.pModel->szName, "weapons/v_")) {
			return ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);
		}
		if (!pCustomBoneToWorld)
			return;
		auto entity = C_BasePlayer::GetPlayerByIndex(pInfo.entity_index);
		if ((g_Options.chams_settings[10].enabled||g_Options.chams_settings[24].enabled||g_Options.chams_settings[38].enabled||g_Options.chams_settings[52].enabled||g_Options.chams_settings[66].enabled)&&(g_LocalPlayer&&g_LocalPlayer->IsAlive()&& entity == g_LocalPlayer)) {
			for (int n = 0; n < 5; n++)
			{
				if (!entity)
					continue;
				if (!entity->IsAlive())
					continue;
				if (!entity->IsPlayer())
					continue;
				if (!g_LocalPlayer->IsAlive())
					continue;
				if (g_LocalPlayer->m_iShotsFired()>=1)
					continue;
				//if (!(g_AntiAim.Desync.simtime > g_LocalPlayer->m_flSimulationTime() - 1) || !(g_AntiAim.Desync.simtimereal > g_LocalPlayer->m_flSimulationTime() - 1)|| !(g_AntiAim.Desync.simtimeupdate > g_LocalPlayer->m_flSimulationTime() - 1))
				//	continue;
				if (g_Options.rage && !g_Options.antiaim_settings.antiaim_knife && g_LocalPlayer->m_hActiveWeapon()->IsKnife())
					continue;
				if (g_Options.chams_settings[14 * n + 10].enabled)
				{ 
				g_Chams.OverrideMaterial(14*n+10);
				ofunc(_this, edx, ctx, state, pInfo, g_AntiAim.Desync.bones);
				}
			}
		}
		for (int n = 0; n < 5; n++) {
			if (g_Chams.Apply(ctx, state, pInfo, pCustomBoneToWorld, n) || n == 0)
				ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);
		}
		for (int n = 0; n < 5; n++)
		{
			if (!g_Options.backtrack_settings.enabled)
				continue;
			if (!g_Options.chams_settings[14 * n + 9].enabled)
				continue;
			if (!entity)
				continue;
			if (!entity->IsAlive())
				continue;
			if (!entity->IsPlayer())
				continue;
			if (entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
				continue;
			g_Chams.OverrideMaterial(13 * n + 9);
			if (g_Options.backtrack_settings.shoulddrawall) {
				for (int i = 0; i < g_Options.backtrack_settings.time; i++)
				{
					if (g_Backtrack.TimeWarpData[pInfo.entity_index][i].absorigin == ((C_BasePlayer*)g_EntityList->GetClientEntity(pInfo.entity_index))->m_vecOrigin())
						continue;
					if (!(g_Backtrack.TimeWarpData[pInfo.entity_index][i].simtime > g_LocalPlayer->m_flSimulationTime() - 1))
						continue;
					ofunc(_this, edx, ctx, state, pInfo, g_Backtrack.TimeWarpData[pInfo.entity_index][i].matrix);
				}
			}
			else {
				int oldestvalid= INT_MAX;
				float oldestsimtime = FLOAT32_MAX;
				for (int i = 0; i < g_Options.backtrack_settings.time; i++)
				{
					if ((!(g_Backtrack.TimeWarpData[pInfo.entity_index][i].simtime > g_LocalPlayer->m_flSimulationTime() - 1))|| (oldestsimtime<g_Backtrack.TimeWarpData[pInfo.entity_index][i].curtime))
						continue;
					else
					{
						oldestvalid = i;
						oldestsimtime = g_Backtrack.TimeWarpData[pInfo.entity_index][i].curtime;
					}
				}
				if(oldestvalid!=INT_MAX)
				ofunc(_this, edx, ctx, state, pInfo, g_Backtrack.TimeWarpData[pInfo.entity_index][oldestvalid].matrix);
			}
		}
		g_MdlRender->ForcedMaterialOverride(nullptr);
	}

	
	
	bool __fastcall hkSvCheatsGetBool(PVOID pConVar, void* edx)
	{
		static auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client.dll"), "85 C0 75 30 38 86");
		static auto ofunc = sv_cheats.get_original<bool(__thiscall *)(PVOID)>(13);
		if (!ofunc)
			return false;

		if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think))
			return true;
		return ofunc(pConVar);
	}
}
