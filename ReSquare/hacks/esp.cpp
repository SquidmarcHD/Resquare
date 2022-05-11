#include "esp.h"
#include "misc.h"
#include "walkbot.h"
#include "rage.h"
#include "resolver.h"
#include "../options.hpp"

RECT GetBBox(C_BaseEntity* ent)
{
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();

	Vector points[] = {
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++) {
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
	}

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++) {
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

void ESP::RenderBoxes(int num, RECT bbox, C_BaseEntity* ent = 0)
{
	if (g_Options.esp_settings[num].type == 1) {
		if (g_Options.esp_settings[num].fillenabled)
			Render::Get().RenderBoxFilled(bbox, g_Options.esp_settings[num].colorfill);
		if (g_Options.esp_settings[num].outlineenabled&& g_Options.esp_settings[num].lineenabled)
			Render::Get().RenderBox(bbox.left, bbox.top, bbox.right, bbox.bottom, g_Options.esp_settings[num].coloroutline, g_Options.esp_settings[num].outlinethiccness*3);
		if (g_Options.esp_settings[num].lineenabled)
			Render::Get().RenderBox(bbox, g_Options.esp_settings[num].color, g_Options.esp_settings[num].linethiccness);
		Vector pos;
		if (g_Options.esp_settings[num].snaplineenabled)
		{
			int screen_w, screen_h, w, h;
			g_EngineClient->GetScreenSize(screen_w, screen_h);
			switch (g_Options.esp_settings[num].typesnapline)
			{
			case 1:
				w = screen_w / 2.f;
				h = (float)screen_h;
				break;
			case 0:
				w = screen_w / 2.f;
				h = 0;
				break;
			case 2:
				w = screen_w / 2.f;
				h = screen_h / 2.f;
				break;
			case 3:
				if (!g_LocalPlayer->IsAlive())
					return;
				Math::WorldToScreen(g_LocalPlayer->GetRenderOrigin(), pos);
				w = pos.x;
				h = pos.y;
				break;
			}
			if (g_Options.esp_settings[num].outlineenabled)
				Render::Get().RenderLine(w, h, (int)(bbox.left + (bbox.right - bbox.left) / 2.f), (int)(bbox.top), g_Options.esp_settings[num].coloroutline, g_Options.esp_settings[num].outlinethiccness * 3);
			Render::Get().RenderLine(w, h, (int)(bbox.left + (bbox.right - bbox.left) / 2.f), (int)(bbox.top), g_Options.esp_settings[num].colorsnap, g_Options.esp_settings[num].snaplinethiccness);
		}
	}
	else if (g_Options.esp_settings[num].type == 2) {
		auto collideable = ent->GetCollideable();

		if (!collideable)
			return;
		auto min = collideable->OBBMins();
		auto max = collideable->OBBMaxs();
		const matrix3x4_t& trans = ent->m_rgflCoordinateFrame();
		Vector points[] = { 
			Vector(min.x, min.y, min.z),
			Vector(min.x, max.y, min.z),
			Vector(max.x, max.y, min.z),
			Vector(max.x, min.y, min.z),
			Vector(min.x, min.y, max.z),
			Vector(min.x, max.y, max.z),
			Vector(max.x, max.y, max.z),
			Vector(max.x, min.y, max.z) };
		Vector pointsTransformed[8];
		for (int i = 0; i < 8; i++) {
			Math::VectorTransform(points[i], trans, pointsTransformed[i]);
		}
		Vector screen_points[8] = {};
		for (int i = 0; i < 8; i++) {
			if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
				return;
		}
		if (g_Options.esp_settings[num].fillenabled)
			Render::Get().Render3dFilledBox({ screen_points[0].x,screen_points[0].y }, { screen_points[1].x,screen_points[1].y }, { screen_points[2].x,screen_points[2].y }, { screen_points[3].x,screen_points[3].y }, { screen_points[4].x,screen_points[4].y }, { screen_points[5].x,screen_points[5].y }, { screen_points[6].x,screen_points[6].y }, { screen_points[7].x,screen_points[7].y }, g_Options.esp_settings[num].colorfill);
		if (g_Options.esp_settings[num].outlineenabled && g_Options.esp_settings[num].lineenabled)
			Render::Get().Render3dBox({ screen_points[0].x,screen_points[0].y }, { screen_points[1].x,screen_points[1].y }, { screen_points[2].x,screen_points[2].y }, { screen_points[3].x,screen_points[3].y }, { screen_points[4].x,screen_points[4].y }, { screen_points[5].x,screen_points[5].y }, { screen_points[6].x,screen_points[6].y }, { screen_points[7].x,screen_points[7].y }, g_Options.esp_settings[num].coloroutline, g_Options.esp_settings[num].outlinethiccness * 3);
		if (g_Options.esp_settings[num].lineenabled)
			Render::Get().Render3dBox({ screen_points[0].x,screen_points[0].y }, { screen_points[1].x,screen_points[1].y }, { screen_points[2].x,screen_points[2].y }, { screen_points[3].x,screen_points[3].y }, { screen_points[4].x,screen_points[4].y }, { screen_points[5].x,screen_points[5].y }, { screen_points[6].x,screen_points[6].y }, { screen_points[7].x,screen_points[7].y }, g_Options.esp_settings[num].color, g_Options.esp_settings[num].linethiccness);
		Vector pos;
		if (g_Options.esp_settings[num].snaplineenabled)
		{
			int screen_w, screen_h, w, h;
			g_EngineClient->GetScreenSize(screen_w, screen_h);
			switch (g_Options.esp_settings[num].typesnapline)
			{
			case 1:
				w = screen_w / 2.f;
				h = (float)screen_h;
				break;
			case 0:
				w = screen_w / 2.f;
				h = 0;
				break;
			case 2:
				w = screen_w / 2.f;
				h = screen_h / 2.f;
				break;
			case 3:
				if (!g_LocalPlayer->IsAlive())
					return;
				Math::WorldToScreen(g_LocalPlayer->GetRenderOrigin(), pos);
				w = pos.x;
				h = pos.y;
				break;
			}
			if (g_Options.esp_settings[num].outlineenabled)
				Render::Get().RenderLine(w, h, (int)((screen_points[0].x + screen_points[1].x + screen_points[2].x + screen_points[3].x) / 4.f), (int)((screen_points[0].y + screen_points[1].y + screen_points[2].y + screen_points[3].y) / 4.f), g_Options.esp_settings[num].coloroutline, g_Options.esp_settings[num].outlinethiccness * 3);
			Render::Get().RenderLine(w, h, (int)((screen_points[0].x + screen_points[1].x + screen_points[2].x + screen_points[3].x) / 4.f), (int)((screen_points[0].y + screen_points[1].y + screen_points[2].y + screen_points[3].y) / 4.f), g_Options.esp_settings[num].colorsnap, g_Options.esp_settings[num].snaplinethiccness);
		}
	}
}
static float nextLBY = -1;
bool ESP::ShouldBreakLBY()
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
void ESP::RenderChargeUi(int charge, float toshift)
{
	if (!g_Options.misc_settings.renderdt)
		return;
	if (!g_EngineClient->IsInGame())
		return;
	int screen_w, screen_h;
	g_EngineClient->GetScreenSize(screen_w, screen_h);
	auto render_h = screen_h / 2.f;
	std::string shift = "";
	for (int i = 0; i < charge; i++)
		shift += "I";
	Render::Get().RenderText(shift, { 0, render_h }, 20, ImVec4{ 1,1,1,1 }, false, false);
	Render::Get().RenderText("DT", { 0, render_h+22 }, 20, ImVec4{ 1* (1 - std::max(0.f,toshift)),1 * std::max(0.f,toshift) ,0,1 });
	Render::Get().RenderText("LBY", { 0, render_h + 44 }, 20, (g_GlobalVars->curtime >= nextLBY) ? ImVec4{1,1,1,1} : ImVec4{ 1 * (1 - std::max(0.f,(nextLBY - g_GlobalVars->curtime) / 1.1f)),1 * std::max(0.f,(nextLBY - g_GlobalVars->curtime) / 1.1f) ,0,1 });
}
void ESP::RenderPath()
{
	if (!g_EngineClient->IsInGame())
		return;
	if (g_Walkbot.path.size() <= 1)
		return;
	if (g_Options.misc_settings.walkbotrenderpath)
	{
		for (int i = 0; i < g_Walkbot.path.size(); i++)
		{
			Vector lastpos, newpos;
			if (!Math::WorldToScreen(g_Walkbot.path.at(i), lastpos))
				continue;
			if (i == g_Walkbot.path.size() - 1) {
				if (!Math::WorldToScreen(g_Walkbot.path.at(0), newpos))
					continue;
			}
			else {
				if (!Math::WorldToScreen(g_Walkbot.path.at(i+1), newpos))
					continue;
			}
			Render::Get().RenderLine(lastpos.x, lastpos.y, newpos.x, newpos.y, g_Options.misc_settings.walkbotpath, 1);
		}
	}
	if (g_Options.misc_settings.walkbotrenderpoints)
	{
		for (int i = 0; i < g_Walkbot.path.size(); i++)
		{
			Vector pos;
			Render::Get().RenderCircle3D(g_Walkbot.path.at(i), 15, 17, g_Options.misc_settings.walkbotpoints);
		}
	}
}
void ESP::RenderHitmarker()
{
	if (g_Misc.lastPlayerHurt < g_GlobalVars->curtime - g_Options.misc_settings.hitmarkertime)
	{
		g_Misc.LastHitPos = Vector(0, 0, -100000);
		return;
	}
	if (!g_LocalPlayer)
		return;
	if (!g_LocalPlayer->IsAlive())
		return;
	if (g_Misc.LastHitPos == Vector(0, 0, -100000))
		return;
	if (g_Options.misc_settings.hitmarkercolor.w == 0)
		return;
	if (!g_Options.misc_settings.hitmarker)
		return;
	Vector mid;
	switch (g_Options.misc_settings.hitmarker)
	{
	case 1:
		int screen_w, screen_h;
		g_EngineClient->GetScreenSize(screen_w, screen_h);
		mid = { screen_w / 2.f, screen_h / 2.f, 0 };
		break;
	case 2:
		if (g_DebugOverlay->ScreenPosition(g_Misc.LastHitPos, mid))
			return;
		break;
	case 3:
		if (g_DebugOverlay->ScreenPosition(g_Misc.LastTracePos, mid))
			return;
		break;
	case 4:
		if (g_DebugOverlay->ScreenPosition(g_Misc.LastHitPos, mid))
			return;
		break;
	}
	float length = g_Options.misc_settings.hitmarkerlength;
	auto color = ((g_Misc.hitbox == HITBOX_HEAD) ? g_Options.misc_settings.headshotcolor : g_Options.misc_settings.hitmarkercolor);
	color.w = 1.f-std::min(((g_GlobalVars->curtime - g_Misc.lastPlayerHurt) / g_Options.misc_settings.hitmarkertime), 1.f);
	if (g_Options.misc_settings.hitmarker != 4)
	{
		Render::Get().RenderLine(mid.x - 10 * length, mid.y - 10 * length, mid.x - 4, mid.y - 4, color);
		Render::Get().RenderLine(mid.x + 10.5f * length, mid.y - 10.5f * length, mid.x + 4.5f, mid.y - 4.5f, color);
		Render::Get().RenderLine(mid.x + 10.5f * length, mid.y + 10.5f * length, mid.x + 4.5f, mid.y + 4.5f, color);
		Render::Get().RenderLine(mid.x - 10 * length, mid.y + 10 * length, mid.x - 4, mid.y + 4, color);
	}
	else 
	{
		Render::Get().RenderText(std::to_string(g_Misc.lastDamage), ImVec2(mid.x, mid.y + length * ((g_GlobalVars->curtime - g_Misc.lastPlayerHurt) / g_Options.misc_settings.hitmarkertime) * -50.f), 14, color);
	}
}
void ESP::RenderQuickPeekInfo()
{
	if (!g_LocalPlayer)
		return;
	if (!g_LocalPlayer->IsAlive())
		return;
	if (g_Misc.peekpos == Vector(0, 0, -100000))
		return;
	if (g_Options.misc_settings.peekvisualisation.w == 0)
		return;
	if (g_Options.misc_settings.hotkeyautopeek == 0)
		return;
	Render::Get().RenderCircle3D(g_Misc.peekpos, 25, 25, g_Options.misc_settings.peekvisualisation);
	Vector start2d, end2d;
	if (g_DebugOverlay->ScreenPosition(g_LocalPlayer->m_vecOrigin(), start2d) || g_DebugOverlay->ScreenPosition(g_Misc.peekpos, end2d))
		return;
	Render::Get().RenderLine(start2d.x, start2d.y, end2d.x, end2d.y, g_Options.misc_settings.peekvisualisation);
}
void ESP::RenderMovementRecorder()
{
	if (!g_LocalPlayer)
		return;
	if (!g_LocalPlayer->IsAlive())
		return;
	if (!g_Options.misc_settings.recorder && !g_Options.misc_settings.recordervisualize)
		return;
	for (int i = 0; i < g_Misc.record.size(); i++)
	{
		Vector lastpos, newpos;
		if (!Math::WorldToScreen(g_Misc.record.at(i).position, lastpos))
			continue;
		if (i == g_Misc.record.size() - 1) {
			return;
		}
		else {
			if (!Math::WorldToScreen(g_Misc.record.at(i+1).position, newpos))
				continue;
		}
		Render::Get().RenderLine(lastpos.x, lastpos.y, newpos.x, newpos.y, g_Options.misc_settings.recordercolor, 1);
	}
}
void ESP::RenderGrenades(C_BaseEntity* ent)
{
	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;
	std::string itemstr = "Grenade";
	const model_t* itemModel = ent->GetModel();
	if (!itemModel)
		return;
	studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(itemModel);
	if (!hdr)
		return;
	itemstr = hdr->szName;
	if (itemstr.find("molotov") != std::string::npos)
		itemstr = "Molotov";
	else if (itemstr.find("flash") != std::string::npos)
		itemstr = "Flashbang";
	else if (itemstr.find("incendiary") != std::string::npos)
		itemstr = "Incendiary Grenade";
	else if (itemstr.find("frag") != std::string::npos)
		itemstr = "High Explosive";
	else if (itemstr.find("smoke") != std::string::npos)
		itemstr = "Smoke";
	else if (itemstr.find("decoy") != std::string::npos)
		itemstr = "Decoy";
	else if (itemstr.find("charge") != std::string::npos)
		itemstr = "Breach Charge";
	else if (itemstr.find("bumpmine") != std::string::npos)
		itemstr = "Bump Mine";
	RenderBoxes(4, bbox, ent);

	auto sz = ImGui::GetFont()->CalcTextSizeA(g_Options.esp_settings[4].namethiccness, FLT_MAX, 0.0f, itemstr.c_str());
	int w = bbox.right - bbox.left;

	if (g_Options.esp_settings[4].nameenabled)
		Render::Get().RenderText(itemstr, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), g_Options.esp_settings[4].namethiccness, g_Options.esp_settings[4].colorname);
	if (g_Options.esp_settings[4].weaponenabled)
		Render::Get().RenderText(ent->m_hOwnerEntity().Get()->GetPlayerInfo().szName, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.top + -1), g_Options.esp_settings[4].namethiccness, g_Options.esp_settings[4].colorname);
}
void ESP::RenderWeapon(C_BaseCombatWeapon* ent)
{
	auto clean_item_name = [](const char* name) -> const char* {
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};
	if (ent->m_hOwnerEntity().IsValid())
		return;

	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	RenderBoxes(6, bbox, ent);

	auto name = clean_item_name(ent->GetClientClass()->m_pNetworkName);

	auto sz = ImGui::GetFont()->CalcTextSizeA(g_Options.esp_settings[6].namethiccness, FLT_MAX, 0.0f, name);
	int w = bbox.right - bbox.left;

	if (g_Options.esp_settings[6].nameenabled)
	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), g_Options.esp_settings[6].namethiccness, g_Options.esp_settings[6].colorname);
}
void ESP::RenderPlantedC4(C_BaseEntity* ent)
{
	auto bbox = GetBBox(ent);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;
	RenderBoxes(5, bbox,ent);
	int bombTimer = std::ceil(ent->m_flC4Blow() - g_GlobalVars->curtime);
	std::string timer = std::to_string(bombTimer);

	auto name = (bombTimer < 0.f) ? "C4" : timer;
	auto sz = ImGui::GetFont()->CalcTextSizeA(g_Options.esp_settings[5].namethiccness, FLT_MAX, 0.0f, name.c_str());
	int w = bbox.right - bbox.left;
	if(g_Options.esp_settings[5].nameenabled)
	Render::Get().RenderText(name, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), g_Options.esp_settings[5].namethiccness, g_Options.esp_settings[5].colorname);
}
void ESP::RenderItemEsp(C_BaseEntity* ent)
{
	std::string itemstr = "Undefined";
	const model_t* itemModel = ent->GetModel();
	if (!itemModel)
		return;
	studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(itemModel);
	if (!hdr)
		return;
	itemstr = hdr->szName;
	if (ent->GetClientClass()->m_ClassID == ClassId_CBumpMine)
		itemstr = "Bump Mine";
	else if (itemstr.find("case_pistol") != std::string::npos)
		itemstr = "Pistol Case";
	else if (itemstr.find("case_light_weapon") != std::string::npos)
		itemstr = "Light Case";
	else if (itemstr.find("case_heavy_weapon") != std::string::npos)
		itemstr = "Heavy Case";
	else if (itemstr.find("case_explosive") != std::string::npos)
		itemstr = "Explosive Case";
	else if (itemstr.find("case_tools") != std::string::npos)
		itemstr = "Tools Case";
	else if (itemstr.find("random") != std::string::npos)
		itemstr = "Airdrop";
	else if (itemstr.find("dz_armor_helmet") != std::string::npos)
		itemstr = "Full Armor";
	else if (itemstr.find("dz_helmet") != std::string::npos)
		itemstr = "Helmet";
	else if (itemstr.find("dz_armor") != std::string::npos)
		itemstr = "Armor";
	else if (itemstr.find("upgrade_tablet") != std::string::npos)
		itemstr = "Tablet Upgrade";
	else if (itemstr.find("briefcase") != std::string::npos)
		itemstr = "Briefcase";
	else if (itemstr.find("parachutepack") != std::string::npos)
		itemstr = "Parachute";
	else if (itemstr.find("dufflebag") != std::string::npos)
		itemstr = "Cash Dufflebag";
	else if (itemstr.find("ammobox") != std::string::npos)
		itemstr = "Ammobox";
	else if (itemstr.find("dronegun") != std::string::npos)
		itemstr = "Turrel";
	else if (itemstr.find("exojump") != std::string::npos)
		itemstr = "Exojump";
	else if (itemstr.find("healthshot") != std::string::npos)
		itemstr = "Healthshot";
	else return;
	if (ent->m_hOwnerEntity().IsValid())
		return;
	auto bbox = GetBBox(ent);
	if (bbox.right == 0 || bbox.bottom == 0)
		return;
	auto sz = ImGui::GetFont()->CalcTextSizeA(g_Options.esp_settings[7].namethiccness, FLT_MAX, 0.0f, itemstr.c_str());
	int w = bbox.right - bbox.left;


	RenderBoxes(7, bbox,ent);
	Render::Get().RenderText(itemstr, ImVec2((bbox.left + w * 0.5f) - sz.x * 0.5f, bbox.bottom + 1), g_Options.esp_settings[7].namethiccness, g_Options.esp_settings[7].colorname);
}
void ESP::RenderPlayers(C_BasePlayer* ent, int num)
{
	auto bbox = GetBBox(ent);
	if (bbox.right == 0 || bbox.bottom == 0)
		return;
	
	RenderBoxes(num, bbox,ent);
	auto head = ent->GetHitboxPos(HITBOX_HEAD);
	auto origin = ent->m_vecOrigin();
	head.z += 15;
	Vector headpos;
	Vector originpos;
	if (!Math::WorldToScreen(head, headpos) ||
		!Math::WorldToScreen(origin, originpos))
		return;
	if (g_Options.esp_settings[num].armor)
	{
		auto  armor = ent->m_ArmorValue();
		float box_h = (float)fabs(bbox.bottom - bbox.top);
		float off = 8;
		int height = (box_h * armor) / 100;
		int x = bbox.left - off;
		int y = bbox.top;
		int w = 3;
		int h = box_h;
		if(g_Options.esp_settings[num].outlineenabled)
			Render::Get().RenderBox(x + 1, y + 1 + (int)g_Options.esp_settings[num].outlinethiccness, x + w - 1, y - height - 2 - (int)g_Options.esp_settings[num].outlinethiccness, g_Options.esp_settings[num].coloroutline, g_Options.esp_settings[num].outlinethiccness*2, true);
		Render::Get().RenderBox(x + 1, y + 1, x + w - 1, y - height - 2, ImVec4(0.2f, 0.2f, 1, 1), 1.f, true);
	}
	if (g_Options.esp_settings[num].health)
	{
		auto  hp = ent->m_iHealth();
		float box_h = (float)fabs(bbox.bottom - bbox.top);
		float off = 4;

		int height = (box_h * hp) / 100;

		int x = bbox.left - off;
		int y = bbox.top;
		int w = 3;
		int h = box_h;
		if (g_Options.esp_settings[num].outlineenabled)
			Render::Get().RenderBox(x + 1, y + 1+ (int)g_Options.esp_settings[num].outlinethiccness, x + w - 1, y - height - 2- (int)g_Options.esp_settings[num].outlinethiccness, g_Options.esp_settings[num].coloroutline, g_Options.esp_settings[num].outlinethiccness*2, true);
		Render::Get().RenderBox(x + 1, y + 1, x + w - 1, y - height - 2, ImVec4(0, 1, 0, 1), 1.f, true);
	}
	if (g_Options.esp_settings[num].nameenabled)
	{
		player_info_t info = ent->GetPlayerInfo();
		auto sz = ImGui::GetFont()->CalcTextSizeA(g_Options.esp_settings[num].namethiccness, FLT_MAX, 0.0f, info.szName);
		Render::Get().RenderText(info.szName, originpos.x - sz.x / 2, headpos.y - sz.y, g_Options.esp_settings[num].namethiccness, g_Options.esp_settings[num].colorname);
	}
	if (g_Options.esp_settings[num].weaponenabled)
	{
		auto weapon = ent->m_hActiveWeapon().Get();
		if (weapon) {
			if (weapon->GetCSWeaponData())
			{ 

			auto text = weapon->GetCSWeaponData()->szWeaponName + 7;
			auto sz = ImGui::GetFont()->CalcTextSizeA(g_Options.esp_settings[num].namethiccness, FLT_MAX, 0.0f, text);
			Render::Get().RenderText(text, originpos.x, originpos.y, g_Options.esp_settings[num].namethiccness, g_Options.esp_settings[num].colorname, true,
				ImGui::GetFont());
			}
		}
	}
	if (g_Options.esp_settings[num].skeletonenabled)
	{
		const model_t* itemModel = ent->GetModel();
		if (!itemModel)
			return;
		studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(itemModel);
		if (!hdr)
			return;
		for (int j = 0; j < hdr->numbones; j++)
		{
			mstudiobone_t* pBone = hdr->GetBone(j);

			if (pBone && (pBone->flags & BONE_USED_BY_HITBOX) && (pBone->parent != -1))
			{
				auto vChild = ent->GetBonePos(j);
				auto vParent = ent->GetBonePos(pBone->parent);
				int iChestBone = 6;
				Vector vBreastBone;
				Vector vUpperDirection = ent->GetBonePos(iChestBone + 1) - ent->GetBonePos(iChestBone);
				vBreastBone = ent->GetBonePos(iChestBone) + vUpperDirection / 2;
				Vector vDeltaChild = vChild - vBreastBone;
				Vector vDeltaParent = vParent - vBreastBone;
				if ((vDeltaParent.Length() < 9 && vDeltaChild.Length() < 9))
					vParent = vBreastBone;

				if (j == iChestBone - 1)
					vChild = vBreastBone;

				if (abs(vDeltaChild.z) < 5 && (vDeltaParent.Length() < 5 && vDeltaChild.Length() < 5) || j == iChestBone)
					continue;
				Vector p1, p2;
				Math::WorldToScreen(vChild, p1);
				Math::WorldToScreen(vParent, p2);
				if (g_Options.esp_settings[num].outlineenabled)
				Render::Get().RenderLine(p1.x, p1.y, p2.x, p2.y, g_Options.esp_settings[num].coloroutline, g_Options.esp_settings[num].outlinethiccness*3);
				Render::Get().RenderLine(p1.x, p1.y, p2.x, p2.y, g_Options.esp_settings[num].colorskeleton, g_Options.esp_settings[num].skeletonthiccness);
			}
		}
	}
}
void ESP::Render()
{
	for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i) {
		auto entity = C_BaseEntity::GetEntityByIndex(i);

		if (!entity)
			continue;

		if (entity == g_LocalPlayer)
			continue;

		if (i <= g_GlobalVars->maxClients) {
			if (entity->IsPlayer() && !entity->IsDormant() && ((C_BasePlayer*)entity)->IsAlive())
			{
				bool is_enemy = entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
				bool is_visible = ((C_BasePlayer*)entity)->visibleTo(g_LocalPlayer, ((C_BasePlayer*)entity));
				if (is_enemy && is_visible && g_Options.esp_settings[0].type)
					RenderPlayers(((C_BasePlayer*)entity), 0);
				else if (is_enemy && g_Options.esp_settings[1].type)
					RenderPlayers(((C_BasePlayer*)entity), 1);
				else if (!is_enemy && is_visible && g_Options.esp_settings[2].type)
					RenderPlayers(((C_BasePlayer*)entity), 2);
				else if (!is_enemy && g_Options.esp_settings[3].type)
					RenderPlayers(((C_BasePlayer*)entity), 3);
			}
		}
		else if (entity->IsPlantedC4() && g_Options.esp_settings[5].type != 0)
			RenderPlantedC4(entity);
		else if (entity->IsLoot() && g_Options.esp_settings[7].type != 0)
			RenderItemEsp(entity);
		else if (entity->IsWeapon() && g_Options.esp_settings[6].type != 0)
			RenderWeapon((C_BaseCombatWeapon*)entity);
		else if (entity->IsGrenade()&&g_Options.esp_settings[4].type != 0)
			RenderGrenades(entity);
	}
	if (g_Options.misc_settings.hotkeyautopeek)
		RenderQuickPeekInfo();
	RenderHitmarker();
	RenderPath();
	RenderMovementRecorder();
	g_Ragebot.RenderDebugPointInfo();
	g_Resolver.Draw();
}
ESP g_ESP;