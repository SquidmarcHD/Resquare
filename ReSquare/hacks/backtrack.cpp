#include "backtrack.h"
#include "rage.h"
#include "../options.hpp"

float DistancePointToLine(Vector Point, Vector LineOrigin, Vector Dir)
{
	auto PointDir = Point - LineOrigin;

	auto TempOffset = PointDir.Dot(Dir) / (Dir.x * Dir.x + Dir.y * Dir.y + Dir.z * Dir.z);
	if (TempOffset < 0.000001f)
		return FLT_MAX;

	auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

	return (Point - PerpendicularPoint).Length();
}
void Backtrack::CreateMoveRecord(CUserCmd* cmd)
{
	if (!g_Options.backtrack_settings.enabled) return;
	if (g_Options.backtrack_settings.time==0) return;

	if (!g_LocalPlayer->IsAlive())
		return;
	
	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto pEntity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
		if (!pEntity || !g_LocalPlayer) continue;
		if (!pEntity->IsPlayer()) continue;
		if (pEntity == g_LocalPlayer) continue;
		if (pEntity->IsDormant()) continue;
		if (!pEntity->IsAlive()) continue;
		if (pEntity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) continue;

		float simtime = pEntity->m_flSimulationTime();
		StoredData record = StoredData{ simtime };
		Vector hitboxPos[19];
		Vector allmultiPoints[19][9];
		for (int bones = 0; bones < 19; bones++)
		{
			hitboxPos[bones] = pEntity->GetHitboxPos(bones);
			if (!g_Options.rage)
				continue;
			static Vector multiPoints[9];
			g_Ragebot.retrieveMultipoints(pEntity, g_Options.misc_settings.multipointsize, multiPoints, bones);
			if (!multiPoints)
				continue;
			g_Ragebot.retrieveMultipoints(pEntity, g_Options.misc_settings.multipointsize, allmultiPoints[bones], bones);
			std::copy(std::begin(allmultiPoints[bones]), std::end(allmultiPoints[bones]), std::begin(record.multipoints[bones]));
		}
		std::copy(std::begin(hitboxPos), std::end(hitboxPos),std::begin(record.hitboxPos));
		record.maxs = pEntity->GetCollideable()->OBBMaxs();
		record.mins = pEntity->GetCollideable()->OBBMins();
		record.absangles = pEntity->m_angAbsAngles();
		record.absorigin = pEntity->m_vecOrigin();
		record.origin = pEntity->m_vecOrigin();
		record.curtime = g_GlobalVars->curtime;
		pEntity->SetupBones(record.matrix, 256, 0x7FF00, g_GlobalVars->curtime);
		TimeWarpData[i][cmd->command_number % (g_Options.backtrack_settings.time)] = record;
	}
}
void Backtrack::CreateMoveSet(CUserCmd* cmd, int &lastbt)
{
	if (!g_Options.backtrack_settings.enabled) return;
	if (g_Options.backtrack_settings.time==0) return;

	int bestTargetIndex = -1;
	float bestFov = FLT_MAX;

	if (!g_LocalPlayer->IsAlive())
		return;
	
	for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
	{
		auto pEntity = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
		if (!pEntity || !g_LocalPlayer) continue;
		if (!pEntity->IsPlayer()) continue;
		if (pEntity == g_LocalPlayer) continue;
		if (pEntity->IsDormant()) continue;
		if (!pEntity->IsAlive()) continue;
		if (pEntity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) continue;

		float simtime = pEntity->m_flSimulationTime();
		Vector ViewDir;
		Math::AngleVectors(cmd->viewangles + (g_LocalPlayer->m_aimPunchAngle() * 2.f), ViewDir);
		auto eyepos = g_LocalPlayer->GetEyePos();
		if (g_Options.misc_settings.prediction)
			eyepos += g_LocalPlayer->m_vecVelocity() * g_GlobalVars->interval_per_tick;
		auto aimpos = pEntity->GetHitboxPos(0);
		if (g_Options.misc_settings.prediction)
			aimpos += pEntity->m_vecVelocity() * g_GlobalVars->interval_per_tick;
		float FOVDistance = DistancePointToLine(aimpos, eyepos, ViewDir);

		if (bestFov > FOVDistance)
		{
			bestFov = FOVDistance;
			bestTargetIndex = i;
		}
	}
	int old_cmdtickcount = cmd->tick_count;
	float bestTargetSimTime = -1;
	if (bestTargetIndex != -1)
	{
		float tempFloat = FLT_MAX;
		Vector ViewDir;
		Math::AngleVectors(cmd->viewangles + (g_LocalPlayer->m_aimPunchAngle() * 2.f), ViewDir);
		for (int t = 0; t < g_Options.backtrack_settings.time; ++t)
		{
			float tempFOVDistance = DistancePointToLine(TimeWarpData[bestTargetIndex][t].hitboxPos[0], g_LocalPlayer->GetEyePos(), ViewDir);
			if (tempFloat > tempFOVDistance && TimeWarpData[bestTargetIndex][t].simtime > g_LocalPlayer->m_flSimulationTime() - 1)
			{
				if (g_LocalPlayer->CanSeePlayer(static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(bestTargetIndex)), TimeWarpData[bestTargetIndex][t].hitboxPos[0])||g_Options.rage)
				{
					tempFloat = tempFOVDistance;
					bestTargetSimTime = TimeWarpData[bestTargetIndex][t].simtime;
				}
			}
		}

		if (bestTargetSimTime >= 0 && cmd->buttons & IN_ATTACK) {
				cmd->tick_count = TIME_TO_TICKS(bestTargetSimTime);

		}
	}				
	lastbt = std::abs(cmd->tick_count-old_cmdtickcount);
}
Backtrack g_Backtrack;