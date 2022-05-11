#pragma once
#include "../valve_sdk/csgostructs.hpp"
#include "../valve_sdk/sdk.hpp"
#include "../helpers/math.hpp"
#include "../helpers/input.hpp"
class Walkbot {
public:
	int currentPath;
	std::vector<Vector> path;
	void Delete(int index);
	void Clear();
	void Reset(IGameEvent* event);
	void Create();
	void RunCreate();
	void Run(CUserCmd *cmd);
};
extern Walkbot g_Walkbot;