#pragma once
#include "inventory.h"
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
uint32_t* CEconItem::GetAccountID()
{
	return reinterpret_cast<uint32_t*>(this + 0x1C);
}
uint64_t* CEconItem::GetItemID()
{
	return reinterpret_cast<uint64_t*>(this + 0x8);
}

uint64_t* CEconItem::GetOriginalID()
{
	return reinterpret_cast<uint64_t*>(this + 0x10);
}

uint16_t* CEconItem::GetDefIndex()
{
	return reinterpret_cast<uint16_t*>(this + 0x24);
}

uint32_t* CEconItem::GetInventory()
{
	return reinterpret_cast<uint32_t*>(this + 0x20);
}

unsigned char* CEconItem::GetFlags()
{
	return reinterpret_cast<unsigned char*>(this + 0x30);
}

unsigned short* CEconItem::GetEconItemData()
{
	return reinterpret_cast<unsigned short*>(this + 0x26);
}

void CEconItem::UpdateEquippedState(unsigned int state)
{
	static auto fnUpdateEquippedState
		= reinterpret_cast<int(__thiscall*)(CEconItem*, unsigned int)>(
			Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 8B 45 08 8B D0")
			);

	fnUpdateEquippedState(this, state);
}

void CEconItem::AddSticker(int index, int kit, float wear, float scale, float rotation)
{
	SetAttributeValue<int>(113 + 4 * index, kit);
	SetAttributeValue<float>(114 + 4 * index, wear);
	SetAttributeValue<float>(115 + 4 * index, scale);
	SetAttributeValue<float>(116 + 4 * index, rotation);
}

void CEconItem::SetStatTrak(int val)
{
	SetAttributeValue<int>(80, val);
	SetAttributeValue<int>(81, 0);
	SetQuality(ITEM_QUALITY_STRANGE);
}

void CEconItem::SetPaintKit(float kit)
{
	SetAttributeValue<float>(6, kit);
}

void CEconItem::SetPaintSeed(float seed)
{
	SetAttributeValue<float>(7, seed);
}

void CEconItem::SetPaintWear(float wear)
{
	SetAttributeValue<float>(8, wear);
}

void CEconItem::SetQuality(ItemQuality quality)
{
	auto data = *GetEconItemData();
	*GetEconItemData() = data ^ (data ^ 32 * quality) & 0x1E0;
}

void CEconItem::SetRarity(ItemRarity rarity)
{
	auto data = *GetEconItemData();
	*GetEconItemData() = (data ^ (rarity << 11)) & 0x7800 ^ data;
}

void CEconItem::SetOrigin(int origin)
{
	auto data = *GetEconItemData();
	*GetEconItemData() = data ^ ((unsigned __int8)data ^ (unsigned __int8)origin) & 0x1F;
}

void CEconItem::SetLevel(int level)
{
	auto data = *GetEconItemData();
	*GetEconItemData() = data ^ (data ^ (level << 9)) & 0x600;
}

void CEconItem::SetInUse(bool in_use)
{
	auto data = *GetEconItemData();
	*GetEconItemData() = data & 0x7FFF | (in_use << 15);
}

void CEconItem::SetCustomName(const char* name)
{
	static auto Address = Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? 8B 46 78 C1 E8 0A A8 01 74 13 8B 46 34");
	static auto fnSetCustomName
		= reinterpret_cast<CEconItem * (__thiscall*)(void*, const char*)>(
			*reinterpret_cast<uintptr_t*>(Address + 1) + Address + 5
			);

	fnSetCustomName(this, name);
}

void CEconItem::SetCustomDesc(const char* name)
{
	static auto Address = Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? 33 DB 39 5E 3C 7E 5E");
	static auto fnSetCustomDesc
		= reinterpret_cast<CEconItem * (__thiscall*)(void*, const char*)>(
			*reinterpret_cast<uintptr_t*>(Address + 1) + Address + 5
			);

	fnSetCustomDesc(this, name);
}

CPlayerInventory* CSInventoryManager::GetLocalPlayerInventory()
{
	typedef CPlayerInventory*(__thiscall* tOriginal)(void*);
	return CallVFunction<tOriginal>(this, 23)(this);
}

CEconItem* CreateEconItem()
{
	static auto fnCreateSharedObjectSubclass_EconItem_
		= reinterpret_cast<CEconItem * (__stdcall*)()>(
			*reinterpret_cast<uintptr_t*>(Utils::PatternScan(GetModuleHandleA("client.dll"), "C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4 C7 45 ? ? ? ? ? 50 C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? C7 45 ? ? ? ? ? E8 ? ? ? ? 83 F8 FF 75 09 8D 45 E4 50 E8 ? ? ? ? 8D 45 E4") + 3)
			);
	return fnCreateSharedObjectSubclass_EconItem_();
}

Options::Sticker nostickers[5];
int knifelistid1[] = { 500,505,506,507,508,509,512,514,515,516,517,518,519,520,521,522,523,525 };
int glovelistid1[] = { 0,5032,5027,5030,5031,5033,5034,5035 };

void InventoryChanger::SetSkin(int definitionindex, int stattrack, int paintkit, int seed, float wear, int origin, int rarity, int quality, std::string customname, std::string customdesc, Options::Sticker* sticker)
{
	if(definitionindex>=500&&definitionindex<=525)
	{ 
		g_Options.misc_settings.knifemodel = std::distance(knifelistid1, std::find(std::begin(knifelistid1), std::end(knifelistid1), definitionindex));//fuck
		g_Options.skins_settings[0].stattrack = stattrack;
		g_Options.skins_settings[0].paintkit = paintkit;
		g_Options.skins_settings[0].seed = seed;
		g_Options.skins_settings[0].wear = wear;
	}
	else if (definitionindex >= 5027 && definitionindex <= 5035)
	{
		g_Options.misc_settings.glovemodel = std::distance(glovelistid1, std::find(std::begin(glovelistid1), std::end(glovelistid1), definitionindex));
		g_Options.misc_settings.glovepaintkit = paintkit;
	}
	else if (definitionindex >= 1 && definitionindex <= 70)
	{
		C_BaseCombatWeapon* weapon; auto index = weapon->getWeaponIndex(definitionindex);
		if (index == 0)
			return;
		g_Options.skins_settings[index].stattrack = stattrack;
		g_Options.skins_settings[index].paintkit = paintkit;
		g_Options.skins_settings[index].seed = seed;
		g_Options.skins_settings[index].wear = wear;
		std::memcpy(g_Options.skins_settings[index].stickers, sticker, ARRAYSIZE(g_Options.skins_settings[index].stickers));
	}

}

void InventoryChanger::Run(int definitionindex, int stattrack, int paintkit, int seed, float wear, int origin, int rarity, int quality, std::string customname, std::string customdesc, Options::Sticker* sticker)
{
		//const static auto LocalInventory = **reinterpret_cast<CPlayerInventory***>(Utils::PatternScan(GetModuleHandleA("client.dll"), "8B 3D ? ? ? ? 85 FF 74 1A") + 0x2); //"56 8B F1 C7 ? ? ? ? ? 8D 4E 2C" ?
		const static auto g_CSInventoryManager = *reinterpret_cast<CSInventoryManager**>(Utils::PatternScan(GetModuleHandleA("client.dll"), "B9 ?? ?? ?? ?? 8D 44 24 10 89 54 24 14") + 0x1);
		auto LocalInventory = g_CSInventoryManager->GetLocalPlayerInventory();
		auto Item = CreateEconItem();
		*Item->GetAccountID() = LocalInventory->GetSteamID();
		*Item->GetDefIndex() = definitionindex;
		*Item->GetItemID() = RandomInt(1, 50000);
		*Item->GetInventory() = 0;
		*Item->GetFlags() = 0;
		*Item->GetOriginalID() = 0;

		for (int i = 0; i < 5; i++)
		{
			if (sticker[i].enabled)
				Item->AddSticker(i, sticker[i].kit, sticker[i].wear, sticker[i].scale, sticker[i].rotation);
		}

		if(stattrack!=-1)
		Item->SetStatTrak((float)stattrack);
		Item->SetPaintKit(paintkit);
		Item->SetPaintSeed(seed);
		Item->SetPaintWear(wear);
		Item->SetOrigin(origin);
		Item->SetRarity((ItemRarity)rarity);
		if(quality!=0)
		Item->SetQuality((ItemQuality)quality);
		Item->SetLevel(1);
		Item->SetInUse(false);
		if(customname.size()!=0)
		Item->SetCustomName(customname.c_str());
		if (customdesc.size() != 0)
		Item->SetCustomDesc(customdesc.c_str());
		LocalInventory->GetBaseTypeCache()->AddObject(Item);
		LocalInventory->soCreated(LocalInventory->getSOID(), (SharedObject*)Item, 4);

		std::add_pointer_t<C_EconItemView* __cdecl(std::uint64_t itemID)> findOrCreateEconItemViewForItemID;
		void(__thiscall * clearInventoryImageRGBA)(C_EconItemView * itemView);
		findOrCreateEconItemViewForItemID = relativeToAbsolute<decltype(findOrCreateEconItemViewForItemID)>((uintptr_t)(Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? 8B CE 83 C4 08")) + 1);
		clearInventoryImageRGBA = reinterpret_cast<decltype(clearInventoryImageRGBA)>(Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 81 EC ? ? ? ? 57 8B F9 C7 47"));


		if (const auto view = findOrCreateEconItemViewForItemID(*Item->GetItemID()))
			clearInventoryImageRGBA(view);
		//g_CVar->ConsolePrintf(std::to_string((std::uintptr_t)Item).c_str());
		//LocalInventory->AddEconItem(Item, 0, 0, 0);
}

int getIndexWeapon(int definitionId)
{
	switch (definitionId) {
	default: return -1;

	case 1: return ItemDefinitionIndex::WEAPON_GLOCK;
	case 2: return ItemDefinitionIndex::WEAPON_HKP2000;
	case 3: return ItemDefinitionIndex::WEAPON_USP_SILENCER;
	case 4: return ItemDefinitionIndex::WEAPON_ELITE;
	case 5: return ItemDefinitionIndex::WEAPON_P250;
	case 6: return ItemDefinitionIndex::WEAPON_TEC9;
	case 7: return ItemDefinitionIndex::WEAPON_FIVESEVEN;
	case 8: return ItemDefinitionIndex::WEAPON_CZ75A;
	case 9: return ItemDefinitionIndex::WEAPON_DEAGLE;
	case 10: return ItemDefinitionIndex::WEAPON_REVOLVER;

	case 11: return ItemDefinitionIndex::WEAPON_NOVA;
	case 12: return ItemDefinitionIndex::WEAPON_XM1014;
	case 13: return ItemDefinitionIndex::WEAPON_SAWEDOFF;
	case 14: return ItemDefinitionIndex::WEAPON_MAG7;
	case 15: return ItemDefinitionIndex::WEAPON_M249;
	case 16: return ItemDefinitionIndex::WEAPON_NEGEV;

	case 17: return ItemDefinitionIndex::WEAPON_MAC10;
	case 18: return ItemDefinitionIndex::WEAPON_MP9;
	case 19: return ItemDefinitionIndex::WEAPON_MP7;
	case 20: return ItemDefinitionIndex::WEAPON_MP5;
	case 21: return ItemDefinitionIndex::WEAPON_UMP45;
	case 22: return ItemDefinitionIndex::WEAPON_P90;
	case 23: return ItemDefinitionIndex::WEAPON_BIZON;

	case 24: return ItemDefinitionIndex::WEAPON_GALILAR;
	case 25: return ItemDefinitionIndex::WEAPON_FAMAS;
	case 26: return ItemDefinitionIndex::WEAPON_AK47;
	case 27: return ItemDefinitionIndex::WEAPON_M4A1;
	case 28: return ItemDefinitionIndex::WEAPON_M4A1_SILENCER;
	case 29: return ItemDefinitionIndex::WEAPON_SSG08;
	case 30: return ItemDefinitionIndex::WEAPON_SG556;
	case 31: return ItemDefinitionIndex::WEAPON_AUG;
	case 32: return ItemDefinitionIndex::WEAPON_AWP;
	case 33: return ItemDefinitionIndex::WEAPON_G3SG1;
	case 34: return ItemDefinitionIndex::WEAPON_SCAR20;
	}
}

void InventoryChanger::AddToInventory()
{
	for (int i = 0; i < 35; i++)
	{
		auto skin = g_Options.skins_settings[i];
		if (!skin.enabled)
			continue;
		if (i == 0)
			Run(knifelistid1[g_Options.misc_settings.knifemodel], skin.stattrack, skin.paintkit, skin.seed, skin.wear, 8, ItemRarity::ITEM_RARITY_IMMORTAL, skin.stattrack>0?ItemQuality::ITEM_QUALITY_STRANGE:ItemQuality::ITEM_QUALITY_UNUSUAL, "", "", nostickers);
		else
			Run(getIndexWeapon(i), skin.stattrack, skin.paintkit, skin.seed, skin.wear, 8, ItemRarity::ITEM_RARITY_DEFAULT, skin.stattrack > 0 ? ItemQuality::ITEM_QUALITY_STRANGE : ItemQuality::ITEM_QUALITY_DEFAULT, "", "", skin.stickers);
	}
	if (g_Options.misc_settings.glovemodel != 0)
	{
		Run(glovelistid1[g_Options.misc_settings.glovemodel], 0, g_Options.misc_settings.glovepaintkit, 0, FLT_MIN, 8, ItemRarity::ITEM_RARITY_IMMORTAL, ItemQuality::ITEM_QUALITY_STRANGE, "", "", nostickers);
	}
}

void InventoryChanger::AddToInventoryIndex(int index)
{
	if (index == -1) {
		if(g_Options.misc_settings.glovemodel!=0)
		Run(glovelistid1[g_Options.misc_settings.glovemodel], 0, g_Options.misc_settings.glovepaintkit, 0, FLT_MIN, 8, ItemRarity::ITEM_RARITY_IMMORTAL, ItemQuality::ITEM_QUALITY_STRANGE, "", "", nostickers);
	}

	auto skin = g_Options.skins_settings[index];
	if (!skin.enabled)
		return;
	if (index == 0)
		Run(knifelistid1[g_Options.misc_settings.knifemodel], skin.stattrack, skin.paintkit, skin.seed, skin.wear, 8, ItemRarity::ITEM_RARITY_IMMORTAL, skin.stattrack > 0 ? ItemQuality::ITEM_QUALITY_STRANGE : ItemQuality::ITEM_QUALITY_UNUSUAL, "", "", nostickers);
	else
		Run(getIndexWeapon(index), skin.stattrack, skin.paintkit, skin.seed, skin.wear, 8, ItemRarity::ITEM_RARITY_DEFAULT, skin.stattrack > 0 ? ItemQuality::ITEM_QUALITY_STRANGE : ItemQuality::ITEM_QUALITY_DEFAULT, "", "", skin.stickers);
	
}
InventoryChanger g_Inventory;