#include "aimbot.h"
#include "backtrack.h"
#include "../options.hpp"
#define TICKS_TO_TIME(t) ( 	g_GlobalVars->interval_per_tick* (t))
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
void Aimbot::RemoveRecoil(CUserCmd* cmd)
{
	static QAngle lastAimPunch{ };
	auto weapon = g_LocalPlayer->m_hActiveWeapon();
	if (!weapon || !weapon->IsGun())
		return;
	int weaponIndex = weapon->getWeaponStandalone(weapon->m_Item().m_iItemDefinitionIndex());
	if (!g_Options.aimbot_settings[weaponIndex].RCS) {
		weaponIndex = weapon->getWeaponClass(weapon->m_Item().m_iItemDefinitionIndex());
		if (!g_Options.aimbot_settings[weaponIndex].RCS) {
			weaponIndex = 0;
			if (!g_Options.aimbot_settings[0].RCS)
				return;
		}
	}
	if (g_LocalPlayer->m_iShotsFired() > 0)
	{
		QAngle localAngles{ };
		g_EngineClient->GetViewAngles(&localAngles);
		localAngles -= lastAimPunch - (float)g_Options.aimbot_settings[weaponIndex].RCSamount * -1 / 50.f * g_LocalPlayer->m_aimPunchAngle();
		QAngle backup = cmd->viewangles;
		if (!g_Options.aimbot_settings[weaponIndex].RCSsilent)
		{
			g_EngineClient->SetViewAngles(&localAngles);
			backup+= (float)g_Options.aimbot_settings[weaponIndex].RCSamount / 50.f * g_LocalPlayer->m_aimPunchAngle();
		}
		cmd->viewangles = backup- (float)g_Options.aimbot_settings[weaponIndex].RCSamount / 50.f * g_LocalPlayer->m_aimPunchAngle();
	}
	lastAimPunch = (float)g_Options.aimbot_settings[weaponIndex].RCSamount*-1 / 50.f * g_LocalPlayer->m_aimPunchAngle();
}
void Aimbot::Aim(CUserCmd* cmd, bool sendpacket, QAngle lastang) {
	static QAngle lastangle;
	Vector besthitboxpos;
	Vector hitboxpos;
	Vector eyepos;
	QAngle aimvector;
	static int lasttick, lastaddtick;
	static C_BasePlayer* enemy;
	static Vector lastadd;
	C_BasePlayer* bestplayer;
	float bestfov = 9999;
	aimvector.pitch = 360;
	aimvector.yaw = 360;
	auto weapon = g_LocalPlayer->m_hActiveWeapon();
	if (!weapon || !weapon->IsGun())
		return;
	int weaponIndex = weapon->getWeaponStandalone(weapon->m_Item().m_iItemDefinitionIndex());
	if (!g_Options.aimbot_settings[weaponIndex].enabled) {
		weaponIndex = weapon->getWeaponClass(weapon->m_Item().m_iItemDefinitionIndex());
		if (!g_Options.aimbot_settings[weaponIndex].enabled) {
			weaponIndex = 0;
			if (!g_Options.aimbot_settings[0].enabled){
				enemy = nullptr;
				return;
			}
		}
	}
	if (g_LocalPlayer->IsFlashed() && !g_Options.aimbot_settings[weaponIndex].shootFlashed)
		return;
	if (!g_LocalPlayer->m_bIsScoped() && g_Options.aimbot_settings[weaponIndex].onlyScoped && weapon->IsSniper())
		return;
	eyepos = g_LocalPlayer->GetEyePos();
	for (int i = 1; i <= g_EngineClient->GetMaxClients(); i++) {
		auto ent = C_BaseEntity::GetEntityByIndex(i);
		auto entity = (C_BasePlayer*)ent;
		if (!entity || !entity->IsAlive())
			continue;
		if (entity == g_LocalPlayer || !entity->IsPlayer())
			continue;
		if (entity->IsDormant() || entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			continue;
		for (int bones = 0; bones < 19; bones++)
		{
			QAngle currentRotation;
			if (!g_Options.aimbot_settings[weaponIndex].bones[bones])
				continue;
			if (!entity->GetHitboxPos(bones, hitboxpos))
				continue;
			if (!g_LocalPlayer->CanSeePlayer(entity, hitboxpos))
				continue;
			currentRotation = Math::CalcAngle(eyepos, hitboxpos)-cmd->viewangles;
			Math::Normalize3(currentRotation);
			float fov = std::hypot(currentRotation.pitch, currentRotation.yaw);
			if (fov<bestfov){
				bestfov = fov;
				aimvector = currentRotation;
				bestplayer = entity;
				besthitboxpos = hitboxpos;
			}
		}
		if (g_Options.backtrack_settings.enabled && g_Options.backtrack_settings.shouldtarget && (g_Options.backtrack_settings.time!=0))
		{
			for (int backtrack = 0; backtrack < g_Options.backtrack_settings.time; backtrack++)
			{
				if (!(g_Backtrack.TimeWarpData[i][backtrack].simtime > g_LocalPlayer->m_flSimulationTime() - 1))
					continue;
				for (int bones = 0; bones < 19; bones++)
				{
					QAngle currentRotation;
					if (!g_Options.aimbot_settings[weaponIndex].bones[bones])
						continue;
					if (!g_LocalPlayer->CanSeePlayer(entity, g_Backtrack.TimeWarpData[i][backtrack].hitboxPos[bones]))
						continue;
					currentRotation = Math::CalcAngle(eyepos, g_Backtrack.TimeWarpData[i][backtrack].hitboxPos[bones]) - *g_LocalPlayer->GetVAngles();
					Math::Normalize3(currentRotation);
					float fov = std::hypot(currentRotation.pitch, currentRotation.yaw);
					if (fov < bestfov) {
						bestfov = fov;
						aimvector = currentRotation;
						bestplayer = entity;
						besthitboxpos = hitboxpos;
					}
				}
			}
		}
	}
	if (bestfov > g_Options.aimbot_settings[weaponIndex].fov) {
		enemy = nullptr;
		return;
	}
	if (aimvector.pitch == 360 && aimvector.yaw == 360) {
		enemy = nullptr;
		return;
	}
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	Math::Normalize3(aimvector);
	Vector vMouse = Vector(cmd->mousedx, cmd->mousedy, 0.0f);;
	QAngle vDelta;
	switch (g_Options.aimbot_settings[weaponIndex].aimassist)
	{	
	case 0:
		vDelta = aimvector / std::max(g_Options.aimbot_settings[weaponIndex].smooth/10, 1.f);
		break;
	case 1:
		vDelta = aimvector /aimvector.Length()*180/ (g_Options.aimbot_settings[weaponIndex].smooth);
		break;
	case 2:
		vDelta = aimvector * 600/(bestfov/2) / aimvector.Length()  / (g_Options.aimbot_settings[weaponIndex].smooth);
		break;
	case 3:
		vDelta = aimvector / (g_Options.aimbot_settings[weaponIndex].smooth*15)*vMouse.Length();
		break;
	case 4:
		vDelta = aimvector / aimvector.Length() * vMouse.Length()*5 / (g_Options.aimbot_settings[weaponIndex].smooth);
		break;
	case 5:
		vDelta = vMouse.Length()*aimvector * 600 / (bestfov * 5) / aimvector.Length() / (g_Options.aimbot_settings[weaponIndex].smooth * 10);
		break;
	case 6:
		if(enemy!=bestplayer)		
		{ 		
			enemy = bestplayer;
			lasttick = g_GlobalVars->tickcount;
		}
		if ((TICKS_TO_TIME(lasttick) + 0.2f) <= TICKS_TO_TIME(g_GlobalVars->tickcount))
		{
			if ((TICKS_TO_TIME(lastaddtick) + 0.05f) <= TICKS_TO_TIME(g_GlobalVars->tickcount))
			{
				lastaddtick = g_GlobalVars->tickcount;
				lastadd.x = (float)RandomInt(-20, 20);
				lastadd.y = (float)RandomInt(-20, 20);
				lastadd.z = (float)RandomInt(-20, 20);
			}
			g_CVar->ConsolePrintf(std::to_string(lastadd.x).c_str());
			g_CVar->ConsolePrintf(std::to_string(lastadd.y).c_str());
			g_CVar->ConsolePrintf(std::to_string(lastadd.z).c_str());
			g_CVar->ConsolePrintf("\n");
			QAngle ang = (Math::CalcAngle(eyepos, hitboxpos + lastadd) - cmd->viewangles);
			Math::Normalize3(ang);
			vDelta = ang / ang.Length() * 180 / std::max(g_Options.aimbot_settings[weaponIndex].smooth*100-(g_GlobalVars->tickcount-lasttick), 1.f);
		}
		break;
	}
	if (vDelta.Length() < aimvector.Length())
	{
		aimvector.pitch = vDelta.pitch;
		aimvector.yaw = vDelta.yaw;
	}
	if (g_Options.aimbot_settings[weaponIndex].aimassist == 6)
	{
		QAngle ang = (Math::CalcAngle(eyepos, hitboxpos + lastadd) - cmd->viewangles);
		Math::Normalize3(ang);
		if (vDelta.Length() < ang.Length())
		{
			aimvector.pitch = vDelta.pitch;
			aimvector.yaw = vDelta.yaw;
		}
		else
		{
			aimvector.pitch = ang.pitch;
			aimvector.yaw = ang.yaw;
		}
	}
	cmd->viewangles += aimvector;	
	QAngle viewangles = cmd->viewangles;
	Math::Normalize3(viewangles);
	cmd->viewangles = viewangles;
	if (!g_Options.aimbot_settings[weaponIndex].silent&&sendpacket)
		g_EngineClient->SetViewAngles(&cmd->viewangles);
}
Aimbot g_Aimbot;