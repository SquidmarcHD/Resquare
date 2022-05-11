#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../helpers/math.hpp"
class Aimbot {
public:
	void Aim(CUserCmd* cmd, bool sendpacket, QAngle lastang);
	void RemoveRecoil(CUserCmd* cmd);
};
extern Aimbot g_Aimbot;