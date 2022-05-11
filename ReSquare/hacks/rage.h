#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../helpers/math.hpp"
class Rage {
public:
	void RenderDebugPointInfo();
	float canScan(C_BasePlayer* entity, const Vector& destination, const CCSWeaponInfo* weaponData, Vector location);
	bool retrieveMultipoints(C_BaseEntity* entity, float multiPointsExpansion, Vector(&multiPoints)[9], int desiredHitBox);
	void Aim(CUserCmd* cmd, int lastTickcount);
	int bestindex = -1;
};
extern Rage g_Ragebot;