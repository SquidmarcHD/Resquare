#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../helpers/math.hpp"
#include "../helpers/input.hpp"

struct MovementRecord
{
	QAngle viewangles = QAngle(0, 0, 0);
	float forwardmove = 0.f;
	float sidemove = 0.f;
	float upmove = 0.f;
	int buttons = 0;
	char impulse = 0;
	short mousex = 0;
	short mousey = 0;
	Vector position = { 0,0,0 };
};

struct BulletImpactInfo
{
	float m_flExpTime;
	Vector m_vecHitPos;
};

class Misc {
public:
	Vector peekpos = { 0,0,-100000 };
	Vector LastHitPos = { 0,0,-100000 };
	Vector LastTracePos = { 0,0,-100000 };
	Vector possibleLastHitPos = { 0,0,-100000 };
	Vector possibleLastTracePos = { 0,0,-100000 };
	float lastEventTime = 0.f;
	float lastPlayerHurt = 0.f;
	int lastDamage = 0;
	int hitbox = 0;
	std::vector<MovementRecord> record;
	std::vector<BulletImpactInfo> bulletImpactInfo;
	void ViewModelCM();
	void ForceCrosshairCM();
	void FogCM();
	void AmbientCM();
	void Clantag();
	void Radar();
	void Bhop(CUserCmd* cmd, bool &sendP);
	void SlowWalk(CUserCmd* cmd);
	void AutoStrafe(CUserCmd* cmd, QAngle backup);
	void AutoBuy(IGameEvent* event);
	void Hitmarker(IGameEvent* event);
	void BulletTracers();
	void BulletTracersRegister(IGameEvent* event);
	void AutoPeak(CUserCmd* cmd, bool& sendP);
	void FakeDuck(CUserCmd* cmd, bool& sendP);
	void FastStop(CUserCmd* cmd);
	void Exploits(CUserCmd* cmd, bool& sendP, int* tickbase, int maxshift, int &lastdt);
	void NadePredict();
	void SpectatorList();
	void KeybindList();
	void RecordMovement(CUserCmd* cmd);
	void PlayMovement(CUserCmd* cmd, int i = 0);
	void RunMovement(CUserCmd* cmd);
	void MotionBlur();
	void Jumpbug(CUserCmd* cmd);
	void EdgeJump(CUserCmd*cmd, int oldFlags);
	void SpeedGraph();
	void SpeedGraphRecord();
	void Rain();
};
extern Misc g_Misc;