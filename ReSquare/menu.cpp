#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "ui.hpp"
#include "config.hpp"
#include "options.hpp"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"
#include <ctime>
#include <shlobj.h>
#include <filesystem>
#include <direct.h>
#include "render.hpp"
#include "hacks/walkbot.h"
#include "hacks/inventory.h"
#include "hacks/skin.h"
#pragma comment(lib, "shell32.lib")
const char* knifelist[] = { "Bayonet", "Flip", "Gut", "Karambit", "M9 Bayonet", "Huntsman", "Falchion", "Bowie", "Butterfly", "Daggers", "Paracord", "Survival",  "Ursus", "Navaja", "Nomad", "Stiletto", "Talon", "Skeleton" };
const char* glovelist[] = { "None", "Wraps", "Bloodhound", "Sporty", "Slick", "Motorcycle", "Specialist", "Hydra"};
const char* skyboxList[] = { "Default", "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_cs15_daylight01_hdr", "sky_cs15_daylight02_hdr", "sky_cs15_daylight03_hdr", "sky_cs15_daylight04_hdr", "sky_csgo_cloudy01", "sky_csgo_night_flat", "sky_csgo_night02", "sky_day02_05_hdr", "sky_day02_05", "sky_dust", "sky_l4d_rural02_ldr", "sky_venice", "vertigo_hdr", "vertigo", "vertigoblue_hdr", "vietnam", "sky_lunacy", "sky_hr_aztec" };
void Menu::Initialize()
{

    _visible = true;
}

void Menu::Shutdown()
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void Menu::OnDeviceLost()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
    ImGui_ImplDX9_CreateDeviceObjects();
}
int inv_definitionindex = 0; int inv_stattrack = -1; int inv_paintkit = 0; int inv_seed = 0; float inv_wear = 0.0001; int inv_origin = 0; int inv_rarity = 0; int inv_quality = 0; static char inv_customname[50] = ""; static char inv_customdesc[550] = "";
int cursticker1 = 0;
int cursticker2 = 0;
int cursticker3[] = { 0,0,0,0,0 };
int cursticker4[] = { 0,0,0,0,0 };
Options::Sticker sticker[5];
static const char*  inv_weaponnames = "deagle\0elite\0fiveseven\0glock\0ak47\0aug\0awp\0famas\0g3sg1\0galilar\0m249\0m4a1\0mac10\0p90\0zone_repulsor\0mp5sd\0ump45\0xm1014\0bizon\0mag7\0negev\0sawedoff\0tec9\0taser\0hkp2000\0mp7\0mp9\0nova\0p250\0shield\0scar20\0sg556\0ssg08\0knifegg\0knife\0flashbang\0hegrenade\0smokegrenade\0molotov\0decoy\0incgrenade\0c4\0item_kevlar\0item_assaultsuit\0item_heavyassaultsuit\0item_nvg\0item_defuser\0item_cutters\0healthshot\0knife_t\0m4a1_silencer\0usp_silencer\0cz75a\0revolver\0tagrenade\0fists\0breachcharge\0tablet\0melee\0axe\0hammer\0spanner\0knife_ghost\0firebomb\0diversion\0frag_grenade\0snowball\0bumpmine\0bayonet\0knife_css\0knife_flip\0knife_gut\0knife_karambit\0knife_m9_bayonet\0knife_tactical\0knife_falchion\0knife_survival_bowie\0knife_butterfly\0knife_push\0knife_cord\0knife_canis\0knife_ursus\0knife_gypsy_jackknife\0knife_outdoor\0knife_stiletto\0knife_widowmaker\0knife_skeleton\0tm_professional_varf5\0ctm_st6_variantj\0ctm_st6_variantl\0ctm_swat_variante\0ctm_swat_variantf\0ctm_swat_variantg\0ctm_swat_varianth\0ctm_swat_varianti\0ctm_swat_variantj\0tm_balkan_variantk\0studded_brokenfang_gloves\0tm_professional_varf\0tm_professional_varg\0tm_professional_varh\0tm_professional_varj\0tm_professional_vari\0tm_professional_varf1\0tm_professional_varf2\0tm_professional_varf3\0tm_professional_varf4\0ctm_gendarmerie_varianta\0ctm_gendarmerie_variantb\0ctm_gendarmerie_variantc\0ctm_gendarmerie_variantd\0ctm_gendarmerie_variante\0ctm_swat_variantk\0ctm_diver_varianta\0ctm_diver_variantb\0ctm_diver_variantc\0tm_jungle_raider_varianta\0tm_jungle_raider_variantb\0tm_jungle_raider_variantc\0tm_jungle_raider_variantd\0tm_jungle_raider_variante\0tm_jungle_raider_variantf\0tm_jungle_raider_variantb2\0tm_jungle_raider_variantf2\0studded_bloodhound_gloves\0t_gloves\0ct_gloves\0sporty_gloves\0slick_gloves\0leather_handwraps\0motorcycle_gloves\0specialist_gloves\0studded_hydra_gloves\0t_map_based\0ct_map_based\0tm_anarchist\0tm_anarchist_varianta\0tm_anarchist_variantb\0tm_anarchist_variantc\0tm_anarchist_variantd\0tm_pirate\0tm_pirate_varianta\0tm_pirate_variantb\0tm_pirate_variantc\0tm_pirate_variantd\0tm_professional\0tm_professional_var1\0tm_professional_var2\0tm_professional_var3\0tm_professional_var4\0tm_separatist\0tm_separatist_varianta\0tm_separatist_variantb\0tm_separatist_variantc\0tm_separatist_variantd\0ctm_gign\0ctm_gign_varianta\0ctm_gign_variantb\0ctm_gign_variantc\0ctm_gign_variantd\0ctm_gsg9\0ctm_gsg9_varianta\0ctm_gsg9_variantb\0ctm_gsg9_variantc\0ctm_gsg9_variantd\0ctm_idf\0ctm_idf_variantb\0ctm_idf_variantc\0ctm_idf_variantd\0ctm_idf_variante\0ctm_idf_variantf\0ctm_swat\0ctm_swat_varianta\0ctm_swat_variantb\0ctm_swat_variantc\0ctm_swat_variantd\0ctm_sas_varianta\0ctm_sas_variantb\0ctm_sas_variantc\0ctm_sas_variantd\0ctm_st6\0ctm_st6_varianta\0ctm_st6_variantb\0ctm_st6_variantc\0ctm_st6_variantd\0tm_balkan_variante\0tm_balkan_varianta\0tm_balkan_variantb\0tm_balkan_variantc\0tm_balkan_variantd\0tm_jumpsuit_varianta\0tm_jumpsuit_variantb\0tm_jumpsuit_variantc\0tm_phoenix_heavy\0ctm_heavy\0tm_leet_varianta\0tm_leet_variantb\0tm_leet_variantc\0tm_leet_variantd\0tm_leet_variante\0tm_leet_variantg\0tm_leet_varianth\0tm_leet_varianti\0tm_leet_variantf\0tm_leet_variantj\0tm_phoenix\0tm_phoenix_varianta\0tm_phoenix_variantb\0tm_phoenix_variantc\0tm_phoenix_variantd\0tm_phoenix_varianth\0tm_phoenix_variantf\0tm_phoenix_variantg\0tm_phoenix_varianti\0ctm_fbi\0ctm_fbi_varianta\0ctm_fbi_variantc\0ctm_fbi_variantd\0ctm_fbi_variante\0ctm_fbi_variantf\0ctm_fbi_variantg\0ctm_fbi_varianth\0ctm_fbi_variantb\0ctm_st6_variantk\0ctm_st6_variante\0ctm_st6_variantg\0ctm_st6_variantm\0ctm_st6_varianti\0ctm_st6_variantn\0tm_balkan_variantf\0tm_balkan_varianti\0tm_balkan_variantg\0tm_balkan_variantj\0tm_balkan_varianth\0tm_balkan_variantl\0ctm_sas\0ctm_sas_variantf\0ctm_sas_variantg\0";
int inv_weaponindex[] = { 1,2,3,4,7,8,9,10,11,13,14,16,17,19,20,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,54,55,56,57,59,60,61,63,64,68,69,70,72,74,75,76,78,80,81,82,83,84,85,500,503,505,506,507,508,509,512,514,515,516,517,518,519,520,521,522,523,525,4613,4619,4680,4711,4712,4713,4714,4715,4716,4718,4725,4726,4727,4728,4730,4732,4733,4734,4735,4736,4749,4750,4751,4752,4753,4756,4757,4771,4772,4773,4774,4775,4776,4777,4778,4780,4781,5027,5028,5029,5030,5031,5032,5033,5034,5035,5036,5037,5038,5039,5040,5041,5042,5043,5044,5045,5046,5047,5048,5049,5050,5051,5052,5053,5054,5055,5056,5057,5058,5059,5060,5061,5062,5063,5064,5065,5066,5067,5068,5069,5070,5071,5072,5073,5074,5075,5076,5077,5078,5079,5080,5081,5082,5083,5084,5085,5086,5087,5088,5089,5090,5091,5092,5093,5094,5095,5096,5097,5100,5101,5102,5103,5104,5105,5106,5107,5108,5109,5200,5201,5202,5203,5204,5205,5206,5207,5208,5300,5301,5302,5303,5304,5305,5306,5307,5308,5400,5401,5402,5403,5404,5405,5500,5501,5502,5503,5504,5505,5600,5601,5602 };

const char chararray[] = {0x20, 0x6C, 0x336, 0x6F, 0x338, 0x73, 0x334, 0x69, 0x334, 0x6E, 0x338, 0x67, 0x335, 0x20, 0x6D, 0x336, 0x315, 0x34B, 0x360, 0x32A, 0x32B, 0x79, 0x337, 0x30A, 0x306, 0x355, 0x20, 0x66, 0x337, 0x343, 0x301, 0x346, 0x33F, 0x310, 0x31A, 0x30F, 0x307, 0x32A, 0x331, 0x324, 0x75, 0x335, 0x301, 0x300, 0x311, 0x302, 0x30D, 0x309, 0x345, 0x32F, 0x347, 0x31E, 0x31E, 0x326, 0x34E, 0x63, 0x337, 0x346, 0x312, 0x307, 0x33D, 0x343, 0x35D, 0x31A, 0x333, 0x348, 0x32F, 0x333, 0x31E, 0x31F, 0x355, 0x329, 0x321, 0x6B, 0x338, 0x343, 0x314, 0x341, 0x33D, 0x339, 0x345, 0x318, 0x353, 0x321, 0x35A, 0x31C, 0x328, 0x69, 0x335, 0x342, 0x30A, 0x30A, 0x341, 0x344, 0x310, 0x33E, 0x33D, 0x319, 0x35A, 0x319, 0x328, 0x6E, 0x337, 0x343, 0x31B, 0x305, 0x327, 0x31D, 0x34D, 0x322, 0x326, 0x67, 0x338, 0x30D, 0x328, 0x348, 0x32A, 0x34D, 0x32B, 0x20, 0x6D, 0x335, 0x357, 0x35D, 0x30D, 0x30C, 0x320, 0x32A, 0x326, 0x31C, 0x327, 0x326, 0x33C, 0x32E, 0x69, 0x334, 0x308, 0x306, 0x302, 0x30E, 0x340, 0x307, 0x344, 0x359, 0x32C, 0x319, 0x328, 0x316, 0x318, 0x345, 0x6E, 0x335, 0x351, 0x30D, 0x303, 0x344, 0x35C, 0x64, 0x336, 0x350, 0x310, 0x358, 0x346, 0x358, 0x357, 0x344, 0x309, 0x33D, 0x312, 0x303, 0x360, 0x309, 0x35D, 0x30C, 0x33D, 0x31D, 0x31C};

bool refreshed = false;
bool cfginit = false;
static float menusize = 1;
static int tab = 8;
static int currentTheme = 0;
static int currentWeapon = 0;
static int currentWeapon1 = 0;
static int currentWeapon2 = 0;
static int currentWeapon3 = 1;
static int currentWeapon4 = 0;
static int currentWeapon5 = 0;
static int currentTarget = 0;
static int currentTarget1 = 0;
static int currentTarget2 = 0;
static int currentAA = 0;
static int currentAngle = 0;
static int currentSkin = 0;
static int currentKnife = 0;
static int currentConfig = 0;
static int additionalTarget = 0;
static char search[50] = "";
static char search2[50] = "";
static char cfgpath[50] = "D:\\Shortcuts\\Neuer Ordner\\Resquare\\";
static char cfgname[50] = "Config.cfg";
static std::vector<std::string> configs;
static const char* smoke[] = { "Off", "Invisible", "Wireframe" };
static const char* doubletaps[] = { "Off", "Normal", "Fast", "Accurate", "Instant", "Defensive","Adaptive","Hide-Shots" };
static const char* hitBoxes[] ={"Head","Neck","Pelvis","Abdomen","Kidneys","Sternum","Clavicles","Left Thigh","Right Thigh","Left Shin","Right Shin","Left Ankle","Right Ankle","Left Hand","Right Hand","Left Arm","Left Forearm","Right Arm","Right Forearm",};
static const char* weaponsall[] = { "Gloves", "Knife", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver", "Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };
static const char* weapons[] = { "All", "Pistols", "Heavy Pistols", "Shotguns", "Machine Guns", "Smgs", "Rifles", "Snipers", "Glock-18", "P2000", "USP-S", "Dual Berettas", "P250", "Tec-9", "Five-Seven", "CZ-75", "Desert Eagle", "Revolver","Nova", "XM1014", "Sawed-off", "MAG-7", "M249", "Negev", "Mac-10", "MP9", "MP7", "MP5-SD", "UMP-45", "P90", "PP-Bizon", "Galil AR", "Famas", "AK-47", "M4A4", "M4A1-S", "SSG-08", "SG-553", "AUG", "AWP", "G3SG1", "SCAR-20" };
static const char* glow[] = { "Normal", "Rim", "Edge", "Edge Pulse"};
static const char* chamtype[] = { "Normal", "Flat", "Gloss", "Trippy", "Glass", "Velvet", "Glow", "Light Chrome", "Chrome", "Chrome Dark", "Pearlescent", "Animated", "EsoGlow"};
static const char* chamstarget[] = { "Local Player", "Enemy Visible", "Enemy Invisible", "Team Visible", "Team Invisible", "Defusing Visible", "Defusing Invisible", "Has C4 Visible", "Has C4 Invisible", "Backtrack", "Desync", "Arms", "Sleeves", "Weapons" };
static char* AntiaimPitchNames[] ={"None","Down","Up","Dance","Fake Up","Fake Down","Random"};
static char* AntiaimYawNames[] ={"None","Normal","Spin","Side","Fake LBY 1","Fake LBY 2","Fake Side LBY","Random","Anti-Brute","Freestanding","Sway","Flick"};
std::vector<std::string> bones = { "HEAD","NECK","PELVIS","STOMACH","LOWER CHEST","CHEST","UPPER CHEST","RIGHT THIGH","LEFT THIGH","RIGHT CALF","LEFT CALF","RIGHT FOOT","LEFT FOOT","RIGHT HAND","LEFT HAND","RIGHT UPPER ARM","RIGHT FOREARM","LEFT UPPER ARM","LEFT FOREARM" };
std::vector<std::string> entities = { "Local Player", "Enemy Visible", "Enemy Invisible", "Team Visible", "Team Invisible", "Defusing Visible", "Defusing Invisible", "Has C4 Visible", "Has C4 Invisible", "Grenades", "Chicken", "Weapons", "Loot" };
std::vector<std::string> espentities = { "Enemy Visible", "Enemy Invisible", "Team Visible", "Team Invisible", "Grenades", "C4", "Weapons", "Loot" };

/*PDIRECT3DTEXTURE9 LoadMenuImage()
{
    PDIRECT3DTEXTURE9 texture;
    HRESULT hr = D3DXCreateTextureFromFileInMemory(g_D3DDevice9, Menu::Get().MenuImage, sizeof(Menu::Get().MenuImage), &texture);
    if (hr != S_OK)
        return false;
    return texture;
}*/

void Menu::Render()
{
    if (g_Skins.skinlistid.size() < 1)
        g_Skins.Dump();
    ImGui::GetIO().MouseDrawCursor = _visible;

    if (menurender<0)
        return; 
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    style.Alpha = std::min(std::max(menurender, 1.f) / 200, 1.f);
    ImGui::SetNextWindowPos(ImVec2(-52 * menusize, 140 * menusize));
    ImGui::SetNextWindowSize(ImVec2((244 - 192 + 192 * std::min(menurender/ 2, 100.f)/100) * menusize, 809 * menusize));
    ImGui::Begin("Re:Square", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SetCursorPos({ 71 * menusize, 84 * menusize });
    ImGui::SetWindowFontScale(menusize*1.4);
    ImGui::TextColored(ImVec4(255, 255, 255, 255), "RE");
    ImGui::SetCursorPos({ (ImGui::CalcTextSize("Re").x + 71* menusize) , (84 * menusize) });
    ImGui::Text("SQUARE");
    ImGui::SetWindowFontScale(menusize*1);
    ImGui::SetCursorPos({ 108 * menusize, 233 * menusize });
    if (ImGui::Button("Legit"))
        tab = 0;
    ImGui::SetCursorPos({ 108 * menusize, (233 + 49 * 1) * menusize });
    if (ImGui::Button("Rage"))
        tab = 1;    
    //ImGui::SetCursorPos({ 108 * menusize, (233 + 49 * 2) * menusize });
    //if (ImGui::Button("Anti-Aim"))
    //    tab = 9;
    ImGui::SetCursorPos({ 108 * menusize, (233 + 49 * 2) * menusize });
    if (ImGui::Button("Esp"))
        tab = 2;
    ImGui::SetCursorPos({ 108 * menusize, (233 + 49 * 3) * menusize });
    if (ImGui::Button("Visuals"))
        tab = 3;
    ImGui::SetCursorPos({ 108 * menusize, (233 + 49 * 4) * menusize });
    if (ImGui::Button("Misc"))
        tab = 4;
    ImGui::SetCursorPos({ 108 * menusize, (233 + 49 * 5) * menusize });
    if (ImGui::Button("Movement"))  
        tab = 5;    
    ImGui::SetCursorPos({ 108 * menusize, (233 + 49 * 6) * menusize });
    if (ImGui::Button("Skins"))
        tab = 6;
    ImGui::SetWindowFontScale(menusize*0.9);
    ImGui::SetCursorPos({ 108 * menusize, 663 * menusize });
    if (ImGui::Button("Config"))
        tab = 7;
    ImGui::SetWindowFontScale(menusize*1);
    ImGui::End();
    ImGui::SetNextWindowPos(ImVec2(480 * menusize, 190 * menusize));
    ImGui::SetNextWindowSize(ImVec2(960 * menusize, 720 * menusize));
    ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    switch (tab)
    {
    case 8:
        ImGui::SetWindowFontScale(menusize * 2);
        ImGui::SetCursorPos({ (ImGui::GetContentRegionAvail().x-ImGui::CalcTextSize("RESQUARE").x) / 2.f, 0.f });
        ImGui::TextColored(ImVec4(255, 255, 255, 255), "RE");
        ImGui::SetCursorPos({ (ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("RESQUARE").x) / 2.f+ImGui::CalcTextSize("RE").x, 0.f });
        ImGui::Text("SQUARE");
        ImGui::SetWindowFontScale(menusize * 1);
        ImGui::Text(
            "[12.03.2022]\n"
            "\n"
            "Current issues :\n"
            "\n"
            "-Animfix is crashing game sometimes, disable it in Config Tab\n"
            "-Disable Anti-Untrusted when using Doubletap\n"
            "-Honestly hits more head without the Resolver\n"
            "-Im genuinly losing my fucking mind\n-i fucking cant anymore\n-another month riddled with bugs and issues i have to fix\n-Backtrack is experimental, always use max targeting otherwise it may brokey");
        ImGui::SetCursorPos({ 6, ImGui::GetWindowSize().y - ImGui::CalcTextSize("A").y * 3 });
        ImGui::Text("[This is a free cheat, if you payed for it, you got scammed]");
        ImGui::SetCursorPos({ 2, ImGui::GetWindowSize().y - ImGui::CalcTextSize("A").y * 2 });
        if(ImGui::Button("[Report a bug : https://discord.gg/9j5NnPTWMv]"))
            ShellExecute(0, 0, L"https://discord.gg/9j5NnPTWMv", 0, 0, SW_SHOW);
        break;
    case 1:
        ImGui::Columns(2, nullptr, false);
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text("    Ragebot");
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::SameLine();
        ImGui::Checkbox("##rageenable", &g_Options.rage);        
        ImGui::BeginGroupPanel("Ragebot", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Combo("Weapon", &currentWeapon5, weapons, 42);
            if (ImGui::ListBoxHeader("##rage", { ImGui::GetContentRegionAvail().x - 9, ImGui::GetTextLineHeightWithSpacing() *5.2f + style.ItemSpacing.y - 5 }))
            {
                for (int n = 0; n < 19; n++) {
                    ImGui::PushID(n);
                    ImGui::Checkbox(hitBoxes[n], &g_Options.ragebot_settings[currentWeapon5].bones[n]);
                    ImGui::PopID();
                }
                ImGui::ListBoxFooter();
            }
            ImGui::Checkbox("Enabled##rage", &g_Options.ragebot_settings[currentWeapon5].enabled);
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##enabledrage", &g_Options.keys.RagebotKey);
            ImGui::PopItemWidth();
            ImGui::Checkbox("Resolver", &g_Options.resolver);
            ImGui::Checkbox("Silent##rage", &g_Options.ragebot_settings[currentWeapon5].silent);
            ImGui::PushItemWidth(180);
            ImGui::Combo("Double-Tap##rage", &g_Options.ragebot_settings[currentWeapon5].dt, doubletaps, 8);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::PushItemWidth(130);
            ImGui::Hotkey("##ragedt", &g_Options.misc_settings.dtkey, ImVec2(0, 0));
            ImGui::PopItemWidth();
            ImGui::SliderFloat("Minchance", &g_Options.ragebot_settings[currentWeapon5].minchance, 0, 100, "%.f");
            ImGui::SliderFloat("Fov", &g_Options.ragebot_settings[currentWeapon5].fov, 0, 360, "%.1f");
            ImGui::SliderInt("Mindamage", &g_Options.ragebot_settings[currentWeapon5].mindmg, 0, 120);
            ImGui::SliderInt("Auto-Stop", &g_Options.ragebot_settings[currentWeapon5].AutoStop, 0, 100);
            ImGui::Checkbox("Auto-Shot##rage", &g_Options.ragebot_settings[currentWeapon5].AutoShot);
            ImGui::Checkbox("Auto-Crouch##rage", &g_Options.ragebot_settings[currentWeapon5].AutoCrouch);
            if(currentWeapon5 == 0 || currentWeapon5 == 29 || currentWeapon5 == 30 || currentWeapon5 == 31 || currentWeapon5 == 32 || currentWeapon5 == 33 || currentWeapon5 == 34)
            ImGui::Checkbox("Auto-Scope##rage", &g_Options.ragebot_settings[currentWeapon5].AutoScope);
            ImGui::Hotkey("QuickPeek##rage", &g_Options.misc_settings.hotkeyautopeek, ImVec2(0, 0));
            ImGui::SameLine();
            ImGui::Checkbox("Choke", &g_Options.misc_settings.chokeonpeek);
            ImGui::Checkbox("Killshot##rage", &g_Options.ragebot_settings[currentWeapon5].killshot);
            ImGui::SameLine();
            ImGui::PushItemWidth(200);
            ImGui::SliderInt("Overkill", &g_Options.ragebot_settings[currentWeapon5].overkill, 0, 100);
            ImGui::PopItemWidth();
        }
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        ImGui::NextColumn();
        ImGui::SetWindowFontScale(menusize*1.3);
        ImGui::Dummy({ 1, ImGui::CalcTextSize(" ").y - 1 });
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::BeginGroupPanel("Backtrack", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2 )});
        {
            ImGui::Checkbox("Enabled##backtrack", &g_Options.backtrack_settings.enabled);
            ImGui::Checkbox("Draw All", &g_Options.backtrack_settings.shoulddrawall);
            ImGui::SliderInt("", &g_Options.backtrack_settings.time, 0, 24, "%d ticks");
            ImGui::SliderInt("Target", &g_Options.backtrack_settings.targetlast, 0, 24);
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Anti-Aim", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2 )});
        {
            ImGui::Checkbox("Enabled##antiaim", &g_Options.antiaim_settings.enabled);
            ImGui::Combo("Angle", &currentAngle, "Real\0Fake\0");
            ImGui::Combo("Type", &currentAA, "Override\0Walking\0Slowwalk\0In Air\0Standing\0");
            ImGui::Checkbox("Override##antiaim", &g_Options.antiaim_settings.overrideenable[currentAA]);
            if (currentAA == 0 || currentAA == 4)
                ImGui::Checkbox("LBY-Breaker##antiaim", &g_Options.antiaim_settings.lbybreak[currentAA]);
            ImGui::Combo("Pitch", &g_Options.antiaim_settings.antiaim_pitch[currentAA], AntiaimPitchNames, ARRAYSIZE(AntiaimPitchNames));
            if (currentAngle == 0)
                ImGui::Combo("Yaw", &g_Options.antiaim_settings.antiaim_yaw[currentAA], AntiaimYawNames, ARRAYSIZE(AntiaimYawNames));
            else
                ImGui::Combo("Yaw", &g_Options.antiaim_settings.antiaim_yaw_fake[currentAA], AntiaimYawNames, ARRAYSIZE(AntiaimYawNames));
            ImGui::Hotkey("Flip Side##antiaim", &g_Options.antiaim_settings.tankAntiaimKey, ImVec2(0, 0));
            //ImGui::Checkbox("Anti-Resolver", &g_Options.antiaim_settings.antiaim_antiresolver);
            //ImGui::Checkbox("On Knife", &g_Options.antiaim_settings.antiaim_knife);
            ImGui::PushItemWidth(240);
            if (currentAngle == 0)
                ImGui::SliderFloat("Custom Value", &g_Options.antiaim_settings.antiaim_spin_speed[currentAA], -360, 360, "%.f");
            else
                ImGui::SliderFloat("Custom Value", &g_Options.antiaim_settings.antiaim_spin_speed_fake[currentAA], -360, 360, "%.f");

            if (currentAngle == 0)
                ImGui::SliderFloat("Additional Yaw", &g_Options.antiaim_settings.addvalue[currentAA], -180, 180, "%.f");
            else
                ImGui::SliderFloat("Additional Yaw", &g_Options.antiaim_settings.addvalue_fake[currentAA], -180, 180, "%.f");

            if (currentAngle == 0)
                ImGui::SliderFloat("Jitter Offset", &g_Options.antiaim_settings.jitter[currentAA], -180, 180, "%.f");
            else
                ImGui::SliderFloat("Jitter Offset", &g_Options.antiaim_settings.jitter_fake[currentAA], -180, 180, "%.f");

            ImGui::Checkbox("At Enemy", &g_Options.antiaim_settings.at_enemy[currentAA]);
            ImGui::SliderInt("Fakelag", &g_Options.antiaim_settings.fakelag_amount[currentAA], 0, 32);
            ImGui::PopItemWidth();
        }
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        if (g_Options.rage)
            g_Options.legit = false;
        break;
    case 9:
        ImGui::Columns(2, nullptr, false);
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text("    Anti-Aim");
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::BeginGroupPanel("Anti-Aim", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Checkbox("Enabled##antiaim", &g_Options.antiaim_settings.enabled);
            ImGui::Combo("Angle", &currentAngle, "Real\0Fake\0");
            ImGui::Combo("Type", &currentAA, "Override\0Walking\0Slowwalk\0In Air\0Standing\0");
            ImGui::Checkbox("Override##antiaim", &g_Options.antiaim_settings.overrideenable[currentAA]);
            if(currentAA==0||currentAA==4)
            ImGui::Checkbox("LBY-Breaker##antiaim", &g_Options.antiaim_settings.lbybreak[currentAA]);
            ImGui::Combo("Pitch", &g_Options.antiaim_settings.antiaim_pitch[currentAA], AntiaimPitchNames, ARRAYSIZE(AntiaimPitchNames));
            if (currentAngle==0)
            ImGui::Combo("Yaw", &g_Options.antiaim_settings.antiaim_yaw[currentAA], AntiaimYawNames, ARRAYSIZE(AntiaimYawNames));
            else
            ImGui::Combo("Yaw", &g_Options.antiaim_settings.antiaim_yaw_fake[currentAA], AntiaimYawNames, ARRAYSIZE(AntiaimYawNames));
            ImGui::Hotkey("Flip Side##antiaim", &g_Options.antiaim_settings.tankAntiaimKey, ImVec2(0, 0));
            //ImGui::Checkbox("Anti-Resolver", &g_Options.antiaim_settings.antiaim_antiresolver);
            //ImGui::Checkbox("On Knife", &g_Options.antiaim_settings.antiaim_knife);
            ImGui::PushItemWidth(240);
            if (currentAngle == 0)
            ImGui::SliderFloat("Custom Value", &g_Options.antiaim_settings.antiaim_spin_speed[currentAA], -360, 360, "%.f");
            else
            ImGui::SliderFloat("Custom Value", &g_Options.antiaim_settings.antiaim_spin_speed_fake[currentAA], -360, 360, "%.f");

            if (currentAngle == 0)
                ImGui::SliderFloat("Additional Yaw", &g_Options.antiaim_settings.addvalue[currentAA], -180, 180, "%.f");
            else
                ImGui::SliderFloat("Additional Yaw", &g_Options.antiaim_settings.addvalue_fake[currentAA], -180, 180, "%.f");            
            
            if (currentAngle == 0)
                ImGui::SliderFloat("Jitter Offset", &g_Options.antiaim_settings.jitter[currentAA], -180, 180, "%.f");
            else
                ImGui::SliderFloat("Jitter Offset", &g_Options.antiaim_settings.jitter_fake[currentAA], -180, 180, "%.f");

            ImGui::Checkbox("At Enemy", &g_Options.antiaim_settings.at_enemy[currentAA]);
            ImGui::SliderInt("Fakelag", &g_Options.antiaim_settings.fakelag_amount[currentAA], 0, 32);
            ImGui::PopItemWidth();
        }
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        ImGui::NextColumn();
        ImGui::SetWindowFontScale(menusize*1.3);
        ImGui::Dummy({ 1, ImGui::CalcTextSize(" ").y - 1 });
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::BeginGroupPanel("The fuck do i put here", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2 )});
        {
        }
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        break;
    case 2:
        ImGui::Columns(2, nullptr, false);
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text("    Wallhacks");
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::BeginGroupPanel("Chams", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::PushItemWidth(240);
            ImGui::Combo("Target", &currentTarget, chamstarget, 14);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::PushItemWidth(100);
            ImGui::SliderInt("", &additionalTarget, 0, 4);
            ImGui::PopItemWidth();
            ImGui::Checkbox("Enabled##chams", &g_Options.chams_settings[currentTarget+additionalTarget*14].enabled);
            ImGui::Checkbox("Through Walls##chams", &g_Options.chams_settings[currentTarget+additionalTarget*14].throughwalls);
            ImGui::Combo("Type", &g_Options.chams_settings[currentTarget+additionalTarget*14].type, chamtype, 13);
            ImGui::SameLine();
            bool openPopup = ImGui::ColorButton("##btn", g_Options.chams_settings[currentTarget+additionalTarget*14].color, ImGuiColorEditFlags_AlphaPreview);
            ImGui::SameLine();
            if (openPopup)
                ImGui::OpenPopup("##popup");

            if (ImGui::BeginPopup("##popup")) {
                ImGui::ColorPicker4(chamstarget[currentTarget], &g_Options.chams_settings[currentTarget+additionalTarget*14].color.x);
                ImGui::EndPopup();
            }
            ImGui::PopID();
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Glow", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2 )});
        {
            if (ImGui::ListBoxHeader("##glow", { ImGui::GetContentRegionAvail().x - 9, ImGui::GetContentRegionAvail().y - 18 }))
            {
                for (int n = 0; n < 13; n++) {
                    ImGui::PushID(n);
                    ImGui::Checkbox("##glow1", &g_Options.glow_settings[n].enabled);
                    ImGui::PopID();
                    ImGui::SameLine();
                    ImGui::PushID(n + 15);
                    ImGui::PushItemWidth(150);
                    ImGui::Combo("##glow2", &g_Options.glow_settings[n].type, glow, 4);
                    ImGui::PopItemWidth();
                    ImGui::PopID();
                    ImGui::SameLine();
                    ImGui::PushID(n + 30);
                    bool openPopup = ImGui::ColorButton("##btn1", g_Options.glow_settings[n].color);
                    ImGui::SameLine();
                    ImGui::Text(entities.at(n).c_str());
                    if (openPopup)
                        ImGui::OpenPopup("##popup");

                    if (ImGui::BeginPopup("##popup")) {
                        ImGui::ColorPicker3(entities.at(n).c_str(), &g_Options.glow_settings[n].color.x);
                        ImGui::EndPopup();
                    }
                    ImGui::PopID();
                }
                ImGui::ListBoxFooter();
            }
        }
        ImGui::EndGroupPanel();
        ImGui::NextColumn();
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text(" ");//to lazy to dummy
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::BeginGroupPanel("Dlight", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2 )});
        {
            ImGui::Combo(" ", &currentTarget1, entities);
            ImGui::Checkbox("Enabled", &g_Options.dlight_settings[currentTarget1].enabled);
            ImGui::PushItemWidth(240);
            ImGui::SliderFloat("Exponent", &g_Options.dlight_settings[currentTarget1].exponent, 1, 30);
            ImGui::SliderFloat("Radius", &g_Options.dlight_settings[currentTarget1].radius, 1, 100);
            ImGui::SliderFloat("Offset", &g_Options.dlight_settings[currentTarget1].offset, 0, 100);
            ImGui::PopItemWidth();
            bool openPopup = ImGui::ColorButton("##btn", g_Options.dlight_settings[currentTarget1].color);
            ImGui::SameLine();
            ImGui::Text("Color");
            if (openPopup)
                ImGui::OpenPopup("##popup1");

            if (ImGui::BeginPopup("##popup1")) {
                ImGui::ColorPicker3(entities.at(currentTarget1).c_str(), &g_Options.dlight_settings[currentTarget1].color.x);
                ImGui::EndPopup();
            }
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Esp", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2 )});
        {
            ImGui::Combo(" ##targets", &currentTarget2, espentities);
            ImGui::Combo("Type", &g_Options.esp_settings[currentTarget2].type, "Disabled\0002D\0003D");
            bool openPopup = ImGui::ColorButton("##espbtn", g_Options.esp_settings[currentTarget2].color, ImGuiColorEditFlags_AlphaPreview);
            if (openPopup)
                ImGui::OpenPopup("##esppopup");

            if (ImGui::BeginPopup("##esppopup")) {
                ImGui::ColorPicker4("Color", &g_Options.esp_settings[currentTarget2].color.x);
                ImGui::EndPopup();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Enabled        ##esp", &g_Options.esp_settings[currentTarget2].lineenabled);
            ImGui::SameLine();
            ImGui::PushID(9991);
            ImGui::PushItemWidth(140);
            ImGui::SliderFloat("", &g_Options.esp_settings[currentTarget2].linethiccness, 0, 3, "%.1f");
            ImGui::PopItemWidth();
            ImGui::PopID();
            bool openPopup1 = ImGui::ColorButton("##espbtn1", g_Options.esp_settings[currentTarget2].coloroutline, ImGuiColorEditFlags_AlphaPreview);
            if (openPopup1)
                ImGui::OpenPopup("##esppopup1");

            if (ImGui::BeginPopup("##esppopup1")) {
                ImGui::ColorPicker4("Color", &g_Options.esp_settings[currentTarget2].coloroutline.x);
                ImGui::EndPopup();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Add Outline", &g_Options.esp_settings[currentTarget2].outlineenabled);
            ImGui::SameLine();
            ImGui::PushID(9992);
            ImGui::PushItemWidth(140);
            ImGui::SliderFloat("", &g_Options.esp_settings[currentTarget2].outlinethiccness, 0, 3, "%.1f");
            ImGui::PopItemWidth();
            ImGui::PopID();
            if (currentTarget2 <= 3) {
                bool openPopup5 = ImGui::ColorButton("##espbtn5", g_Options.esp_settings[currentTarget2].colorskeleton, ImGuiColorEditFlags_AlphaPreview);
                if (openPopup5)
                    ImGui::OpenPopup("##esppopup5");

                if (ImGui::BeginPopup("##esppopup5")) {
                    ImGui::ColorPicker4("Color", &g_Options.esp_settings[currentTarget2].colorskeleton.x);
                    ImGui::EndPopup();
                }
                ImGui::SameLine();
                ImGui::Checkbox("Skeleton       ##esp", &g_Options.esp_settings[currentTarget2].skeletonenabled);
                ImGui::SameLine();
                ImGui::PushID(9995);
                ImGui::PushItemWidth(140);
                ImGui::SliderFloat("", &g_Options.esp_settings[currentTarget2].skeletonthiccness, 0, 3, "%.1f");
                ImGui::PopItemWidth();
                ImGui::PopID();
            }
            bool openPopup2 = ImGui::ColorButton("##espbtn2", g_Options.esp_settings[currentTarget2].colorsnap, ImGuiColorEditFlags_AlphaPreview);
            if (openPopup2)
                ImGui::OpenPopup("##esppopup2");

            if (ImGui::BeginPopup("##esppopup2")) {
                ImGui::ColorPicker4("Color", &g_Options.esp_settings[currentTarget2].colorsnap.x);
                ImGui::EndPopup();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Snapline       ", &g_Options.esp_settings[currentTarget2].snaplineenabled);
            ImGui::SameLine();
            ImGui::PushID(9993);
            ImGui::PushItemWidth(140);
            ImGui::SliderFloat("", &g_Options.esp_settings[currentTarget2].snaplinethiccness, 0, 3, "%.1f");
            ImGui::PopItemWidth();
            ImGui::PopID();
            ImGui::Combo("Snap Type", &g_Options.esp_settings[currentTarget2].typesnapline, "Top\0Bottom\0Crosshair");
            bool openPopup3 = ImGui::ColorButton("##espbtn3", g_Options.esp_settings[currentTarget2].colorfill, ImGuiColorEditFlags_AlphaPreview);
            if (openPopup3)
                ImGui::OpenPopup("##esppopup3");

            if (ImGui::BeginPopup("##esppopup3")) {
                ImGui::ColorPicker4("Color", &g_Options.esp_settings[currentTarget2].colorfill.x);
                ImGui::EndPopup();
            }
            ImGui::SameLine();
            ImGui::Checkbox("Fill##esp", &g_Options.esp_settings[currentTarget2].fillenabled);
            if(currentTarget2<=3){
                ImGui::Checkbox("Display Health", &g_Options.esp_settings[currentTarget2].health);
                ImGui::SameLine();
                ImGui::Checkbox("Display Armor", &g_Options.esp_settings[currentTarget2].armor);
                //ImGui::Checkbox("Show Info", &g_Options.esp_settings[currentTarget2].info);
                ImGui::Checkbox("Name", &g_Options.esp_settings[currentTarget2].nameenabled);
                ImGui::SameLine();
                ImGui::Checkbox("Show Weapon", &g_Options.esp_settings[currentTarget2].weaponenabled);
            }
            else if (currentTarget2 == 4) {
                ImGui::Checkbox("Grenade Type Name", &g_Options.esp_settings[currentTarget2].nameenabled);
                ImGui::Checkbox("Show Grenade Owner", &g_Options.esp_settings[currentTarget2].weaponenabled);
            }
            else {
                ImGui::Checkbox("Show Name", &g_Options.esp_settings[currentTarget2].nameenabled);
            }

            bool openPopup4 = ImGui::ColorButton("##espbtn4", g_Options.esp_settings[currentTarget2].colorname, ImGuiColorEditFlags_AlphaPreview);
            if (openPopup4)
                ImGui::OpenPopup("##esppopup4");

            if (ImGui::BeginPopup("##esppopup4")) {
                ImGui::ColorPicker4("Color", &g_Options.esp_settings[currentTarget2].colorname.x);
                ImGui::EndPopup();
            }            
            ImGui::SameLine();
            ImGui::PushID(9994);
            ImGui::PushItemWidth(140);
            ImGui::SliderFloat("Font Size", &g_Options.esp_settings[currentTarget2].namethiccness, 1, 30, "%.1f");
            ImGui::PopItemWidth();
            ImGui::PopID();
        }
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        break;
    case 3:
        ImGui::Columns(2, nullptr, false);
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text("    Visuals");
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::PushItemWidth(240);
        ImGui::BeginGroupPanel("Visuals", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Checkbox("No Flash", &g_Options.visual_settings.noflash);
            ImGui::Combo("Smoke", &g_Options.visual_settings.nosmoke, smoke, 3);
            ImGui::Checkbox("Thirdperson", &g_Options.visual_settings.thirdperson);
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##0", &g_Options.visual_settings.thirdpersonKey, ImVec2(0, 0));
            ImGui::PopItemWidth();
            ImGui::SliderFloat("Distance", &g_Options.visual_settings.thirdpersondist, 0, 300, "%.f");
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Ambience", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Checkbox("Ambient Lighting", &g_Options.visual_settings.ambientenabled);
            ImGui::SameLine();
            bool openPopup = ImGui::ColorButton("##btn2", g_Options.visual_settings.ambientcolor);
            if (openPopup)
                ImGui::OpenPopup("##popup2");

            if (ImGui::BeginPopup("##popup2")) {
                ImGui::ColorPicker3("Color##ambient", &g_Options.visual_settings.ambientcolor.x);
                ImGui::EndPopup();
            }
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Color", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            bool openPopup = ImGui::ColorButton("##btn3", g_Options.visual_settings.worldcolor, ImGuiColorEditFlags_AlphaPreview);
            ImGui::SameLine();
            ImGui::Checkbox("World##colorwrld", &g_Options.visual_settings.worldenabled);
            if (openPopup)
                ImGui::OpenPopup("##popup3");

            if (ImGui::BeginPopup("##popup3")) {
                ImGui::ColorPicker4("Color##colorwrld", &g_Options.visual_settings.worldcolor.x);
                ImGui::EndPopup();
            }
            bool openPopup1 = ImGui::ColorButton("##btn4", g_Options.visual_settings.skycolor);
            ImGui::SameLine();
            ImGui::Checkbox("Sky##colorsky", &g_Options.visual_settings.skyenabled);
            if (openPopup1)
                ImGui::OpenPopup("##popup4");

            if (ImGui::BeginPopup("##popup4")) {
                ImGui::ColorPicker3("Sky##colorsky", &g_Options.visual_settings.skycolor.x);
                ImGui::EndPopup();
            }
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Fog", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Checkbox("Enabled", &g_Options.visual_settings.fog.enabled);
            ImGui::SameLine();
            bool openPopup = ImGui::ColorButton("##btn", g_Options.visual_settings.fog.color);
            if (openPopup)
                ImGui::OpenPopup("##popup");

            if (ImGui::BeginPopup("##popup")) {
                ImGui::ColorPicker3("Color", &g_Options.visual_settings.fog.color.x);
                ImGui::EndPopup();
            }
            ImGui::SliderFloat("Density", &g_Options.visual_settings.fog.maxdensity, 0.0f, 1.0f, "%.2f");
            ImGui::SliderInt("Start", &g_Options.visual_settings.fog.start, 0, 1000);
            ImGui::SliderInt("End", &g_Options.visual_settings.fog.end, 0, 2000);
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Bullet Tracers", { 466 * menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Checkbox("Enabled##tracers", &g_Options.visual_settings.tracer);
            ImGui::SameLine();
            bool openPopup1 = ImGui::ColorButton("##btn1", g_Options.visual_settings.tracercolor);
            if (openPopup1)
                ImGui::OpenPopup("##popup1");

            if (ImGui::BeginPopup("##popup1")) {
                ImGui::ColorPicker3("Color", &g_Options.visual_settings.tracercolor.x);
                ImGui::EndPopup();
            }
            ImGui::SliderFloat("Duration", &g_Options.visual_settings.tracertime, 0.1f, 2.f, "%.1f");
            ImGui::SliderFloat("Wave", &g_Options.visual_settings.tracerwave, 0.0f, 2.f, "%.1f");
        }
        ImGui::PopItemWidth();
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        ImGui::NextColumn();
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text(" ");
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::PushItemWidth(240);
        ImGui::BeginGroupPanel("Bloom", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Checkbox("Custom Bloom", &g_Options.visual_settings.bloomenabled);
            ImGui::SliderFloat("World exposure", &g_Options.visual_settings.bloomworldExposure, 0.0f, 100.f, "%.1f");
            ImGui::SliderFloat("Model ambient", &g_Options.visual_settings.bloomModelAmbient, 0.0f, 100.f, "%.1f");
            ImGui::SliderFloat("Bloom scale", &g_Options.visual_settings.bloomScale, 0.0f, 100.f, "%.1f");
        }        
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Sky", { 466 * menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Combo("Skybox", &g_Options.visual_settings.skybox, skyboxList, 26);
            ImGui::Combo("Weather", &g_Options.misc_settings.weather, "None\0Rain\0Snow\0"/*Ash\0Snowstorm\0Particle Rain\0Particle Ash\0Particle Rainstorm\0Particle Snow\0"*/);
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Viewmodel", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::SliderFloat("Fov", &g_Options.visual_settings.fov, -60, 60, "%.f");
            ImGui::SliderFloat("Scoped Fov", &g_Options.visual_settings.scopedfov, -60, 60, "%.f");
            ImGui::SliderFloat("Viewmodel Fov", &g_Options.visual_settings.viewmodelfov, -60, 60, "%.f");
            ImGui::Checkbox("Viewmodel Enabled", &g_Options.visual_settings.viewmodelenabled);
            ImGui::SliderFloat("Viewmodel X", &g_Options.visual_settings.viewmodelx, -20, 20, "%.1f");
            ImGui::SliderFloat("Viewmodel Y", &g_Options.visual_settings.viewmodely, -20, 20, "%.1f");
            ImGui::SliderFloat("Viewmodel Z", &g_Options.visual_settings.viewmodelz, -20, 20, "%.1f");
            ImGui::SliderFloat("Ratio", &g_Options.visual_settings.aspectratio, 0.f, 4.f, "%.1f");
        }

        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        ImGui::PopItemWidth();
        break;
    case 4:
        ImGui::Columns(2, nullptr, false);
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text("    Miscellaneous");
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::PushItemWidth(240);
        ImGui::BeginGroupPanel("Render", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::SliderInt("Limiter", &g_Options.misc_settings.limiter, 1, 10);
            ImGui::Checkbox("Force Crosshair", &g_Options.visual_settings.forcecrosshair);
            ImGui::Checkbox("Remove Scope", &g_Options.visual_settings.noscope);
            ImGui::Checkbox("Extend Render Range", &g_Options.misc_settings.listleaves);
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Miscellaneous", { 466 * menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Checkbox("Grenade Predict", &g_Options.misc_settings.grenadePredict);
            ImGui::Checkbox("Spectator List", &g_Options.misc_settings.specList);
            bool openPopup = ImGui::ColorButton("##btn", g_Options.misc_settings.aimbotfov, ImGuiColorEditFlags_AlphaPreview);
            ImGui::SameLine();
            ImGui::Text("Aimbot Fov");
            if (openPopup)
                ImGui::OpenPopup("##popup");

            if (ImGui::BeginPopup("##popup")) {
                ImGui::ColorPicker4("Color", &g_Options.misc_settings.aimbotfov.x);
                ImGui::EndPopup();
            }      
            bool openPopup1 = ImGui::ColorButton("##btn1", g_Options.misc_settings.peekvisualisation, ImGuiColorEditFlags_AlphaPreview);
            ImGui::SameLine();
            ImGui::Text("QuickPeek Visualisation");
            if (openPopup1)
                ImGui::OpenPopup("##popup1");

            if (ImGui::BeginPopup("##popup1")) {
                ImGui::ColorPicker4("Color", &g_Options.misc_settings.peekvisualisation.x);
                ImGui::EndPopup();
            }
            ImGui::Combo("Hit Marker", &g_Options.misc_settings.hitmarker, "None\0Crosshair\0Enemy\0Impact\0Damage\0");
            ImGui::SameLine();
            bool openPopup2 = ImGui::ColorButton("##btn2", g_Options.misc_settings.hitmarkercolor, ImGuiColorEditFlags_AlphaPreview);
            if (openPopup2)
                ImGui::OpenPopup("##popup2");

            if (ImGui::BeginPopup("##popup2")) {
                ImGui::ColorPicker4("Color", &g_Options.misc_settings.hitmarkercolor.x);
                ImGui::EndPopup();
            }
            ImGui::SameLine();
            bool openPopup3 = ImGui::ColorButton("##btn3", g_Options.misc_settings.headshotcolor, ImGuiColorEditFlags_AlphaPreview);
            if (openPopup3)
                ImGui::OpenPopup("##popup3");

            if (ImGui::BeginPopup("##popup3")) {
                ImGui::ColorPicker4("Color", &g_Options.misc_settings.headshotcolor.x);
                ImGui::EndPopup();
            }
            ImGui::SliderFloat("Marker Length", &g_Options.misc_settings.hitmarkerlength, 1, 3, "%.1f");
            ImGui::SliderFloat("Marker Duration", &g_Options.misc_settings.hitmarkertime, 0.05f,1.f, "%.2f");
            ImGui::Checkbox("Auto-Pistol", &g_Options.misc_settings.autopistol);
            ImGui::Checkbox("Show Ragebot logs", &g_Options.misc_settings.showlogs);
            ImGui::Checkbox("List Keybinds", &g_Options.misc_settings.showkeybinds);
        }
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        ImGui::PopItemWidth();
        ImGui::NextColumn();
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text(" ");
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::PushItemWidth(240);
        ImGui::BeginGroupPanel("Miscellaneous", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Combo("Clantag", &g_Options.misc_settings.clantag, "Disabled\0Ping-Pong\0Scroll\0Reverse-Scroll\0Loading\0Shrink\0Static\0None\0");
            ImGui::Text("Menu Key");
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##menukey", &g_Options.keys.MenuKey);
            ImGui::Checkbox("Radar", &g_Options.misc_settings.radar);
            ImGui::Combo("Hit Sounds", &g_Options.misc_settings.hitsound, "None\0Metal\0Switch\0Bell\0Glass\0");
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("AutoBuy", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Combo("Primary", &g_Options.misc_settings.primary, "None\0Galil AR/Famas\0AK-47/M4A4/M4A1-S\0SSG-08\0SG-553/AUG\0AWP\0G3SG1/SCAR-20\0Mac-10/MP9\0MP7/MP5-SD\0UMP-45\0P90\0PP-Bizon\0Nova\0XM1014\0Sawed-off/MAG-7\0M249\0Negev\0");
            ImGui::Combo("Secondary", &g_Options.misc_settings.secondary, "None\0Glock-18/P2000/USP-S\0Dual Berettas\0P250\0Tec-9/Five-Seven/CZ-75\0Desert Eagle/Revolver\0");
            ImGui::Combo("Armor", &g_Options.misc_settings.kevlar, "None\0Kevlar\0Helmet\0");
        }
        ImGui::EndGroupPanel();     
        ImGui::BeginGroupPanel("Exploits and Net", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Text("Choke all Packets");
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##chokeallpackets", &g_Options.misc_settings.netlosskey);
            ImGui::PopItemWidth();
            ImGui::Text("Invalidate UserCMD");
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##invalidateusercmd", &g_Options.misc_settings.invalidatecmdkey);
            ImGui::PopItemWidth();
            ImGui::Text("Shift Tickbase");
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##shifttickbase", &g_Options.misc_settings.tickbasekey);
            ImGui::PopItemWidth();
            ImGui::SameLine();
            ImGui::PushItemWidth(90);
            ImGui::InputInt("##amount", &g_Options.misc_settings.tickbaseamount);
            ImGui::PopItemWidth();
            ImGui::SliderInt("Charging Speed", &g_Options.misc_settings.chargespeed, 1, 7);
            ImGui::Checkbox("Show Charge", &g_Options.misc_settings.renderdt);
            ImGui::SliderFloat("Fake-Latency", &g_Options.misc_settings.fakelatency, 0, 1000, "%.f ms");
            ImGui::Checkbox("Prediction", &g_Options.misc_settings.prediction);
            //ImGui::Checkbox("Simulate Extra Ticks", &g_Options.misc_settings.teleport);
        }
        ImGui::PopItemWidth();
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        break;
    case 5:
        ImGui::Columns(2, nullptr, false);
        ImGui::SetWindowFontScale(menusize * 1.2);
        ImGui::Text("    Movement");
        ImGui::SetWindowFontScale(menusize * 1);
        ImGui::PushItemWidth(240);
        ImGui::BeginGroupPanel("Movement", { 466 * menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Checkbox("Bhop", &g_Options.misc_settings.bhop);
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::SliderInt("Chance", &g_Options.misc_settings.bhopHitchance, 1, 100);
            ImGui::PopItemWidth();
            ImGui::Checkbox("FastStop", &g_Options.misc_settings.faststop);
            ImGui::Checkbox("FastDuck", &g_Options.misc_settings.fastduck);
            ImGui::Checkbox("Slowwalk", &g_Options.misc_settings.slowwalk);
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##slowwalk", &g_Options.misc_settings.slowwalkkey);
            ImGui::PopItemWidth();
            ImGui::Text("FakeDuck");
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##fakeduck", &g_Options.misc_settings.fakeduck);
            ImGui::PopItemWidth();
            ImGui::PushItemWidth(240);
            ImGui::Combo("Auto-Strafe", &g_Options.misc_settings.autostrafe, "Disabled\0Legit\0Directional\0Rage\0");
            ImGui::PopItemWidth();
            ImGui::PushItemWidth(240);
            ImGui::Combo("Effects", &g_Options.misc_settings.blurtype, "Disabled\0Camera\0Stim\0Danger\0");
            ImGui::PopItemWidth();
            ImGui::SliderFloat("Min Speed", &g_Options.misc_settings.startingspeed, 0, 2000, "%.f");
            ImGui::SliderFloat("Max Speed", &g_Options.misc_settings.maxspeed, 125, 2000, "%.f");
            ImGui::Checkbox("Speed Graph", &g_Options.misc_settings.movementgraph);

            ImGui::Checkbox("EdgeBug", &g_Options.misc_settings.edgebug);
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##edgebug", &g_Options.misc_settings.edgebugkey);
            ImGui::PopItemWidth();
            ImGui::Checkbox("Edge Jump", &g_Options.misc_settings.edgejump);
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##edgejump", &g_Options.misc_settings.edgejumpkey);
            ImGui::PopItemWidth();
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Recorder", { 466 * menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Checkbox("Enabled##recorder", &g_Options.misc_settings.recorder);
            ImGui::Checkbox("Set Angles", &g_Options.misc_settings.recordersetAngles);
            ImGui::Text("Record");
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##record", &g_Options.misc_settings.recordbutton);
            ImGui::PopItemWidth();
            ImGui::Text("Replay");
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##replay", &g_Options.misc_settings.replaybutton);
            ImGui::PopItemWidth();
            bool openPopup = ImGui::ColorButton("##btn", g_Options.misc_settings.recordercolor, ImGuiColorEditFlags_AlphaPreview);
            ImGui::SameLine();
            ImGui::Checkbox("Visualize", &g_Options.misc_settings.recordervisualize);
            if (openPopup)
                ImGui::OpenPopup("##popup");

            if (ImGui::BeginPopup("##popup")) {
                ImGui::ColorPicker4("Color", &g_Options.misc_settings.recordercolor.x);
                ImGui::EndPopup();
            }
        }
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        ImGui::PopItemWidth();
        ImGui::NextColumn();
        ImGui::SetWindowFontScale(menusize * 1.2);
        ImGui::Text(" ");
        ImGui::SetWindowFontScale(menusize * 1);
        ImGui::PushItemWidth(240);
        ImGui::BeginGroupPanel("Walkbot", { 466 * menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Checkbox("Walkbot", &g_Options.misc_settings.walkbot);
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("", &g_Options.misc_settings.walkbotbutton);
            ImGui::PopItemWidth();
            ImGui::Checkbox("Render Path", &g_Options.misc_settings.walkbotrenderpath);
            ImGui::SameLine();
            bool openPopup3 = ImGui::ColorButton("##btn3", g_Options.misc_settings.walkbotpath, ImGuiColorEditFlags_AlphaPreview);
            if (openPopup3)
                ImGui::OpenPopup("##popup3");

            if (ImGui::BeginPopup("##popup3")) {
                ImGui::ColorPicker4("Color", &g_Options.misc_settings.walkbotpath.x);
                ImGui::EndPopup();
            }
            ImGui::Checkbox("Render Points", &g_Options.misc_settings.walkbotrenderpoints);
            ImGui::SameLine();
            bool openPopup4 = ImGui::ColorButton("##btn4", g_Options.misc_settings.walkbotpoints, ImGuiColorEditFlags_AlphaPreview);
            if (openPopup4)
                ImGui::OpenPopup("##popup4");

            if (ImGui::BeginPopup("##popup4")) {
                ImGui::ColorPicker4("Color", &g_Options.misc_settings.walkbotpoints.x);
                ImGui::EndPopup();
            }
            ImGui::SliderInt("Speed", &g_Options.misc_settings.walkbotspeed, 1, 100);
            ImGui::SliderInt("Distance", &g_Options.misc_settings.walkbotdist, 25, 500);
            ImGui::SliderInt("Path", &g_Walkbot.currentPath, 0, g_Walkbot.path.size());
            if (ImGui::Button("Delete"))
                g_Walkbot.Delete(g_Walkbot.currentPath);
            ImGui::SameLine();
            if (ImGui::Button("Clear"))
                g_Walkbot.Clear();
        }
        ImGui::PopItemWidth();
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        break;
    case 6:
        ImGui::Columns(2, nullptr, false);
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text("    Skins");
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::BeginGroupPanel("Skinchanger", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::PushItemWidth(240);
            ImGui::Combo("Weapon##skins", &currentWeapon3, weaponsall, 36);
            if (currentWeapon3 != 0)
            {
                ImGui::Checkbox("Enabled##skins", &g_Options.skins_settings[currentWeapon3 - 1].enabled);
                if (currentWeapon3 - 1 == 0) {
                    ImGui::Combo("Knife##skins", &g_Options.misc_settings.knifemodel, knifelist, 18);
                }
                else
                {
                    ImGui::PushItemWidth(200);
                    ImGui::Combo("Slot:", &cursticker2, "Slot 1\0Slot 2\0Slot 3\0Slot 4\0Slot 5\0");
                    ImGui::PopItemWidth();
                    ImGui::SameLine();
                    bool openPopup2 = ImGui::Button("Customize");
                    if (openPopup2)
                        ImGui::OpenPopup("##popup2");
                    if (ImGui::IsPopupOpen("##popup2")) ImGui::SetWindowSize({ 320, 0 });
                    if (ImGui::BeginPopup("##popup2")) {
                        ImGui::SetWindowSize({ 520, 0 });
                        ImGui::PushItemWidth(200);
                        ImGui::Checkbox("Enabled", &g_Options.skins_settings[currentWeapon3-1].stickers[cursticker2].enabled);
                        ImGui::Combo("Sticker", &cursticker4[cursticker2], g_Skins.stickerlist);
                        g_Options.skins_settings[currentWeapon3 - 1].stickers[cursticker2].kit = g_Skins.stickerlistid.at(cursticker4[cursticker2]);
                        ImGui::SliderFloat("Wear", &g_Options.skins_settings[currentWeapon3 - 1].stickers[cursticker2].wear, 0, 1, "%.1f");
                        ImGui::SliderFloat("Rotation", &g_Options.skins_settings[currentWeapon3 - 1].stickers[cursticker2].rotation, -360, 360, "%.f");
                        ImGui::SliderFloat("Scale", &g_Options.skins_settings[currentWeapon3 - 1].stickers[cursticker2].scale, 0, 4, "%.1f");
                        ImGui::PopItemWidth();
                        ImGui::EndPopup();
                    }
                }
                ImGui::SliderInt("Seed", &g_Options.skins_settings[currentWeapon3 - 1].seed, 0, 1000);
                ImGui::SliderFloat("Wear", &g_Options.skins_settings[currentWeapon3 - 1].wear, 0.f, 1.f);
                ImGui::SliderInt("Stattrack", &g_Options.skins_settings[currentWeapon3 - 1].stattrack, -1, 999999);
                if (ImGui::Button("Add all Skins to Inventory"))
                    g_Inventory.AddToInventory();
                if (ImGui::Button("Add Skin to Inventory"))
                    g_Inventory.AddToInventoryIndex(currentWeapon3-1);
                ImGui::InputText("Search:", search, IM_ARRAYSIZE(search));
                if (ImGui::ListBoxHeader("##skins", { ImGui::GetContentRegionAvail().x - 9, ImGui::GetContentRegionAvail().y - 18 }))
                {
                    for (int n = 0; n < g_Skins.skinlist.size(); n++) {
                        if (g_Skins.skinlist[n].find(search) != std::string::npos && 10000 > g_Skins.skinlistid[n])
                        {
                            ImGui::PushID(n);
                            const bool is_selected = (currentSkin == n);
                            bool is_paintkit = false;
                            if (g_Options.skins_settings[currentWeapon3 - 1].paintkit == g_Skins.skinlistid[n])
                            {
                                is_paintkit = true;
                                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_NavWindowingHighlight));
                            }
                            if (ImGui::Selectable(g_Skins.skinlist[n].c_str(), is_selected)) {
                                currentSkin = n;
                                g_Options.skins_settings[currentWeapon3 - 1].paintkit = g_Skins.skinlistid[n];
                                g_ClientState->ForceFullUpdate();
                            }
                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                            if (is_paintkit)
                                ImGui::PopStyleColor();
                            ImGui::PopID();
                        }
                    }
                    ImGui::ListBoxFooter();
                }
            }
            else
            {
                ImGui::Combo("Glove Type##skins", &g_Options.misc_settings.glovemodel, glovelist, 8);
                if (ImGui::Button("Add all Skins to Inventory"))
                    g_Inventory.AddToInventory();
                if (ImGui::Button("Add Skin to Inventory"))
                    g_Inventory.AddToInventoryIndex(-1);
                ImGui::InputText("Search:", search, IM_ARRAYSIZE(search));
                if (ImGui::ListBoxHeader("##skins", { ImGui::GetContentRegionAvail().x - 9, ImGui::GetContentRegionAvail().y - 18 }))
                {
                    for (int n = 0; n < g_Skins.skinlist.size(); n++) {
                        if (g_Skins.skinlist[n].find(search) != std::string::npos&&10000<=g_Skins.skinlistid[n])
                        {
                            ImGui::PushID(n);
                            const bool is_selected = (currentSkin == n);
                            bool is_paintkit = false;
                            if (g_Options.misc_settings.glovepaintkit == g_Skins.skinlistid[n])
                            {
                                is_paintkit = true;
                                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_NavWindowingHighlight));
                            }
                            if (ImGui::Selectable(g_Skins.skinlist[n].c_str(), is_selected)) {
                                currentSkin = n;
                                g_Options.misc_settings.glovepaintkit = g_Skins.skinlistid[n];
                                g_ClientState->ForceFullUpdate();
                            }
                            if (is_selected) {
                                ImGui::SetItemDefaultFocus();
                            }
                            if (is_paintkit)
                                ImGui::PopStyleColor();
                            ImGui::PopID();
                        }
                    }
                    ImGui::ListBoxFooter();
                }
            }
        }
        ImGui::EndGroupPanel();
        ImGui::NextColumn();
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text(" ");//to lazy to dummy
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::BeginGroupPanel("Inventory", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2 )});
        {
            ImGui::Combo("Item:", &inv_definitionindex, inv_weaponnames);
            ImGui::Combo("Rarity:", &inv_rarity, "Default\0Common\0Uncommon\0Rare\0Mythical\0Legendary\0Ancient\0Immortal\0");
            ImGui::Combo("Quality:", &inv_quality, "Default\0Genuine\0Vintage\0Unusual\0Skin\0Community\0Developer\0Selfmade\0Customized\0Strange\0Completed\0unk2\0Tournament\0");
            ImGui::PushID(0xDead);
            ImGui::PushItemWidth(200);
            ImGui::Combo("Slot:", &cursticker1, "Slot 1\0Slot 2\0Slot 3\0Slot 4\0Slot 5\0");
            ImGui::PopItemWidth();
            ImGui::SameLine();
            bool openPopup = ImGui::Button("Customize");
            if (openPopup)
                ImGui::OpenPopup("##popup");
            if(ImGui::IsPopupOpen("##popup")) ImGui::SetWindowSize({ 320, 0 });
            if (ImGui::BeginPopup("##popup")) {

                ImGui::PushItemWidth(200);
                ImGui::Checkbox("Enabled", &sticker[cursticker1].enabled);
                ImGui::Combo("Sticker", &cursticker3[cursticker1], g_Skins.stickerlist);
                sticker[cursticker1].kit = g_Skins.stickerlistid.at(cursticker3[cursticker1]);
                ImGui::SliderFloat("Wear", &sticker[cursticker1].wear, 0, 1, "%.1f");
                ImGui::SliderFloat("Rotation", &sticker[cursticker1].rotation, -360, 360, "%.f");
                ImGui::SliderFloat("Scale", &sticker[cursticker1].scale, 0, 4, "%.1f");
                ImGui::PopItemWidth();
                ImGui::EndPopup();
            }
            ImGui::PopItemWidth();
            ImGui::SliderInt("Seed", &inv_seed, 0, 1000);
            ImGui::SliderFloat("Wear", &inv_wear, 0.f, 1.f);
            ImGui::SliderInt("Stattrack", &inv_stattrack, -1, 999999);
            ImGui::InputText("Name:", inv_customname, IM_ARRAYSIZE(inv_customname));
            ImGui::InputText("Description:", inv_customdesc, IM_ARRAYSIZE(inv_customdesc));
            ImGui::InputText("Search:", search2, IM_ARRAYSIZE(search2));
            ImGui::PopID();
            if (ImGui::Button("Set in Skinchanger"))
            {
                g_Inventory.SetSkin(inv_weaponindex[inv_definitionindex], inv_stattrack, inv_paintkit, inv_seed, inv_wear, 8, inv_rarity, inv_quality, (std::string)(inv_customname), (std::string)(inv_customdesc), sticker);
            }            
            if (ImGui::Button("Add to Inventory"))
            {
                g_Inventory.Run(inv_weaponindex[inv_definitionindex], inv_stattrack, inv_paintkit, inv_seed, inv_wear, 8, inv_rarity, inv_quality, (std::string)(inv_customname), (std::string)(inv_customdesc), sticker);
                //g_Inventory.Run(1, 150, 37, 0, 0,8,4,0,"","");
            }
            if (ImGui::ListBoxHeader("##inv", { ImGui::GetContentRegionAvail().x - 9, ImGui::GetContentRegionAvail().y - 18 }))
            {
                for (int n = 0; n < g_Skins.skinlist.size(); n++) {
                    if (g_Skins.skinlist[n].find(search2) != std::string::npos)
                    {
                        ImGui::PushID(n+1000);
                        const bool is_selected = (currentSkin == n);
                        bool is_paintkit = false;
                        if (inv_paintkit == g_Skins.skinlistid[n])
                        {
                            is_paintkit = true;
                            ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_NavWindowingHighlight));
                        }
                        if (ImGui::Selectable(g_Skins.skinlist[n].c_str(), is_selected)) {
                            currentSkin = n;
                           inv_paintkit = g_Skins.skinlistid[n];
                            g_ClientState->ForceFullUpdate();
                        }
                        if (is_selected) {
                            ImGui::SetItemDefaultFocus();
                        }
                        if (is_paintkit)
                            ImGui::PopStyleColor();
                        ImGui::PopID();
                    }
                }
                ImGui::ListBoxFooter();
            }
        }
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        break;
    case 7:
        ImGui::Columns(2, nullptr, false);
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text("    Config");
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::PushItemWidth(240);
        ImGui::BeginGroupPanel("Config", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::PopItemWidth();
            ImGui::InputText("Path:", cfgpath, IM_ARRAYSIZE(cfgpath));
            ImGui::PushItemWidth(240);
            if (ImGui::Button("Refresh"))
                refreshed = false;
            ImGui::SameLine();
            if (ImGui::Button("Load") && configs.size() > currentConfig)
            {
                std::ifstream myfile;
                myfile.open(cfgpath + configs.at(currentConfig));
                std::string contents((std::istreambuf_iterator<char>(myfile)),std::istreambuf_iterator<char>());
                const char* cfgarray = contents.c_str();
                memcpy(&g_Options, cfgarray, sizeof(g_Options));
                g_Options.antiaim_settings.antiuntrusted = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("Save")&&configs.size()>currentConfig)
            {
                std::ofstream myfile;
                myfile.open(cfgpath+configs.at(currentConfig));
                char buffer[sizeof(g_Options)];
                memcpy(&buffer, &g_Options, sizeof(g_Options));
                myfile.write(buffer, sizeof(g_Options));
                myfile.close();
            }
            ImGui::SameLine();
            if (ImGui::Button("Delete") && configs.size() > currentConfig){
                std::filesystem::remove((cfgpath + configs.at(currentConfig)));
                refreshed = false;
            }
            if (!cfginit)
            {
                PWSTR   ppszPath;
                HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &ppszPath);
                std::wstring myPath;
                if (SUCCEEDED(hr)) {
                    myPath = ppszPath;
                }
                CoTaskMemFree(ppszPath);
                std::string s(myPath.begin(), myPath.end());
                s += "\\Resquare\\";
                _mkdir(s.c_str());        
                std::string pathcolors = s;
                pathcolors += "Colors\\";
                _mkdir(pathcolors.c_str());
                strcpy(cfgpath, s.c_str());
                cfginit = true;
            }
            if (!refreshed)
            {
                configs.clear();
                for (const auto& entry : std::filesystem::directory_iterator(std::string(cfgpath)))
                {
                    auto temp1 = entry.path().string();
                    if (entry.is_directory())
                        continue;
                    temp1.erase(temp1.begin(), temp1.begin()+ std::string(cfgpath).size());
                    configs.push_back(temp1);
                }
                refreshed = true;
            }
            if (ImGui::ListBoxHeader("##cfgs", { ImGui::GetContentRegionAvail().x - 9, ImGui::GetContentRegionAvail().y - 18 }))
            {
                for (int n = 0; n < configs.size(); n++) {
                    ImGui::PushID(n);
                    const bool is_selected = (currentConfig == n);
                    if (is_selected)
                        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_NavWindowingHighlight));
                    if (ImGui::Selectable(configs.at(n).c_str(), is_selected)) {
                        currentConfig = n;
                    }
                    if (is_selected) {
                        ImGui::PopStyleColor();
                    }
                    ImGui::PopID();
                }
                ImGui::InputText("##path", cfgname, IM_ARRAYSIZE(cfgname));
                ImGui::SameLine();
                if (ImGui::Button("Create")) {
                    std::ofstream{ (std::string(cfgpath)+"/"+std::string(cfgname)) };
                    std::ofstream myfile;
                    myfile.open(cfgpath + std::string(cfgname));
                    char buffer[sizeof(g_Options)];
                    memcpy(&buffer, &g_Options, sizeof(g_Options));
                    myfile.write(buffer, sizeof(g_Options));
                    myfile.close();
                    refreshed = false;
                }
                ImGui::ListBoxFooter();
            }
        }
        ImGui::EndGroupPanel();
        ImGui::NextColumn();
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text(" ");
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::BeginGroupPanel("Additional Settings", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2) });
        {
            ImGui::InputInt("Max Points", &g_Options.misc_settings.maxBones, 171, 2052);
            ImGui::SliderInt("MultiPoints", &g_Options.misc_settings.maxBonesperJoint, 1, 9);
            ImGui::SliderFloat("Point Size", &g_Options.misc_settings.multipointsize, 0.01, 1, "%.2f");
            ImGui::Checkbox("Debug Points", &g_Options.misc_settings.debugmultipoints);
            ImGui::Checkbox("Disable Animfix", &g_Options.misc_settings.disableAnim);
            ImGui::Checkbox("Anti-Untrusted", &g_Options.antiaim_settings.antiuntrusted);
            //ImGui::SliderFloat("Testing", &g_Options.misc_settings.antiaimtest, -58, 58, "%.f");
            ImGui::Checkbox("Debug Info", &g_Options.misc_settings.drawDebug);
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Menu", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2 )});
        {
            ImGui::InputFloat("Menu Size", &menusize, 0.1f, 2.f, "%.1f");
            int iteration = 0;
                //0=21 text 22 in dunkeler, 23 +0.1
                //2 bis 4 bg
                //7 bis 9 slider usw
                //15 bis 17 scroll
                //18,20 things + 19 with 0.4f alpha 40 in dunkeler/alpha
            if(true)
            {
                ImGui::PushID(0xABAF + iteration * 10);
                bool openPopup = ImGui::ColorButton("##btn", style.Colors[0], ImGuiColorEditFlags_AlphaPreview);
                ImGui::SameLine();
                ImGui::Text("Text");
                if (openPopup)
                    ImGui::OpenPopup("##popup");
                if (ImGui::BeginPopup("##popup")) {
                    ImGui::ColorPicker4("Color", &style.Colors[0].x);
                    ImGui::EndPopup();
                }
                style.Colors[21] = style.Colors[0];
                style.Colors[22] = { style.Colors[0].x,style.Colors[0].y,style.Colors[0].z,style.Colors[0].w -0.2f };
                style.Colors[23] = { style.Colors[0].x,style.Colors[0].y,style.Colors[0].z,style.Colors[0].w + 0.1f };
                iteration++;
                ImGui::PopID();
            }            
            if(true)
            {
                ImGui::PushID(0xABAF + iteration * 10);
                bool openPopup = ImGui::ColorButton("##btn", style.Colors[2], ImGuiColorEditFlags_AlphaPreview);
                ImGui::SameLine();
                ImGui::Text("Background");
                if (openPopup)
                    ImGui::OpenPopup("##popup");
                if (ImGui::BeginPopup("##popup")) {
                    ImGui::ColorPicker4("Color", &style.Colors[2].x);
                    ImGui::EndPopup();
                }
                style.Colors[4] = style.Colors[2];
                iteration++;
                ImGui::PopID();
            }            
            if(true)
            {
                ImGui::PushID(0xABAF + iteration * 10);
                bool openPopup = ImGui::ColorButton("##btn", style.Colors[7], ImGuiColorEditFlags_AlphaPreview);
                ImGui::SameLine();
                ImGui::Text("Widgets");
                if (openPopup)
                    ImGui::OpenPopup("##popup");
                if (ImGui::BeginPopup("##popup")) {
                    ImGui::ColorPicker4("Color", &style.Colors[7].x);
                    ImGui::EndPopup();
                }
                style.Colors[8] = { style.Colors[7].x + 0.2f,style.Colors[7].y + 0.2f,style.Colors[7].z + 0.2f,style.Colors[7].w - 0.3f };
                style.Colors[9] = { style.Colors[7].x + 0.2f,style.Colors[7].y + 0.2f,style.Colors[7].z + 0.2f,style.Colors[7].w - 0.1f };
                iteration++;
                ImGui::PopID();
            }            
            if(false)
            {
                ImGui::PushID(0xABAF + iteration * 10);
                bool openPopup = ImGui::ColorButton("##btn", style.Colors[0], ImGuiColorEditFlags_AlphaPreview);
                ImGui::SameLine();
                ImGui::Text("Scroll");
                if (openPopup)
                    ImGui::OpenPopup("##popup");
                if (ImGui::BeginPopup("##popup")) {
                    ImGui::ColorPicker4("Color", &style.Colors[0].x);
                    ImGui::EndPopup();
                }
                style.Colors[21] = style.Colors[0];
                style.Colors[22] = { style.Colors[0].x,style.Colors[0].y,style.Colors[0].z,style.Colors[0].w -0.2f };
                style.Colors[23] = { style.Colors[0].x,style.Colors[0].y,style.Colors[0].z,style.Colors[0].w + 0.1f };
                iteration++;
                ImGui::PopID();
            }            
            if(true)
            {
                ImGui::PushID(0xABAF + iteration * 10);
                bool openPopup = ImGui::ColorButton("##btn", style.Colors[18], ImGuiColorEditFlags_AlphaPreview);
                ImGui::SameLine();
                ImGui::Text("Interface");
                if (openPopup)
                    ImGui::OpenPopup("##popup");
                if (ImGui::BeginPopup("##popup")) {
                    ImGui::ColorPicker4("Color", &style.Colors[18].x);
                    ImGui::EndPopup();
                }
                style.Colors[20] = style.Colors[18];
                style.Colors[19] = { style.Colors[18].x,style.Colors[18].y,style.Colors[18].z,style.Colors[18].w - 0.4f };
                style.Colors[40] = style.Colors[19];
                iteration++;
                ImGui::PopID();
            }
            ImGui::PushItemWidth(180);
            ImGui::Combo("Theme", &currentTheme, "Default\0Slot 1\0Slot 2\0Slot 3\0Slot 4\0Slot 5\0Slot 6\0Slot 7\0Slot 8\0Slot 9\0Slot 10\0");
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (currentTheme)
            {
                if (ImGui::Button("Save##theme"))
                {
                    std::ofstream myfile;
                    std::string colorpath = std::string(std::string(cfgpath) + "/Colors/Slot"+ std::to_string(currentTheme));
                    std::ofstream{ colorpath };
                    myfile.open(colorpath.c_str());
                    char buffer[sizeof(ImGui::GetStyle().Colors)];
                    memcpy(&buffer, &ImGui::GetStyle().Colors, sizeof(ImGui::GetStyle().Colors));
                    myfile.write(buffer, sizeof(ImGui::GetStyle().Colors));
                    myfile.close();
                }
                ImGui::SameLine();
                if (ImGui::Button("Load##theme"))
                {
                    std::ifstream myfile;
                    std::string colorpath = (std::string)cfgpath + "/Colors/Slot" + std::to_string(currentTheme);
                    myfile.open(colorpath.c_str());
                    std::string contents((std::istreambuf_iterator<char>(myfile)), std::istreambuf_iterator<char>());
                    const char* cfgarray = contents.c_str();
                    memcpy(&ImGui::GetStyle().Colors, cfgarray, sizeof(ImGui::GetStyle().Colors));
                }
            }
            else
            {
                if (ImGui::Button("Load##theme"))
                    ImGui::StyleColorsDark();
            }

        }
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::PopItemWidth();
        ImGui::EndGroupPanel();
        break;
    default:
        ImGui::Columns(2, nullptr, false);
        ImGui::SetWindowFontScale(menusize*1.2);
        ImGui::Text("    Legit");
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::SameLine();
        ImGui::Checkbox("##legitenable", &g_Options.legit);
        ImGui::BeginGroupPanel("Aimbot", { 466*menusize, ImGui::GetContentRegionAvail().y });
        {
            ImGui::Combo("Weapon", &currentWeapon, weapons, 42);
            if (ImGui::ListBoxHeader("##aimbot", { (ImGui::GetContentRegionAvail().x - 9), ((ImGui::GetTextLineHeightWithSpacing() * ((currentWeapon == 0 || currentWeapon == 5) ? 4.9f : 6.2f)+((currentWeapon == 0 || currentWeapon == 5) ? 1 : 0) + style.ItemSpacing.y)-1) }))
            {
                for (int n = 0; n < 19; n++){
                    ImGui::PushID(n);
                    ImGui::Checkbox(bones.at(n).c_str(), &g_Options.aimbot_settings[currentWeapon].bones[n]);
                    ImGui::PopID();
                }
                ImGui::ListBoxFooter();
            }
            ImGui::Text("On Key");
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##enabledaim", &g_Options.keys.AimbotKey);
            ImGui::PopItemWidth();
            ImGui::Combo("Type:", &g_Options.aimbot_settings[currentWeapon].aimassist, "Smoothing\0Linear\0Exponential\0Aim-Assist\0Linear Aim-Assist\0Exponential Aim-Assist\0Humanized\0");
            ImGui::SliderFloat("Smooth", &g_Options.aimbot_settings[currentWeapon].smooth, 1, 1000, "%.1f");
            ImGui::SliderFloat("Fov", &g_Options.aimbot_settings[currentWeapon].fov, 0, 360,"%.1f");
            ImGui::Checkbox("Silent##aimbot", &g_Options.aimbot_settings[currentWeapon].silent);
            ImGui::Checkbox("Ignore Flash##aimbot", &g_Options.aimbot_settings[currentWeapon].shootFlashed);
            if (currentWeapon == 0 || currentWeapon == 5)
                ImGui::Checkbox("Scoped Only##aimbot", &g_Options.aimbot_settings[currentWeapon].onlyScoped);
            ImGui::Checkbox("Enabled##aimbot", &g_Options.aimbot_settings[currentWeapon].enabled);
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Recoil", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2 )});
        {
            ImGui::Combo("Weapon##recoil", &currentWeapon1, weapons, 42);
            ImGui::Checkbox("Negate Recoil##recoil", &g_Options.aimbot_settings[currentWeapon1].RCS);
            ImGui::SliderInt("Amount", &g_Options.aimbot_settings[currentWeapon1].RCSamount, 0, 100, "%d%");
            ImGui::Checkbox("Silent Recoil##recoil", &g_Options.aimbot_settings[currentWeapon1].RCSsilent);
        }
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 19 });
        ImGui::EndGroupPanel();
        ImGui::NextColumn();
        ImGui::SetWindowFontScale(menusize*1.3);
        ImGui::Dummy({ 1, ImGui::CalcTextSize(" ").y - 1 });
        ImGui::SetWindowFontScale(menusize*1);
        ImGui::BeginGroupPanel("Legit-AA", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2 )});
        {
            ImGui::Checkbox("Enabled##legitaa", &g_Options.antiaim_settings.legitaa);
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##glip", &g_Options.antiaim_settings.legitAntiaimKey);
            ImGui::PopItemWidth();
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Triggerbot", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2 )});
        {
            ImGui::Combo("Weapon##triggerbot", &currentWeapon2, weapons, 42);
            if (ImGui::ListBoxHeader("##triggerbot", { ImGui::GetContentRegionAvail().x - 9, (ImGui::GetTextLineHeightWithSpacing() * 7.4f) + style.ItemSpacing.y - 3 }))
            {
                for (int n = 0; n < 19; n++) {
                    ImGui::PushID(n+19);
                    ImGui::Checkbox(bones.at(n).c_str(), &g_Options.triggerbot_settings[currentWeapon2].bones[n]);
                    ImGui::PopID();
                }
                ImGui::ListBoxFooter();
            }
            ImGui::Checkbox("Enabled##triggerbot", &g_Options.triggerbot_settings[currentWeapon2].enabled);
            ImGui::SameLine();
            ImGui::PushItemWidth(150);
            ImGui::Hotkey("##enabledtrigger", &g_Options.keys.TriggerbotKey);
            ImGui::PopItemWidth();
            ImGui::Checkbox("Scoped Only##triggerbot", &g_Options.triggerbot_settings[currentWeapon2].onlyScoped);
            ImGui::Checkbox("Ignore Flash##triggerbot", &g_Options.triggerbot_settings[currentWeapon2].shootFlashed);
            ImGui::SliderInt("Delay", &g_Options.triggerbot_settings[currentWeapon2].shotDelay, 0, 2500, "%d ms");
        }
        ImGui::EndGroupPanel();
        ImGui::BeginGroupPanel("Backtrack", { 466 * menusize , (ImGui::GetContentRegionAvail().y / 2 - 2 )});
        {
            ImGui::Checkbox("Enabled##backtrack", &g_Options.backtrack_settings.enabled);
            ImGui::Checkbox("Draw All", &g_Options.backtrack_settings.shoulddrawall);
            ImGui::Checkbox("Target Backtrack", &g_Options.backtrack_settings.shouldtarget);
            ImGui::SliderInt("", &g_Options.backtrack_settings.time, 0, 12, "%d ticks");
        }
        ImGui::Dummy({ 0,ImGui::GetContentRegionAvail().y - 18 });
        ImGui::EndGroupPanel();
        if (g_Options.legit)
            g_Options.rage = false;
        break;
    }
    ImGui::End();
}

void Menu::Toggle()
{
    if(!g_Unload)
    { 
    lastTime = std::clock();
    _visible = !_visible;
    }
}
