#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "valve_sdk/Misc/Color.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
class Options
{
public:
	struct Sticker { bool enabled = 0; int kit = 1; float wear = 0; float scale = 1; float rotation = 0; };
	struct AimbotSettings
	{
		bool enabled = false;
		float smooth = 1;
		float fov = 360;
		bool shootFlashed = false;
		bool onlyScoped = true;
		bool silent = false;
		bool RCS = false;
		bool RCSsilent = true;
		int RCSamount = 100;
		bool bones[19] = { false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };
		int aimassist = 0;
		int pad1[2];
		bool pad2[2];
	};
	struct RagebotSettings
	{
		bool enabled = false;
		int mindmg = 0;
		float minchance = 0;
		float fov = 360;
		bool killshot = true;
		bool silent = true;
		int overkill = 10;
		bool AutoShot = true;
		bool AutoScope = true;
		int AutoStop = 0;
		bool AutoCrouch = false;
		//bool AutoSafepoint = false;
		float multipointexpansion = 0.42;
		bool bones[19] = { false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false };
		int dt = 0;
		int pad1[3];
		bool pad2[2];
	};
	struct TriggerbotSettings
	{
		bool enabled = false;
		int shotDelay = 500;
		bool shootFlashed = false;
		bool onlyScoped = true;
		bool bones[19] = { true,true,true,true,true,true,true,true,true,true,true,false,false,false,false,false,false,false,false  };
		int pad1[3];
		bool pad2[2];
	};
	struct FogSettings
	{
		bool enabled = false;
		ImVec4 color = ImVec4(1, 1, 1, 1);
		int end = 100;
		int start = 0;
		float maxdensity = 0.9f;
		int pad1[3];
		bool pad2[2];
	};
	struct VisualSettings
	{
		bool noscope = false;
		bool forcecrosshair = false;
		bool thirdperson = false;
		bool noflash = false;
		int nosmoke = 0;
		float thirdpersondist = 300;
		float fov = 0;
		float scopedfov = -50;
		float viewmodelfov = 0;
		bool bloomenabled = false;
		float bloomworldExposure = 0.0f;
		float bloomModelAmbient = 0.0f;
		float bloomScale = 0.0f;
		int skybox = 0;
		bool rain = false;
		bool viewmodelenabled = false;
		float viewmodelx = 1, viewmodely = 1, viewmodelz = -1;
		FogSettings fog = {};
		bool ambientenabled = false;
		ImVec4 ambientcolor = ImVec4(0, 0, 0, 1);
		int thirdpersonKey = 0;
		bool worldenabled = false;
		ImVec4 worldcolor = ImVec4(1, 1, 1, 1);
		bool skyenabled = false;
		ImVec4 skycolor = ImVec4(1, 1, 1, 1);
		bool tracer = false;
		ImVec4 tracercolor = ImVec4{ 1,1,1,1 };
		float tracertime = 0.75f;
		float tracerwave = 0.f;

		float aspectratio = 0;
		int pad1[20];
		bool pad2[4];
	};
	struct MiscSettings
	{
		bool bhop = false;
		bool fastduck = false;
		int fakeduck = 0;
		bool slowwalk = false;
		int clantag = 0;
		int slowwalkkey = 0;
		ImVec4 aimbotfov = ImVec4{ 0,0,0,0 };
		int limiter = 1;
		int autostrafe = 0;
		bool radar = false;
		int primary = 0;
		int secondary = 0;
		int kevlar = 0;
		int hotkeyautopeek = 0;
		bool chokeonpeek = false;
		bool faststop = false;
		ImVec4 peekvisualisation = ImVec4{ 0,0,0,0 };

		//hitmarker
		int hitsound = 0;
		int hitmarker = 0;
		ImVec4 hitmarkercolor = ImVec4{ 1,1,1,1 };
		float hitmarkerlength = 1;
		float hitmarkertime = 0.75f;

		//walkbot
		bool walkbot = false;
		int walkbotspeed = 100;
		bool walkbotrenderpath = false;
		bool walkbotrenderpoints = false;
		ImVec4 walkbotpath = ImVec4{ 0,1,0,0.75f };
		ImVec4 walkbotpoints = ImVec4{ 1,0,0,0.75f };
		int walkbotbutton = 0;
		int walkbotdist = 75;

		//misc other
		int maxBones = 65664;
		int maxBonesperJoint = 9;
		bool grenadePredict = false;
		bool specList = false;
		bool disableAnim = false;

		//recorder
		bool recorder = false;
		int recordbutton = 0;
		int replaybutton = 0;
		bool recordersetAngles = true;
		bool recordervisualize = false;
		ImVec4 recordercolor = ImVec4{ 1,1,1,1 };

		//blur
		float maxspeed = 500;
		float startingspeed = 250;
		int blurtype = 0;
		
		ImVec4 headshotcolor = ImVec4{ 1,0,0,1 };

		bool movementgraph = false;

		int knifemodel = 0;
		int glovemodel = 0;
		int glovepaintkit = 0;

		bool debugmultipoints = false;
		float multipointsize = 0.42f;
		int weather = 0;
		bool drawDebug = false;
		int bhopHitchance = 100;
		int edgebugkey = 0;
		bool edgebug = false;
		bool edgejump = false;
		bool autopistol = false;
		bool listleaves = false;
		bool showlogs = false;
		bool showkeybinds = false;
		int pad1[20];
		//exploit section
		int netlosskey = 0;
		int tickbasekey = 0;
		int tickbaseamount = 0;
		int invalidatecmdkey = 0;
		int chargespeed = 2;
		bool renderdt = false;
		int dtkey = 0;
		float fakelatency = 0;
		int edgejumpkey = 0;
		bool prediction = true;
		int pad3[9];
		bool pad4[3];
	};
	struct GlowSettings
	{
		ImVec4 color = ImVec4{ 1,0,0,1 };
		bool enabled = false;
		int type = 0;
		int pad1[10];
		bool pad2[2];
	};
	struct DLightSettings
	{
		ImVec4 color = ImVec4{ 1,0,0,1 };
		bool enabled = false;
		float radius = 25;
		float exponent = 10;
		float offset = 35;
		int pad1[2];
		bool pad2[1];
	};
	struct ChamsSettings
	{
		ImVec4 color = ImVec4{ 1,0,0,1 };
		bool enabled = false;
		bool throughwalls = true;
		int type = 0;
		int pad1[2];
		bool pad2[1];
	};
	struct SkinsSettings
	{
		bool enabled = false;
		int paintkit = 0;
		int seed = 0;
		float wear = 0.001f;
		int stattrack = -1;
		Sticker stickers[5];
		int pad1[6];
		bool pad2[1];
	};
	struct BacktrackSettings
	{
		bool enabled = false;
		bool shoulddrawall = true;
		bool shouldtarget = true;
		int time = 0;
		int targetlast = 1;
		int pad1[2];
		bool pad2[1];
	};
	struct AntiAimSettings
	{
		bool enabled = false;
		bool legitaa = false;
		bool legitaaflip = false;
		float antiaim_edge_dist = 0.0f;
		bool antiaim_antiresolver = false;
		bool overrideenable[5] = { true, false, false, false, false };
		bool lbybreak[5] = { false, false, false, false, false };
		int antiaim_pitch[5] = { 0,0,0,0,0 };
		int antiaim_yaw[5] = { 0,0,0,0,0 };
		int antiaim_yaw_fake[5] = { 0,0,0,0,0 };
		float antiaim_spin_speed[5] = { 0,0,0,0,0 };
		float antiaim_spin_speed_fake[5] = { 0,0,0,0,0 };		
		
		float addvalue[5] = { 0,0,0,0,0 };
		float addvalue_fake[5] = { 0,0,0,0,0 };		
		
		float jitter[5] = { 0,0,0,0,0 };
		float jitter_fake[5] = { 0,0,0,0,0 };
		
		bool at_enemy[5] = { 0,0,0,0,0 };
		bool antiaim_knife = true;
		int tankAntiaimKey = 0;
		int legitAntiaimKey = 0;
		bool antiuntrusted = true;
		int fakelag_amount[5] = { 0,0,0,0,0 };
		bool aaSide = false;
		int pad1[40];
	};
	struct EspSettings
	{
		int type = 0;
		bool lineenabled = false;
		float linethiccness = 1.f;
		ImVec4 color = ImVec4{ 1,1,1,1 };

		bool outlineenabled = false;
		float outlinethiccness = 1.f;
		ImVec4 coloroutline = ImVec4{ 0,0,0,1 };

		bool fillenabled = false;
		ImVec4 colorfill = ImVec4{ 1,1,1,0.3f };

		int typesnapline = 0;
		bool snaplineenabled = false;
		float snaplinethiccness = 1.f;
		ImVec4 colorsnap = ImVec4{ 1,1,1,1 };

		bool nameenabled = false;
		bool weaponenabled = false;
		float namethiccness = 14.f;
		ImVec4 colorname = ImVec4{ 1,1,1,1 };

		bool skeletonenabled = false;
		float skeletonthiccness = 1.f;
		ImVec4 colorskeleton = ImVec4{ 1,1,1,1 };

		bool health = false;
		bool armor = false;
		bool info = false; //defusing has c4 ?
		int pad1[4];
		bool pad2[2];
	};
	struct KeyBinds
	{
		int RagebotKey = 0;
		int TriggerbotKey = 0;
		int AimbotKey = 0;
		int MenuKey = VK_INSERT;
		int pad1[4];
	};
	bool legit = true;
	bool rage = false;
	int placeholder = 0;
	int placeholder2 = 0;
	bool resolver = true;
	AimbotSettings aimbot_settings[42]			= { {},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{} };
	TriggerbotSettings triggerbot_settings[42]	= { {},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{} };
	RagebotSettings ragebot_settings[42]		= { {},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{} };
	GlowSettings glow_settings[13]				= { {ImVec4(90.f/255.f, 0.f/255.f, 255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,150.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,200.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,45.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,200.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(150.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,100.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(200.f/255.f,255.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,30.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,150.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,20.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,135.f/255.f,0.f/255.f,255.f/255.f)} };
	DLightSettings dlight_settings[13]			= { {ImVec4(90.f/255.f, 0.f/255.f, 255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,150.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,200.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,45.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,200.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(150.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,100.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(200.f/255.f,255.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,30.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,150.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,20.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,135.f/255.f,0.f/255.f,255.f/255.f)} };
	ChamsSettings chams_settings[70]			= { {ImVec4(90.f/255.f, 0.f/255.f, 255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,150.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,200.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,45.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,200.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(150.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,100.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(200.f/255.f,255.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,30.f/255.f,255.f/255.f)},{ImVec4(225.f/255.f,225.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(107.f/255.f, 147.f/255.f, 157.f/255.f, 1)},{ImVec4(0.f/255.f,150.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,20.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(90.f/255.f, 0.f/255.f, 255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,150.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,200.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,45.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,200.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(150.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,100.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(200.f/255.f,255.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,30.f/255.f,255.f/255.f)},{ImVec4(225.f/255.f,225.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(107.f/255.f, 147.f/255.f, 157.f/255.f,1)},{ImVec4(0.f/255.f,150.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,20.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(90.f/255.f, 0.f/255.f, 255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,150.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,200.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,45.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,200.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(150.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,100.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(200.f/255.f,255.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,30.f/255.f,255.f/255.f)},{ImVec4(225.f/255.f,225.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(107.f/255.f, 147.f/255.f, 157.f/255.f,1)},{ImVec4(0.f/255.f,150.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,20.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(90.f/255.f, 0.f/255.f, 255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,150.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,200.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,45.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,200.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(150.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,100.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(200.f/255.f,255.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,30.f/255.f,255.f/255.f)},{ImVec4(225.f/255.f,225.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(107.f/255.f, 147.f/255.f, 157.f/255.f,1)},{ImVec4(0.f/255.f,150.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,20.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(90.f/255.f, 0.f/255.f, 255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,150.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,200.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,45.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,200.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(150.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,100.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(200.f/255.f,255.f/255.f,0.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,255.f/255.f,30.f/255.f,255.f/255.f)},{ImVec4(225.f/255.f,225.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(107.f/255.f, 147.f/255.f, 157.f/255.f,1)},{ImVec4(0.f/255.f,150.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(255.f/255.f,0.f/255.f,255.f/255.f,255.f/255.f)},{ImVec4(0.f/255.f,20.f/255.f,255.f/255.f,255.f/255.f)} };
	SkinsSettings skins_settings[35]			= { {},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{} };
	EspSettings esp_settings[8]					= { {},{},{},{},{},{},{},{} };
	VisualSettings visual_settings				= {};
	MiscSettings misc_settings					= {};
	BacktrackSettings backtrack_settings		= {};
	AntiAimSettings antiaim_settings			= {};
	KeyBinds keys								= {};
};
/*
GlowSettings glow_settings[13]				= { {Color(90, 0, 255,255)},{Color(0,255,150,255)},{Color(0,200,255,255)},{Color(255,45,0,255)},{Color(255,200,0,255)},{Color(150,0,255,255)},{Color(0,100,255,255)},{Color(200,255,0,255)},{Color(0,255,30,255)},{Color(0,150,255,255)},{Color(255,0,255,255)},{Color(0,20,255,255)},{Color(255,135,0,255)} };
DLightSettings dlight_settings[13]			= { {Color(90, 0, 255,255)},{Color(0,255,150,255)},{Color(0,200,255,255)},{Color(255,45,0,255)},{Color(255,200,0,255)},{Color(150,0,255,255)},{Color(0,100,255,255)},{Color(200,255,0,255)},{Color(0,255,30,255)},{Color(0,150,255,255)},{Color(255,0,255,255)},{Color(0,20,255,255)},{Color(255,135,0,255)} };
ChamsSettings chams_settings[70]			= { {Color(90, 0, 255,255)},{Color(0,255,150,255)},{Color(0,200,255,255)},{Color(255,45,0,255)},{Color(255,200,0,255)},{Color(150,0,255,255)},{Color(0,100,255,255)},{Color(200,255,0,255)},{Color(0,255,30,255)},{Color(225,225,255,255)},{Color(107, 147, 157)},{Color(0,150,255,255)},{Color(255,0,255,255)},{Color(0,20,255,255)},{Color(90, 0, 255,255)},{Color(0,255,150,255)},{Color(0,200,255,255)},{Color(255,45,0,255)},{Color(255,200,0,255)},{Color(150,0,255,255)},{Color(0,100,255,255)},{Color(200,255,0,255)},{Color(0,255,30,255)},{Color(225,225,255,255)},{Color(107, 147, 157)},{Color(0,150,255,255)},{Color(255,0,255,255)},{Color(0,20,255,255)},{Color(90, 0, 255,255)},{Color(0,255,150,255)},{Color(0,200,255,255)},{Color(255,45,0,255)},{Color(255,200,0,255)},{Color(150,0,255,255)},{Color(0,100,255,255)},{Color(200,255,0,255)},{Color(0,255,30,255)},{Color(225,225,255,255)},{Color(107, 147, 157)},{Color(0,150,255,255)},{Color(255,0,255,255)},{Color(0,20,255,255)},{Color(90, 0, 255,255)},{Color(0,255,150,255)},{Color(0,200,255,255)},{Color(255,45,0,255)},{Color(255,200,0,255)},{Color(150,0,255,255)},{Color(0,100,255,255)},{Color(200,255,0,255)},{Color(0,255,30,255)},{Color(225,225,255,255)},{Color(107, 147, 157)},{Color(0,150,255,255)},{Color(255,0,255,255)},{Color(0,20,255,255)},{Color(90, 0, 255,255)},{Color(0,255,150,255)},{Color(0,200,255,255)},{Color(255,45,0,255)},{Color(255,200,0,255)},{Color(150,0,255,255)},{Color(0,100,255,255)},{Color(200,255,0,255)},{Color(0,255,30,255)},{Color(225,225,255,255)},{Color(107, 147, 157)},{Color(0,150,255,255)},{Color(255,0,255,255)},{Color(0,20,255,255)} };

*/
inline Options g_Options;
inline bool   g_Unload;
