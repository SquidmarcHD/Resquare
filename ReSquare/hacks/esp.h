#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../helpers/math.hpp"
#include "../render.hpp"
class ESP {
public:
	void RenderBoxes(int num, RECT bbox, C_BaseEntity* ent);
	bool ShouldBreakLBY();
	void RenderChargeUi(int charge, float toshift);
	void RenderPath();
	void RenderHitmarker();
	void RenderQuickPeekInfo();
	void RenderMovementRecorder();
	void RenderGrenades(C_BaseEntity* ent);
	void RenderWeapon(C_BaseCombatWeapon* ent);
	void RenderPlantedC4(C_BaseEntity* ent);
	void RenderItemEsp(C_BaseEntity* ent);
	void RenderPlayers(C_BasePlayer* ent, int num);
	void Render();
};
extern ESP g_ESP;