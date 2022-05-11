#include "desync.h"
#include "rage.h"
#include "../options.hpp"
#include <algorithm>
#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);
#define TICKS_TO_TIME(t) ( 	g_GlobalVars->interval_per_tick* (t))
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / g_GlobalVars->interval_per_tick ) )
enum AntiAimPitch_t 
{
	ANTIAIM_PITCH_NONE,

	ANTIAIM_PITCH_DOWN,
	ANTIAIM_PITCH_UP,
	ANTIAIM_PITCH_DANCE,
	ANTIAIM_PITCH_FAKEUP,
	ANTIAIM_PITCH_FAKEDOWN,
	ANTIAIM_PITCH_RANDOM
};
enum AntiAimYaw_t
{
	ANTIAIM_YAW_NONE,

	ANTIAIM_YAW_SPIN,
	ANTIAIM_YAW_STATIC_FORWARD,
	ANTIAIM_YAW_STATIC_RIGHT,
	ANTIAIM_YAW_STATIC_BACKWARDS,
	ANTIAIM_YAW_STATIC_LEFT,
	ANTIAIM_YAW_BACKWARDS,
	ANTIAIM_YAW_LEFT,
	ANTIAIM_YAW_RIGHT,
	ANTIAIM_YAW_SIDE,
	ANTIAIM_YAW_FAKE_LBY1,
	ANTIAIM_YAW_FAKE_LBY2,
	ANTIAIM_YAW_JITTER,
	ANTIAIM_YAW_BACKJITTER,
	ANTIAIM_YAW_FAKE_SIDE_LBY,
	ANTIAIM_YAW_RANDOM,
	ANTIAIM_YAW_CUSTOM,
	ANTIAIM_YAW_FREESTANDING
};
static float nextLBY = -1;
bool ShouldBreakLBY()
{
	if (g_LocalPlayer->m_vecVelocity().Length2D() > 0.1f || !(g_LocalPlayer->m_fFlags() & FL_ONGROUND))
		nextLBY = g_GlobalVars->curtime + 0.22f;
	if (g_GlobalVars->curtime >= nextLBY)
	{
		nextLBY = g_GlobalVars->curtime + 1.1f;
		return true;
	}
	return false;
}

void AntiAim::CorrectMovement(QAngle vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove)
{
	float deltaView;
	float f1;
	float f2;

	if (vOldAngles.yaw < 0.f)
		f1 = 360.0f + vOldAngles.yaw;
	else
		f1 = vOldAngles.yaw;

	if (pCmd->viewangles.yaw < 0.0f)
		f2 = 360.0f + pCmd->viewangles.yaw;
	else
		f2 = pCmd->viewangles.yaw;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);

	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(DEG2RAD(deltaView)) * fOldForward + cos(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
	pCmd->sidemove = sin(DEG2RAD(deltaView)) * fOldForward + sin(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
}
static int antiresolver = 0;
void GetEnemyAntiResolver()
{
	for (int i = 1; i <= g_EngineClient->GetMaxClients(); i++) {
		auto ent = C_BaseEntity::GetEntityByIndex(i);
		auto entity = (C_BasePlayer*)ent;
		if (!entity || !entity->IsAlive())
			continue;
		if (entity == g_LocalPlayer || !entity->IsPlayer())
			continue;
		if (entity->IsDormant() || entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			continue;
		if (std::abs(TIME_TO_TICKS(entity->m_hActiveWeapon()->m_fLastShotTime())-TIME_TO_TICKS(g_GlobalVars->curtime))>1)
			continue;
		if ((std::abs(Math::CalcAngle(entity->GetEyePos(), entity->GetEyePos()).yaw - entity->m_angEyeAngles().yaw) + std::abs(Math::CalcAngle(entity->GetEyePos(), entity->GetEyePos()).pitch - entity->m_angEyeAngles().pitch)) > 13.f)
			continue;
		antiresolver++;
	}
}

struct angle_data {
	float angle;
	float thickness;
	angle_data(const float angle, const float thickness) : angle(angle), thickness(thickness) {}
};

bool TraceToExit(Vector& point, Vector& dir, const float step_size, float max_distance)
{
	float flDistance = 0;

	while (flDistance <= max_distance)
	{
		flDistance += step_size;

		point += dir * flDistance;
		int point_contents = g_EngineTrace->GetPointContents(point, MASK_SHOT_HULL);
		if (!(point_contents & MASK_SHOT_HULL))
		{
			// found first free point
			return true;
		}
	}

	return false;
}

float GetThickness(Vector& start, Vector& end, float distance) {
	Vector dir = end - start;
	Vector step = start;
	if (dir.Length() > distance && distance != -1)
		return -1;
	dir.NormalizeInPlace();
	CTraceFilterWorldOnly filter;
	trace_t trace;
	Ray_t ray;
	float thickness = 0;
	while (true) {
		ray.Init(step, end);
		g_EngineTrace->TraceRay(ray, MASK_SHOT_HULL, &filter, &trace);

		if (!trace.DidHit())
			return thickness;

		const Vector lastStep = trace.endpos;
		step = trace.endpos;

		if ((end - start).Length() <= (step - start).Length())
			break;


		if (!TraceToExit(step, dir, 5, 90))
			return FLT_MAX;

		thickness += (step - lastStep).Length();
	}
	return thickness;
}

float GetBestHeadAngle(bool flip, int cur)
{/*
	Vector position = g_LocalPlayer->GetEyePos();
	Vector head = { 0,0,0 };
	g_LocalPlayer->GetHitboxPos(0, head);
	int leasthits = INT32_MAX;

	float radiusout = Vector(position.x,position.y,0).DistTo(Vector(head.x,head.y,0));
	float step = M_PI * 2.0 / 16;
	float bestang = -1;
	for (float a = 0; a < (M_PI * 2.0); a += step)
	{
		Vector locationout(radiusout * cos(a) + position.x, radiusout * sin(a) + position.y, position.z);
		Vector locationin(radiusin * cos(a) + position.x, radiusin * sin(a) + position.y, position.z);
		int hits = 0;
		for (int i = 1; i <= g_EngineClient->GetMaxClients(); i++) {
			auto ent = C_BaseEntity::GetEntityByIndex(i);
			auto entity = (C_BasePlayer*)ent;
			if (!entity || !entity->IsAlive())
				continue;
			if (entity == g_LocalPlayer || !entity->IsPlayer())
				continue;
			if (entity->IsDormant() || entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
				continue;
			Vector pelvis = { 0,0,0 };
			entity->GetHitboxPos(HITBOX_PELVIS, pelvis);
			if (!g_LocalPlayer->m_hActiveWeapon())
				continue;
			hits+=g_Ragebot.canScan(entity, pelvis, g_LocalPlayer->m_hActiveWeapon()->GetCSWeaponData(), locationout);
			hits+=g_Ragebot.canScan(entity, pelvis, g_LocalPlayer->m_hActiveWeapon()->GetCSWeaponData(), locationin);
		}

		if (hits < leasthits)
		{
			leasthits = hits;
			bestang = RAD2DEG(a);
		}
	}
	return bestang;*/
	float bestang = -1;
	std::vector< angle_data > points;

	auto local_position = g_LocalPlayer->GetEyePos();
	std::vector< float > scanned = {};

	for (auto i = 0; i <= 64; i++) {
		auto enemy = dynamic_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
		if (enemy == nullptr) continue;
		if (enemy == g_LocalPlayer) continue;
		if (!enemy->IsAlive()) continue;
		if (enemy->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) continue;
		if (enemy->IsDormant()) continue;
		if (!enemy->IsPlayer()) continue;

		QAngle view = Math::CalcAngle(local_position,enemy->GetEyePos());

		for (auto y = 0; y < 8; y++) {
			auto ang = Math::NormalizeYaw((y * 45) + view.yaw);
			auto found = false;//check similar angle

			for (auto i2 : scanned)
				if (std::abs(Math::NormalizeYaw(i2 - ang)) < 20.f)
					found = true;

			if (found)
				continue;

			points.emplace_back(ang,-1);
			scanned.push_back(ang);
		}
	}

	for (auto i = 0; i <= 64; i++) {
		auto enemy = dynamic_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
		if (enemy == nullptr) continue;
		if (enemy == g_LocalPlayer) continue;
		if (!enemy->IsAlive()) continue;
		if (enemy->m_iTeamNum() == g_LocalPlayer->m_iTeamNum()) continue;
		if (enemy->IsDormant()) continue;
		if (!enemy->IsPlayer()) continue;

		auto found = false;
		auto points_copy = points;
		auto enemy_eyes = enemy->GetEyePos();

		for (auto& z : points_copy)
		{
			const QAngle tmp(10, z.angle, 0.0f);
			Vector head;
			Math::AngleVectors(tmp, head);
			head *= ((16.0f + 3.0f) + ((16.0f + 3.0f) * sin(DEG2RAD(10.0f)))) + 7.0f;
			head += local_position;
			float distance = -1;
			auto enemy_weapon = enemy->m_hActiveWeapon().Get();
			if (enemy_weapon) {
				auto weapon_data = enemy_weapon->GetCSWeaponData();
				if (weapon_data)
					distance = weapon_data->flRange;
			}
			float local_thickness = GetThickness(head, enemy_eyes, distance);
			z.thickness = local_thickness;

			if (local_thickness > 0) // if theres a thickness of 0 dont use this data
			{
				found = true;
			}
		}

		if (!found)
			continue;

		for (unsigned int z = 0; points_copy.size() > z; z++)
			if (points_copy[z].thickness < points[z].thickness || points[z].thickness == -1)
				points[z].thickness = points_copy[z].thickness;
	}
	int best = 0;
	float worst = 700;
	for (auto& i : points) {
		if (((int)i.thickness > best || i.thickness == -1) && i.thickness != 0)
		{
			best = (int)i.thickness;
			bestang = i.angle;
		}
		if ((i.thickness < worst || i.thickness == -1) && i.thickness != 0)
			worst = i.thickness;
	}
	if(best!=0&&worst<100&&best!=-1)
	return bestang;
	return -1;
}

bool DoAntiAimY(QAngle& angle, int command_number, bool bFlip, int currentAAType)
{
	int aa_type = g_Options.antiaim_settings.antiaim_pitch[currentAAType];

	static float pDance = 0.0f;

	switch (aa_type)
	{
		// lol
	case ANTIAIM_PITCH_NONE:
		break;

	case ANTIAIM_PITCH_DOWN:
		angle.pitch = 89.0f;
		break;
	case ANTIAIM_PITCH_UP:
		angle.pitch = -89.0f;
		break;
	case ANTIAIM_PITCH_RANDOM:
		angle.pitch = RandomInt(-89, 89);
		break;
	case ANTIAIM_PITCH_DANCE:
		pDance += 45.0f;
		if (pDance > 100)
			pDance = 0.0f;
		else if (pDance > 75.f)
			angle.pitch = -89.f;
		else if (pDance < 75.f)
			angle.pitch = 89.f;
		break;
	case ANTIAIM_PITCH_FAKEUP:
		angle.pitch = 540;
		return true;
	case ANTIAIM_PITCH_FAKEDOWN:
		angle.pitch = -540;
		return true;
	}
	return false;
}

void DoAntiAimX(QAngle& angle, bool bFlip, int currentAAType) // yaw
{
	int aa_type = bFlip ? g_Options.antiaim_settings.antiaim_yaw_fake[currentAAType] : g_Options.antiaim_settings.antiaim_yaw[currentAAType];
	if (aa_type == 0)
		return;
	static bool yFlip;
	static bool yFlipFake;
	static bool yFlipJitter;
	static bool yFlipJitterFake;
	float temp;
	int random;
	int maxJitter;
	Vector hitboxpos;
	Vector eyepos;
	QAngle aimvector;
	float bestfov = 9999;
	aimvector.pitch = 0;
	aimvector.yaw = 0;
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
		QAngle currentRotation;
		if (!entity->GetHitboxPos(0, hitboxpos))
			continue;
		currentRotation = Math::CalcAngle(eyepos, hitboxpos) - *g_LocalPlayer->GetVAngles();
		Math::Normalize3(currentRotation);
		float fov = std::hypot(currentRotation.pitch, currentRotation.yaw);
		if (fov < bestfov) {
			bestfov = fov;
			aimvector = Math::CalcAngle(eyepos, hitboxpos);
		}
	}

	GetEnemyAntiResolver();
	if (g_Options.antiaim_settings.at_enemy[currentAAType]&&bestfov!=9999)
		angle.yaw = aimvector.yaw;
	if (aa_type == 9) {
		float bestang = GetBestHeadAngle(bFlip,currentAAType);
		if (bestang != -1)
			angle.yaw = bestang + (bFlip ? g_Options.antiaim_settings.antiaim_spin_speed_fake[currentAAType] : g_Options.antiaim_settings.antiaim_spin_speed[currentAAType]);
	}
	angle.yaw += (g_Options.antiaim_settings.aaSide ? -1 : 1) * (bFlip ? (g_Options.antiaim_settings.addvalue_fake[currentAAType]) : (g_Options.antiaim_settings.addvalue[currentAAType]));
	if (bFlip) {
		angle.yaw += (g_Options.antiaim_settings.aaSide ? -1 : 1) * ((yFlipJitter ? 1 : -1) * (g_Options.antiaim_settings.jitter_fake[currentAAType]));
		yFlipJitter = !yFlipJitter;
	}
	else
	{
		angle.yaw += (g_Options.antiaim_settings.aaSide ? -1 : 1) * ((yFlipJitterFake ? 1 : -1) * (g_Options.antiaim_settings.jitter[currentAAType]));
		yFlipJitterFake = !yFlipJitterFake;
	}
	switch (aa_type)
	{
	case 1:
		break;
	case 3:
		if (bFlip)
		{
			yFlip ? angle.yaw += 90.f : angle.yaw -= 90.0f;
			yFlip = !yFlip;
			break;
		}
		else
		{
			yFlipFake ? angle.yaw += 90.f : angle.yaw -= 90.0f;
			yFlipFake = !yFlipFake;
			break;
		}
	case 4:
		bFlip ? angle.yaw += g_LocalPlayer->m_flLowerBodyYawTarget() - 90.0f : angle.yaw -= g_LocalPlayer->m_flLowerBodyYawTarget() + 90.0f;
		break;
	case 5:
		if (bFlip) {
			angle.yaw += g_LocalPlayer->m_flLowerBodyYawTarget() - (float)RandomInt(180, 360);
		}
		else
			angle.yaw -= g_LocalPlayer->m_flLowerBodyYawTarget() + RandomInt(30, 61);
		break;
	case 2:
		angle.yaw += fmodf(g_GlobalVars->curtime * ((360.0f / 1.61803398874989484820f) * g_Options.antiaim_settings.antiaim_spin_speed[currentAAType] / 120.f), 360.0f);
		break;
	case 7:
		angle.yaw += RandomInt(-1 * (int)g_Options.antiaim_settings.antiaim_spin_speed[currentAAType], (int)g_Options.antiaim_settings.antiaim_spin_speed[currentAAType]);
		break;
	case 6:
		bFlip ? angle.yaw -= g_LocalPlayer->m_flLowerBodyYawTarget() - (fmodf(g_GlobalVars->curtime * ((360.0f / 1.61803398874989484820f) * g_Options.antiaim_settings.antiaim_spin_speed[currentAAType]), 360.0f)) : angle.yaw += g_LocalPlayer->m_flLowerBodyYawTarget() + 90.0f;
		break;
	case 8:
		switch (antiresolver%5) {
		case 0:
			break;
		case 1:
			angle.yaw += bFlip?g_Options.antiaim_settings.antiaim_spin_speed_fake[currentAAType]:g_Options.antiaim_settings.antiaim_spin_speed[currentAAType];
			break;
		case 2: 
			angle.yaw -= bFlip ? g_Options.antiaim_settings.antiaim_spin_speed_fake[currentAAType] : g_Options.antiaim_settings.antiaim_spin_speed[currentAAType];
			break;
		case 3: 
			angle.yaw += bFlip ? g_Options.antiaim_settings.antiaim_spin_speed_fake[currentAAType] : g_Options.antiaim_settings.antiaim_spin_speed[currentAAType];
			angle.yaw += bFlip ? g_Options.antiaim_settings.antiaim_spin_speed_fake[currentAAType] : g_Options.antiaim_settings.antiaim_spin_speed[currentAAType];
			break;
		case 4:
			angle.yaw -= bFlip ? g_Options.antiaim_settings.antiaim_spin_speed_fake[currentAAType] : g_Options.antiaim_settings.antiaim_spin_speed[currentAAType];
			angle.yaw -= bFlip ? g_Options.antiaim_settings.antiaim_spin_speed_fake[currentAAType] : g_Options.antiaim_settings.antiaim_spin_speed[currentAAType];
			break;
		}
		break;
	case 10:
		angle.yaw += std::sin(g_GlobalVars->curtime * 3.1415) * (bFlip ? g_Options.antiaim_settings.antiaim_spin_speed_fake[currentAAType] : g_Options.antiaim_settings.antiaim_spin_speed[currentAAType]);
		break;
	case 11:
		if(g_GlobalVars->tickcount%64==0)
		angle.yaw += bFlip ? g_Options.antiaim_settings.antiaim_spin_speed_fake[currentAAType] : g_Options.antiaim_settings.antiaim_spin_speed[currentAAType];
		break;
	}
	angle.Normalize();
}
void AntiAim::LegitAA(CUserCmd *cmd, bool& bSendPacket)
{
	if ((cmd->buttons & IN_USE) || (cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_GRENADE1) || (cmd->buttons & IN_GRENADE2) || (cmd->buttons & IN_ATTACK2 && g_LocalPlayer->m_hActiveWeapon()->m_Item().m_iItemDefinitionIndex() == WEAPON_REVOLVER)) {
		bSendPacket = true;
		return;
	}
	if (g_Options.antiaim_settings.legitaa && !(cmd->buttons & IN_ATTACK))
	{
		static bool real = false;
		real = !real;
		float angley = cmd->viewangles.yaw;
		bSendPacket = !real;
		if (!bSendPacket) {
			angley -= g_Options.antiaim_settings.legitaaflip?60:-60;
		}
		if (std::abs(cmd->sidemove) < 5.0f) {
			if (cmd->buttons & IN_DUCK)
				cmd->sidemove = 3.6f;
			else
				cmd->sidemove = 1.1f;
		}
		cmd->viewangles.yaw = angley;
		cmd->viewangles.Normalize();
	}
}
void AntiAim::Run(CUserCmd* cmd, bool& sendPacket)
{
	QAngle oldAngle = cmd->viewangles;
	float oldForward = cmd->forwardmove;
	float oldSideMove = cmd->sidemove;

	QAngle angle = cmd->viewangles;

	auto localplayer = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
	if (!localplayer)
		return;

	auto activeWeapon = (C_BaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(g_LocalPlayer->m_hActiveWeapon());
	if (!activeWeapon)
		return;
	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER)
		return;
	static bool bFlip;
	bFlip = !bFlip;
	int currentAAType = -1;
	if ((!(g_LocalPlayer->m_fFlags() & FL_ONGROUND)) && g_Options.antiaim_settings.overrideenable[3])
		currentAAType = 3;
	else if ((abs(g_LocalPlayer->m_vecVelocity().x) <= 5.f && abs(g_LocalPlayer->m_vecVelocity().y) <= 5.f) && g_Options.antiaim_settings.overrideenable[4])
		currentAAType = 4;
	else if (g_Options.antiaim_settings.overrideenable[2] && InputSys::Get().IsKeyDown(g_Options.misc_settings.slowwalkkey))
		currentAAType = 2;
	else if ((abs(g_LocalPlayer->m_vecVelocity().x) > 5.f || abs(g_LocalPlayer->m_vecVelocity().y) > 5.f) && g_Options.antiaim_settings.overrideenable[1])
		currentAAType = 1;
	else if (g_Options.antiaim_settings.overrideenable[0])
		currentAAType = 0;
	if (currentAAType == -1)
		return;
	if (cmd->tick_count % (g_Options.antiaim_settings.fakelag_amount[currentAAType] + 1) == 0) {
		sendPacket = true;
	}
	else
	{
		sendPacket = false;
	}
	if (!g_Options.antiaim_settings.antiaim_knife && (activeWeapon->GetCSWeaponData()->iWeaponType == WEAPONTYPE_KNIFE))
		sendPacket = true;

	if ((cmd->buttons & IN_USE) || (cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_GRENADE1) || (cmd->buttons & IN_GRENADE2) || (cmd->buttons & IN_ATTACK2 && activeWeapon->m_Item().m_iItemDefinitionIndex() == WEAPON_REVOLVER))
		sendPacket = true;
	else if (!(cmd->buttons & IN_USE) && !(cmd->buttons & IN_ATTACK) && !(cmd->buttons & IN_GRENADE1) && !(cmd->buttons & IN_GRENADE2) && !(cmd->buttons & IN_ATTACK2 && activeWeapon->m_Item().m_iItemDefinitionIndex() == WEAPON_REVOLVER))
	{
		auto weapon = g_LocalPlayer->m_hActiveWeapon();
		if (weapon) {
			if (((g_Options.antiaim_settings.antiaim_knife&&(activeWeapon->GetCSWeaponData()->iWeaponType == WEAPONTYPE_KNIFE))|| (activeWeapon->GetCSWeaponData()->iWeaponType != WEAPONTYPE_KNIFE))&& !(activeWeapon->GetCSWeaponData()->iWeaponType == WEAPONTYPE_GRENADE))
			{
				auto shouldbreak = ShouldBreakLBY();
				if (g_Options.antiaim_settings.lbybreak[currentAAType] && shouldbreak) {
					sendPacket = false;
				}
				if (g_Options.antiaim_settings.fakelag_amount[currentAAType] == 0.0f) sendPacket = bFlip;
				else
					bFlip = sendPacket;
				if (DoAntiAimY(angle, cmd->command_number, bFlip, currentAAType)&&!g_Options.antiaim_settings.antiuntrusted)
				{
					cmd->forwardmove = -1 * cmd->forwardmove;
				}
				DoAntiAimX(angle, bFlip, currentAAType);
				if (g_Options.antiaim_settings.lbybreak[currentAAType] && shouldbreak) {
					angle.yaw += 180;
					if (g_Options.antiaim_settings.antiuntrusted)
						angle.Normalize();
				}
				
				if(!g_Options.antiaim_settings.lbybreak[currentAAType])
				{
					if (std::abs(cmd->sidemove) < 5.0f) {
						if (cmd->buttons & IN_DUCK)
							cmd->sidemove = 3.6f;
						else
							cmd->sidemove = 1.1f;
					}
				}
				cmd->viewangles = angle;

				if (g_Options.antiaim_settings.antiaim_antiresolver)
				{
					static bool antiResolverFlip = false;
					if (cmd->viewangles.yaw == g_LocalPlayer->m_flLowerBodyYawTarget())
					{
						if (antiResolverFlip) cmd->viewangles.yaw += 23.f;
						else cmd->viewangles.yaw -= 23.f;

						antiResolverFlip = !antiResolverFlip;
					}
				}
			}
		}
	}
}
AntiAim g_AntiAim;

#define clamp(v, min, max) (v > max ? max : (v < min ? min : v))
#define CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MAX 58.0f
#define CSGO_ANIM_AIMMATRIX_DEFAULT_YAW_MIN -58.0f
#define CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MAX 90.0f
#define CSGO_ANIM_AIMMATRIX_DEFAULT_PITCH_MIN -90.0f
#define CSGO_ANIM_AIM_NARROW_WALK	0.8f
#define CSGO_ANIM_AIM_NARROW_RUN	0.5f
#define CSGO_ANIM_AIM_NARROW_CROUCHMOVING	0.5f
#define CSGO_ANIM_LOWER_CATCHUP_IDLE	100.0f

float AngleDiff(float destAngle, float srcAngle)
{
	float delta;

	delta = fmodf(destAngle - srcAngle, 360.0f);
	if (destAngle > srcAngle)
	{
		if (delta >= 180)
			delta -= 360;
	}
	else
	{
		if (delta <= -180)
			delta += 360;
	}
	return delta;
}

template <class T>
FORCEINLINE T Lerp(float flPercent, T const& A, T const& B)
{
	return A + (B - A) * flPercent;
}

inline float anglemod(float a)
{
	a = (360.f / 65536) * ((int)(a * (65536.f / 360.0f)) & 65535);
	return a;
}


float ApproachAngle(float target, float value, float speed)
{
	target = anglemod(target);
	value = anglemod(value);

	float delta = target - value;

	if (speed < 0)
		speed = -speed;

	if (delta < -180)
		delta += 360;
	else if (delta > 180)
		delta -= 360;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}


//fml
void LocalAnimations::Sync()
{
	//setupvelocity be like
	auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	if (!pLocal) return;
	auto animstate = pLocal->GetPlayerAnimState();
	if (!animstate)
		return;
	// if the player is looking far enough to either side, turn the feet to keep them within the extent of the aim matrix
	auto m_flFootYawLast = animstate->m_flGoalFeetYaw;//?
	auto m_flFootYaw = clamp(animstate->m_flGoalFeetYaw, -360, 360);
	float flEyeFootDelta = AngleDiff(animstate->m_flEyeYaw, m_flFootYaw);

	// narrow the available aim matrix width as speed increases
	float flAimMatrixWidthRange = Lerp(clamp(animstate->m_flFeetSpeedForwardsOrSideWays, 0, 1), 1.0f, Lerp(animstate->m_flStopToFullRunningFraction, CSGO_ANIM_AIM_NARROW_WALK, CSGO_ANIM_AIM_NARROW_RUN));
	if (animstate->m_fDuckAmount > 0)
	{
		flAimMatrixWidthRange = Lerp(animstate->m_fDuckAmount * clamp(animstate->m_flFeetSpeedUnknownForwardOrSideways, 0, 1), flAimMatrixWidthRange, CSGO_ANIM_AIM_NARROW_CROUCHMOVING);
	}

	float flTempYawMax = *(float*)(uintptr_t(animstate)+ 0x334)  * flAimMatrixWidthRange;
	float flTempYawMin = *(float*)(uintptr_t(animstate) + 0x330) * flAimMatrixWidthRange;

	if (flEyeFootDelta > flTempYawMax)
	{
		m_flFootYaw = animstate->m_flEyeYaw - abs(flTempYawMax);
	}
	else if (flEyeFootDelta < flTempYawMin)
	{
		m_flFootYaw = animstate->m_flEyeYaw + abs(flTempYawMin);
	}
	m_flFootYaw = Math::NormalizeYaw(m_flFootYaw);


	// pull the lower body direction towards the eye direction, but only when the player is moving
	if (animstate->m_bOnGround)
	{
		auto m_flVelocityLengthXY = std::min(pLocal->m_vecVelocity().Length(), 260.0f);
		if (m_flVelocityLengthXY > 0.1f)
		{
			m_flFootYaw = ApproachAngle(animstate->m_flEyeYaw, m_flFootYaw, animstate->m_iLastClientSideAnimationUpdateFramecount * (30.0f + 20.0f * animstate->m_flStopToFullRunningFraction));
		}
		else
		{
			m_flFootYaw = ApproachAngle(pLocal->m_flLowerBodyYawTarget(), m_flFootYaw, animstate->m_iLastClientSideAnimationUpdateFramecount * CSGO_ANIM_LOWER_CATCHUP_IDLE);
		}
	}
	pLocal->GetPlayerAnimState()->m_flGoalFeetYaw = m_flFootYaw;
}
/*
bool fresh_tick()
{
	static int old_tick_count;

	if (old_tick_count != g_GlobalVars->tickcount)
	{
		old_tick_count = g_GlobalVars->tickcount;
		return true;
	}

	return false;
}

void LocalAnimations::update_fake_animations()
{
	auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	if (!pLocal) return;
	auto alloc = !animstate;
	auto change = !alloc && handle != &pLocal->GetRefEHandle();
	auto reset = !alloc && !change && pLocal->m_flSpawnTime() != spawntime;

	if (change)
		free(animstate);

	if (reset)
	{
		pLocal->ResetAnimationState(animstate);
		spawntime = pLocal->m_flSpawnTime();
	}

	if (alloc || change)
	{
		animstate = (CCSGOPlayerAnimState*)malloc(sizeof(CCSGOPlayerAnimState));

		if (animstate)
			pLocal->CreateAnimationState(animstate);

		handle = (CBaseHandle*)&pLocal->GetRefEHandle();
		spawntime = pLocal->m_flSpawnTime();
	}

	if (!alloc && !change && !reset)
	{
		float frametime = g_GlobalVars->frametime;

		if (pbSendPacket)
		{
			std::array<AnimationLayer, 13> networked_layers;
			std::memcpy(&networked_layers, pLocal->GetAnimOverlays(), sizeof(AnimationLayer) * 13);

			auto backup_abs_angles = pLocal->m_angAbsAngles();
			auto backup_poses = pLocal->m_flPoseParameter();
			if (pLocal->m_fFlags() & FL_ONGROUND)
				pLocal->m_fFlags() |= FL_ONGROUND;
			else
			{
				if (pLocal->GetAnimOverlays()[4].m_flWeight != 1.f && pLocal->GetAnimOverlays()[5].m_flWeight != 0.f)
					pLocal->m_fFlags() |= FL_ONGROUND;

				if (pLocal->m_fFlags() & FL_ONGROUND)
					pLocal->m_fFlags() &= ~FL_ONGROUND;
			}
			*reinterpret_cast<int*>(pLocal + 0xA68) = g_GlobalVars->frametime;
			g_GlobalVars->frametime = 23.91753135f;
			pLocal->UpdateAnimationState(animstate, pviewangles);
			auto backup = g_LocalPlayer->m_fEffects();
			pLocal->m_fEffects() |= 8u;
			pLocal->SetupBones(g_AntiAim.Desync.bones, 128, 524032 - 66666, false);
			pLocal->m_fEffects() = backup;
			std::memcpy(pLocal->GetAnimOverlays(), &networked_layers, sizeof(AnimationLayer) * 13);
			pLocal->m_flPoseParameter() = backup_poses;
			pLocal->m_angAbsAngles() = backup_abs_angles;
		}
		g_GlobalVars->frametime = frametime;
	}
}

struct infos
{
	std::array<float, 24> m_poses;
	AnimationLayer m_overlays;

};

void LocalAnimations::update_local_animations(CCSGOPlayerAnimState* animstate, updateanims_fn pointer)
{
	auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	if (!pLocal) return;
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;
	const auto state = pLocal->GetPlayerAnimState();

	if (!state)
		return;
	static std::array< infos, 13 > animation_frames;
	std::memcpy(&animation_frames.at(pLocal->EntIndex()).m_overlays, pLocal->GetAnimOverlays(), (sizeof(AnimationLayer) * 13));
	animation_frames.at(pLocal->EntIndex()).m_poses = pLocal->m_flPoseParameter();
	static float proper_abs = state->m_flGoalFeetYaw;
	static std::array<float, 24> sent_pose_params = pLocal->m_flPoseParameter();
	static std::array< AnimationLayer, 13 > o_layers;
	if (fresh_tick()) // Only update animations each tick, though we are doing this each frame.
	{
		//pLocal->m_bClientSideAnimation() = true;
		pLocal->UpdateAnimationState(state, pviewangles);
		pointer(pLocal);
		std::memcpy(&o_layers, pLocal->GetAnimOverlays(), sizeof(AnimationLayer) * 13);
		if (pbSendPacket)
		{
			m_server_abs_rotation = state->m_flGoalFeetYaw;
			proper_abs = state->m_flGoalFeetYaw;
			sent_pose_params = pLocal->m_flPoseParameter();
		}
	}
	//pLocal->m_bClientSideAnimation() = false;
	pLocal->SetAbsAngles(QAngle(0, proper_abs, 0));
	std::memcpy(pLocal->GetAnimOverlays(), &o_layers, sizeof(AnimationLayer) * 13);
	std::memcpy(pLocal->GetAnimOverlays(), &animation_frames.at(pLocal->EntIndex()).m_overlays, (sizeof(AnimationLayer) * 13));
	pLocal->m_flPoseParameter() = animation_frames.at(pLocal->EntIndex()).m_poses;
	pLocal->m_flPoseParameter() = sent_pose_params;
}*/
void LocalAnimations::AnimationFix(ClientFrameStage_t stage)
{
	auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	if (!pLocal) return;
	auto animstate = pLocal->GetPlayerAnimState();
	if (!animstate)
		return;
	if (!g_Options.visual_settings.thirdperson || !pLocal || !pLocal->GetAnimOverlays())
		return;
	if (stage == FRAME_NET_UPDATE_END)
	{
		fake_server_update = false;

		if (pLocal->m_flSimulationTime() != fake_simulation_time)
		{
			fake_server_update = true;
			fake_simulation_time = pLocal->m_flSimulationTime();
		}
		pLocal->GetAnimOverlays()[3].m_flWeight = 0.0f;
		pLocal->GetAnimOverlays()[3].m_flCycle = 0.0f;
		pLocal->GetAnimOverlays()[12].m_flWeight = 0.0f;

		update_fake_animations();
	}
	else if (stage == FRAME_RENDER_START)
	{

		real_server_update = false;

		if (pLocal->m_flSimulationTime() != real_simulation_time)
		{
			real_server_update = true;
			real_simulation_time = pLocal->m_flSimulationTime();
		}

		pLocal->GetAnimOverlays()[3].m_flWeight = 0.0f;
		pLocal->GetAnimOverlays()[3].m_flCycle = 0.0f;
		pLocal->GetAnimOverlays()[12].m_flWeight = 0.0f;

		update_local_animations(animstate);
	}
	/*if (stage == FRAME_RENDER_START)
	{
		auto animstate = g_LocalPlayer->GetPlayerAnimState(); //-V807

		if (!animstate)
			return;
		real_server_update = false;
		fake_server_update = false;

		if (g_LocalPlayer->m_flSimulationTime() != real_simulation_time || g_LocalPlayer->m_flSimulationTime() != fake_simulation_time)
		{
			real_server_update = true;
			fake_server_update = true;

			real_simulation_time = g_LocalPlayer->m_flSimulationTime();
			fake_simulation_time = g_LocalPlayer->m_flSimulationTime();
		}

		g_LocalPlayer->GetAnimOverlays()[3].m_flWeight = 0.0f;
		g_LocalPlayer->GetAnimOverlays()[3].m_flCycle = 0.0f;
		g_LocalPlayer->GetAnimOverlays()[12].m_flWeight = 0.0f;

		update_fake_animations();
		g_LocalPlayer->InvalidateBoneCache();
		update_local_animations(animstate);
	}*/
}

void LocalAnimations::update_fake_animations()
{
	auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	if (!pLocal) return;
	auto alloc = !local_data.animstate;
	auto change = !alloc && handle != &pLocal->GetRefEHandle();
	auto reset = !alloc && !change && pLocal->m_flSpawnTime() != spawntime;

	if (change)
		free(local_data.animstate);

	if (reset)
	{
		pLocal->ResetAnimationState(local_data.animstate);
		spawntime = pLocal->m_flSpawnTime();
	}

	if (alloc || change)
	{
		local_data.animstate = (CCSGOPlayerAnimState*)malloc(sizeof(CCSGOPlayerAnimState));

		if (local_data.animstate)
			pLocal->CreateAnimationState(local_data.animstate);

		handle = (CBaseHandle*)&pLocal->GetRefEHandle();
		spawntime = pLocal->m_flSpawnTime();
	}

	if (!alloc && !change && !reset && fake_server_update)
	{
		/*
		float pose_parameter[24]; //-V688
		memcpy(pose_parameter, &pLocal->m_flPoseParameter(), 24 * sizeof(float));
		if (!pLocal)
			return;
		AnimationLayer layers[15]; //-V688
		if (!pLocal || !pLocal->GetAnimOverlays())
			return;
		memcpy(layers, pLocal->GetAnimOverlays(), pLocal->GetNumAnimOverlays() * sizeof(AnimationLayer));
		if (!pLocal)
			return;
		auto backup_frametime = g_GlobalVars->frametime;
		auto backup_curtime = g_GlobalVars->curtime;

		g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;
		g_GlobalVars->curtime = pLocal->m_flSimulationTime();

		pLocal->UpdateAnimationState(local_data.animstate, local_data.fake_angles);

		local_data.animstate->m_bInHitGroundAnimation = false;
		local_data.animstate->m_fLandingDuckAdditiveSomething = 0.0f;
		local_data.animstate->m_flHeadHeightOrOffsetFromHittingGroundAnimation = 1.0f;

		pLocal->InvalidateBoneCache();
		g_CVar->FindVar("r_jiggle_bones")->SetValue(0);
		auto backup = pLocal->m_fEffects();
		pLocal->m_fEffects() |= 8u;
		pLocal->SetupBones(g_AntiAim.Desync.bones, 256, BONE_USED_BY_ANYTHING, pLocal->m_flSimulationTime());
		pLocal->m_fEffects() = backup;
		if (false)//?
		{
			for (auto& i : g_AntiAim.Desync.bones)
			{
				i[0][3] -= pLocal->GetRenderOrigin().x;
				i[1][3] -= pLocal->GetRenderOrigin().y;
				i[2][3] -= pLocal->GetRenderOrigin().z;
			}
		}

		g_GlobalVars->frametime = backup_frametime;
		g_GlobalVars->curtime = backup_curtime;

		memcpy(&pLocal->m_flPoseParameter(), pose_parameter, 24 * sizeof(float));
		if (!pLocal || !pLocal->GetAnimOverlays())
			return;
		memcpy(pLocal->GetAnimOverlays(), layers, pLocal->GetNumAnimOverlays() * sizeof(AnimationLayer));
		if (!pLocal)
			return;*/
	if (pLocal->m_flSimulationTime() != pLocal->m_flOldSimulationTime())
	{
		AnimationLayer networked_layers[15];
		memcpy(networked_layers, pLocal->GetAnimOverlays(), 15 * sizeof(AnimationLayer));

		auto backup_abs_angles = pLocal->m_angAbsAngles();
		auto backup_poses = pLocal->m_flPoseParameter();

		pLocal->UpdateAnimationState(local_data.animstate, local_data.fake_angles);
		g_CVar->FindVar("r_jiggle_bones")->SetValue(0);
		auto backup = pLocal->m_fEffects();
		pLocal->m_fEffects() |= 8u;
		pLocal->SetupBones(g_AntiAim.Desync.bones, 128, 0x7FF00, g_GlobalVars->curtime);
		pLocal->m_fEffects() = backup;
		memcpy(pLocal->GetAnimOverlays(), networked_layers, 15 * sizeof(AnimationLayer));

		pLocal->m_flPoseParameter() = backup_poses;
		pLocal->m_angAbsAngles() = backup_abs_angles;
	}
	}
}
void LocalAnimations::update_local_animations(CCSGOPlayerAnimState* animstate)
{
	auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	if (!pLocal) return;

	if (tickcount != g_GlobalVars->tickcount)
	{
		tickcount = g_GlobalVars->tickcount;
		if (!pLocal || !pLocal->GetAnimOverlays())
			return;
		memcpy(layers, pLocal->GetAnimOverlays(), pLocal->GetNumAnimOverlays() * sizeof(AnimationLayer));
		if (!pLocal)
			return;
		if (local_data.animstate)
			animstate->m_fDuckAmount = local_data.animstate->m_fDuckAmount;

		animstate->m_iLastClientSideAnimationUpdateFramecount = 0;
		pLocal->UpdateAnimationState(animstate, local_data.real_angles);
		if (real_server_update)
		{
			abs_angles = animstate->m_flGoalFeetYaw;
			memcpy(pose_parameter, &pLocal->m_flPoseParameter(), 24 * sizeof(float));
			if (!pLocal)
				return;
		}
	}
	else
		animstate->m_iLastClientSideAnimationUpdateFramecount = g_GlobalVars->framecount;

	animstate->m_flGoalFeetYaw = pLocal->m_flLowerBodyYawTarget();
	pLocal->SetAbsAngles(QAngle(0.0f, abs_angles, 0.0f));

	memcpy(pLocal->GetAnimOverlays(), layers, pLocal->GetNumAnimOverlays() * sizeof(AnimationLayer));
	if (!pLocal || !pLocal->GetAnimOverlays())
		return;
	memcpy(&pLocal->m_flPoseParameter(), pose_parameter, 24 * sizeof(float));
}
LocalAnimations g_LocalAnimations;