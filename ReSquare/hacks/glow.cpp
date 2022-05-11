#include "glow.h"
#include "../options.hpp"
void Glow::Exec()
{
    for (auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.m_Size; i++) {
        ImVec4 color = ImVec4{};
        int type = 0;
        bool is_enemy = true;
        bool is_visible = false;
        auto& glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
        auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);
        if (glowObject.IsUnused())
            continue;
        if (!entity || entity->IsDormant())
            continue;
        auto classid = entity->GetClientClass()->m_ClassID;
        switch (classid)
        {
        case ClassId::ClassId_CCSPlayer:
            if (!entity->IsAlive())
                continue;
            if (!entity->IsPlayer())
                continue;
            if (!g_Options.glow_settings[0].enabled && !g_Options.glow_settings[1].enabled && !g_Options.glow_settings[2].enabled && !g_Options.glow_settings[3].enabled && !g_Options.glow_settings[4].enabled && !g_Options.glow_settings[5].enabled && !g_Options.glow_settings[6].enabled && !g_Options.glow_settings[7].enabled && !g_Options.glow_settings[8].enabled)
                continue;
            is_enemy = entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
            is_visible = entity->visibleTo(g_LocalPlayer, entity);
            if (entity == g_LocalPlayer && g_Options.glow_settings[0].enabled) {
                
                color = g_Options.glow_settings[0].color;
                type = g_Options.glow_settings[0].type;
                break;
            }
            if (entity == g_LocalPlayer && !g_Options.glow_settings[0].enabled) continue;
            if (entity->HasC4() && is_enemy) {
                if (is_visible&& g_Options.glow_settings[7].enabled)
                {
                     
                    color = g_Options.glow_settings[7].color;
                    type = g_Options.glow_settings[7].type;
                    break;
                }
                else if(g_Options.glow_settings[8].enabled)
                {
                     
                    color = g_Options.glow_settings[8].color;
                    type = g_Options.glow_settings[8].type;
                    break;
                }
            }
            if (entity->m_bIsDefusing() && is_enemy) {
                if (is_visible && g_Options.glow_settings[5].enabled)
                {
                     
                    color = g_Options.glow_settings[5].color;
                    type = g_Options.glow_settings[5].type;
                    break;
                }
                else if (g_Options.glow_settings[6].enabled)
                {
                     
                    color = g_Options.glow_settings[6].color;
                    type = g_Options.glow_settings[6].type;
                    break;
                }
            }
            if (!is_enemy) {
                if (is_visible && g_Options.glow_settings[3].enabled)
                {
                     
                    color = g_Options.glow_settings[3].color;
                    type = g_Options.glow_settings[3].type;
                }
                else if (g_Options.glow_settings[4].enabled)
                {
                     
                    color = g_Options.glow_settings[4].color;
                    type = g_Options.glow_settings[4].type;
                }
                break;
            }
            if (is_enemy) {
                if (is_visible && g_Options.glow_settings[1].enabled)
                {
                     
                    color = g_Options.glow_settings[1].color;
                    type = g_Options.glow_settings[1].type;
                }
                else if (g_Options.glow_settings[2].enabled)
                {
                    
                    color = g_Options.glow_settings[2].color;
                    type = g_Options.glow_settings[2].type;
                }
                break;
            }
            continue;
        case ClassId_CSmokeGrenadeProjectile:
        case ClassId_CMolotovProjectile:
        case ClassId_CBaseCSGrenadeProjectile:
        case ClassId_CBreachChargeProjectile:
        case ClassId_CBumpMineProjectile:
        case ClassId_CDecoyProjectile:
        case ClassId_CSensorGrenadeProjectile:
            if (g_Options.glow_settings[9].enabled) {
                color = g_Options.glow_settings[9].color;
                type = g_Options.glow_settings[9].type;
                break;
            }
            continue;
        case ClassId::ClassId_CChicken:
            if (g_Options.glow_settings[10].enabled) {
                entity->m_bShouldGlow() = true;
                color = g_Options.glow_settings[10].color;
                type = g_Options.glow_settings[10].type;
                break;
            }
            continue;
        default:
            if (g_Options.glow_settings[11].enabled && entity->IsWeapon()) {
                color = g_Options.glow_settings[11].color;
                type = g_Options.glow_settings[11].type;
                break;
            }
            if (g_Options.glow_settings[12].enabled && entity->IsLoot()) {
                color = g_Options.glow_settings[12].color;
                type = g_Options.glow_settings[12].type;
                break;
            }
            continue;
        }
        if (color.y == 0 && color.x == 0 && color.z == 0)
            continue;
            entity->m_bShouldGlow() = true;
            glowObject.m_flRed = color.x;
            glowObject.m_flGreen = color.y;
            glowObject.m_flBlue = color.z;
            glowObject.m_flAlpha = 1.0f;
            glowObject.m_nGlowStyle = type;
            glowObject.m_bRenderWhenOccluded = true;
            glowObject.m_bRenderWhenUnoccluded = false;
    }
}
void Glow::ExecDlight()
{
    for (auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.m_Size; i++) {
        Options::DLightSettings values = Options::DLightSettings();
        bool is_enemy = true;
        bool is_visible = false;
        auto& glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
        auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);
        if (glowObject.IsUnused())
            continue;
        if (!entity || entity->IsDormant())
            continue;
        auto classid = entity->GetClientClass()->m_ClassID;
        switch (classid)
        {
        case ClassId::ClassId_CCSPlayer:
            if (!entity->IsAlive())
                continue;
            if (!entity->IsPlayer())
                continue;
            if (!g_Options.dlight_settings[0].enabled && !g_Options.dlight_settings[1].enabled && !g_Options.dlight_settings[2].enabled && !g_Options.dlight_settings[3].enabled && !g_Options.dlight_settings[4].enabled && !g_Options.dlight_settings[5].enabled && !g_Options.dlight_settings[6].enabled && !g_Options.dlight_settings[7].enabled && !g_Options.dlight_settings[8].enabled)
                continue;
            is_enemy = entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
            is_visible = entity->visibleTo(g_LocalPlayer, entity);
            if (entity == g_LocalPlayer && g_Options.dlight_settings[0].enabled) {

                values = g_Options.dlight_settings[0];
                break;
            }
            if (entity == g_LocalPlayer && !g_Options.glow_settings[0].enabled) continue;
            if (entity->HasC4() && is_enemy) {
                if (is_visible && g_Options.dlight_settings[7].enabled)
                {

                    values = g_Options.dlight_settings[7];
                    break;
                }
                else if (g_Options.dlight_settings[8].enabled)
                {

                    values = g_Options.dlight_settings[8];
                    break;
                }
            }
            if (entity->m_bIsDefusing() && is_enemy) {
                if (is_visible && g_Options.dlight_settings[5].enabled)
                {

                    values = g_Options.dlight_settings[5];
                    break;
                }
                else if (g_Options.dlight_settings[6].enabled)
                {

                    values = g_Options.dlight_settings[6];
                    break;
                }
            }
            if (!is_enemy) {
                if (is_visible && g_Options.dlight_settings[3].enabled)
                {

                    values = g_Options.dlight_settings[3];
                }
                else if (g_Options.dlight_settings[4].enabled)
                {

                    values = g_Options.dlight_settings[4];
                }
                break;
            }
            if (is_enemy) {
                if (is_visible && g_Options.dlight_settings[1].enabled)
                {

                    values = g_Options.dlight_settings[1];
                }
                else if (g_Options.dlight_settings[2].enabled)
                {

                    values = g_Options.dlight_settings[2];
                }
                break;
            }
            continue;
        case ClassId_CSmokeGrenadeProjectile:
        case ClassId_CMolotovProjectile:
        case ClassId_CBaseCSGrenadeProjectile:
        case ClassId_CBreachChargeProjectile:
        case ClassId_CBumpMineProjectile:
        case ClassId_CDecoyProjectile:
        case ClassId_CSensorGrenadeProjectile:
            if (g_Options.dlight_settings[9].enabled) {
                values = g_Options.dlight_settings[9];
                break;
            }
            continue;
        case ClassId::ClassId_CChicken:
            if (g_Options.dlight_settings[10].enabled) {
                entity->m_bShouldGlow() = true;
                values = g_Options.dlight_settings[10];
                break;
            }
            continue;
        default:
            if (g_Options.dlight_settings[11].enabled && entity->IsWeapon()) {
                values = g_Options.dlight_settings[11];
                break;
            }
            if (g_Options.dlight_settings[12].enabled && entity->IsLoot()) {
                values = g_Options.dlight_settings[12];
                break;
            }
            continue;
        }
        if (!values.enabled)
            continue;
        if (values.color.x == 0 && values.color.y == 0 && values.color.z == 0)
            continue;
        C_Dlight* pElight = g_Dlight->CL_AllocDlight(entity->EntIndex());
        pElight->origin = entity->m_vecOrigin() + Vector(0.0f, 0.0f, values.offset);
        pElight->m_Direction = entity->m_vecOrigin();
        pElight->radius =values.radius;
        pElight->color.r = static_cast<int>(values.color.x*255.f);
        pElight->color.g = static_cast<int>(values.color.y*255.f);
        pElight->color.b = static_cast<int>(values.color.z*255.f);
        pElight->color.exponent = values.exponent;
        pElight->die = g_GlobalVars->curtime + 0.05f;
        pElight->decay = pElight->radius / 5.0f;
        pElight->key = entity->EntIndex();
    }
}
Glow g_Glow;