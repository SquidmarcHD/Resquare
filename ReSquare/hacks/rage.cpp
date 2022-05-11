#include "backtrack.h"
#include "raytracer.h"
#include "rage.h"
#include "../render.hpp"
#include "../options.hpp"
bool attack = false;
Vector transform(const matrix3x4_t& mat, const Vector vector)
{
    return Vector{ vector.dotProduct({ mat[0][0], mat[0][1], mat[0][2] }) + mat[0][3],
                   vector.dotProduct({ mat[1][0], mat[1][1], mat[1][2] }) + mat[1][3],
                   vector.dotProduct({ mat[2][0], mat[2][1], mat[2][2] }) + mat[2][3] };
}
float GetHitgroupDamageMult(int iHitGroup)
{
    switch (iHitGroup)
    {
    case HITGROUP_GENERIC:
        return 1.f;
    case HITGROUP_HEAD:
        return 4.f;
    case HITGROUP_CHEST:
        return 1.f;
    case HITGROUP_STOMACH:
        return 1.25f;
    case HITGROUP_LEFTARM:
        return 1.f;
    case HITGROUP_RIGHTARM:
        return 1.f;
    case HITGROUP_LEFTLEG:
        return 0.75f;
    case HITGROUP_RIGHTLEG:
        return 0.75f;
    case HITGROUP_GEAR:
        return 1.f;
    default:
        break;
    }

    return 1.f;
}
constexpr bool isArmored(int hitGroup, bool helmet) noexcept
{
    switch (hitGroup) {
    case HITGROUP_HEAD:
        return helmet;

    case HITGROUP_CHEST:
    case HITGROUP_STOMACH:
    case HITGROUP_LEFTARM:
    case HITGROUP_RIGHTARM:
        return true;
    default:
        return false;
    }
}
void ScaleDamage(int hitgroup, C_BasePlayer* enemy, float weapon_armor_ratio, float& current_damage)
{
    current_damage *=  GetHitgroupDamageMult(hitgroup);

    if (enemy->m_ArmorValue() > 0)
    {
        if (hitgroup == HITGROUP_HEAD)
        {
            if (enemy->m_bHasHelmet())
                current_damage *= (weapon_armor_ratio * .5f);
        }
        else
        {
            current_damage *= (weapon_armor_ratio * .5f);
        }
    }
}
static uint8_t* memoryexitfn = 0;

static bool traceToExit(const trace_t& enterTrace, const Vector& start, const Vector& direction, Vector& end, trace_t& exitTrace)
{
    bool result = false;
    if (memoryexitfn == 0)
        memoryexitfn = Utils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 83 EC 4C F3 0F 10 75");
    const auto traceToExitFn = memoryexitfn;
    __asm {
        push 0
        push 0
        push 0
        push exitTrace
        mov eax, direction
        push[eax]Vector.z
        push[eax]Vector.y
        push[eax]Vector.x
        mov eax, start
        push[eax]Vector.z
        push[eax]Vector.y
        push[eax]Vector.x
        mov edx, enterTrace
        mov ecx, end
        call traceToExitFn
        add esp, 40
        mov result, al
    }
    return result;
}
void TraceLine(Vector& absStart, Vector& absEnd, unsigned int mask, IClientEntity* ignore, CGameTrace* ptr)
{
    
    Ray_t ray;
    ray.Init(absStart, absEnd);
    CTraceFilter filter;
    filter.pSkip = ignore;

    g_EngineTrace->TraceRay(ray, mask, &filter, ptr);
}
bool IsBreakableEntity(IClientEntity* entityPtr)
{
    if (!entityPtr || !entityPtr->EntIndex())
        return false;

    const auto result = reinterpret_cast<bool(__thiscall*)(IClientEntity*)>(Utils::PatternScan(GetModuleHandleW(L"client.dll"), "55 8B EC 51 56 8B F1 85 F6 74 68"));
    if (!result) {
        return false;
    }

    const ClientClass* cclass = entityPtr->GetClientClass();

    if (!cclass) {
        return false;
    }

    const ClassId currentClassId = cclass->m_ClassID;

    if (!result &&
        (currentClassId == ClassId::ClassId_CBaseDoor ||
            currentClassId == ClassId::ClassId_CBreakableSurface ||
            (currentClassId == ClassId::ClassId_CBaseEntity && entityPtr->GetCollideable()->GetSolid() == 1)))
        return true;

    return result;
}
bool traceToExitalt(CGameTrace* enter_trace, Vector start, Vector dir, CGameTrace* exit_trace)
{
    Vector end = { 0.f, 0.f, 0.f };
    float distance = 0.f;
    signed int distCheck = 25;
    int firstContents = 0;

    do
    {
        distance += 5.f;
        end = start + dir * distance;

        if (!firstContents) firstContents = g_EngineTrace->GetPointContents(end, MASK_SHOT | CONTENTS_GRATE);

        const int pointContents = g_EngineTrace->GetPointContents(end, MASK_SHOT | CONTENTS_GRATE);

        if (!(pointContents & (MASK_SHOT_HULL | CONTENTS_HITBOX)) || pointContents & CONTENTS_HITBOX && pointContents != firstContents)
        {
            const Vector newEnd = end - (dir * 4.f);

            Ray_t ray;
            ray.Init(end, newEnd);

            g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, nullptr, exit_trace);

            if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX)
            {
                Ray_t ray2;
                ray2.Init(end, start);
                CTraceFilter filter2;
                filter2.pSkip = exit_trace->hit_entity;
                g_EngineTrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter2, exit_trace);

                if (exit_trace->DidHit() && !exit_trace->startsolid) return true;

                continue;
            }
            if (exit_trace->DidHit() && !exit_trace->startsolid)
            {
                if (enter_trace->surface.flags & SURF_NODRAW || !(exit_trace->surface.flags & SURF_NODRAW)) {
                    if (exit_trace->plane.normal.dotProduct(dir) <= 1.f)
                        return true;

                    continue;
                }

                if (IsBreakableEntity(enter_trace->hit_entity)
                    && IsBreakableEntity(exit_trace->hit_entity))
                    return true;

                continue;
            }

            if (exit_trace->surface.flags & SURF_NODRAW)
            {
                if (IsBreakableEntity(enter_trace->hit_entity)
                    && IsBreakableEntity(exit_trace->hit_entity))
                {
                    return true;
                }
                else if (!(enter_trace->surface.flags & SURF_NODRAW))
                {
                    continue;
                }
            }

            if ((!enter_trace->hit_entity
                || enter_trace->hit_entity->EntIndex() == 0)
                && (IsBreakableEntity(enter_trace->hit_entity)))
            {
                exit_trace = enter_trace;
                exit_trace->endpos = start + dir;
                return true;
            }
            continue;
        }
        distCheck--;
    } while (distCheck);

    return false;
}
bool traceToExitalt2(CGameTrace* enter_trace, Vector start, Vector dir, CGameTrace* exit_trace)
{
    Vector end = { 0.f, 0.f, 0.f };
    float distance = 0.f;
    signed int distCheck = 25;
    int firstContents = 0;
    do
    {
        distance += 4.f;
        end = start + dir * distance;
        if (!firstContents) firstContents = g_EngineTrace->GetPointContents(end, MASK_SHOT | CONTENTS_GRATE);
        const int pointContents = g_EngineTrace->GetPointContents(end, MASK_SHOT | CONTENTS_GRATE);
        if (!(pointContents & (MASK_SHOT_HULL | CONTENTS_HITBOX)) || pointContents & CONTENTS_HITBOX && pointContents != firstContents)
        {
            const Vector newEnd = end - (dir * 4.f);

            Ray_t ray;
            ray.Init(end, newEnd);

            g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, nullptr, exit_trace);
            auto enterentity = enter_trace->hit_entity;
            auto exitentity = exit_trace->hit_entity;
            bool exithit = exit_trace->DidHit();
            bool isbreakableenter = IsBreakableEntity(enterentity);
            bool isbreakableexit = IsBreakableEntity(exitentity);
            if (exit_trace->startsolid && exit_trace->surface.flags & SURF_HITBOX)
            {
                Ray_t ray2;
                ray2.Init(end, start);
                CTraceFilter filter2;
                filter2.pSkip = exitentity;
                g_EngineTrace->TraceRay(ray, MASK_SHOT_HULL | CONTENTS_HITBOX, &filter2, exit_trace);
                if (exit_trace->DidHit() && !exit_trace->startsolid) return true;
                continue;
            }
            if (exithit && !exit_trace->startsolid)
            {
                if (enter_trace->surface.flags & SURF_NODRAW || !(exit_trace->surface.flags & SURF_NODRAW)) {
                    if (exit_trace->plane.normal.dotProduct(dir) <= 1.f)
                        return true;
                    continue;
                }
                if (isbreakableenter && isbreakableexit)
                    return true;
                continue;
            }
            if (exit_trace->surface.flags & SURF_NODRAW)
            {
                if (isbreakableenter && isbreakableexit)
                    return true;
                else if (!(enter_trace->surface.flags & SURF_NODRAW))
                    continue;
            }
            if ((!enterentity || enterentity->EntIndex() == 0) && (isbreakableenter))
            {
                exit_trace = enter_trace;
                exit_trace->endpos = start + dir;
                return true;
            }
            continue;
        }
        distCheck--;
    } while (distCheck);

    return false;
}
bool traceToExitalt3(CGameTrace* enter_trace, Vector start, Vector dir, CGameTrace* exit_trace)
{
    float flDistance = 0;
    Vector point = start;
    while (flDistance <= 125)
    {
        flDistance += 5;

        point += dir * flDistance;
        int point_contents = g_EngineTrace->GetPointContents_WorldOnly(point, MASK_SHOT_HULL);
        if (!(point_contents & MASK_SHOT_HULL)|| point_contents & CONTENTS_HITBOX)
        {
            TraceLine(point, enter_trace->endpos, MASK_SHOT | CONTENTS_GRATE, nullptr, exit_trace);
            return true;
        }
    }
}
bool traceToExitalt4(CGameTrace &enterTrace, Vector start, Vector dir, CGameTrace &exitTrace)
{
    float maxDistance = 90.f, rayExtension = 5.f, currentDistance = 0;
    int firstContents = 0;
    while (currentDistance <= maxDistance)
    {
        currentDistance += rayExtension;
        Vector start = start + dir * currentDistance;
        if (!firstContents)
            firstContents = g_EngineTrace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);
        int pointContents = g_EngineTrace->GetPointContents(start, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr);
        if (!(pointContents & MASK_SHOT_HULL) || pointContents & CONTENTS_HITBOX && pointContents != firstContents)
        {
            Vector end = start - (dir * rayExtension);
            TraceLine(start, end, MASK_SHOT_HULL | CONTENTS_HITBOX, nullptr, &exitTrace);
            if (exitTrace.startsolid && exitTrace.surface.flags & SURF_HITBOX)
            {
                TraceLine(start, start, MASK_SHOT_HULL, exitTrace.hit_entity, &exitTrace);
                if (exitTrace.DidHit() && !exitTrace.startsolid)
                {
                    start = exitTrace.endpos;
                    return true;
                }
                continue;
            }
            if (exitTrace.DidHit() && !exitTrace.startsolid)
            {
                if (IsBreakableEntity(enterTrace.hit_entity) && IsBreakableEntity(exitTrace.hit_entity))
                    return true;
                if (enterTrace.surface.flags & SURF_NODRAW || !(exitTrace.surface.flags & SURF_NODRAW) && (exitTrace.plane.normal.Dot(dir) <= 1.f))
                {
                    float multAmount = exitTrace.fraction * 4.f;
                    start -= dir * multAmount;
                    return true;
                }
                continue;
            }
            if (!exitTrace.DidHit() || exitTrace.startsolid)
            {
                if (enterTrace.hit_entity!=NULL && !enterTrace.DidHit() && IsBreakableEntity(enterTrace.hit_entity))
                {
                    exitTrace = enterTrace;
                    exitTrace.endpos = start + dir;
                    return true;
                }

                continue;
            }
        }
    }
    return false;
}
static float handleBulletPenetration(surfacedata_t* enterSurfaceData, CGameTrace& enterTrace, const Vector& direction, Vector& result, float penetration, float damage) noexcept
{
    Vector end;
    CGameTrace exitTrace;
    
    if (!traceToExitalt3(&enterTrace, enterTrace.endpos, direction, &exitTrace))
        return -1.0f;/*
    if (!traceToExit(enterTrace, enterTrace.endpos, direction, end, exitTrace))
        return -1.0f;*/
    surfacedata_t* exitSurfaceData = g_PhysSurface->GetSurfaceData(exitTrace.surface.surfaceProps);

    float damageModifier = 0.16f;
    float penetrationModifier = (enterSurfaceData->game.flPenetrationModifier + exitSurfaceData->game.flPenetrationModifier) / 2.0f;

    if (enterSurfaceData->game.material == 71 || enterSurfaceData->game.material == 89) {
        damageModifier = 0.05f;
        penetrationModifier = 3.0f;
    }
    else if (enterTrace.contents >> 3 & 1 || enterTrace.surface.flags >> 7 & 1) {
        penetrationModifier = 1.0f;
    }

    if (enterSurfaceData->game.material == exitSurfaceData->game.material) {
        if (exitSurfaceData->game.material == 85 || exitSurfaceData->game.material == 87)
            penetrationModifier = 3.0f;
        else if (exitSurfaceData->game.material == 76)
            penetrationModifier = 2.0f;
    }
    //Utils::ConsolePrint(std::to_string(g_LocalPlayer->m_hActiveWeapon()->GetCSWeaponData()->flArmorRatio).c_str());
    damage -= 11.25f / penetration / penetrationModifier + damage * damageModifier + (exitTrace.endpos - enterTrace.endpos).LengthSqr() / 24.0f / penetrationModifier;
    result = exitTrace.endpos;
    return damage;
}
float Rage::canScan(C_BasePlayer* entity, const Vector& destination, const CCSWeaponInfo* weaponData, Vector location = g_LocalPlayer->GetEyePos())
{
    float damage{ static_cast<float>(weaponData->iDamage) };

    Vector start{ location };
    Vector direction{ destination - start };
    direction /= direction.Length();

    int hitsLeft = 4;

    while (damage >= 1.0f && hitsLeft) {
        CGameTrace trace;
        Ray_t ray;
        ray.Init(start, destination);
        CTraceFilter filter;
        filter.pSkip = g_LocalPlayer;
        g_EngineTrace->TraceRay(ray, 0x4600400B, &filter, &trace);

        if (trace.fraction == 1.0f)
            break;
        if (trace.hit_entity == entity && trace.hitgroup > 0 && trace.hitgroup <= 7) {
            damage = GetHitgroupDamageMult(trace.hitgroup) * damage * std::pow(weaponData->flRangeModifier, trace.fraction * weaponData->flRange / 500.0f);
            auto basePlayer = (C_BasePlayer*)trace.hit_entity;
            if (float armorRatio{ weaponData->flArmorRatio / 2.0f }; isArmored(trace.hitgroup, basePlayer->m_bHasHelmet()))
                damage -= (basePlayer->m_ArmorValue() < damage * armorRatio / 2.0f ? basePlayer->m_ArmorValue() * 4.0f : damage) * (1.0f - armorRatio);

            return damage;
        }
        const auto surfaceData = g_PhysSurface->GetSurfaceData(trace.surface.surfaceProps);

        if (surfaceData->game.flPenetrationModifier < 0.1f)
            break;

        damage = handleBulletPenetration(surfaceData, trace, direction, start, g_LocalPlayer->m_hActiveWeapon()->getWeaponPenetration(g_LocalPlayer->m_hActiveWeapon()->m_Item().m_iItemDefinitionIndex()), damage);
        hitsLeft--;
    }
    return 0;
}
bool Rage::retrieveMultipoints(C_BaseEntity* entity, float multiPointsExpansion, Vector(&multiPoints)[9], int desiredHitBox)
{
    if (desiredHitBox < 0 || desiredHitBox > 18)
        return false;

    if (!entity)
        return false;

    static matrix3x4_t bones[256];

    if (!entity->SetupBones(bones, ARRAYSIZE(bones), BONE_USED_BY_ANYTHING, 0.0f))
        return false;

    const model_t* model = entity->GetModel();

    if (!model)
        return false;

    studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(model);

    if (!hdr)
        return false;
    auto animating = (C_BasePlayer*)entity;
    int hitBoxSet = animating->m_nHitboxSet();

    if (hitBoxSet < 0)
        return false;

    mstudiohitboxset_t* hitBoxSetPtr = hdr->GetHitboxSet(hitBoxSet);

    if (!hitBoxSetPtr)
        return false;

    mstudiobbox_t* box = hitBoxSetPtr->GetHitbox(desiredHitBox);

    if (!box)
        return false;

    Vector min = box->bbmin;
    Vector max = box->bbmax;

    if (box->m_flRadius > 0.0f)
    {
        min -= Vector{ box->m_flRadius * multiPointsExpansion, box->m_flRadius * multiPointsExpansion, box->m_flRadius * multiPointsExpansion, };
        max += Vector{ box->m_flRadius * multiPointsExpansion, box->m_flRadius * multiPointsExpansion, box->m_flRadius * multiPointsExpansion, };
    };

    static Vector locations[9];

    locations[0] = (min + max) * 0.5f;

    locations[1] = Vector{ min.x, min.y, min.z, };
    locations[2] = Vector{ min.x, max.y, min.z, };
    locations[3] = Vector{ max.x, max.y, min.z, };
    locations[4] = Vector{ max.x, min.y, min.z, };
    locations[5] = Vector{ max.x, max.y, max.z, };
    locations[6] = Vector{ min.x, max.y, max.z, };
    locations[7] = Vector{ min.x, min.y, max.z, };
    locations[8] = Vector{ max.x, min.y, max.z, };

    for (int multiPoint = 0; multiPoint < 9; multiPoint++)
    {
        if (multiPoint > 0)
        {
            locations[multiPoint] = (((locations[multiPoint] + locations[0]) * 0.5f) + locations[multiPoint]) * 0.5f;
        };

        multiPoints[multiPoint] = transform(bones[box->bone], locations[multiPoint]);
    };

    return true;
}
void Rage::Aim(CUserCmd* cmd, int lastTickcount)
{
    int checkedBones = 0;
	Vector eyepos;
    Vector aimpos = Vector(99999, 99999, 99999);
    int bestlocalindex = -1;
    auto backupcmd = cmd;
    float bestdamage = 0;
	auto weapon = g_LocalPlayer->m_hActiveWeapon();
    if (attack) {
        cmd->buttons &= ~IN_ATTACK;
        attack = false;
    }
    if (weapon->m_flNextPrimaryAttack() > g_GlobalVars->curtime)
        return;
	if (!weapon || !weapon->IsGun())
		return;
    if (!weapon->m_iClip1())
        return;
    int weaponIndex = weapon->getWeaponStandalone(weapon->m_Item().m_iItemDefinitionIndex());
    if (!g_Options.ragebot_settings[weaponIndex].enabled) {
        weaponIndex = weapon->getWeaponClass(weapon->m_Item().m_iItemDefinitionIndex());
        if (!g_Options.ragebot_settings[weaponIndex].enabled) {
            weaponIndex = 0;
            if (!g_Options.ragebot_settings[0].enabled)
                return;
        }
    }
    auto weapondata = weapon->GetCSWeaponData();
	eyepos = g_LocalPlayer->GetEyePos();
    if (g_Options.misc_settings.prediction)
        eyepos += g_LocalPlayer->m_vecVelocity() * g_GlobalVars->interval_per_tick;
	for (int i = 1; i <= g_EngineClient->GetMaxClients(); i++) {
		auto ent = C_BaseEntity::GetEntityByIndex(i);
		auto entity = (C_BasePlayer*)ent;
		if (!entity || !entity->IsAlive() || entity->m_bGunGameImmunity())
			continue;
		if (entity == g_LocalPlayer || !entity->IsPlayer())
			continue;
		if (entity->IsDormant() || entity->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
			continue;
        entity->InvalidateBoneCache();
        for (int bones = 0; bones < 19; bones++)
        {
            if (!g_Options.ragebot_settings[weaponIndex].bones[bones])
                continue;
            static Vector multiPoints[9];
            retrieveMultipoints(entity, /*g_Options.ragebot_settings[weaponIndex].multipointexpansion*/g_Options.misc_settings.multipointsize, multiPoints, bones);
            if (!multiPoints)
                continue;
            QAngle currentRotation = Math::CalcAngle(eyepos, multiPoints[0]) - *g_LocalPlayer->GetVAngles();
            Math::Normalize3(currentRotation);
            float fov = std::hypot(currentRotation.pitch, currentRotation.yaw);
            if (fov < g_Options.ragebot_settings[weaponIndex].fov)
            {
                for (int point = 0; point < g_Options.misc_settings.maxBonesperJoint; point++) {
                    checkedBones += 1;
                    if (checkedBones > g_Options.misc_settings.maxBones)
                        return;
                    float dmg = 0;
                    dmg = canScan(entity, multiPoints[point], weapondata);
                    if (dmg > bestdamage && ((dmg >= (float)g_Options.ragebot_settings[weaponIndex].mindmg) || (g_Options.ragebot_settings[weaponIndex].killshot && (dmg >= ((float)entity->m_iHealth() + (float)g_Options.ragebot_settings[weaponIndex].overkill)))||g_GlobalVars->tickcount-lastTickcount<5)) {
                        aimpos = multiPoints[point];
                        bestlocalindex = i;
                        bestdamage = dmg;
                    }
                }
            }
        }
        std::vector<int> validsims;
        for (int backtrack = g_Options.backtrack_settings.targetlast; backtrack > 0; backtrack--)
        {
            int oldestvalid = INT_MAX;
            float oldestsimtime = FLOAT32_MAX;
            for (int valids = 0; valids < g_Options.backtrack_settings.time; valids++)
            {
                if ((!(g_Backtrack.TimeWarpData[i][valids].simtime > g_LocalPlayer->m_flSimulationTime() - 1)) || (oldestsimtime < g_Backtrack.TimeWarpData[i][valids].curtime))
                    continue;
                bool found = false;
                for (int invalid = 0; invalid < validsims.size(); invalid++)
                {
                    if (valids == validsims.at(invalid)) {
                        found = true;
                        break;
                    }
                }
                if (found)
                    continue;
                else
                {
                    oldestvalid = valids;
                    oldestsimtime = g_Backtrack.TimeWarpData[i][valids].curtime;
                }
            }
            if (oldestvalid != INT_MAX)
                validsims.push_back(oldestvalid);
        }            
        matrix3x4_t backupbones[128];
        Vector backupmaxs, backupmins, backupabsorigin, backuporigin;
        if (validsims.size() > 0) {            
            std::memcpy(backupbones, (entity + 0x26A8), sizeof(backupbones));
            backupmaxs = entity->GetCollideable()->OBBMaxs();
            backupmins = entity->GetCollideable()->OBBMins();
            backupabsorigin = entity->m_vecOrigin();
            backuporigin = entity->m_vecOrigin();
        }
        for (int backtrack = 0; backtrack < validsims.size(); backtrack++)
        {
            std::memcpy((void*)(entity + 0x26A8), g_Backtrack.TimeWarpData[i][validsims.at(backtrack)].matrix, sizeof(backupbones));
            entity->GetCollideable()->OBBMaxs() = g_Backtrack.TimeWarpData[i][validsims.at(backtrack)].maxs;
            entity->GetCollideable()->OBBMins() = g_Backtrack.TimeWarpData[i][validsims.at(backtrack)].mins;
            entity->SetAbsOrigin(g_Backtrack.TimeWarpData[i][validsims.at(backtrack)].absorigin);
            entity->m_vecOrigin() = g_Backtrack.TimeWarpData[i][validsims.at(backtrack)].origin;                
            entity->InvalidateBoneCache();
            for (int bones = 0; bones < 19; bones++)
            {
                if (!g_Options.ragebot_settings[weaponIndex].bones[bones])
                    continue;                
                if (!(g_Backtrack.TimeWarpData[i][validsims.at(backtrack)].simtime > g_LocalPlayer->m_flSimulationTime() - 1))
                    continue;
                static Vector multiPoints[9];


                std::copy(std::begin(g_Backtrack.TimeWarpData[i][validsims.at(backtrack)].multipoints[bones]), std::end(g_Backtrack.TimeWarpData[i][validsims.at(backtrack)].multipoints[bones]), std::begin(multiPoints));
                QAngle currentRotation = Math::CalcAngle(eyepos, multiPoints[0]) - *g_LocalPlayer->GetVAngles();
                Math::Normalize3(currentRotation);
                float fov = std::hypot(currentRotation.pitch, currentRotation.yaw);
                if (fov < g_Options.ragebot_settings[weaponIndex].fov)
                {
                    for (int point = 0; point < g_Options.misc_settings.maxBonesperJoint; point++) {
                        checkedBones += 1;
                        if (checkedBones > g_Options.misc_settings.maxBones)
                            return;
                        float dmg = 0;
                        dmg = canScan(entity, multiPoints[point], weapondata);
                        if (dmg > bestdamage && ((dmg >= (float)g_Options.ragebot_settings[weaponIndex].mindmg) || (g_Options.ragebot_settings[weaponIndex].killshot && (dmg >= ((float)entity->m_iHealth() + (float)g_Options.ragebot_settings[weaponIndex].overkill))) || g_GlobalVars->tickcount - lastTickcount < 5)) {
                            aimpos = multiPoints[point];
                            bestlocalindex = i;
                            bestdamage = dmg;
                        }
                    }
                }

            }                

        }
        if (validsims.size() > 0)
        {
            std::memcpy((void*)(entity + 0x26A8), backupbones, sizeof(backupbones));
            entity->GetCollideable()->OBBMaxs() = backupmaxs;
            entity->GetCollideable()->OBBMins() = backupmins;
            entity->SetAbsOrigin(backupabsorigin);
            entity->m_vecOrigin() = backuporigin;
        }
        if (aimpos.x == 99999 && aimpos.y == 99999 && aimpos.z == 99999)
            continue;
        int weapontype = weapon->getWeaponIndex(weapon->m_Item().m_iItemDefinitionIndex());
        if (g_Options.ragebot_settings[weaponIndex].AutoScope && !g_LocalPlayer->m_bIsScoped() &&( weapontype == 29 || weapontype == 30 || weapontype == 31 || weapontype == 32 || weapontype == 33 || weapontype == 34))
            cmd->buttons |= IN_ATTACK2;
        if (g_Options.ragebot_settings[weaponIndex].AutoCrouch)
            cmd->buttons |= IN_DUCK;
        if (g_Options.ragebot_settings[weaponIndex].AutoStop!=0){
            cmd->sidemove = backupcmd->sidemove-(backupcmd->sidemove * ((float)(g_Options.ragebot_settings[weaponIndex].AutoStop) /100.0f));
            cmd->forwardmove = backupcmd->forwardmove - (backupcmd->forwardmove * ((float)(g_Options.ragebot_settings[weaponIndex].AutoStop)/100.0f));
        }
        if (((1 / (20 * (eyepos.DistTo(aimpos) / 130) * (weapon->GetSpread() + weapon->GetInaccuracy()))) * 135 < g_Options.ragebot_settings[weaponIndex].minchance) || g_GlobalVars->tickcount - lastTickcount < 5) {
            aimpos = Vector(99999, 99999, 99999);
            continue;
        }
	}
    if (aimpos.x == 99999 && aimpos.y == 99999 && aimpos.z == 99999) {
        return;
    }
    if (weapon->m_flNextPrimaryAttack() > g_GlobalVars->curtime) {
        return;
    }
    if (g_Options.misc_settings.prediction)
        aimpos += ((C_BasePlayer*)C_BaseEntity::GetEntityByIndex(bestlocalindex))->m_vecVelocity() * g_GlobalVars->interval_per_tick;
    QAngle aimvector = Math::CalcAngle(eyepos, aimpos);
	Math::Normalize3(aimvector);
	cmd->viewangles = aimvector;
	cmd->viewangles -= g_LocalPlayer->m_aimPunchAngle() * 2;
    if (g_Options.ragebot_settings[weaponIndex].AutoShot){
        cmd->buttons |= IN_ATTACK;
        attack = true;
        bestindex = bestlocalindex;
    }
    if (!g_Options.ragebot_settings[weaponIndex].silent)
        g_EngineClient->SetViewAngles(&cmd->viewangles);
}

void Rage::RenderDebugPointInfo()
{
    if (!g_Options.misc_settings.debugmultipoints)
        return;
    auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
    if (!pLocal || !pLocal->IsAlive() || !g_EngineClient->IsInGame()||!g_Options.rage)
        return;
    int checkedBones = 0;
    auto weapon = pLocal->m_hActiveWeapon();
    if (!weapon || !weapon->IsGun())
        return;
    int weaponIndex = weapon->getWeaponStandalone(weapon->m_Item().m_iItemDefinitionIndex());
    if (!g_Options.ragebot_settings[weaponIndex].enabled) {
        weaponIndex = weapon->getWeaponClass(weapon->m_Item().m_iItemDefinitionIndex());
        if (!g_Options.ragebot_settings[weaponIndex].enabled) {
            weaponIndex = 0;
            if (!g_Options.ragebot_settings[0].enabled)
                return;
        }
    }
    for (int i = 1; i <= g_EngineClient->GetMaxClients(); i++) {
        auto ent = C_BaseEntity::GetEntityByIndex(i);
        auto entity = (C_BasePlayer*)ent;
        if (!entity || !entity->IsAlive() || entity->m_bGunGameImmunity())
            continue;
        if (entity == pLocal || !entity->IsPlayer())
            continue;
        if (entity->IsDormant() || entity->m_iTeamNum() == pLocal->m_iTeamNum())
            continue;
        for (int bones = 0; bones < 19; bones++)
        {
            if (!g_Options.ragebot_settings[weaponIndex].bones[bones])
                continue;
            static Vector multiPoints[9];
            retrieveMultipoints(entity, /*g_Options.ragebot_settings[weaponIndex].multipointexpansion*/g_Options.misc_settings.multipointsize, multiPoints, bones);
            if (!multiPoints)
                continue;
            entity->InvalidateBoneCache();
            for (int point = 0; point < g_Options.misc_settings.maxBonesperJoint; point++) {
                checkedBones += 1;
                if (checkedBones > g_Options.misc_settings.maxBones)
                    return;
                Vector out;
                if(Math::WorldToScreen(multiPoints[point], out))
                Render::Get().RenderCircleFilled(out.x, out.y, 2, 3, ImVec4{ 1,1,1,1 });
            }
        }
        std::vector<int> validsims;
        for (int backtrack = g_Options.backtrack_settings.targetlast; backtrack > 0; backtrack--)
        {
            int oldestvalid = INT_MAX;
            float oldestsimtime = FLOAT32_MAX;
            for (int valids = 0; valids < g_Options.backtrack_settings.time; valids++)
            {
                if ((!(g_Backtrack.TimeWarpData[i][valids].simtime > g_LocalPlayer->m_flSimulationTime() - 1)) || (oldestsimtime < g_Backtrack.TimeWarpData[i][valids].curtime))
                    continue;
                bool found = false;
                for (int invalid = 0; invalid < validsims.size(); invalid++)
                {
                    if (valids == validsims.at(invalid)) {
                        found = true;
                        break;
                    }
                }
                if (found)
                    continue;
                else
                {
                    oldestvalid = valids;
                    oldestsimtime = g_Backtrack.TimeWarpData[i][valids].curtime;
                }
            }
            if (oldestvalid != INT_MAX)
                validsims.push_back(oldestvalid);
        }
        for (int backtrack = 0; backtrack < validsims.size(); backtrack++)
        {
            for (int bones = 0; bones < 19; bones++)
            {
                if (!g_Options.ragebot_settings[weaponIndex].bones[bones])
                    continue;
                static Vector multiPoints[9];
                entity->InvalidateBoneCache();
                if (!(g_Backtrack.TimeWarpData[i][validsims.at(backtrack)].simtime > pLocal->m_flSimulationTime() - 1))
                    continue;
                std::copy(std::begin(g_Backtrack.TimeWarpData[i][validsims.at(backtrack)].multipoints[bones]), std::end(g_Backtrack.TimeWarpData[i][validsims.at(backtrack)].multipoints[bones]), std::begin(multiPoints));
                for (int point = 0; point < g_Options.misc_settings.maxBonesperJoint; point++) {
                    checkedBones += 1;
                    if (checkedBones > g_Options.misc_settings.maxBones)
                        return;
                    Vector out;
                    if (Math::WorldToScreen(multiPoints[point], out))
                        Render::Get().RenderCircleFilled(out.x, out.y, 2, 3, ImVec4{ 1,1,1,1 });
                }
            }
        }
    }
}
Rage g_Ragebot;