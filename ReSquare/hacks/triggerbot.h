#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../helpers/math.hpp"
class Triggerbot {
public:
	void Trigger(CUserCmd* cmd);
};
extern Triggerbot g_Triggerbot;