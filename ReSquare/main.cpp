#define NOMINMAX
#include <Windows.h>

#include "valve_sdk/sdk.hpp"
#include "helpers/utils.hpp"
#include "helpers/input.hpp"

#include "hooks.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "render.hpp"
#include "hacks/skin.h"

#define SEQUENCE_DEFAULT_DRAW 0
#define SEQUENCE_DEFAULT_IDLE1 1
#define SEQUENCE_DEFAULT_IDLE2 2
#define SEQUENCE_DEFAULT_LIGHT_MISS1 3
#define SEQUENCE_DEFAULT_LIGHT_MISS2 4
#define SEQUENCE_DEFAULT_HEAVY_MISS1 9
#define SEQUENCE_DEFAULT_HEAVY_HIT1 10
#define SEQUENCE_DEFAULT_HEAVY_BACKSTAB 11
#define SEQUENCE_DEFAULT_LOOKAT01 12

#define SEQUENCE_BUTTERFLY_DRAW 0
#define SEQUENCE_BUTTERFLY_DRAW2 1
#define SEQUENCE_BUTTERFLY_LOOKAT01 13
#define SEQUENCE_BUTTERFLY_LOOKAT03 15

#define SEQUENCE_FALCHION_IDLE1 1
#define SEQUENCE_FALCHION_HEAVY_MISS1 8
#define SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP 9
#define SEQUENCE_FALCHION_LOOKAT01 12
#define SEQUENCE_FALCHION_LOOKAT02 13

#define SEQUENCE_DAGGERS_IDLE1 1
#define SEQUENCE_DAGGERS_LIGHT_MISS1 2
#define SEQUENCE_DAGGERS_LIGHT_MISS5 6
#define SEQUENCE_DAGGERS_HEAVY_MISS2 11
#define SEQUENCE_DAGGERS_HEAVY_MISS1 12

#define SEQUENCE_BOWIE_IDLE1 1
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);


typedef void(*RecvVarProxyFn)(const CRecvProxyData* pData, void* pStruct, void* pOut);
RecvVarProxyFn fnSequenceProxyFn = nullptr;
RecvVarProxyFn oRecvnModelIndex;

void SetViewModelSequence2(const CRecvProxyData* pDataConst, void* pStruct, void* pOut)
{
	CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);
	C_BaseViewModel* pViewModel = (C_BaseViewModel*)pStruct;

	if (pViewModel) {
		IClientEntity* pOwner = g_EntityList->GetClientEntityFromHandle(pViewModel->m_hOwner());
		if (pOwner && pOwner->EntIndex() == g_EngineClient->GetLocalPlayer()) {
			const model_t* pModel = g_MdlInfo->GetModel(pViewModel->m_nModelIndex());
			const char* szModel = g_MdlInfo->GetModelName(pModel);
			int sequence = pData->m_Value.m_Int;
            if (!strcmp(szModel, "models/weapons/v_knife_butterfly.mdl")) {
                switch (sequence) {
                case SEQUENCE_DEFAULT_DRAW:
                    sequence = SEQUENCE_BUTTERFLY_DRAW;
                    break;
                case SEQUENCE_DEFAULT_LOOKAT01:
                    sequence = SEQUENCE_BUTTERFLY_LOOKAT01;
                    break;
                default:
                    sequence = sequence + 1;
                    break;
                }
            }
            else if (!strcmp(szModel, "models/weapons/v_knife_falchion_advanced.mdl")) {
                switch (sequence) {
                case SEQUENCE_DEFAULT_IDLE2:
                    sequence = SEQUENCE_FALCHION_IDLE1;
                    break;
                case SEQUENCE_DEFAULT_HEAVY_MISS1:
                    sequence = SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP;
                    break;
                case SEQUENCE_DEFAULT_LOOKAT01:
                    sequence = SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02;
                    break;
                case SEQUENCE_DEFAULT_DRAW:
                case SEQUENCE_DEFAULT_IDLE1:
                    sequence = sequence;
                    break;
                default:
                    sequence = sequence - 1;
                    break;
                }
            }
            else if (!strcmp(szModel, "models/weapons/v_knife_push.mdl")) {
                switch (sequence) {
                case SEQUENCE_DEFAULT_IDLE2:
                    sequence = SEQUENCE_DAGGERS_IDLE1;
                    break;
                case SEQUENCE_DEFAULT_LIGHT_MISS1:
                case SEQUENCE_DEFAULT_LIGHT_MISS2:
                    sequence = SEQUENCE_DAGGERS_LIGHT_MISS1;
                    break;
                case SEQUENCE_DEFAULT_HEAVY_MISS1:
                    sequence = SEQUENCE_DAGGERS_HEAVY_MISS2;
                    break;
                case SEQUENCE_DEFAULT_HEAVY_HIT1:
                case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
                case SEQUENCE_DEFAULT_LOOKAT01:
                    sequence = sequence + 3;
                    break;
                case SEQUENCE_DEFAULT_DRAW:
                case SEQUENCE_DEFAULT_IDLE1:
                    sequence = sequence;
                    break;
                default:
                    sequence = sequence + 2;
                    break;
                }
            }
            else if (!strcmp(szModel, "models/weapons/v_knife_survival_bowie.mdl")) {
                switch (sequence) {
                case SEQUENCE_DEFAULT_DRAW:
                case SEQUENCE_DEFAULT_IDLE1:
                    sequence = sequence;
                    break;
                case SEQUENCE_DEFAULT_IDLE2:
                    sequence = SEQUENCE_BOWIE_IDLE1;
                    break;
                default:
                    sequence = sequence - 1;
                    break;
                }
            }
            else if (
                !strcmp(szModel, "models/weapons/v_knife_ursus.mdl")
                || (!strcmp(szModel, "models/weapons/v_knife_skeleton.mdl"))
                || (!strcmp(szModel, "models/weapons/v_knife_outdoor.mdl"))
                || (!strcmp(szModel, "models/weapons/v_knife_cord.mdl"))
                || (!strcmp(szModel, "models/weapons/v_knife_canis.mdl"))
                )
            {
                sequence = sequence + 1;
            }
            else if (!strcmp(szModel, "models/weapons/v_knife_stiletto.mdl")) {
                switch (sequence) {
                case SEQUENCE_DEFAULT_LOOKAT01:
                    sequence = 12;
                    break;
                }
            }
            else if (!strcmp(szModel, "models/weapons/v_knife_widowmaker.mdl")) {
                switch (sequence) {
                case SEQUENCE_DEFAULT_LOOKAT01:
                    sequence = 14;
                    break;
                }
            }
			pData->m_Value.m_Int = sequence;
		}
	}
	fnSequenceProxyFn(pData, pStruct, pOut);

}

void AnimationFixHook()
{
	for (ClientClass* pClass = g_CHLClient->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
		if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
			RecvTable* pClassTable = pClass->m_pRecvTable;

			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
					continue;
				fnSequenceProxyFn = static_cast<RecvVarProxyFn>(pProp->m_ProxyFn);
				pProp->m_ProxyFn = static_cast<RecvVarProxyFn>(SetViewModelSequence2);

				break;
			}

			break;
		}
	}
}

void AnimationFixUnhook()
{
	for (ClientClass* pClass = g_CHLClient->GetAllClasses(); pClass; pClass = pClass->m_pNext) {
		if (!strcmp(pClass->m_pNetworkName, "CBaseViewModel")) {
			RecvTable* pClassTable = pClass->m_pRecvTable;

			for (int nIndex = 0; nIndex < pClassTable->m_nProps; nIndex++) {
				RecvProp* pProp = &pClassTable->m_pProps[nIndex];

				if (!pProp || strcmp(pProp->m_pVarName, "m_nSequence"))
					continue;
				pProp->m_ProxyFn = fnSequenceProxyFn;

				break;
			}

			break;
		}
	}
}

BOOL WINAPI OnDllDetach()
{
	AnimationFixUnhook();
    Menu::Get()._visible = false;
    if (Menu::Get().IsVisible())
        Menu::Get().Toggle();
    Hooks::Shutdown();

    //Menu::Get().Shutdown();
    return TRUE;
}

DWORD WINAPI OnDllAttach(LPVOID base)
{
    while (!GetModuleHandleA("serverbrowser.dll"))
        Sleep(1000);
    Interfaces::Initialize();
    Interfaces::Dump();
	AnimationFixHook();
    NetvarSys::Get().Initialize();
    InputSys::Get().Initialize();
    Render::Get().Initialize();
    Menu::Get().Initialize();
    Hooks::Initialize();

    InputSys::Get().RegisterHotkey(VK_DELETE, [base]() {
        g_Unload = true;
    });

    while (!g_Unload)
        Sleep(1000);

    OnDllDetach();
    FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);

    // unreachable
    //return TRUE;
}

BOOL WINAPI DllMain(
    _In_      HINSTANCE hinstDll,
    _In_      DWORD     fdwReason,
    _In_opt_  LPVOID    lpvReserved
)
{
    switch (fdwReason) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hinstDll);
        CreateThread(nullptr, 0, OnDllAttach, hinstDll, 0, nullptr);
        return TRUE;
    case DLL_PROCESS_DETACH:
        if (lpvReserved == nullptr)
            return OnDllDetach();
        return TRUE;
    default:
        return TRUE;
    }
}
