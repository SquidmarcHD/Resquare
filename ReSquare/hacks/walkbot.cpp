#include "walkbot.h"
#include "../options.hpp"

void Walkbot::Delete(int index)
{
	if (currentPath == index)
	{
		if (currentPath == path.size())
			currentPath -= 1;
		else 
			currentPath += 1;
	}
	if(path.size()!=0)
	path.erase(path.begin()+index);
}

void Walkbot::Clear()
{
	g_Options.misc_settings.walkbot = false;
	path.clear();
}

void Walkbot::Reset(IGameEvent* event)
{
	if (!strcmp(event->GetName(), "round_freeze_end"))
	{
		int bestIndex = -1;
		float bestDist = 9999999;
		for (int i = 0; i < g_Walkbot.path.size(); i++)
		{
			auto dist = g_LocalPlayer->m_vecOrigin().DistTo(path.at(i));
			if (dist < bestDist)
			{
				bestIndex = i;
				bestDist = dist;
			}
		}
		currentPath = bestIndex;
	}
}

void Walkbot::Create()
{
	path.push_back(g_LocalPlayer->m_vecOrigin());
}

static Vector lastCreated = { 0,0,0 };

void Walkbot::RunCreate()
{
	static bool Pressed = false;
	if (InputSys::Get().IsKeyDown(g_Options.misc_settings.walkbotbutton) && !Pressed)
	{
		lastCreated = g_LocalPlayer->m_vecOrigin();
		Create();
		Pressed = true;
	}
	else if (!InputSys::Get().IsKeyDown(g_Options.misc_settings.walkbotbutton) && Pressed)
	{
		Pressed = false;
	}
	if (InputSys::Get().IsKeyDown(g_Options.misc_settings.walkbotbutton) && lastCreated.DistTo(g_LocalPlayer->m_vecOrigin()) >= g_Options.misc_settings.walkbotdist) {
		lastCreated = g_LocalPlayer->m_vecOrigin();
		Create();
	}
}

void Walkbot::Run(CUserCmd *cmd)
{
	if (!g_LocalPlayer)
		return;
	if (!g_LocalPlayer->IsAlive())
		return;
	RunCreate();
	if (!g_Options.misc_settings.walkbot)
		return;
	if (path.size() <= currentPath)
		currentPath = 0;
	if (path.size() == 0)
		return;
	auto Angle = Math::CalcAngle(g_LocalPlayer->m_vecOrigin(),path.at(currentPath));
	Math::WalktoPos(cmd, Angle.yaw, g_Options.misc_settings.walkbotspeed*4.5f);
	if (Vector(g_LocalPlayer->m_vecOrigin().x, g_LocalPlayer->m_vecOrigin().y, 0).DistTo(Vector(path.at(currentPath).x, path.at(currentPath).y, 0)) <= 15)
		currentPath += 1;
}

Walkbot g_Walkbot;