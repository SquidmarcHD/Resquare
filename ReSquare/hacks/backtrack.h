#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../helpers/math.hpp"
#define TICK_INTERVAL			(g_GlobalVars->interval_per_tick)
#define TIME_TO_TICKS( dt )		( (int)( 0.5f + (float)(dt) / TICK_INTERVAL ) )
struct StoredData
{
	float simtime;
	float curtime;
	Vector hitboxPos[19];
	Vector multipoints[19][9];
	Vector mins;
	Vector maxs;
	Vector absorigin;
	Vector origin;
	QAngle absangles;
	matrix3x4_t matrix[256];
};

class Backtrack
{
public:
	void CreateMoveRecord(CUserCmd* cmd);
	void CreateMoveSet(CUserCmd* cmd, int &lastbt);
	int nLatestTick;
	StoredData TimeWarpData[64][25];
};


extern Backtrack g_Backtrack;