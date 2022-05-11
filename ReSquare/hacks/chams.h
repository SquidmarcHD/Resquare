#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../helpers/math.hpp"
class Chams {
public:
	uint8_t* patternchams;
	uint8_t* patternsetkv;
	uint8_t* patternfindkv;
	std::vector<IMaterial*> materiallist = {};
	bool isinit = false;
	void InitMats();
	bool Apply(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix, int n);
	void ApplyPlayersSceneEnd();
	void OverrideMaterial(int z);
};
extern Chams g_Chams;