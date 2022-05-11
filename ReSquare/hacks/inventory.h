#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../helpers/math.hpp"
#include "../options.hpp"

/*template <typename T>
static constexpr auto relativeToAbsolute(uintptr_t address)
{
	return (T)(address + 4 + *reinterpret_cast<std::int32_t*>(address));
}*/

enum ItemQuality
{
	ITEM_QUALITY_DEFAULT,
	ITEM_QUALITY_GENUINE,
	ITEM_QUALITY_VINTAGE,
	ITEM_QUALITY_UNUSUAL,
	ITEM_QUALITY_SKIN,
	ITEM_QUALITY_COMMUNITY,
	ITEM_QUALITY_DEVELOPER,
	ITEM_QUALITY_SELFMADE,
	ITEM_QUALITY_CUSTOMIZED,
	ITEM_QUALITY_STRANGE,
	ITEM_QUALITY_COMPLETED,
	ITEM_QUALITY_UNK2,
	ITEM_QUALITY_TOURNAMENT
};

enum ItemRarity
{
	ITEM_RARITY_DEFAULT,
	ITEM_RARITY_COMMON,
	ITEM_RARITY_UNCOMMON,
	ITEM_RARITY_RARE,
	ITEM_RARITY_MYTHICAL,
	ITEM_RARITY_LEGENDARY,
	ITEM_RARITY_ANCIENT,
	ITEM_RARITY_IMMORTAL
};
class EconItemAttributeDefinition;
class ItemSchema
{
public:
	EconItemAttributeDefinition* getAttributeDefinitionInterface(int index)
	{
		typedef EconItemAttributeDefinition* (__thiscall* tOriginal)(void*, int);
		return CallVFunction<tOriginal>(this, 27)(this, index);
	}
};
class ItemSystem {
public:
	ItemSchema* getItemSchema()
	{
		typedef ItemSchema*(__thiscall* tOriginal)(void*);
		return CallVFunction<tOriginal>(this, 0)(this);
	}
};

class CEconItem
{
	unsigned short* GetEconItemData();
	void UpdateEquippedState(unsigned int state);
public:
	uint32_t* GetInventory();
	uint32_t* GetAccountID();
	uint16_t* GetDefIndex();
	uint64_t* GetItemID();
	uint64_t* GetOriginalID();
	unsigned char* GetFlags();
	void SetQuality(ItemQuality quality);
	void SetRarity(ItemRarity rarity);
	void SetOrigin(int origin);
	void SetLevel(int level);
	void SetInUse(bool in_use);
	void SetCustomName(const char* name);
	void SetCustomDesc(const char* name);
	void SetPaintSeed(float seed);
	void SetPaintKit(float kit);
	void SetPaintWear(float wear);
	void SetStatTrak(int val);
	void AddSticker(int index, int kit, float wear, float scale, float rotation);
	/*
	void setDynamicAttributeValue(CEconItem* thisptr, EconItemAttributeDefinition* attribute, void* value) const noexcept
	{
		reinterpret_cast<void(__thiscall*)(CEconItem*, EconItemAttributeDefinition*, void*)>(Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 3C 53 8B 5D 08 56 57 6A"))(thisptr, attribute, value);
	}
	template<typename TYPE>
	void SetAttributeValue(int index, TYPE val)
	{
		std::add_pointer_t<ItemSystem* __cdecl()> itemSystem;
		itemSystem = relativeToAbsolute<decltype(itemSystem)>((std::uintptr_t)(Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? 0F B7 0F")) + 1);

		if (const auto attribute = itemSystem()->getItemSchema()->getAttributeDefinitionInterface(index)) {
			setDynamicAttributeValue(this, attribute, &val);
			g_CVar->ConsolePrintf("found");
		}
	}*/

	uintptr_t GetItemSchema()
	{
		static auto fnGetItemSchema
			= reinterpret_cast<uintptr_t(__stdcall*)()>(
				Utils::PatternScan(GetModuleHandleA("client.dll"), "A1 ? ? ? ? 85 C0 75 53")
				);
		return fnGetItemSchema();
	}

	template<typename TYPE>
	void SetAttributeValue(int index, TYPE val)
	{
		auto v15 = (DWORD*)GetItemSchema();
		auto v16 = *(DWORD*)(v15[72] + 4 * index);

		static auto fnSetDynamicAttributeValue
			= reinterpret_cast<int(__thiscall*)(CEconItem*, DWORD, void*)>(
				Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 3C 53 8B 5D 08 56 57 6A")
			);

			fnSetDynamicAttributeValue(this, v16, &val);
	}
};
class CSharedObjectTypeCache
{
public:
	void CSharedObjectTypeCache::AddObject(void* obj)
	{
		typedef void(__thiscall* tOriginal)(void*, void*);
		CallVFunction<tOriginal>(this, 1)(this, obj);
	}

	void CSharedObjectTypeCache::RemoveObject(void* obj)
	{
		typedef void(__thiscall* tOriginal)(void*, void*);
		CallVFunction<tOriginal>(this, 3)(this, obj);
	}

	std::vector<CEconItem*> CSharedObjectTypeCache::GetEconItems()
	{
		std::vector<CEconItem*> ret;

		auto size = *reinterpret_cast<size_t*>(this + 0x18);

		auto data = *reinterpret_cast<uintptr_t**>(this + 0x4);

		for (size_t i = 0; i < size; i++)
			ret.push_back(reinterpret_cast<CEconItem*>(data[i]));

		return ret;
	}
};

struct SOID {
	std::uint64_t id;
	std::uint32_t type;
	std::uint32_t padding;
};
class SharedObject;
class CPlayerInventory
{
public:
	void CPlayerInventory::RemoveItem(uint64_t ID)
	{
		static auto fnRemoveItem
			= reinterpret_cast<int(__thiscall*)(void*, int64_t)>(
				Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 56 57 FF 75 0C 8B F1")
				);

		fnRemoveItem(this, ID);
	}

	void CPlayerInventory::RemoveItem(CEconItem* item)
	{
		RemoveItem(*item->GetItemID());
		GetBaseTypeCache()->RemoveObject(item);
	}

	void CPlayerInventory::ClearInventory()
	{
		auto BaseTypeCache = this->GetBaseTypeCache();
		auto items = BaseTypeCache->GetEconItems();
		for (auto item : items)
		{
			RemoveItem(*item->GetItemID());
			BaseTypeCache->RemoveObject(item);
		}
	}
	auto getSOC() noexcept
	{
		return *reinterpret_cast<void**>(std::uintptr_t(this) + 0x90);
	}
	CSharedObjectTypeCache* CPlayerInventory::GetBaseTypeCache()
	{
		CSharedObjectTypeCache* (__thiscall * fnGCSDK_CSharedObjectCache_CreateBaseTypeCache)(void*thisptr, int classID);
		fnGCSDK_CSharedObjectCache_CreateBaseTypeCache
			= relativeToAbsolute<decltype(fnGCSDK_CSharedObjectCache_CreateBaseTypeCache)>(
				(uintptr_t)(Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? 8D 4D 0F")) + 1
				);


		return fnGCSDK_CSharedObjectCache_CreateBaseTypeCache(getSOC(), 1);
	}

	uint32_t CPlayerInventory::GetSteamID()
	{
		return *reinterpret_cast<uint32_t*>(this + 0x8);
	}

	CUtlVector< C_EconItemView* >* CPlayerInventory::GetInventoryItems()
	{
		return reinterpret_cast<CUtlVector<C_EconItemView*>*>(this + 0x2C);
	}

	C_EconItemView* CPlayerInventory::getInventoryItemByItemID(unsigned long long itemID) {
		static auto oGetInventoryItemByItemID = (C_EconItemView * (__thiscall*)(void*, unsigned long long))(Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? 8B 33 8B D0")+0x1);
		auto econ = oGetInventoryItemByItemID(this, itemID);
		if (!econ || !*(BYTE*)((unsigned int)econ + 0x204))
			return nullptr;
		else
			return econ;
	}
	/*
	bool CPlayerInventory::AddEconItem(CEconItem* item, int a3, int a4, char a5)
	{
		static auto fnAddEconItem
			= reinterpret_cast<C_EconItemView * (__thiscall*)(void*, CEconItem*, int, int, char)>(
				Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 A1 ? ? ? ? 83 EC 14 53 56 57 8B F9 8B")
				);

		GetBaseTypeCache()->AddObject(item);


		auto ret = fnAddEconItem(this, item, a3, a4, a5);

		if (ret)
		{
			auto i = getInventoryItemByItemID(*item->GetItemID());

			*reinterpret_cast<bool*>((uintptr_t)i + 0xA1) = 1;
		}

		return ret;
	}
	*/
	auto getSOID()
	{
		return *reinterpret_cast<SOID*>(std::uintptr_t(this) + 0x8);
	}
	void soCreated(SOID soid, SharedObject* object, int eventt)
	{
		typedef void (__thiscall* tOriginal)(void*, SOID, SharedObject*, int);
		CallVFunction<tOriginal>(this, 0)(this, soid, object, eventt);
	}
	bool CPlayerInventory::AddEconItem(CEconItem* item, int a3, int a4, char a5)
	{
		bool(__thiscall * addEconItem)(CPlayerInventory * thisptr, CEconItem * item, bool updateAckFile, bool writeAckFile, bool checkForNewItems);
		addEconItem = relativeToAbsolute<decltype(addEconItem)>((uintptr_t)(Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? 84 C0 74 E7")) + 1);
		auto ret = addEconItem(this, item, a3, a4, a5);
		if (false)//ret)
		{
			auto i = getInventoryItemByItemID(*item->GetItemID());

			*reinterpret_cast<bool*>((uintptr_t)i + 0xA1) = 1;
		}
		return true;
	}
};
class CSInventoryManager
{
public:
	CPlayerInventory* GetLocalPlayerInventory();
};
class InventoryChanger
{
public:
	void SetSkin(int definitionindex, int stattrack, int paintkit, int seed, float wear, int origin, int rarity, int quality, std::string customname, std::string customdesc, Options::Sticker* sticker);
	void Run(int definitionindex, int stattrack, int paintkit, int seed, float wear, int origin, int rarity, int quality, std::string customname, std::string customdesc, Options::Sticker* sticker);
	void AddToInventory();
	void AddToInventoryIndex(int index);
};
extern InventoryChanger g_Inventory;