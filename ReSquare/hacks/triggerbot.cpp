#include "triggerbot.h"
#include "../options.hpp"
static bool attack = false;
static bool attacklast = false;
static bool scanhit = false;
static float lasttime = -1;
void Triggerbot::Trigger(CUserCmd* cmd) {
	if (g_LocalPlayer->m_flNextAttack() > g_GlobalVars->curtime)
		return;
	auto weapon = g_LocalPlayer->m_hActiveWeapon();
	if (!weapon || !weapon->IsGun())
		return;
	int weaponIndex = weapon->getWeaponStandalone(weapon->m_Item().m_iItemDefinitionIndex());
	if (!g_Options.triggerbot_settings[weaponIndex].enabled) {
		weaponIndex = weapon->getWeaponClass(weapon->m_Item().m_iItemDefinitionIndex());
		if (!g_Options.triggerbot_settings[weaponIndex].enabled) {
			weaponIndex = 0;
			if (!g_Options.triggerbot_settings[0].enabled)
				return;
		}
	}
	static float realtime = g_GlobalVars->realtime;
	if (attack) {
		cmd->buttons &= IN_ATTACK;
		attack = false;
	}
	if (g_LocalPlayer->IsFlashed() && !g_Options.triggerbot_settings[weaponIndex].shootFlashed)
		return;
	if (!g_LocalPlayer->m_bIsScoped() && g_Options.triggerbot_settings[weaponIndex].onlyScoped && weapon->IsSniper())
		return;
	Vector eyepos = g_LocalPlayer->GetEyePos();
	Vector forward;
	QAngle viewangles = cmd->viewangles + g_LocalPlayer->m_aimPunchAngle() * 2.0f;
	Math::AngleVectors(viewangles, forward);
	forward = forward.Normalized();
	Vector endpos = eyepos + (forward * weapon->GetCSWeaponData()->flRange);
	Ray_t ray;
	ray.Init(eyepos, endpos);
	CGameTrace trace;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;
	g_EngineTrace->TraceRay(ray, 0x46004009, &filter, &trace);
	if (!trace.hit_entity)
		return;
	C_BasePlayer* player = nullptr;
	player = (C_BasePlayer*)trace.hit_entity;

	if (!g_Options.triggerbot_settings[weaponIndex].bones[trace.hitbox])
		return;
	if (player->IsPlayer()
		&& player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum()
		&& player->IsAlive() && !player->IsDormant())
	{
		if (lasttime == -1)
			lasttime = std::clock();
		if (std::clock() < lasttime + (float)g_Options.triggerbot_settings[weaponIndex].shotDelay&&!scanhit)
			return;
		lasttime = -1;
		scanhit = true;
		if(!attacklast)
			cmd->buttons |= IN_ATTACK;
		else
			cmd->buttons &= IN_ATTACK;
		attacklast = !attacklast;
		attack = true;
	}
	else{
		attacklast = false;
		scanhit = false;
	}
}
Triggerbot g_Triggerbot;