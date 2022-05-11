#include "skin.h"
#include "../options.hpp"
#include "../hooks.hpp"

std::string knifelistname[] = { "models/weapons/v_knife_bayonet.mdl","models/weapons/v_knife_flip.mdl","models/weapons/v_knife_gut.mdl","models/weapons/v_knife_karam.mdl","models/weapons/v_knife_m9_bay.mdl","models/weapons/v_knife_tactical.mdl","models/weapons/v_knife_falchion_advanced.mdl","models/weapons/v_knife_survival_bowie.mdl","models/weapons/v_knife_butterfly.mdl","models/weapons/v_knife_push.mdl","models/weapons/v_knife_cord.mdl","models/weapons/v_knife_canis.mdl","models/weapons/v_knife_ursus.mdl","models/weapons/v_knife_gypsy_jackknife.mdl","models/weapons/v_knife_outdoor.mdl","models/weapons/v_knife_stiletto.mdl","models/weapons/v_knife_widowmaker.mdl","models/weapons/v_knife_skeleton.mdl" };
std::string glovelistname[] = { "","models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl",	"models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl" ,"models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl" ,"models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl" ,"models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl" ,"models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" ,"models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl" };
int knifelistid[] = { 500,505,506,507,508,509,512,514,515,516,517,518,519,520,521,522,523,525 };
int glovelistid[] = { 0,5032,5027,5030,5031,5033,5034,5035};
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);

void ApplySkin(C_BaseAttributableItem* pWeapon, player_info_t pInfo)
{
	auto index = ((C_BaseCombatWeapon*) pWeapon)->getWeaponIndex(pWeapon->m_Item().m_iItemDefinitionIndex());
		pWeapon->m_nFallbackPaintKit() = g_Options.skins_settings[index].paintkit;
		pWeapon->m_nFallbackSeed() = g_Options.skins_settings[index].seed;
		pWeapon->m_nFallbackStatTrak() = g_Options.skins_settings[index].stattrack;
		pWeapon->m_flFallbackWear() = std::max(0.01f, g_Options.skins_settings[index].wear);
		pWeapon->m_Item().m_iAccountID() = pInfo.xuid_low;
		pWeapon->m_Item().m_iItemIDHigh() = -1;
}

void ApplyGlove(C_BaseAttributableItem* pGlove)
{
	pGlove->m_Item().m_iItemDefinitionIndex() = glovelistid[g_Options.misc_settings.glovemodel];

	pGlove->m_nFallbackPaintKit() = g_Options.misc_settings.glovepaintkit;
	pGlove->m_nFallbackSeed() = 1;
	pGlove->m_flFallbackWear() = 0.01f;

	pGlove->m_Item().m_iItemIDHigh() = -1;

	pGlove->m_nModelIndex() = g_MdlInfo->GetModelIndex(glovelistname[g_Options.misc_settings.glovemodel].c_str());
	pGlove->PreDataUpdate(0);
}

static int LastGlove = 0;
static int LastSkin = 0;

static CreateClientClassFn GetWearableCreateFn()
{
	auto clazz = g_CHLClient->GetAllClasses();

	while (strcmp(clazz->m_pNetworkName, "CEconWearable"))
		clazz = clazz->m_pNext;

	return clazz->m_pCreateFn;
}
using setOrAdd = void(__thiscall *)(std::uintptr_t, const char* attribute);

void setOrAddAttributeValueByName(std::uintptr_t attributeList, const char* attribute, float value)
{
	__asm movd xmm2, value
	const static setOrAdd setOrAddAttributeValueByNameFunction = relativeToAbsolute<setOrAdd>((uintptr_t)Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? 8B 8D ? ? ? ? 85 C9 74 10") + 1);

	setOrAddAttributeValueByNameFunction(attributeList, attribute);
}

void setOrAddAttributeValueByName(std::uintptr_t attributeList, const char* attribute, int value)
{
	setOrAddAttributeValueByName(attributeList, attribute, *reinterpret_cast<float*>(&value) /* hack, but CSGO does that */);
}

void apply_sticker_changer(int index, C_BaseAttributableItem* item)
{
	const auto attributeList = std::uintptr_t(item) + NetvarSys::Get().GetOffset("DT_BaseAttributableItem", "m_Item") + 0x244;
	for (std::size_t i = 0; i < 5; ++i) {
		const auto& sticker = g_Options.skins_settings[index].stickers[i];
		if (!sticker.enabled)
			continue;
		const auto attributeString = "sticker slot " + std::to_string(i) + ' ';
		setOrAddAttributeValueByName(attributeList, (attributeString + "id").c_str(), sticker.kit);
		setOrAddAttributeValueByName(attributeList, (attributeString + "wear").c_str(), sticker.wear);
		setOrAddAttributeValueByName(attributeList, (attributeString + "scale").c_str(), sticker.scale);
		setOrAddAttributeValueByName(attributeList, (attributeString + "rotation").c_str(), sticker.rotation);
	}
}

void Skins::Dump()
{
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	// Search the relative calls

	// call    ItemSystem
	// push    dword ptr [esi+0Ch]
	// lea     ecx, [eax+4]
	// call    CEconItemSchema::GetPaintKitDefinition

	auto sig_address = Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? FF 76 0C 8D 48 04 E8");
	auto item_system_offset = *reinterpret_cast<int32_t*>(sig_address + 1);
	auto item_system_fn = reinterpret_cast<CCStrike15ItemSystem * (*)()>(sig_address + 5 + item_system_offset);
	auto item_schema = reinterpret_cast<CCStrike15ItemSchema*>(uintptr_t(item_system_fn()) + sizeof(void*));
	{
		auto get_paint_kit_definition_offset = *reinterpret_cast<int32_t*>(sig_address + 11 + 1);
		auto get_paint_kit_definition_fn = reinterpret_cast<CPaintKit * (__thiscall*)(CCStrike15ItemSchema*, int)>(sig_address + 11 + 5 + get_paint_kit_definition_offset);
		auto start_element_offset = *reinterpret_cast<intptr_t*>(uintptr_t(get_paint_kit_definition_fn) + 8 + 2);
		auto head_offset = start_element_offset - 12;
		auto map_head = reinterpret_cast<Head_t<int, CPaintKit*>*>(uintptr_t(item_schema) + head_offset);
		for (int i = 0; i <= map_head->last_element; ++i)
		{
			auto paint_kit = map_head->memory[i].value;

			if (paint_kit->id == 9001)
				continue;

			const wchar_t* wide_name = g_Localize->Find(paint_kit->item_name.buffer + 1);
			auto name = converter.to_bytes(wide_name);
			g_Skins.skinlist.push_back(std::to_string(paint_kit->id)+": "+name);
			g_Skins.skinlistid.push_back(paint_kit->id);
		}
	}

	auto sticker_sig = Utils::PatternScan(GetModuleHandleA("client.dll"), "53 8D 48 04 E8 ? ? ? ? 8B 4D 10") + 4;
	auto get_sticker_kit_definition_offset = *reinterpret_cast<intptr_t*>(sticker_sig + 1);
	auto get_sticker_kit_definition_fn = reinterpret_cast<CPaintKit * (__thiscall*)(CCStrike15ItemSchema*, int)>(sticker_sig + 5 + get_sticker_kit_definition_offset);
	auto start_element_offset = *reinterpret_cast<intptr_t*>(uintptr_t(get_sticker_kit_definition_fn) + 8 + 2);
	auto head_offset = start_element_offset - 12;

	auto map_head = reinterpret_cast<Head_t<int, CStickerKit*>*>(uintptr_t(item_schema) + head_offset);
	for (int i = 0; i <= map_head->last_element; ++i)
	{
		auto sticker_kit = map_head->memory[i].value;

		char sticker_name_if_valve_fucked_up_their_translations[64];

		auto sticker_name_ptr = sticker_kit->item_name.buffer + 1;

		if (strstr(sticker_name_ptr, "StickerKit_dhw2014_dignitas"))
		{
			strcpy_s(sticker_name_if_valve_fucked_up_their_translations, "StickerKit_dhw2014_teamdignitas");
			strcat_s(sticker_name_if_valve_fucked_up_their_translations, sticker_name_ptr + 27);
			sticker_name_ptr = sticker_name_if_valve_fucked_up_their_translations;
		}

		const wchar_t* wide_name = g_Localize->Find(sticker_name_ptr);
		auto name = converter.to_bytes(wide_name);

		g_Skins.stickerlistid.push_back(sticker_kit->id);
		g_Skins.stickerlist.push_back(name);
	}

}

void Skins::ExecuteCheck()
{			
	/*auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	if (!pLocal) return;
	UINT* weapons = (UINT*)pLocal->m_hMyWeapons();
	player_info_t player_info;
	if (!g_EngineClient->GetPlayerInfo(pLocal->EntIndex(), &player_info)) return;
	if (weapons) {
		for (int nIndex = 0; weapons[nIndex]; nIndex++) {
			C_BaseAttributableItem* weapon = (C_BaseAttributableItem*)g_EntityList->GetClientEntityFromHandle(weapons[nIndex]);
			if (!weapon)
				continue;
			C_BaseCombatWeapon* weaponC = (C_BaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(weapons[nIndex]);
			if (!weaponC)
				continue;
			if (weaponC->IsGun())
			{
				auto index = weaponC->getWeaponIndex(weapon->m_Item().m_iItemDefinitionIndex());
				if (index == 0)
					continue;
				if (!g_Options.skins_settings[index].enabled)
					continue;
				Skins::ApplySkins(weapon, g_Options.skins_settings[index].paintkit, g_Options.skins_settings[index].seed, g_Options.skins_settings[index].wear, g_Options.skins_settings[index].stattrack);
			}
			else if (g_Options.skins_settings[0].enabled && weaponC->IsKnife())
			{
				if (!pLocal->m_hViewModel())
					continue;
				weapon->m_Item().m_iItemDefinitionIndex() = knifelistid[g_Options.misc_settings.knifemodel];
				weapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(knifelistname[g_Options.misc_settings.knifemodel].c_str());
				weaponC->m_iViewModelIndex() = g_MdlInfo->GetModelIndex(knifelistname[g_Options.misc_settings.knifemodel].c_str());
				if (weaponC == pLocal->m_hActiveWeapon())
					pLocal->m_hViewModel()->m_nModelIndex() = g_MdlInfo->GetModelIndex(knifelistname[g_Options.misc_settings.knifemodel].c_str());
				Skins::ApplySkins(weapon, g_Options.skins_settings[0].paintkit, g_Options.skins_settings[0].seed, g_Options.skins_settings[0].wear, g_Options.skins_settings[0].stattrack);
				/*int Sequence = g_LocalPlayer->m_nSequence();
				int originalActivity = g_LocalPlayer->GetSequenceActivity(Sequence);
				Sequence = originalActivity;
				g_LocalPlayer->m_hViewModel()->SendViewModelMatchingSequence(Sequence);*//*

			}
		}
	}
	if (g_Options.misc_settings.glovemodel&&(LastGlove!=g_Options.misc_settings.glovemodel||LastSkin!=g_Options.misc_settings.glovepaintkit))
	{
		DWORD* hMyWearables = (DWORD*)pLocal->m_hMyWearables();
		if (hMyWearables)
		{
			for (ClientClass* pClass = g_CHLClient->GetAllClasses(); pClass; pClass = pClass->m_pNext)
			{
				if (!pClass)
					continue;
				if (pClass->m_ClassID != ClassId_CEconWearable)
					continue;
				if (!std::strstr(pClass->m_pNetworkName, "EconWearable"))
					continue;
				int iEntry = (g_EntityList->GetHighestEntityIndex() + 1);
				const auto serial = rand() % 0x1000;

				pClass->m_pCreateFn(iEntry, serial);
				hMyWearables[0] = iEntry | (serial << 16);
				break;
			}
			auto pEnt = (C_BaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(hMyWearables[0]);
			if (pEnt)
			{
				//pEnt->GetIndex() = -1;
				pEnt->m_Item().m_iItemDefinitionIndex() = glovelistid[g_Options.misc_settings.glovemodel];
				//pEnt->m_iViewModelIndex() = g_MdlInfo->GetModelIndex(glovelistname[g_Options.misc_settings.glovemodel].c_str());
				//g_LocalPlayer->m_hViewModel()->m_nModelIndex() = g_MdlInfo->GetModelIndex(glovelistname[g_Options.misc_settings.glovemodel].c_str());
				Skins::ApplySkins(pEnt, g_Options.misc_settings.glovepaintkit, 1, 0.00000001f, -1);
				*reinterpret_cast<int*>(uintptr_t(pEnt) + 0x64) = -1;
				//pEnt->SetGloveModelIndex(g_MdlInfo->GetModelIndex(glovelistname[g_Options.misc_settings.glovemodel].c_str()));
				pEnt->PreDataUpdate(0);
				//pEnt->m_Item().m_iAccountID() = pLocal->GetPlayerInfo().xuid_low;
			}
			LastGlove = g_Options.misc_settings.glovemodel;
			LastSkin = g_Options.misc_settings.glovepaintkit;
		}
	}*/

	if (!g_EngineClient->IsInGame()) return;
	auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	if (!pLocal) return;
	player_info_t player_info;
	if (!g_EngineClient->GetPlayerInfo(pLocal->EntIndex(), &player_info)) return;
	{
		auto wearables = pLocal->m_hMyWearables();

		static CBaseHandle glove_handle = 0;

		auto glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(wearables[0]));

		if (!glove)
		{
			auto our_glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(glove_handle));

			if (our_glove)
			{
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}

		if (!pLocal->IsAlive())
		{
			auto glove = static_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(*pLocal->m_hMyWearables()));
			if (!glove)
				return;

			glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
			glove->GetClientNetworkable()->Release();

			return;
		}

		if (!glove)
		{
			static auto create_wearable_fn = GetWearableCreateFn();

			auto entry = g_EntityList->GetHighestEntityIndex() + 1;
			auto serial = rand() % 0x1000;

			//glove = static_cast<C_BaseAttributableItem*>(create_wearable_fn(entry, serial));
			create_wearable_fn(entry, serial);
			glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntity(entry));

			{
				static auto set_abs_origin_fn = reinterpret_cast<void(__thiscall*)(void*, const Vector&)>
					(Utils::PatternScan(GetModuleHandle(L"client.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1"));

				static const Vector new_pos = { 10000.f, 10000.f, 10000.f };

				set_abs_origin_fn(glove, new_pos);
			}

			((DWORD*)pLocal->m_hMyWearables())[0] = entry | serial << 16;

			glove_handle = wearables[0];
		}

		*reinterpret_cast<int*>(uintptr_t(glove) + 0x64) = -1;
		ApplyGlove(glove);
	}

	const auto weapons = pLocal->m_hMyWeapons();
	for (size_t i = 0; weapons[i].IsValid(); i++)
	{
		auto pWeapon = static_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(weapons[i]));
		if (!pWeapon) continue;
		auto index = ((C_BaseCombatWeapon*)pWeapon)->getWeaponIndex(pWeapon->m_Item().m_iItemDefinitionIndex());
		if (g_Options.skins_settings[0].enabled && ((C_BaseCombatWeapon*)pWeapon)->IsKnife()) {
			if (!pLocal->m_hViewModel())
				continue;
			pWeapon->m_Item().m_iItemDefinitionIndex() = knifelistid[g_Options.misc_settings.knifemodel];
			//((C_BaseCombatWeapon*)pWeapon)->m_iViewModelIndex() = g_MdlInfo->GetModelIndex(knifelistname[g_Options.misc_settings.knifemodel].c_str());
			pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(knifelistname[g_Options.misc_settings.knifemodel].c_str());
			if (pWeapon == pLocal->m_hActiveWeapon() && pLocal->m_hActiveWeapon()->IsKnife()) {
				((C_BaseCombatWeapon*)pWeapon)->m_iViewModelIndex() = g_MdlInfo->GetModelIndex(knifelistname[g_Options.misc_settings.knifemodel].c_str());
				pLocal->m_hViewModel()->m_nModelIndex() = g_MdlInfo->GetModelIndex(knifelistname[g_Options.misc_settings.knifemodel].c_str());
			}
		}
		else if (index == 0)
			continue;
		if (!g_Options.skins_settings[index].enabled)
			continue;
		ApplySkin(pWeapon, player_info);
		apply_sticker_changer(index, pWeapon);
	}
}
Skins g_Skins;