#include "misc.h"
#include "../options.hpp"
#include <algorithm>

void Misc::ViewModelCM()
{
	if (g_Options.visual_settings.viewmodelenabled)
	{
		g_CVar->FindVar("viewmodel_offset_x")->m_fnChangeCallbacks.SetSize(false);
		g_CVar->FindVar("viewmodel_offset_y")->m_fnChangeCallbacks.SetSize(false);
		g_CVar->FindVar("viewmodel_offset_z")->m_fnChangeCallbacks.SetSize(false);
		g_CVar->FindVar("viewmodel_offset_x")->SetValue(g_Options.visual_settings.viewmodelx);
		g_CVar->FindVar("viewmodel_offset_y")->SetValue(g_Options.visual_settings.viewmodely);
		g_CVar->FindVar("viewmodel_offset_z")->SetValue(g_Options.visual_settings.viewmodelz);
	}
}

void Misc::ForceCrosshairCM()
{
	if (g_Options.visual_settings.forcecrosshair) {
		g_CVar->FindVar("weapon_debug_spread_show")->SetValue(1);
		g_CVar->FindVar("weapon_debug_spread_gap")->SetValue(2);
	}
	else {
		g_CVar->FindVar("weapon_debug_spread_show")->SetValue(0);
		g_CVar->FindVar("weapon_debug_spread_gap")->SetValue(0.67f);
	}
}

void Misc::FogCM()
{
	g_CVar->FindVar("fog_enable")->m_fnChangeCallbacks.SetSize(false);
	g_CVar->FindVar("fog_enableskybox")->m_fnChangeCallbacks.SetSize(false);
	g_CVar->FindVar("fog_override")->m_fnChangeCallbacks.SetSize(false);
	g_CVar->FindVar("fog_end")->m_fnChangeCallbacks.SetSize(false);
	g_CVar->FindVar("fog_start")->m_fnChangeCallbacks.SetSize(false);
	g_CVar->FindVar("fog_maxdensity")->m_fnChangeCallbacks.SetSize(false);
	g_CVar->FindVar("fog_colorskybox")->m_fnChangeCallbacks.SetSize(false);
	g_CVar->FindVar("fog_endskybox")->m_fnChangeCallbacks.SetSize(false);
	g_CVar->FindVar("fog_startskybox")->m_fnChangeCallbacks.SetSize(false);
	g_CVar->FindVar("fog_maxdensityskybox")->m_fnChangeCallbacks.SetSize(false);
	g_CVar->FindVar("fog_colorskybox")->m_fnChangeCallbacks.SetSize(false);
	g_CVar->FindVar("fog_enable")->SetValue(g_Options.visual_settings.fog.enabled);
	g_CVar->FindVar("fog_enableskybox")->SetValue(g_Options.visual_settings.fog.enabled);
	g_CVar->FindVar("fog_override")->SetValue(g_Options.visual_settings.fog.enabled);
	if (g_Options.visual_settings.fog.enabled) {
		g_CVar->FindVar("fog_end")->SetValue((float)g_Options.visual_settings.fog.end);
		g_CVar->FindVar("fog_start")->SetValue((float)g_Options.visual_settings.fog.start);
		g_CVar->FindVar("fog_maxdensity")->SetValue(g_Options.visual_settings.fog.maxdensity);
		g_CVar->FindVar("fog_color")->SetValue((std::to_string((float)(g_Options.visual_settings.fog.color.x * 255)) + " " + std::to_string((float)(g_Options.visual_settings.fog.color.y * 255)) + " " + std::to_string((float)(g_Options.visual_settings.fog.color.z * 255))).c_str());
		g_CVar->FindVar("fog_endskybox")->SetValue((float)g_Options.visual_settings.fog.end);
		g_CVar->FindVar("fog_startskybox")->SetValue((float)g_Options.visual_settings.fog.start);
		g_CVar->FindVar("fog_maxdensityskybox")->SetValue(g_Options.visual_settings.fog.maxdensity);
		g_CVar->FindVar("fog_colorskybox")->SetValue((std::to_string((float)(g_Options.visual_settings.fog.color.x * 255)) + " " + std::to_string((float)(g_Options.visual_settings.fog.color.y * 255)) + " " + std::to_string((float)(g_Options.visual_settings.fog.color.z * 255))).c_str());
	}
}

void Misc::AmbientCM()
{
	if (g_Options.visual_settings.ambientenabled)
	{
		g_CVar->FindVar("mat_ambient_light_r")->SetValue(g_Options.visual_settings.ambientcolor.x);
		g_CVar->FindVar("mat_ambient_light_g")->SetValue(g_Options.visual_settings.ambientcolor.y);
		g_CVar->FindVar("mat_ambient_light_b")->SetValue(g_Options.visual_settings.ambientcolor.z);
	}
}

static std::vector<std::vector<std::string>> ClanTags
{
		{
	""
	},{
	"^ESQUARE",
	"R^SQUARE",
	"RE^QUARE",
	"RES^UARE",
	"RESQ^ARE",
	"RESQU^RE",
	"RESQUA^E",
	"RESQUAR^",
	"RESQUA^E",
	"RESQU^RE",
	"RESQ^ARE",
	"RES^UARE",
	"RE^QUARE",
	"R^SQUARE",
	"^ESQUARE"
	},{
	"E RESQUAR",
	"RE RESQUA",
	"ARE RESQU",
	"UARE RESQ",
	"QUARE RES",
	"SQUARE RE",
	"ESQUARE R",
	"RESQUARE "
	},{
	"ESQUARE R",
	"SQUARE RE",
	"QUARE RES",
	"UARE RESQ",
	"ARE RESQU",
	"RE RESQUA",
	"E RESQUAR",
	"RESQUARE "
	},{
	"RESQUARE-",
	"RESQUARE/",
	"RESQUARE|",
	"RESQUARE\\",
	"RESQUARE-",
	"RESQUARE/",
	"RESQUARE|",
	"RESQUARE\\"
	},{
	"RESQUARE",
	"RESQUAR",
	"RESQUA",
	"RESQU",
	"RESQ",
	"RES",
	"RE",
	"R",
	" ",
	"R",
	"RE",
	"RES",
	"RESQ",
	"RESQU",
	"RESQUA",
	"RESQUAR"
	},{
	"RESQUARE"
	},{
		""
	}
};

void Misc::Clantag()
{
	if (g_Options.misc_settings.clantag == 0)
		return;
	static float lastTime = 0.0f;
	float currentTime = g_GlobalVars->curtime * 2.0f;
	if (lastTime != currentTime)
	{
		std::string buf = ClanTags[g_Options.misc_settings.clantag][(int)currentTime % ClanTags[g_Options.misc_settings.clantag].size()];
		Utils::SetClantag(buf.c_str());
		lastTime = currentTime;
	}
}

void Misc::Radar()
{
	if (g_Options.misc_settings.radar)
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
			entity->m_bSpotted() = true;
		}
	}
}

void Misc::Bhop(CUserCmd* cmd, bool &sendP)
{
	if (g_Options.misc_settings.bhop)
	{
		static auto wasLastTimeOnGround{ g_LocalPlayer->m_fFlags() & 1 };
		auto chance = rand() % 100;
		bool doBhop = chance < g_Options.misc_settings.bhopHitchance;
		if (!(g_LocalPlayer->m_fFlags() & 1) && g_LocalPlayer->m_nMoveType() != MOVETYPE_LADDER && !wasLastTimeOnGround&&doBhop) {
			cmd->buttons &= ~IN_JUMP;
		}
		else if(cmd->buttons & IN_JUMP) sendP = true;
		wasLastTimeOnGround = g_LocalPlayer->m_fFlags() & 1;
	}
}

void Misc::SlowWalk(CUserCmd* cmd)
{
	auto weapon = g_LocalPlayer->m_hActiveWeapon();
	if (InputSys::Get().IsKeyDown(g_Options.misc_settings.slowwalkkey)) {
		const float maxSpeed = (g_LocalPlayer->m_bIsScoped() ? weapon->GetCSWeaponData()->flSpreadAlt : weapon->GetCSWeaponData()->flSpread) / 3;

		if (abs(cmd->forwardmove) > 10.f && abs(cmd->sidemove) > 10.f) {
			const float maxSpeedRoot = maxSpeed * 0.70710678118654752440f;
			cmd->forwardmove = cmd->forwardmove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
			cmd->sidemove = cmd->sidemove < 0.0f ? -maxSpeedRoot : maxSpeedRoot;
		}
		else if (abs(cmd->forwardmove) > 10.f) {
			cmd->forwardmove = cmd->forwardmove < 0.0f ? -maxSpeed : maxSpeed;
		}
		else if (abs(cmd->sidemove) > 10.f) {
			cmd->sidemove = cmd->sidemove < 0.0f ? -maxSpeed : maxSpeed;
		}
	}
}
#define CheckIfNonValidNumber(x) (fpclassify(x) == FP_INFINITE || fpclassify(x) == FP_NAN || fpclassify(x) == FP_SUBNORMAL)
void Misc::AutoStrafe(CUserCmd* cmd, QAngle backup)
{
	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER)
		return;

	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		return;
	static auto cl_sidespeed = g_CVar->FindVar("cl_sidespeed");
	auto side_speed = cl_sidespeed->GetFloat();

	if (g_Options.misc_settings.autostrafe == 1)
	{
		if (cmd->forwardmove>0)
		{
			QAngle engine_angles;
			g_EngineClient->GetViewAngles(&engine_angles);

			auto velocity = g_LocalPlayer->m_vecVelocity();

			cmd->forwardmove = std::min(5850.0f / velocity.Length2D(), side_speed);
			cmd->sidemove = cmd->command_number % 2 ? side_speed : -side_speed;

			auto yaw_velocity = Math::CalcAngle(Vector(0.0f, 0.0f, 0.0f), velocity).yaw;
			auto ideal_rotation = std::clamp(RAD2DEG(atan2(15.0f, velocity.Length2D())), 0.0f, 45.0f);

			auto yaw_rotation = fabs(yaw_velocity - engine_angles.yaw) + (cmd->command_number % 2 ? ideal_rotation : -ideal_rotation);
			auto ideal_yaw_rotation = yaw_rotation < 5.0f ? yaw_velocity : engine_angles.yaw;

			Math::RotateMovement(cmd, ideal_yaw_rotation);
		}
	}
	else if (g_Options.misc_settings.autostrafe == 2)
	{
		static auto old_yaw = 0.0f;

		auto get_velocity_degree = [](float velocity)
		{
			auto tmp = RAD2DEG(atan(30.0f / velocity));

			if (CheckIfNonValidNumber(tmp) || tmp > 90.0f)
				return 90.0f;

			else if (tmp < 0.0f)
				return 0.0f;
			else
				return tmp;
		};

		if (g_LocalPlayer->m_nMoveType() != MOVETYPE_WALK)
			return;

		auto velocity = g_LocalPlayer->m_vecVelocity();
		velocity.z = 0.0f;

		auto forwardmove = cmd->forwardmove;
		auto sidemove = cmd->sidemove;

		if (velocity.Length2D() < 5.0f && !forwardmove && !sidemove)
			return;

		static auto flip = false;
		flip = !flip;

		auto turn_direction_modifier = flip ? 1.0f : -1.0f;
		auto viewangles = cmd->viewangles;

		if (forwardmove || sidemove)
		{
			cmd->forwardmove = 0.0f;
			cmd->sidemove = 0.0f;

			auto turn_angle = atan2(-sidemove, forwardmove);
			viewangles.yaw += turn_angle * 57.295779513082f;
		}
		else if (forwardmove)
			cmd->forwardmove = 0.0f;

		auto strafe_angle = RAD2DEG(atan(15.0f / velocity.Length2D()));

		if (strafe_angle > 90.0f)
			strafe_angle = 90.0f;
		else if (strafe_angle < 0.0f)
			strafe_angle = 0.0f;

		auto temp = Vector(0.0f, viewangles.yaw - old_yaw, 0.0f);
		temp.y = Math::NormalizeYaw(temp.y);

		auto yaw_delta = temp.y;
		old_yaw = viewangles.yaw;

		auto abs_yaw_delta = fabs(yaw_delta);

		if (abs_yaw_delta <= strafe_angle || abs_yaw_delta >= 30.0f)
		{
			QAngle velocity_angles;
			Math::VectorAngles(velocity, velocity_angles);

			temp = Vector(0.0f, viewangles.yaw - velocity_angles.yaw, 0.0f);
			temp.y = Math::NormalizeYaw(temp.y);

			auto velocityangle_yawdelta = temp.y;
			auto velocity_degree = get_velocity_degree(velocity.Length2D()) * 0.01f;

			if (velocityangle_yawdelta <= velocity_degree || velocity.Length2D() <= 15.0f)
			{
				if (-velocity_degree <= velocityangle_yawdelta || velocity.Length2D() <= 15.0f)
				{
					viewangles.yaw += strafe_angle * turn_direction_modifier;
					cmd->sidemove = side_speed * turn_direction_modifier;
				}
				else
				{
					viewangles.yaw = velocity_angles.yaw - velocity_degree;
					cmd->sidemove = side_speed;
				}
			}
			else
			{
				viewangles.yaw = velocity_angles.yaw + velocity_degree;
				cmd->sidemove = -side_speed;
			}
		}
		else if (yaw_delta > 0.0f)
			cmd->sidemove = -side_speed;
		else if (yaw_delta < 0.0f)
			cmd->sidemove = side_speed;

		auto move = Vector(cmd->forwardmove, cmd->sidemove, 0.0f);
		auto speed = move.Length();

		QAngle angles_move;
		Math::VectorAngles(move, angles_move);

		auto normalized_x = fmod(cmd->viewangles.pitch + 180.0f, 360.0f) - 180.0f;
		auto normalized_y = fmod(cmd->viewangles.yaw + 180.0f, 360.0f) - 180.0f;

		auto yaw = DEG2RAD(normalized_y - viewangles.yaw + angles_move.yaw);

		if (normalized_x >= 90.0f || normalized_x <= -90.0f || cmd->viewangles.pitch >= 90.0f && cmd->viewangles.pitch <= 200.0f || cmd->viewangles.pitch <= -90.0f && cmd->viewangles.pitch <= 200.0f)
			cmd->forwardmove = -cos(yaw) * speed;
		else
			cmd->forwardmove = cos(yaw) * speed;

		cmd->sidemove = sin(yaw) * speed;
	}
	else if (g_Options.misc_settings.autostrafe == 3)
	{
		static auto old_yaw = 0.0f;

		auto get_velocity_degree = [](float velocity)
		{
			auto tmp = RAD2DEG(atan(30.0f / velocity));

			if (CheckIfNonValidNumber(tmp) || tmp > 90.0f)
				return 90.0f;

			else if (tmp < 0.0f)
				return 0.0f;
			else
				return tmp;
		};

		if (g_LocalPlayer->m_nMoveType() != MOVETYPE_WALK)
			return;

		auto velocity = g_LocalPlayer->m_vecVelocity();
		velocity.z = 0;

		static auto flip = false;
		flip = !flip;

		auto turn_direction_modifier = flip ? 1.0f : -1.0f;
		auto viewangles = cmd->viewangles;

		auto strafe_angle = RAD2DEG(atan(15.0f / velocity.Length2D()));

		if (strafe_angle > 90.0f)
			strafe_angle = 90.0f;
		else if (strafe_angle < 0.0f)
			strafe_angle = 0.0f;

		auto temp = Vector(0.0f, viewangles.yaw - old_yaw, 0.0f);
		temp.y = Math::NormalizeYaw(temp.y);

		auto yaw_delta = temp.y;
		old_yaw = viewangles.yaw;

		auto abs_yaw_delta = fabs(yaw_delta);

		if (abs_yaw_delta <= strafe_angle || abs_yaw_delta >= 30.0f)
		{
			QAngle velocity_angles;
			Math::VectorAngles(velocity, velocity_angles);

			temp = Vector(0.0f, viewangles.yaw - velocity_angles.yaw, 0.0f);
			temp.y = Math::NormalizeYaw(temp.y);

			auto velocityangle_yawdelta = temp.y;
			auto velocity_degree = get_velocity_degree(velocity.Length2D()) * 0.01f;

			if (velocityangle_yawdelta <= velocity_degree || velocity.Length2D() <= 15.0f)
			{
				if (-velocity_degree <= velocityangle_yawdelta || velocity.Length2D() <= 15.0f)
				{
					viewangles.yaw += strafe_angle * turn_direction_modifier;
					cmd->sidemove = side_speed * turn_direction_modifier;
				}
				else
				{
					viewangles.yaw = velocity_angles.yaw - velocity_degree;
					cmd->sidemove = side_speed;
				}
			}
			else
			{
				viewangles.yaw = velocity_angles.yaw + velocity_degree;
				cmd->sidemove = -side_speed;
			}
		}
		else if (yaw_delta > 0.0f)
			cmd->sidemove = -side_speed;
		else if (yaw_delta < 0.0f)
			cmd->sidemove = side_speed;

		auto move = Vector(cmd->forwardmove, cmd->sidemove, 0.0f);
		auto speed = move.Length();

		QAngle angles_move;
		Math::VectorAngles(move, angles_move);

		auto normalized_x = fmod(cmd->viewangles.pitch + 180.0f, 360.0f) - 180.0f;
		auto normalized_y = fmod(cmd->viewangles.yaw + 180.0f, 360.0f) - 180.0f;

		auto yaw = DEG2RAD(normalized_y - viewangles.yaw + angles_move.yaw);

		if (normalized_x >= 90.0f || normalized_x <= -90.0f || cmd->viewangles.pitch >= 90.0f && cmd->viewangles.pitch <= 200.0f || cmd->viewangles.pitch <= -90.0f && cmd->viewangles.pitch <= 200.0f) //-V648
			cmd->forwardmove = -cos(yaw) * speed;
		else
			cmd->forwardmove = cos(yaw) * speed;

		cmd->sidemove = sin(yaw) * speed;
	}
}

void Misc::AutoBuy(IGameEvent* event)
{
	if (!strcmp(event->GetName(), "enter_buyzone"))
	{
		switch (g_Options.misc_settings.primary)
		{
		case 0:
			break;
		case 1:
			g_EngineClient->ClientCmd("buy galilar; buy famas;");
			break;
		case 2:
			g_EngineClient->ClientCmd("buy ak47; buy m4a1;");
			break;
		case 3:
			g_EngineClient->ClientCmd("buy ssg08;");
			break;
		case 4:
			g_EngineClient->ClientCmd("buy sg556; buy aug;");
			break;
		case 5:
			g_EngineClient->ClientCmd("buy awp;");
			break;
		case 6:
			g_EngineClient->ClientCmd("buy scar20; buy g3sg1;");
			break;
		case 7:
			g_EngineClient->ClientCmd("buy mac10; buy mp9;");
			break;
		case 8:
			g_EngineClient->ClientCmd("buy mp7;");
			break;
		case 9:
			g_EngineClient->ClientCmd("buy ump45;");
			break;
		case 10:
			g_EngineClient->ClientCmd("buy p90;");
			break;
		case 11:
			g_EngineClient->ClientCmd("buy bizon;");
			break;
		case 12:
			g_EngineClient->ClientCmd("buy nova;");
			break;
		case 13:
			g_EngineClient->ClientCmd("buy xm1014;");
			break;
		case 14:
			g_EngineClient->ClientCmd("buy sawedoff; buy mag7;");
			break;
		case 15:
			g_EngineClient->ClientCmd("buy m249;");
			break;
		case 16:
			g_EngineClient->ClientCmd("buy negev;");
			break;
		}
		switch (g_Options.misc_settings.secondary)
		{
		case 0:
			break;
		case 1:
			g_EngineClient->ClientCmd("buy hkp2000; buy usp_silencer; buy glock;");
			break;
		case 2:
			g_EngineClient->ClientCmd("buy elite;");
			break;
		case 3:
			g_EngineClient->ClientCmd("buy p250;");
			break;
		case 4:
			g_EngineClient->ClientCmd("buy tec9; buy fn57;");
			break;
		case 5:
			g_EngineClient->ClientCmd("buy deagle; buy revolver;");
			break;
		}
		switch (g_Options.misc_settings.kevlar)
		{
		case 0:
			break;
		case 1:
			g_EngineClient->ClientCmd("buy vest;");
			break;
		case 2:
			g_EngineClient->ClientCmd("buy vesthelm;");
			break;
		}
	}
}
void Misc::Hitmarker(IGameEvent* event)
{
	if (!g_LocalPlayer)
		return;
	static float lastMarked = 0.f;
	if (lastMarked!=g_GlobalVars->curtime&&(g_Options.misc_settings.hitmarker || g_Options.misc_settings.hitsound != 0))
	{
		if (!strcmp(event->GetName(), "player_hurt"))
		{
			if (g_GlobalVars->curtime == lastPlayerHurt)
				return;
			int i = g_EngineClient->GetPlayerForUserID(event->GetInt("userid"));
			int attacker = g_EngineClient->GetPlayerForUserID(event->GetInt("attacker"));
			if (attacker != g_LocalPlayer->EntIndex())
				return;
			C_BasePlayer* user = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(i));
			if (!user)
				return;
			lastDamage = event->GetInt("dmg_health");
			lastPlayerHurt = g_GlobalVars->curtime;
		}
		if (!strcmp(event->GetName(), "bullet_impact"))
		{
			C_BasePlayer* shooter = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))));
			if (shooter&& shooter == g_LocalPlayer)
			{
				possibleLastTracePos = Vector(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));
				Vector eyepos = g_LocalPlayer->GetEyePos();
				Ray_t ray;
				ray.Init(eyepos, Vector(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z")));
				CGameTrace trace;
				CTraceFilter filter;
				filter.pSkip = g_LocalPlayer;
				g_EngineTrace->TraceRay(ray, 0x46004009, &filter, &trace);
				if (!trace.hit_entity) {
					possibleLastHitPos = Vector(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));
					return;
				}
				possibleLastHitPos = ((C_BasePlayer*)trace.hit_entity)->GetHitboxPos(trace.hitbox);
				hitbox = trace.hitbox;
				lastEventTime = g_GlobalVars->curtime;
			}
		}
		std::string hitSounds[4]{
		"play physics/metal/metal_solid_impact_bullet2",
		"play buttons/arena_switch_press_02",
		"play training/timer_bell",
		"play physics/glass/glass_impact_bullet1"
		};
		if (lastEventTime == g_GlobalVars->curtime && lastPlayerHurt == g_GlobalVars->curtime) {
			if(g_Options.misc_settings.hitsound != 0)
			g_EngineClient->ExecuteClientCmd(hitSounds[g_Options.misc_settings.hitsound - 1].c_str());
			LastHitPos = possibleLastHitPos;
			LastTracePos = possibleLastTracePos;
			//g_EngineClient->ExecuteClientCmd("play buttons/arena_switch_press_02");
			lastMarked = g_GlobalVars->curtime;//if it work aint do not fixnt it
		}
	}
}
static bool has_shot = false;
void Misc::AutoPeak(CUserCmd* cmd, bool& sendP)
{
	static bool pressed = false;
	static bool last = false;
	if (InputSys::Get().IsKeyDown(g_Options.misc_settings.hotkeyautopeek) && !pressed)
	{
		if (!last || peekpos == Vector(0, 0, -100000))
			peekpos = g_LocalPlayer->m_vecOrigin();
		else
			peekpos = Vector(0, 0, -100000);
		last = !last;
		pressed = true;
	}
	else if (!InputSys::Get().IsKeyDown(g_Options.misc_settings.hotkeyautopeek) && pressed)
	{
		pressed = false;
	}
	if (cmd->buttons & IN_ATTACK)
		has_shot = true;
	if (peekpos == Vector(0, 0, -100000))
		return;
	if (g_LocalPlayer->m_vecOrigin().DistTo(peekpos) > 25.f) {
		if(has_shot)
		Math::WalktoPos(cmd, Math::CalcAngle(g_LocalPlayer->m_vecOrigin(), peekpos).yaw, 450);
		else
		{
			if (g_Options.misc_settings.chokeonpeek)
			{
				if ((cmd->tick_count % 15) == 0)
					sendP = true;
				else sendP = false;
			}
		}
	}
	else if (has_shot)
	{
		has_shot = false;
		peekpos = Vector(0, 0, -100000);
	}
}

void Misc::FakeDuck(CUserCmd* cmd, bool& sendP)
{
	static int fakeduckChokeAmount = 0;

	if (InputSys::Get().IsKeyDown(g_Options.misc_settings.fakeduck))
	{
		cmd->buttons |= IN_BULLRUSH;
		if (fakeduckChokeAmount > 14)
		{
			fakeduckChokeAmount = 0;
			sendP = true;
		}
		else {
			sendP = false;
		}
		fakeduckChokeAmount++;
		auto choke = g_ClientState->m_nChokedCommands;
		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		{
			if (choke >= 7) {
				cmd->buttons |= IN_DUCK;
			}
			else {
				cmd->buttons &= ~IN_DUCK;
			}
		}
	}
	else {
		fakeduckChokeAmount = 0;
	}
}

void Misc::BulletTracersRegister(IGameEvent* event)
{
	if (!event)
		return;
	auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	if (!pLocal) return;
	if (g_Options.visual_settings.tracer)
	{
		if (g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) == g_EngineClient->GetLocalPlayer() && pLocal && pLocal->IsAlive())
		{
			float x = event->GetFloat("x"), y = event->GetFloat("y"), z = event->GetFloat("z");
			bulletImpactInfo.push_back({ g_GlobalVars->curtime, Vector(x, y, z) });
		}
	}

}

void Misc::BulletTracers()
{

	if (!g_Options.visual_settings.tracer)
		return;
	auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
	if (!pLocal) return;
	if (!g_EngineClient->IsInGame() || !pLocal || !pLocal->IsAlive())
	{
		bulletImpactInfo.clear();
		return;
	}

	std::vector<BulletImpactInfo>& impacts = bulletImpactInfo;

	if (impacts.empty())
		return;


	for (size_t i = 0; i < impacts.size(); i++)
	{
		auto current_impact = impacts.at(i);

		BeamInfo_t beamInfo;

		beamInfo.m_nType = TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/physbeam.vmt";
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = g_Options.visual_settings.tracertime;
		beamInfo.m_flWidth = 2.f;
		beamInfo.m_flEndWidth = 2.f;
		beamInfo.m_flFadeLength = 0.5f;
		beamInfo.m_flAmplitude = g_Options.visual_settings.tracerwave;
		beamInfo.m_flBrightness = 255.f;
		beamInfo.m_flSpeed = 0.03f*g_Options.visual_settings.tracerwave;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;
		beamInfo.m_flRed = g_Options.visual_settings.tracercolor.x * 255.f;
		beamInfo.m_flGreen = g_Options.visual_settings.tracercolor.y * 255.f;
		beamInfo.m_flBlue = g_Options.visual_settings.tracercolor.z * 255.f;
		beamInfo.m_nSegments = -1;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = 0x40;
		if (!g_Input->m_fCameraInThirdPerson && pLocal->m_hViewModel()) {
			beamInfo.m_vecStart = pLocal->m_hViewModel()->getViewmodelAttachment(/*pLocal->m_hActiveWeapon()->AttachmentSlot(pLocal->m_hViewModel())*/1);
		}
		else
			beamInfo.m_vecStart = pLocal->GetEyePos();
		beamInfo.m_vecEnd = current_impact.m_vecHitPos;

		auto beam = g_RenderBeams->CreateBeamPoints(beamInfo);
		if (beam)
			g_RenderBeams->DrawBeam(beam);
		impacts.erase(impacts.begin() + i);
	}
}

constexpr auto deg2rad(float degrees) noexcept { return degrees * (IM_PI / 180.0f); }
constexpr auto rad2deg(float radians) noexcept { return radians * (180.0f / IM_PI); }
static auto fromAngle(const QAngle& angle)
{
	return Vector{ std::cos(deg2rad(angle.pitch)) * std::cos(deg2rad(angle.yaw)),
				   std::cos(deg2rad(angle.pitch)) * std::sin(deg2rad(angle.yaw)),
				  -std::sin(deg2rad(angle.pitch)) };
}
auto toAngle(const Vector& vector)
{
	return QAngle{rad2deg(std::atan2(-vector.z, std::hypot(vector.x, vector.y))),rad2deg(std::atan2(vector.y, vector.x)),0.0f };
}
void Misc::FastStop(CUserCmd* cmd)
{
	if (!g_Options.misc_settings.faststop)
		return;
	if (g_LocalPlayer->m_nMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->m_nMoveType() == MOVETYPE_LADDER || !(g_LocalPlayer->m_fFlags() & 1) || cmd->buttons & IN_JUMP)
		return;
	if ((cmd->buttons & (IN_MOVELEFT | IN_MOVERIGHT | IN_FORWARD | IN_BACK))&&(cmd->forwardmove!=0|| cmd->sidemove != 0))
		return;

	const auto velocity = g_LocalPlayer->m_vecVelocity();
	const auto speed = velocity.Length2D();
	if (speed < 15.0f)
		return;

	QAngle direction = toAngle(velocity);
	direction.yaw = cmd->viewangles.yaw - direction.yaw;

	const auto negatedDirection = fromAngle(direction) * -speed;
	cmd->forwardmove = negatedDirection.x;
	cmd->sidemove = negatedDirection.y;
}

void Misc::Exploits(CUserCmd* cmd, bool& sendP, int* tickbase, int maxshift, int &lastdt)
{
	if (InputSys::Get().IsKeyDown(g_Options.misc_settings.netlosskey))
		sendP = false;
	if (InputSys::Get().IsKeyDown(g_Options.misc_settings.invalidatecmdkey))
	{
		cmd->command_number = INT_MAX;//p9000 airstuckz code
		cmd->tick_count = INT_MAX;
	}
	if (InputSys::Get().IsKeyDown(g_Options.misc_settings.tickbasekey) && g_Options.misc_settings.tickbaseamount <= maxshift && *tickbase == 0) {
		*tickbase = std::min(g_Options.misc_settings.tickbaseamount, maxshift);
		lastdt = g_GlobalVars->tickcount;
	}
}

void Misc::NadePredict()
{
	if (g_CVar->FindVar("cl_grenadepreview")->GetBool() != g_Options.misc_settings.grenadePredict)
	{
		g_CVar->FindVar("cl_grenadepreview")->m_fnChangeCallbacks.SetSize(false);
		g_CVar->FindVar("cl_grenadepreview")->SetValue((bool)g_Options.misc_settings.grenadePredict);
	}
}

void Misc::SpectatorList()
{
	if (!g_Options.misc_settings.specList)
		return;
	int specs = 0;
	std::string spect = "";
	if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected())
	{
		int localIndex = g_EngineClient->GetLocalPlayer();
		C_BasePlayer* pLocalEntity = C_BasePlayer::GetPlayerByIndex(localIndex);
		if (pLocalEntity)
		{
			for (int i = 0; i < g_EngineClient->GetMaxClients(); i++)
			{
				C_BasePlayer* pBaseEntity = C_BasePlayer::GetPlayerByIndex(i);
				if (!pBaseEntity)
					continue;
				if (pBaseEntity->m_iHealth() > 0)
					continue;
				if (pBaseEntity == pLocalEntity)
					continue;
				if (pBaseEntity->IsDormant())
					continue;
				if (pBaseEntity->m_hObserverTarget() != pLocalEntity)
					continue;

				player_info_t pInfo;
				g_EngineClient->GetPlayerInfo(pBaseEntity->EntIndex(), &pInfo);
				if (pInfo.ishltv)
					continue;

				spect += pInfo.szName;
				spect += "\n";
				specs++;
			}
		}
	}
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 backupColors[48];
	memcpy(backupColors, style.Colors, sizeof(style.Colors));
	auto backupAlpha = style.Alpha;
	style.WindowRounding = 0.f;
	style.Alpha = 1;
	ImGui::StyleColorsLight();
	ImGui::SetNextWindowSize({ 250.f, 0.f });
	if (ImGui::Begin("Spectator List", &g_Options.misc_settings.specList, ImVec2(0, 0), 0.4F, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
	{
		if (specs == 0) spect += "None\n";
		ImGui::Text(spect.c_str());
	}
	ImGui::End();
	memcpy(style.Colors,backupColors, sizeof(style.Colors));
	style.Alpha = backupAlpha;
	style.WindowRounding = 31.f;
}

void Misc::RecordMovement(CUserCmd* cmd)
{
	record.push_back({ cmd->viewangles, cmd->forwardmove, cmd->sidemove, cmd->upmove, cmd->buttons, cmd->impulse, cmd->mousedx, cmd->mousedy, g_LocalPlayer->m_vecOrigin() });
}

void Misc::PlayMovement(CUserCmd* cmd, int i)
{
	cmd->viewangles = record.at(i).viewangles;
	cmd->forwardmove = record.at(i).forwardmove;
	cmd->sidemove = record.at(i).sidemove;
	cmd->upmove = record.at(i).upmove;
	cmd->buttons = record.at(i).buttons;
	cmd->impulse = record.at(i).impulse;
	cmd->mousedx = record.at(i).mousex;
	cmd->mousedy = record.at(i).mousey;
	if (g_Options.misc_settings.recordersetAngles)
		g_EngineClient->SetViewAngles(&record.at(i).viewangles);
}

static int currentframe = 0;
static bool hasreachedstartpos = false;
	static bool recording = false;
	static bool replaying = false;

	static bool recordPress = false;
	static bool replayPress = false;
void Misc::RunMovement(CUserCmd* cmd)
{
	if (!g_Options.misc_settings.recorder)
		return;

	if (InputSys::Get().IsKeyDown(g_Options.misc_settings.replaybutton) && !replayPress)
	{
		replaying = !replaying;
		if (replaying) {
			currentframe = 0;
			hasreachedstartpos = false;
		}
		replayPress = true;
	}
	else if (!InputSys::Get().IsKeyDown(g_Options.misc_settings.replaybutton) && replayPress)
		replayPress = false;

	if (InputSys::Get().IsKeyDown(g_Options.misc_settings.recordbutton) && !recordPress)
	{
		recording = !recording;
		if (recording)
			record.clear();
		recordPress = true;
	}
	else if (!InputSys::Get().IsKeyDown(g_Options.misc_settings.recordbutton) && recordPress)
		recordPress = false;

	if (currentframe >= record.size())
	{
		currentframe = 0;
		replaying = false;
	}
	if(record.size()>1)
	if (g_LocalPlayer->m_vecOrigin().DistTo(record.at(0).position) < 3)
		hasreachedstartpos = true;
	if (record.size() > 1)
	{
		if (replaying && !hasreachedstartpos)
		{
			Math::WalktoPos(cmd, Math::CalcAngle(g_LocalPlayer->m_vecOrigin(), record.at(0).position).yaw, g_LocalPlayer->m_vecOrigin().DistTo(record.at(0).position));
		}
		if (replaying && hasreachedstartpos)
		{
			PlayMovement(cmd, currentframe);
			currentframe++;
			recording = false;
		}
	}
	if (recording)
	{
		RecordMovement(cmd);
	}
}

void Misc::KeybindList()
{
	if (!g_Options.misc_settings.showkeybinds)
		return;
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec4 backupColors[48];
	memcpy(backupColors, style.Colors, sizeof(style.Colors));
	auto backupAlpha = style.Alpha;
	style.WindowRounding = 0.f;
	style.Alpha = 1;
	ImGui::StyleColorsLight();
	ImGui::SetNextWindowSize({ 250.f, 0.f });
	if (ImGui::Begin("Keybind List", &g_Options.misc_settings.specList, ImVec2(0, 0), 0.4F, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
	{
		if (InputSys::Get().IsKeyDown(g_Options.misc_settings.dtkey) && g_Options.rage)
			ImGui::Text("DT enabled");
		if (InputSys::Get().IsKeyDown(g_Options.keys.RagebotKey) && g_Options.rage)
			ImGui::Text("Ragebot Enabled");
		if (InputSys::Get().IsKeyDown(g_Options.keys.AimbotKey) && g_Options.legit)
			ImGui::Text("Aimbot Enabled");
		if (InputSys::Get().IsKeyDown(g_Options.keys.TriggerbotKey) && g_Options.legit)
			ImGui::Text("Triggerbot Enabled");
		if (InputSys::Get().IsKeyDown(g_Options.misc_settings.tickbasekey))
			ImGui::Text("Shifting");
		if (InputSys::Get().IsKeyDown(g_Options.misc_settings.edgejumpkey) && g_Options.misc_settings.edgejump)
			ImGui::Text("Edgejump");
		if (InputSys::Get().IsKeyDown(g_Options.misc_settings.edgebugkey) && g_Options.misc_settings.edgebug)
			ImGui::Text("Edgebug");
		if (InputSys::Get().IsKeyDown(g_Options.misc_settings.invalidatecmdkey))
			ImGui::Text("Airstuck");
		if (InputSys::Get().IsKeyDown(g_Options.misc_settings.netlosskey))
			ImGui::Text("Choking");
		if (InputSys::Get().IsKeyDown(g_Options.misc_settings.slowwalkkey) && g_Options.misc_settings.slowwalk)
			ImGui::Text("Slowwalk");
		if (InputSys::Get().IsKeyDown(g_Options.misc_settings.fakeduck))
			ImGui::Text("Fakeduck");
		if (g_Options.rage && g_Options.antiaim_settings.enabled && g_Options.antiaim_settings.aaSide)
			ImGui::Text("Anti-Aim flip side");
		if (g_Options.legit && g_Options.antiaim_settings.legitaa && g_Options.antiaim_settings.legitaaflip)
			ImGui::Text("Legit AA flip side");//record replay walkbot
		if (g_Options.misc_settings.walkbot && InputSys::Get().IsKeyDown(g_Options.misc_settings.walkbotbutton))
			ImGui::Text("Recording Walkbot Path");
		if (g_Options.misc_settings.recorder && recording)
			ImGui::Text("Recording Movement");
		if (g_Options.misc_settings.recorder && replaying)
			ImGui::Text("Replaying Movement");
		if (g_Options.misc_settings.hotkeyautopeek != 0 && peekpos != Vector(0, 0, -100000))
			ImGui::Text("QuickPeek");
	}
	ImGui::End();
	memcpy(style.Colors, backupColors, sizeof(style.Colors));
	style.Alpha = backupAlpha;
	style.WindowRounding = 31.f;
}

void Misc::EdgeJump(CUserCmd* cmd, int oldFlags)
{
	if (!g_Options.misc_settings.edgejump)
		return;
	if (!InputSys::Get().IsKeyDown(g_Options.misc_settings.edgejumpkey))
		return;
	if (const auto mt = g_LocalPlayer->m_nMoveType(); mt == MOVETYPE_LADDER || mt == MOVETYPE_NOCLIP)
		return;

	if ((oldFlags & 1) && !(g_LocalPlayer->m_fFlags() & 1))
		cmd->buttons |= IN_JUMP;
}
#undef xor
void DrawScreenEffectMaterial(IMaterial* material)
{
    int w, h;
    g_EngineClient->GetScreenSize(w, h);
	static auto drawFunction = (Utils::PatternScan(GetModuleHandleW(L"client.dll"), "57 8D 44 24 18 89 4C 24 14") - 0xB);
	__asm {
		__asm push h
		__asm push w
		__asm push 0
		__asm xor edx, edx
		__asm mov ecx, material
		__asm call drawFunction
	}
}

std::vector<float> speedhistory;

void Misc::SpeedGraphRecord()
{
	if (!g_Options.misc_settings.movementgraph)
		return;
	if (speedhistory.size() > 128)
		speedhistory.erase(speedhistory.begin());
	speedhistory.push_back(Vector(g_LocalPlayer->m_vecVelocity().x, g_LocalPlayer->m_vecVelocity().y,0).Length() );
}

#define	MAX_EDICT_BITS				11			        
#define	MAX_EDICTS					(1<<MAX_EDICT_BITS)

void Misc::Rain()
{
	static void* rain_networkable = nullptr;
	static C_Precipitation* rain_ent = (C_Precipitation*)g_EntityList->GetClientEntity(MAX_EDICTS - 1);
	static ClientClass* precipitation_client_class = nullptr;
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected()) {
		rain_networkable = rain_ent = nullptr;
		return;
	}

	int localplayer_index = g_EngineClient->GetLocalPlayer();
	C_BasePlayer* localplayer = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(localplayer_index));
	if (!localplayer) return;

	if (!localplayer->IsAlive())
		return;
	if (!precipitation_client_class) {
		for (auto pclass = g_CHLClient->GetAllClasses(); pclass && !precipitation_client_class; pclass = pclass->m_pNext)
			if (strstr(pclass->m_pNetworkName, "CPrecipitation") && pclass->m_ClassID == ClassId_CPrecipitation)
			{
				precipitation_client_class = pclass;
			}

	}


	else {
		if (!rain_ent && precipitation_client_class && precipitation_client_class->m_pCreateFn) {
			rain_networkable = ((void* (*)(int, int))precipitation_client_class->m_pCreateFn)(MAX_EDICTS - 1, 0);
			if (rain_networkable) {
				rain_ent = (C_Precipitation*)g_EntityList->GetClientEntity(MAX_EDICTS - 1);

				rain_ent->GetPrecipitationType() = g_Options.misc_settings.weather - 1;

				rain_ent->PreDataUpdate(0);
				rain_ent->OnPreDataChanged(0);

				rain_ent->GetMins() = Vector(-32767.0f, -32767.0f, -32767.0f);
				rain_ent->GetMaxs() = Vector(32767.0f, 32767.0f, 32767.0f);

				rain_ent->OnDataChanged(0);
				rain_ent->PostDataUpdate(0);
			}
		}
	}
	if (rain_ent) {
		if(rain_ent->GetPrecipitationType() != (g_Options.misc_settings.weather - 1))
		rain_ent->GetPrecipitationType() = g_Options.misc_settings.weather - 1;
	}

}

void Misc::SpeedGraph()
{
	if (!g_Options.misc_settings.movementgraph)
		return;
	float* speedarray = &speedhistory[0];
	ImGuiStyle& style = ImGui::GetStyle();
	auto backupAlpha = style.Alpha;
	ImVec4 backupColors[48];
	memcpy(backupColors, style.Colors, sizeof(style.Colors));
	style.WindowRounding = 0.f;
	style.FrameRounding = 0.f;
	style.Alpha = 1;
	ImGui::StyleColorsLight();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0, 0, 0, 0);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0, 0, 0, 0);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0, 0, 0, 0);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(1, 1, 1, 1);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1, 1, 1, 1);
	style.Colors[ImGuiCol_Text] = ImVec4(1, 1, 1, 1);
	ImGui::SetNextWindowSize({ 0.f, 0.f });
	if (ImGui::Begin("SpeedGraph", &g_Options.misc_settings.movementgraph, ImVec2(0, 0), 0.1F, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
	{
		ImGui::BringWindowToFocusFront(ImGui::GetCurrentWindow());
		ImGui::PlotLines("", speedarray, speedhistory.size(), 0, ((speedhistory.size() ==0) ? 0 : (std::to_string( ((int)(speedhistory.at(speedhistory.size()-1)))).c_str())), 0, FLT_MAX, ImVec2{ 300, 150 });
	}
	ImGui::End();
	//style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
	memcpy(style.Colors, backupColors, sizeof(style.Colors));
	style.Alpha = backupAlpha;
	style.FrameRounding = 10.f;
	style.WindowRounding = 31.f;
}

void Misc::MotionBlur()
{
	if (!g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive() || !g_Options.misc_settings.blurtype)
		return;

	static constexpr const char* effects[]{
	"effects/dronecam",
	"effects/healthboost",
	"effects/dangerzone_screen",
	};
	auto material = g_MatSystem->FindMaterial(effects[g_Options.misc_settings.blurtype-1]);
	bool empty = false;
	material->FindVar("$c0_x", &empty)->SetValue(std::max(0.f,std::min((g_LocalPlayer->m_vecVelocity().Length()-g_Options.misc_settings.startingspeed)/(g_Options.misc_settings.maxspeed), 1.f)));
	DrawScreenEffectMaterial(material);
}

void Misc::Jumpbug(CUserCmd* cmd) {
	float max_radias = M_PI * 2;
	float step = max_radias / 128;
	float xThick = 23;
	static bool bDidJump = true;
	static bool unduck = true;
	if (!g_Options.misc_settings.edgebug)
		return;
	if (!InputSys::Get().IsKeyDown(g_Options.misc_settings.edgebugkey))
		return;

	if (g_LocalPlayer->m_fFlags() & (1 << 0)) {
		if (unduck) {
			bDidJump = false;
			cmd->buttons &= ~IN_DUCK; // duck
			cmd->buttons |= IN_JUMP; // jump
			unduck = false;
		}
		Vector pos = g_LocalPlayer->m_vecOrigin();
		for (float a = 0.f; a < max_radias; a += step) {
			Vector pt;
			pt.x = (xThick * cos(a)) + pos.x;
			pt.y = (xThick * sin(a)) + pos.y;
			pt.z = pos.z;


			Vector pt2 = pt;
			pt2.z -= 6;

			trace_t fag;

			Ray_t ray;
			ray.Init(pt, pt2);

			CTraceFilter flt;
			flt.pSkip = g_LocalPlayer;
			g_EngineTrace->TraceRay(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);

			if (fag.fraction != 1.f && fag.fraction != 0.f) {
				bDidJump = true;
				cmd->buttons |= IN_DUCK; // duck
				cmd->buttons &= ~IN_JUMP; // jump
				unduck = true;
			}
		}
		for (float a = 0.f; a < max_radias; a += step) {
			Vector pt;
			pt.x = ((xThick - 2.f) * cos(a)) + pos.x;
			pt.y = ((xThick - 2.f) * sin(a)) + pos.y;
			pt.z = pos.z;

			Vector pt2 = pt;
			pt2.z -= 6;

			trace_t fag;

			Ray_t ray;
			ray.Init(pt, pt2);

			CTraceFilter flt;
			flt.pSkip = g_LocalPlayer;
			g_EngineTrace->TraceRay(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);

			if (fag.fraction != 1.f && fag.fraction != 0.f) {
				bDidJump = true;
				cmd->buttons |= IN_DUCK; // duck
				cmd->buttons &= ~IN_JUMP; // jump
				unduck = true;
			}
		}
		for (float a = 0.f; a < max_radias; a += step) {
			Vector pt;
			pt.x = ((xThick - 20.f) * cos(a)) + pos.x;
			pt.y = ((xThick - 20.f) * sin(a)) + pos.y;
			pt.z = pos.z;

			Vector pt2 = pt;
			pt2.z -= 6;

			trace_t fag;

			Ray_t ray;
			ray.Init(pt, pt2);

			CTraceFilter flt;
			flt.pSkip = g_LocalPlayer;
			g_EngineTrace->TraceRay(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);

			if (fag.fraction != 1.f && fag.fraction != 0.f) {
				bDidJump = true;
				cmd->buttons |= IN_DUCK; // duck
				cmd->buttons &= ~IN_JUMP; // jump
				unduck = true;
			}
		}
	}
}
Misc g_Misc;
