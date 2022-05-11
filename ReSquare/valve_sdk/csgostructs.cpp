#include "csgostructs.hpp"
#include "../Helpers/Math.hpp"
#include "../Helpers/Utils.hpp"

//increase it if valve added some funcs to baseentity :lillulmoa:
constexpr auto VALVE_ADDED_FUNCS = 0ull;

bool C_BaseEntity::IsPlayer()
{
	//index: 152
	//ref: "effects/nightvision"
	//sig: 8B 92 ? ? ? ? FF D2 84 C0 0F 45 F7 85 F6
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 158 + VALVE_ADDED_FUNCS)(this);
}

bool C_BaseEntity::IsLoot() {
	return GetClientClass()->m_ClassID == ClassId_CPhysPropAmmoBox ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropLootCrate ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropRadarJammer ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropWeaponUpgrade ||
		GetClientClass()->m_ClassID == ClassId_CDrone ||
		GetClientClass()->m_ClassID == ClassId_CDronegun ||
		GetClientClass()->m_ClassID == ClassId_CItem_Healthshot ||
		GetClientClass()->m_ClassID == ClassId_CItemCash || 
		GetClientClass()->m_ClassID == ClassId_CBumpMine;
}
bool C_BaseEntity::IsGrenade() {
	return
		GetClientClass()->m_ClassID == ClassId_CSmokeGrenadeProjectile ||
		GetClientClass()->m_ClassID == ClassId_CMolotovProjectile ||
		GetClientClass()->m_ClassID == ClassId_CBaseCSGrenadeProjectile ||
		GetClientClass()->m_ClassID == ClassId_CBreachChargeProjectile ||
		GetClientClass()->m_ClassID == ClassId_CBumpMineProjectile ||
		GetClientClass()->m_ClassID == ClassId_CDecoyProjectile ||
		GetClientClass()->m_ClassID == ClassId_CSensorGrenadeProjectile;
}
bool C_BaseEntity::IsWeapon()
{
	//index: 160
	//ref: "CNewParticleEffect::DrawModel"
	//sig: 8B 80 ? ? ? ? FF D0 84 C0 74 6F 8B 4D A4
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 166 + VALVE_ADDED_FUNCS)(this);
}


bool C_BaseEntity::IsPlantedC4()
{
	return GetClientClass()->m_ClassID == ClassId_CPlantedC4;
}

bool C_BaseEntity::IsDefuseKit()
{
	return GetClientClass()->m_ClassID == ClassId_CBaseAnimating;
}

CCSWeaponInfo* C_BaseCombatWeapon::GetCSWeaponData()
{
	return g_WeaponSystem->GetWpnData(this->m_Item().m_iItemDefinitionIndex());
}

bool C_BaseCombatWeapon::HasBullets()
{
	return !IsReloading() && m_iClip1() > 0;
}

bool C_BaseCombatWeapon::CanFire()
{
	auto owner = this->m_hOwnerEntity().Get();
	if (!owner)
		return false;

	if (IsReloading() || m_iClip1() <= 0)
		return false;

	auto flServerTime = owner->m_nTickBase() * g_GlobalVars->interval_per_tick;

	if (owner->m_flNextAttack() > flServerTime)
		return false;


	return m_flNextPrimaryAttack() <= flServerTime;
}

int C_BaseCombatWeapon::getWeaponIndex(int weaponId)
{
	switch (weaponId) {
	default: return 0;

	case ItemDefinitionIndex::WEAPON_GLOCK: return 1;
	case ItemDefinitionIndex::WEAPON_HKP2000: return 2;
	case ItemDefinitionIndex::WEAPON_USP_SILENCER: return 3;
	case ItemDefinitionIndex::WEAPON_ELITE: return 4;
	case ItemDefinitionIndex::WEAPON_P250: return 5;
	case ItemDefinitionIndex::WEAPON_TEC9: return 6;
	case ItemDefinitionIndex::WEAPON_FIVESEVEN: return 7;
	case ItemDefinitionIndex::WEAPON_CZ75A: return 8;
	case ItemDefinitionIndex::WEAPON_DEAGLE: return 9;
	case ItemDefinitionIndex::WEAPON_REVOLVER: return 10;

	case ItemDefinitionIndex::WEAPON_NOVA: return 11;
	case ItemDefinitionIndex::WEAPON_XM1014: return 12;
	case ItemDefinitionIndex::WEAPON_SAWEDOFF: return 13;
	case ItemDefinitionIndex::WEAPON_MAG7: return 14;
	case ItemDefinitionIndex::WEAPON_M249: return 15;
	case ItemDefinitionIndex::WEAPON_NEGEV: return 16;

	case ItemDefinitionIndex::WEAPON_MAC10: return 17;
	case ItemDefinitionIndex::WEAPON_MP9: return 18;
	case ItemDefinitionIndex::WEAPON_MP7: return 19;
	case ItemDefinitionIndex::WEAPON_MP5: return 20;
	case ItemDefinitionIndex::WEAPON_UMP45: return 21;
	case ItemDefinitionIndex::WEAPON_P90: return 22;
	case ItemDefinitionIndex::WEAPON_BIZON: return 23;

	case ItemDefinitionIndex::WEAPON_GALILAR: return 24;
	case ItemDefinitionIndex::WEAPON_FAMAS: return 25;
	case ItemDefinitionIndex::WEAPON_AK47: return 26;
	case ItemDefinitionIndex::WEAPON_M4A1: return 27;
	case ItemDefinitionIndex::WEAPON_M4A1_SILENCER: return 28;
	case ItemDefinitionIndex::WEAPON_SSG08: return 29;
	case ItemDefinitionIndex::WEAPON_SG556: return 30;
	case ItemDefinitionIndex::WEAPON_AUG: return 31;
	case ItemDefinitionIndex::WEAPON_AWP: return 32;
	case ItemDefinitionIndex::WEAPON_G3SG1: return 33;
	case ItemDefinitionIndex::WEAPON_SCAR20: return 34;
	}
}

int C_BaseCombatWeapon::getWeaponStandalone(int weaponId)
{
	switch (weaponId) {
	default: return -1;

	case ItemDefinitionIndex::WEAPON_GLOCK: return 8;
	case ItemDefinitionIndex::WEAPON_HKP2000: return 9;
	case ItemDefinitionIndex::WEAPON_USP_SILENCER: return 10;
	case ItemDefinitionIndex::WEAPON_ELITE: return 11;
	case ItemDefinitionIndex::WEAPON_P250: return 12;
	case ItemDefinitionIndex::WEAPON_TEC9: return 13;
	case ItemDefinitionIndex::WEAPON_FIVESEVEN: return 14;
	case ItemDefinitionIndex::WEAPON_CZ75A: return 15;
	case ItemDefinitionIndex::WEAPON_DEAGLE: return 16;
	case ItemDefinitionIndex::WEAPON_REVOLVER: return 17;

	case ItemDefinitionIndex::WEAPON_NOVA: return 18;
	case ItemDefinitionIndex::WEAPON_XM1014: return 19;
	case ItemDefinitionIndex::WEAPON_SAWEDOFF: return 20;
	case ItemDefinitionIndex::WEAPON_MAG7: return 21;
	case ItemDefinitionIndex::WEAPON_M249: return 22;
	case ItemDefinitionIndex::WEAPON_NEGEV: return 23;

	case ItemDefinitionIndex::WEAPON_MAC10: return 24;
	case ItemDefinitionIndex::WEAPON_MP9: return 25;
	case ItemDefinitionIndex::WEAPON_MP7: return 26;
	case ItemDefinitionIndex::WEAPON_MP5: return 27;
	case ItemDefinitionIndex::WEAPON_UMP45: return 28;
	case ItemDefinitionIndex::WEAPON_P90: return 29;
	case ItemDefinitionIndex::WEAPON_BIZON: return 30;

	case ItemDefinitionIndex::WEAPON_GALILAR: return 31;
	case ItemDefinitionIndex::WEAPON_FAMAS: return 32;
	case ItemDefinitionIndex::WEAPON_AK47: return 33;
	case ItemDefinitionIndex::WEAPON_M4A1: return 34;
	case ItemDefinitionIndex::WEAPON_M4A1_SILENCER: return 35;
	case ItemDefinitionIndex::WEAPON_SSG08: return 36;
	case ItemDefinitionIndex::WEAPON_SG556: return 37;
	case ItemDefinitionIndex::WEAPON_AUG: return 38;
	case ItemDefinitionIndex::WEAPON_AWP: return 39;
	case ItemDefinitionIndex::WEAPON_G3SG1: return 40;
	case ItemDefinitionIndex::WEAPON_SCAR20: return 41;
	}
}
int C_BaseCombatWeapon::getWeaponClass(int weaponId)
{
	switch (weaponId) {
	default: return -1;

	case ItemDefinitionIndex::WEAPON_GLOCK:
	case ItemDefinitionIndex::WEAPON_HKP2000:
	case ItemDefinitionIndex::WEAPON_USP_SILENCER:
	case ItemDefinitionIndex::WEAPON_ELITE:
	case ItemDefinitionIndex::WEAPON_P250:
	case ItemDefinitionIndex::WEAPON_TEC9: 
	case ItemDefinitionIndex::WEAPON_FIVESEVEN:
	case ItemDefinitionIndex::WEAPON_CZ75A: return 1;
	case ItemDefinitionIndex::WEAPON_DEAGLE:
	case ItemDefinitionIndex::WEAPON_REVOLVER: return 2;

	case ItemDefinitionIndex::WEAPON_NOVA:
	case ItemDefinitionIndex::WEAPON_XM1014:
	case ItemDefinitionIndex::WEAPON_SAWEDOFF:
	case ItemDefinitionIndex::WEAPON_MAG7: return 3;
	case ItemDefinitionIndex::WEAPON_M249:
	case ItemDefinitionIndex::WEAPON_NEGEV: return 4;

	case ItemDefinitionIndex::WEAPON_MAC10:
	case ItemDefinitionIndex::WEAPON_MP9:
	case ItemDefinitionIndex::WEAPON_MP7:
	case ItemDefinitionIndex::WEAPON_MP5:
	case ItemDefinitionIndex::WEAPON_UMP45:
	case ItemDefinitionIndex::WEAPON_P90:
	case ItemDefinitionIndex::WEAPON_BIZON: return 5;

	case ItemDefinitionIndex::WEAPON_GALILAR:
	case ItemDefinitionIndex::WEAPON_FAMAS:
	case ItemDefinitionIndex::WEAPON_AK47:
	case ItemDefinitionIndex::WEAPON_M4A1:
	case ItemDefinitionIndex::WEAPON_M4A1_SILENCER:
	case ItemDefinitionIndex::WEAPON_SG556:
	case ItemDefinitionIndex::WEAPON_AUG: return 6;
	case ItemDefinitionIndex::WEAPON_SSG08:
	case ItemDefinitionIndex::WEAPON_AWP:
	case ItemDefinitionIndex::WEAPON_G3SG1:
	case ItemDefinitionIndex::WEAPON_SCAR20: return 7;
	}
}
float C_BaseCombatWeapon::getWeaponPenetration(int weaponId)//haha sex joke ghetto fix
{
	switch (weaponId) {
	default: return 1;

	case ItemDefinitionIndex::WEAPON_DEAGLE:
	case ItemDefinitionIndex::WEAPON_REVOLVER:
	case ItemDefinitionIndex::WEAPON_M249:
	case ItemDefinitionIndex::WEAPON_NEGEV:
	case ItemDefinitionIndex::WEAPON_GALILAR:
	case ItemDefinitionIndex::WEAPON_FAMAS:
	case ItemDefinitionIndex::WEAPON_AK47:
	case ItemDefinitionIndex::WEAPON_M4A1:
	case ItemDefinitionIndex::WEAPON_M4A1_SILENCER:
	case ItemDefinitionIndex::WEAPON_SG556:
	case ItemDefinitionIndex::WEAPON_AUG: return 2.f;
	
	case ItemDefinitionIndex::WEAPON_AWP:
	case ItemDefinitionIndex::WEAPON_SSG08:
	case ItemDefinitionIndex::WEAPON_G3SG1:
	case ItemDefinitionIndex::WEAPON_SCAR20: return 2.5f;
	}
}
bool C_BaseCombatWeapon::IsGrenade()
{
	return GetCSWeaponData()->iWeaponType == WEAPONTYPE_GRENADE;
}

bool C_BaseCombatWeapon::IsGun()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_C4:
		return false;
	case WEAPONTYPE_GRENADE:
		return false;
	case WEAPONTYPE_KNIFE:
		return false;
	case WEAPONTYPE_UNKNOWN:
		return false;
	default:
		return true;
	}
}

bool C_BaseCombatWeapon::IsKnife()
{
	if (this->m_Item().m_iItemDefinitionIndex() == WEAPON_TASER) return false;
	return GetCSWeaponData()->iWeaponType == WEAPONTYPE_KNIFE;
}
int C_BaseCombatWeapon::GetType()
{
	return(GetCSWeaponData()->iWeaponType);
}
bool C_BaseCombatWeapon::IsRifle()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_RIFLE:
		return true;
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	case WEAPONTYPE_SHOTGUN:
		return true;
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsPistol()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_PISTOL:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsSniper()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_SNIPER_RIFLE:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsReloading()
{
	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
	return *(bool*)((uintptr_t)this + inReload);
}

float C_BaseCombatWeapon::GetInaccuracy()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 483 + VALVE_ADDED_FUNCS)(this);
}

float C_BaseCombatWeapon::GetSpread()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 452 + VALVE_ADDED_FUNCS)(this);
}

void C_BaseCombatWeapon::UpdateAccuracyPenalty()
{
	CallVFunction<void(__thiscall*)(void*)>(this, 484 + VALVE_ADDED_FUNCS)(this);
}

float C_BaseCombatWeapon::GetFireRate()
{
	switch (m_Item().m_iItemDefinitionIndex()) {
	default: return 0.21875f;

	case ItemDefinitionIndex::WEAPON_GLOCK: return 0.15f;
	case ItemDefinitionIndex::WEAPON_HKP2000: return 0.170f;
	case ItemDefinitionIndex::WEAPON_USP_SILENCER: return 0.170f;
	case ItemDefinitionIndex::WEAPON_ELITE: return 0.12f;
	case ItemDefinitionIndex::WEAPON_P250: return 0.15f;
	case ItemDefinitionIndex::WEAPON_TEC9: return 0.12f;
	case ItemDefinitionIndex::WEAPON_FIVESEVEN: return 0.15f;
	case ItemDefinitionIndex::WEAPON_CZ75A: return 0.1f;
	case ItemDefinitionIndex::WEAPON_DEAGLE: return 0.21875f;
	case ItemDefinitionIndex::WEAPON_REVOLVER: return 0.21875f;
	case ItemDefinitionIndex::WEAPON_NOVA: return 0.21875f;
	case ItemDefinitionIndex::WEAPON_XM1014: return 0.21875f;
	case ItemDefinitionIndex::WEAPON_SAWEDOFF: return 0.21875f;
	case ItemDefinitionIndex::WEAPON_MAG7: return 0.21875f;
	case ItemDefinitionIndex::WEAPON_M249: return 0.08f;
	case ItemDefinitionIndex::WEAPON_NEGEV: return 0.075f;
	case ItemDefinitionIndex::WEAPON_MAC10: return 0.075f;
	case ItemDefinitionIndex::WEAPON_MP9: return 0.07f;
	case ItemDefinitionIndex::WEAPON_MP7: return 0.08f;
	case ItemDefinitionIndex::WEAPON_MP5: return 0.08f;
	case ItemDefinitionIndex::WEAPON_UMP45: return 0.090f;
	case ItemDefinitionIndex::WEAPON_P90: return 0.07f;
	case ItemDefinitionIndex::WEAPON_BIZON: return 0.08f;
	case ItemDefinitionIndex::WEAPON_GALILAR: return 0.09f;
	case ItemDefinitionIndex::WEAPON_FAMAS: return 0.09f;
	case ItemDefinitionIndex::WEAPON_AK47: return 0.1f;
	case ItemDefinitionIndex::WEAPON_M4A1: return 0.09f;
	case ItemDefinitionIndex::WEAPON_M4A1_SILENCER: return 0.1f;
	case ItemDefinitionIndex::WEAPON_SG556: return 0.11f;
	case ItemDefinitionIndex::WEAPON_AUG: return 0.1f;
	case ItemDefinitionIndex::WEAPON_SSG08: return 0.21875f;
	case ItemDefinitionIndex::WEAPON_AWP: return 0.21875f;
	case ItemDefinitionIndex::WEAPON_G3SG1: return 0.21875f;
	case ItemDefinitionIndex::WEAPON_SCAR20: return 0.21875f;
	}
}

CUtlVector<IRefCounted*>& C_BaseCombatWeapon::m_CustomMaterials()
{	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "83 BE ? ? ? ? ? 7F 67") + 2) - 12;
	return *(CUtlVector<IRefCounted*>*)((uintptr_t)this + inReload);
}

bool* C_BaseCombatWeapon::m_bCustomMaterialInitialized()
{
	static auto currentCommand = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "C6 86 ? ? ? ? ? FF 50 04") + 2);
	return (bool*)((uintptr_t)this + currentCommand);
}

CUserCmd*& C_BasePlayer::m_pCurrentCommand()
{
	static auto currentCommand = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "89 BE ? ? ? ? E8 ? ? ? ? 85 FF") + 2);
	return *(CUserCmd**)((uintptr_t)this + currentCommand);
}

int C_BasePlayer::GetNumAnimOverlays()
{
	return *(int*)((DWORD)this + 0x298C);
}
uint32_t* C_BasePlayer::GetSkipFrame()
{
	return (uint32_t*)((uintptr_t)this + 0xA68);
}

AnimationLayer *C_BasePlayer::GetAnimOverlays()
{
	return *(AnimationLayer**)((DWORD)this + 0x2990);
}

AnimationLayer *C_BasePlayer::GetAnimOverlay(int i)
{
	if (i < 15)
		return &GetAnimOverlays()[i];
	return nullptr;
}

int C_BasePlayer::GetSequenceActivity(int sequence)
{
	auto hdr = g_MdlInfo->GetStudiomodel(this->GetModel());

	if (!hdr)
		return -1;

	// sig for stuidohdr_t version: 53 56 8B F1 8B DA 85 F6 74 55
	// sig for C_BaseAnimating version: 55 8B EC 83 7D 08 FF 56 8B F1 74 3D
	// c_csplayer vfunc 242, follow calls to find the function.
	// Thanks @Kron1Q for merge request
	static auto get_sequence_activity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(Utils::PatternScan(GetModuleHandle(L"client.dll"), "55 8B EC 53 8B 5D ? 56 8B F1 83 FB"));

	return get_sequence_activity(this, hdr, sequence);
}

CCSGOPlayerAnimState *C_BasePlayer::GetPlayerAnimState()
{
	return *(CCSGOPlayerAnimState**)((DWORD)this + 0x9960);
}

void C_BasePlayer::UpdateAnimationState(CCSGOPlayerAnimState *state, QAngle angle)
{
	static auto UpdateAnimState = Utils::PatternScan(
		GetModuleHandleA("client.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");

	if (!UpdateAnimState)
		return;

	__asm {
		push 0
	}

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call UpdateAnimState
	}
}

void C_BasePlayer::ResetAnimationState(CCSGOPlayerAnimState *state)
{
	using ResetAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*);
	static auto ResetAnimState = (ResetAnimState_t)Utils::PatternScan(GetModuleHandleA("client.dll"), "56 6A 01 68 ? ? ? ? 8B F1");
	if (!ResetAnimState)
		return;

	ResetAnimState(state);
}

void C_BasePlayer::CreateAnimationState(CCSGOPlayerAnimState *state)
{
	using CreateAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*, C_BasePlayer*);
	static auto CreateAnimState = (CreateAnimState_t)Utils::PatternScan(GetModuleHandleA("client.dll"), "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46");
	if (!CreateAnimState)
		return;

	CreateAnimState(state, this);
}

Vector C_BasePlayer::GetEyePos()
{
	return m_vecOrigin() + m_vecViewOffset();
}

player_info_t C_BasePlayer::GetPlayerInfo()
{
	player_info_t info;
	g_EngineClient->GetPlayerInfo(EntIndex(), &info);
	return info;
}

bool C_BasePlayer::IsAlive()
{
	return m_lifeState() == LIFE_ALIVE;
}

bool C_BasePlayer::IsFlashed()
{
	static auto m_flFlashMaxAlpha = NetvarSys::Get().GetOffset("DT_CSPlayer", "m_flFlashMaxAlpha");
	return *(float*)((uintptr_t)this + m_flFlashMaxAlpha - 0x8) > 200.0;
}

bool C_BasePlayer::HasC4()
{
	static auto fnHasC4
		= reinterpret_cast<bool(__thiscall*)(void*)>(
			Utils::PatternScan(GetModuleHandleW(L"client.dll"), "56 8B F1 85 F6 74 31")
			);

	return fnHasC4(this);
}

Vector C_BasePlayer::GetHitboxPos(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
		auto studio_model = g_MdlInfo->GetStudiomodel(GetModel());
		if (studio_model) {
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) {
				auto
					min = Vector{},
					max = Vector{};

				Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
				Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

				return (min + max) / 2.0f;
			}
		}
	}
	return Vector{};
}

mstudiobbox_t* C_BasePlayer::GetHitbox(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
		auto studio_model = g_MdlInfo->GetStudiomodel(GetModel());
		if (studio_model) {
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) {
				return hitbox;
			}
		}
	}
	return nullptr;
}

bool C_BasePlayer::GetHitboxPos(int hitbox, Vector &output)
{
	if (hitbox >= HITBOX_MAX)
		return false;

	const model_t *model = this->GetModel();
	if (!model)
		return false;

	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(model);
	if (!studioHdr)
		return false;

	matrix3x4_t matrix[MAXSTUDIOBONES];
	if (!this->SetupBones(matrix, MAXSTUDIOBONES, 0x100, 0))
		return false;

	mstudiobbox_t *studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(hitbox);
	if (!studioBox)
		return false;

	Vector min, max;

	Math::VectorTransform(studioBox->bbmin, matrix[studioBox->bone], min);
	Math::VectorTransform(studioBox->bbmax, matrix[studioBox->bone], max);

	output = (min + max) * 0.5f;

	return true;
}

Vector C_BasePlayer::GetBonePos(int bone)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];
	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.0f)) {
		return boneMatrix[bone].at(3);
	}
	return Vector{};
}
bool C_BasePlayer::visibleTo(C_BasePlayer* player, C_BasePlayer* other)
{
	assert(IsAlive());

	if (other->CanSeePlayer(player, player->m_angAbsOrigin() + Vector{ 0.0f, 0.0f, 5.0f }))
		return true;

	if (other->CanSeePlayer(player, player->GetEyePos() + Vector{ 0.0f, 0.0f, 5.0f }))
		return true;

	const auto model = player->GetModel();
	if (!model)
		return false;

	const auto studioModel = g_MdlInfo->GetStudiomodel(model);
	if (!studioModel)
		return false;

	const auto set = studioModel->GetHitboxSet(player->m_nHitboxSet());
	if (!set)
		return false;

	matrix3x4_t boneMatrices[MAXSTUDIOBONES];
	if (!player->SetupBones(boneMatrices, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f))
		return false;

	for (const auto boxNum : { Hitboxes::HITBOX_CHEST, Hitboxes::HITBOX_LEFT_FOREARM, Hitboxes::HITBOX_RIGHT_FOREARM }) {
		const auto hitbox = set->GetHitbox(boxNum);
		if (hitbox && other->CanSeePlayer(player, boneMatrices[hitbox->bone].GetOrigin()))
			return true;
	}

	return false;
}
bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, int hitbox)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto endpos = player->GetHitboxPos(hitbox);

	ray.Init(GetEyePos(), endpos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, const Vector& pos)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	ray.Init(GetEyePos(), pos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

void C_BasePlayer::UpdateClientSideAnimation()
{
	return CallVFunction<void(__thiscall*)(void*)>(this, 224 + VALVE_ADDED_FUNCS)(this);
}

void C_BasePlayer::InvalidateBoneCache()
{
	static DWORD addr = (DWORD)Utils::PatternScan(GetModuleHandleA("client.dll"), "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81");

	*(int*)((uintptr_t)this + 0xA30) = g_GlobalVars->framecount; //we'll skip occlusion checks now
	*(int*)((uintptr_t)this + 0xA28) = 0;//clear occlusion flags

	unsigned long g_iModelBoneCounter = **(unsigned long**)(addr + 10);
	*(unsigned int*)((DWORD)this + 0x2924) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
	*(unsigned int*)((DWORD)this + 0x2690) = (g_iModelBoneCounter - 1); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
}

int C_BasePlayer::m_nMoveType()
{
	return *(int*)((uintptr_t)this + 0x25C);
}

QAngle* C_BasePlayer::GetVAngles()
{
	static auto deadflag = NetvarSys::Get().GetOffset("DT_BasePlayer", "deadflag");
	return (QAngle*)((uintptr_t)this + deadflag + 0x4);
}

void C_BasePlayer::SetVAngles(QAngle vangles)
{
	static auto deadflag = NetvarSys::Get().GetOffset("DT_BasePlayer", "deadflag");
	*(QAngle*)((uintptr_t)this + deadflag + 0x4)=vangles;
}
void C_BasePlayer::SetAbsOrigin(const Vector& origin)
{
	using SetAbsOriginFn = void(__thiscall*)(void*, const Vector& origin);
	static SetAbsOriginFn SetAbsOrigin = (SetAbsOriginFn)Utils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8");
	SetAbsOrigin(this, origin);
}

void C_BasePlayer::SetAbsAngles(const QAngle& angles)
{
	using SetAbsAnglesFn = void(__thiscall*)(void*, const QAngle& angles);
	static SetAbsAnglesFn SetAbsAngles = (SetAbsAnglesFn)Utils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8");
	SetAbsAngles(this, angles);
}
void C_BaseAttributableItem::SetGloveModelIndex(int modelIndex)
{
	return CallVFunction<void(__thiscall*)(void*, int)>(this, 75)(this, modelIndex);
}

void C_BaseViewModel::SendViewModelMatchingSequence(int sequence)
{
	return CallVFunction<void(__thiscall*)(void*, int)>(this, 247 + VALVE_ADDED_FUNCS)(this, sequence);
}

Vector C_BaseEntity::getViewmodelAttachment(int number)
{
	Vector origin;
	CallVFunction<bool(__thiscall*)(void*, int, Vector&)>(this, 84)(this, number, origin);
	return origin;
}

int C_BaseEntity::AttachmentSlot(C_BaseViewModel* viewmodel)
{
	return CallVFunction<int(__thiscall*)(void*, C_BaseViewModel*)>(this, 468)(this, viewmodel);
}

float_t C_BasePlayer::m_flSpawnTime()
{
	return *(float_t*)((uintptr_t)this + 0xA370);
}
