#include "chams.h"
#include "../options.hpp"
#include "../hooks.hpp"
//Local player Allies Enemies has Defuser has C4 Weapons Hands Sleeves Backtrack
void Chams::InitMats()
{
    KeyValues Generator;
    patternchams = Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? 83 C4 04 89 45 D8");
    patternsetkv = Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? 89 77 38");
    patternfindkv = Utils::PatternScan(GetModuleHandleA("client.dll"), "E8 ? ? ? ? F7 45");
	materiallist.push_back(g_MatSystem->FindMaterial("debug/debugambientcube"));//normal
	materiallist.push_back(g_MatSystem->FindMaterial("debug/debugdrawflat"));//flat
	materiallist.push_back(g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/gloss"));//glossy
	materiallist.push_back(g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/crystal_clear"));//trippy
	materiallist.push_back(g_MatSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass"));//glass
	materiallist.push_back(g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/velvet"));//velvet
	materiallist.push_back(g_MatSystem->FindMaterial("dev/glow_armsrace"));//glow1
    materiallist.push_back(g_MatSystem->FindMaterial("models/inventory_items/trophy_majors/gold"));//chrome light
    materiallist.push_back(g_MatSystem->FindMaterial("models/player/ct_fbi/ct_fbi_glass"));//chrome
    materiallist.push_back(g_MatSystem->FindMaterial("models/gibs/glass/glass"));//chrome dark
	materiallist.push_back(g_MatSystem->CreateMaterial("pearlescent", Generator.CreateFromString("VertexLitGeneric", "$ambientonly 1 $phong 1 $pearlescent 2 $basemapalphaphongmask 1", patternchams)));//pearescent
	materiallist.push_back(g_MatSystem->CreateMaterial("animated", Generator.CreateFromString("VertexLitGeneric", "$envmap editor/cube_vertigo $envmapcontrast 1 $basetexture dev/zone_warning proxies { texturescroll { texturescrollvar $basetexturetransform texturescrollrate 0.6 texturescrollangle 90 } }", patternchams)));//pearescent
	materiallist.push_back(g_MatSystem->CreateMaterial("esoglow", Generator.CreateFromString("VertexLitGeneric", "$additive 1 $envmap models/effects/cube_white $envmapfresnel 1 $alpha 0.8", patternchams)));//pearescent
    isinit = true;

}
bool Chams::Apply(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* matrix, int n)
{
    if (!isinit)
        InitMats();
    static auto fnDME = Hooks::mdlrender_hook.get_original<decltype(&Hooks::hkDrawModelExecute)>(index::DrawModelExecute);

    const auto mdl = info.pModel;
    int z = -1;
    bool is_arm = strstr(mdl->szName, "arms") != nullptr;
    bool is_sleeve = strstr(mdl->szName, "sleeve") != nullptr;
    bool is_weapon = strstr(mdl->szName, "weapons/v_") != nullptr;
    bool is_fist = std::strstr(mdl->szName, "fists") != nullptr;
    bool is_player = strstr(mdl->szName, "models/player") != nullptr;
    if (is_player) {
        auto entity = C_BasePlayer::GetPlayerByIndex(info.entity_index);
        if (!entity)
            return false;
        if (!entity->IsAlive())
            return false;
        if (!entity->IsPlayer())
            return false;
        bool is_enemy = entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
        bool is_visible = entity->visibleTo(g_LocalPlayer, entity);
        if (entity == g_LocalPlayer && g_Options.chams_settings[0 + 14 * n].enabled) z = 0;
        if (entity == g_LocalPlayer && !g_Options.chams_settings[0 + 14 * n].enabled) return false;
        if (!is_enemy&&entity!=g_LocalPlayer) {
            if (is_visible && g_Options.chams_settings[3 + 14 * n].enabled) z = 3;
            else if (g_Options.chams_settings[4 + 14 * n].enabled) z = 4;
        }
        if (is_enemy) {
            if (is_visible && g_Options.chams_settings[1 + 14 * n].enabled) z = 1;
            else if (g_Options.chams_settings[2 + 14 * n].enabled) z = 2;
        }
        if (entity->HasC4() && is_enemy) {
            if (is_visible && g_Options.chams_settings[7 + 14 * n].enabled) z = 7;
            else if (g_Options.chams_settings[8 + 14 * n].enabled) z = 8;
        }
        if (entity->m_bIsDefusing() && is_enemy) {
            if (is_visible && g_Options.chams_settings[5 + 14 * n].enabled) z = 5;
            else if (g_Options.chams_settings[6 + 14 * n].enabled) z = 6;
        }
    }
    else if (is_sleeve && g_Options.chams_settings[12 + 14 * n].enabled)
        z = 12;
    else if (is_arm && g_Options.chams_settings[11 + 14 * n].enabled)
        z = 11;
    else if (is_weapon && g_Options.chams_settings[13 + 14 * n].enabled && !is_arm && !is_sleeve && !is_fist)
        z = 13;
    if (z == -1)
        return false;

    else {
        z = z + 14 * n;
        OverrideMaterial(z);
    }
    return true;
}
void Chams::ApplyPlayersSceneEnd()
{
    for (int n = 0; n<5; n++) {
        for (int i = 1; i <= g_EngineClient->GetMaxClients(); i++) {
            int z = -1;
            auto entity = C_BasePlayer::GetPlayerByIndex(i);
            if (entity && g_LocalPlayer) {
                if (!entity->IsAlive())
                    continue;
                if (!entity->IsPlayer())
                    continue;
                bool is_enemy = entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
                bool is_visible = entity->visibleTo(g_LocalPlayer, entity);
                if (entity == g_LocalPlayer && g_Options.chams_settings[0 + 13 * n].enabled) z = 0;
                if (!is_enemy) {
                    if (is_visible && g_Options.chams_settings[3 + 13 * n].enabled) z = 3;
                    else if (!is_visible && g_Options.chams_settings[4 + 13 * n].enabled) z = 4;
                }
                if (is_enemy) {
                    if (is_visible && g_Options.chams_settings[1 + 13 * n].enabled) z = 1;
                    else if (!is_visible && g_Options.chams_settings[2 + 13 * n].enabled) z = 2;
                }
                if (entity->HasC4() && is_enemy) {
                    if (is_visible && g_Options.chams_settings[7 + 13 * n].enabled) z = 7;
                    else if (!is_visible && g_Options.chams_settings[8 + 13 * n].enabled) z = 8;
                }
                if (entity->m_bHasDefuser() && is_enemy) {
                    if (is_visible && g_Options.chams_settings[5 + 13 * n].enabled) z = 5;
                    else if (!is_visible && g_Options.chams_settings[6 + 13 * n].enabled) z = 6;
                }
            }
            if (z == -1)
                continue;
            z = z + 13 * n;
            OverrideMaterial(z);
            entity->DrawModel(0x1, 255);
            g_MdlRender->ForcedMaterialOverride(nullptr);
        }
    }
}
void Chams::OverrideMaterial(int z)
{
	IMaterial* material = nullptr;
	material = materiallist.at(g_Options.chams_settings[z].type);
	material->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, g_Options.chams_settings[z].throughwalls);
	//material->ColorModulate(g_Options.chams_settings[z].color.r() / 255.0f, g_Options.chams_settings[z].color.g() / 255.0f, g_Options.chams_settings[z].color.b() / 255.0f);
	material->AlphaModulate(g_Options.chams_settings[z].color.w);
	g_MdlRender->ForcedMaterialOverride(material);
    bool bFound = false;
    auto pVar = material->FindVar("$envmaptint", &bFound);
    if (bFound)
        (*(void(__thiscall**)(int, float, float, float))(*(DWORD*)pVar + 44))((uintptr_t)pVar, g_Options.chams_settings[z].color.x, g_Options.chams_settings[z].color.y, g_Options.chams_settings[z].color.z);
    g_RenderView->SetColorModulation(g_Options.chams_settings[z].color.x, g_Options.chams_settings[z].color.y, g_Options.chams_settings[z].color.z);
}
Chams g_Chams;