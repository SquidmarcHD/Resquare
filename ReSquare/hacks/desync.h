#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../helpers/math.hpp"
#include "../helpers/input.hpp"

using updateanims_fn = void(__thiscall*)(C_BaseEntity*);

struct DesyncChamsMatrices
{
	matrix3x4_t bones[256];
	float simtime;
	float simtimeupdate;
	float simtimereal;
};
class AntiAim {
public:
	DesyncChamsMatrices Desync;
	void CorrectMovement(QAngle vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
	void LegitAA(CUserCmd* pCmd, bool& bSendPacket);
	void Run(CUserCmd* cmd, bool& sendPacket);
	void ManageFake();
};
struct Local_data
{
	bool visualize_lag = false;
	CCSGOPlayerAnimState* animstate = nullptr;

	QAngle stored_real_angles = { 0,0,0 };
	QAngle real_angles = { 0,0,0 };
	QAngle fake_angles = { 0,0,0 };
};
class LocalAnimations
{
public:
	bool real_server_update = false;
	bool fake_server_update = false;

	float real_simulation_time = 0.0f;
	float fake_simulation_time = 0.0f;

	CBaseHandle* handle = nullptr;

	float spawntime = 0.0f;
	float tickcount = 0.0f;

	float abs_angles = 0.0f;
	float pose_parameter[24];
	AnimationLayer layers[15];

	void update_fake_animations();
	void update_local_animations(CCSGOPlayerAnimState* animstate);
	Local_data local_data;
	void Sync();
	void run(ClientFrameStage_t stage);
	void AnimationFix(ClientFrameStage_t stage);
};
extern LocalAnimations g_LocalAnimations;
extern AntiAim g_AntiAim;