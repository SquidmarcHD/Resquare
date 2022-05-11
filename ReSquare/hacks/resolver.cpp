#include "resolver.h"
#include "..\valve_sdk\csgostructs.hpp"
#include "..\helpers\math.hpp"
#include "rage.h"
#include "backtrack.h"
#include "../options.hpp"
#include "../render.hpp"
#include <algorithm>

inline float NormalizeAngle(float flAng)
{
    flAng = flAng + 180.0f;
    flAng = fmod(flAng, 360.0f);
    if (flAng != flAng)
        flAng = 0.0f;
    if (flAng < 0.0f)
        flAng += 360.0f;
    flAng -= 180.0f;

    return flAng;
}

float MaxDesyncAngle(C_BasePlayer* ent) noexcept
{
    const auto animState = ent->GetPlayerAnimState();

    if (!animState)
        return 0.0f;

    float yawModifier = (animState->m_flStopToFullRunningFraction * -0.3f - 0.2f) * std::clamp(animState->m_flFeetSpeedForwardsOrSideWays, 0.0f, 1.0f) + 1.0f;

    if (animState->m_fDuckAmount > 0.0f)
        yawModifier += (animState->m_fDuckAmount * std::clamp(animState->m_flFeetSpeedUnknownForwardOrSideways, 0.0f, 1.0f) * (0.5f - yawModifier));

    return animState->m_fvelocitySubtractY * yawModifier;
}

void Resolver::UpdateAnims()
{
    for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
    {
        auto entity = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(i));

        if (!entity || !entity->IsPlayer() || entity->IsDormant() || !entity->IsAlive())
            continue;
    }
}

float GetNetworkLatency()
{
    INetChannelInfo* nci = g_EngineClient->GetNetChannelInfo();
    if (nci)
    {
        return nci->GetAvgLatency(FLOW_INCOMING);
    }
    return 0.0f;
}

//Find out how many ticks player is choking
int GetNumberOfTicksChoked(C_BasePlayer* pEntity)
{
    float flSimulationTime = pEntity->m_flSimulationTime();
    float flSimDiff = g_GlobalVars->curtime - flSimulationTime;
    float latency = GetNetworkLatency();
    return TIME_TO_TICKS(std::max(0.0f, flSimDiff - latency));
}

void Resolver::OnFramestageNotify()
{
    auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
    if (!pLocal) return;
    if (!pLocal->IsAlive()) return;
    for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
    {
        auto entity = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !entity || !pLocal || !entity->IsPlayer() || entity->IsDormant()
                || !entity->IsAlive() || entity->m_iTeamNum() == pLocal->m_iTeamNum())
            continue;
        //
        //entity->UpdateClientSideAnimation();

        bool InFakeLag = false;
        if (GetNumberOfTicksChoked(entity)>0)
            InFakeLag = true;
        SavedResolverData[i].LastSimtime = entity->m_flSimulationTime();
        if (InFakeLag)
            g_Resolver.GResolverData[i].Fake = g_GlobalVars->tickcount;

        if (entity == pLocal)
            continue;
        SavedResolverData[i].LastPos = entity->m_vecOrigin();
        if ( g_GlobalVars->tickcount- g_Resolver.GResolverData[i].Fake <= 24)
            g_Resolver.GResolverData[i].BreakingLC = ( entity->m_vecOrigin() - SavedResolverData[i].LastPos ).LengthSqr() > 4096.f;
		if (g_GlobalVars->tickcount - g_Resolver.GResolverData[i].Fake > 24)
			continue;
        g_Resolver.GResolverData[i].InFakelag = InFakeLag;
        ResolverAnimDataStorage AnimationStorage;
        AnimationStorage.UpdateAnimationLayers ( entity );
        SavedResolverData[i].LastVel = entity->m_vecVelocity().Length2D();

        //if ( g_GlobalVars->tickcount - g_Resolver.GResolverData[i].Fake < 5)
        //    AddCurrentYaw ( entity, i );

        //float avgang = 0.f;
        //bool b = GetAverageYaw ( i, avgang );

		auto animstate = entity->GetPlayerAnimState();
		if (animstate)
		{
			// missed shot <= 2
			if(g_Resolver.GResolverData[i].Shots <= 2)
			{
                float avgang = 0.f;
                bool b = GetAverageYaw ( i, avgang );
                if(b)
                animstate->m_flGoalFeetYaw = animstate->m_flEyeYaw + std::max(MaxDesyncAngle(entity),std::min(avgang, -MaxDesyncAngle(entity)));
			}
			else
			{
                animstate->m_flGoalFeetYaw = animstate->m_flEyeYaw + std::max(MaxDesyncAngle(entity), std::min(60.f-((g_Resolver.GResolverData[i].Shots-2)%8)*15, -MaxDesyncAngle(entity)));
			}
		}
    }
}

//GRRRRRR DROGEN
void Resolver::Draw()
{
    auto* pLocal = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer()));
    if (!g_Options.misc_settings.drawDebug) return;
    if (!pLocal) return;
    if (!pLocal->IsAlive()) return;
    for (int i = 1; i < g_EngineClient->GetMaxClients(); i++)
    {
        auto entity = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(i));

        if (!entity || !pLocal || !entity->IsPlayer() || entity->IsDormant() || !entity->IsAlive())
            continue;
        Vector out, goal;
        if (Math::WorldToScreen(entity->m_vecOrigin(), out))
        {
            bool Moving = entity->m_vecVelocity().Length2D() > 0.1f;
            bool SlowWalk = Moving && entity->m_vecVelocity().Length2D() < 84.f;
            bool InAir = !(entity->m_fFlags() & FL_ONGROUND);
            bool Standing = !Moving && !InAir;
            bool FakeDucking = entity->m_flDuckSpeed()==8&& entity->m_flDuckAmount()>0.1f&&entity->m_flDuckAmount() <0.8f;
            std::string debugText = "";
            debugText += "Player Simtime: " + std::to_string(entity->m_flSimulationTime()) + "\n";
            debugText += "Old Simtime: " + std::to_string(entity->m_flOldSimulationTime()) + "\n";
            debugText += "Current Simtime: " + std::to_string(g_GlobalVars->curtime) + "\n";
            debugText += "Difference Simtime: " + std::to_string(GetNumberOfTicksChoked(entity)) + "\n";
            debugText += Moving ? "Moving\n" : "";  
            debugText += SlowWalk ? "SlowWalk\n" : "";
            debugText += InAir ? "Airborne\n" : "";
            debugText += Standing ? "Standing\n" : "";
            debugText += FakeDucking ? "In FakeDuck\n" : "";
            if(g_GlobalVars->tickcount - g_Resolver.GResolverData[i].Fake <= 24)
            debugText += "FakeLagging: ";
            debugText += std::to_string(g_GlobalVars->tickcount - g_Resolver.GResolverData[i].Fake);
            debugText += "\n";
            debugText += "Misses: " + std::to_string(g_Resolver.GResolverData[i].Shots) + "\n";
            debugText += "MaxDesyncDelta: " + std::to_string(MaxDesyncAngle(entity)) + "\n";

            if (entity->GetPlayerAnimState())
            {
                if (Math::WorldToScreen(Math::Indicators(entity->m_vecOrigin(), 45.f, entity->GetPlayerAnimState()->m_flGoalFeetYaw), goal)) {
                    Render::Get().RenderLine(out.x, out.y, goal.x, goal.y, { 1, 0, 0, 1 });
                    Render::Get().RenderText("Feet", { goal.x, goal.y }, 10.f, { 1, 1, 1, 1 });
                }
                if (Math::WorldToScreen(Math::Indicators(entity->m_vecOrigin(), 45.f, entity->GetPlayerAnimState()->m_flEyeYaw), goal)) {
                    Render::Get().RenderLine(out.x, out.y, goal.x, goal.y, { 0, 1, 0, 1 });
                    Render::Get().RenderText("Eye", { goal.x, goal.y }, 10.f, { 1, 1, 1, 1 });
                }
                if (Math::WorldToScreen(Math::Indicators(entity->m_vecOrigin(), 45.f, entity->m_flLowerBodyYawTarget()), goal)) {
                    Render::Get().RenderLine(out.x, out.y, goal.x, goal.y, { 0, 0, 1, 1 });
                    Render::Get().RenderText("LBY", { goal.x, goal.y }, 10.f, { 1, 1, 1, 1 });
                }
                if (Math::WorldToScreen(entity->m_vecOrigin() + (entity->m_vecVelocity()/4.f), goal)) {
                    Render::Get().RenderLine(out.x, out.y, goal.x, goal.y, { 1, 0.65, 0, 1 });
                    Render::Get().RenderText("Vel", { goal.x, goal.y }, 10.f, { 1, 1, 1, 1 });
                }
                debugText += "\n\n";
                debugText += "AnimLayer[6]:\nPBR: "+ std::to_string(entity->GetAnimOverlay(6)->m_flPlaybackRate) + "\nWeight:" + std::to_string(entity->GetAnimOverlay(6)->m_flWeight);
            }
            Render::Get().RenderText(debugText, { out.x, out.y - 10 }, 12, { 1,1,1,1 });
        }
    }

}

void PrintRSHeader()
{
    g_CVar->ConsolePrintf("[");
    g_CVar->ConsoleColorPrintf((uint8_t*)&Color(230, 230, 230, 255), "Re");
    g_CVar->ConsoleColorPrintf((uint8_t*)&Color(0, 0, 255, 255), "Square");
    g_CVar->ConsolePrintf("] ");
}

void Resolver::OnFireEvent ( IGameEvent* event , int lastbt)
{
    if ( !g_LocalPlayer )
        return;
    if (!g_LocalPlayer->IsAlive())
        return;
    static float LastPlayerHurt = 0.f;
    std::string startlog = std::string(">\003[\001Re\x0cSquare\003]");
    if ( !strcmp ( event->GetName(), "player_hurt" ) )
    {
        if ( g_GlobalVars->curtime == LastPlayerHurt )
            return;

        LastPlayerHurt = g_GlobalVars->curtime;

        int i = g_EngineClient->GetPlayerForUserID ( event->GetInt ( "userid" ) );
        int attacker = g_EngineClient->GetPlayerForUserID ( event->GetInt ( "attacker" ) );

        if ( attacker != g_LocalPlayer->EntIndex() )
            return;

        C_BasePlayer* user = static_cast<C_BasePlayer*> ( g_EntityList->GetClientEntity ( i ) );

        if ( !user )
            return;
        if (user->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
            return;
        std::string hitgroup = "";

            switch (event->GetInt("hitgroup"))
            {
            case HITGROUP_HEAD:
                hitgroup = "head";
                break;
            case HITGROUP_LEFTLEG:
                hitgroup = "left leg";
                break;
            case HITGROUP_RIGHTLEG:
                hitgroup = "right leg";
                break;
            case HITGROUP_STOMACH:
                hitgroup = "stomach";
                break;
            default:
                hitgroup = "body";
                break;
            }

        g_Resolver.GResolverData[i].Shots--;
        float avg;
        GetAverageYaw(i, avg);
        if (g_Resolver.GResolverData[i].Shots > 2  && g_GlobalVars->tickcount-g_Resolver.GResolverData[i].Fake<=24)
            AddCurrentYaw(user, i, 60.f - ((g_Resolver.GResolverData[i].Shots - 2) % 8) * 15);

        std::string log = std::string("Hit " + std::string(user->GetPlayerInfo().szName) + " for " + std::to_string(event->GetInt("dmg_health")) + " in the " + hitgroup +((lastbt != -1) ? (" with " + std::to_string(lastbt+1) + " backtrack") : "")+" with Average " + std::to_string(avg) + "° Desync while Resolver was " + (g_Options.resolver ? std::string("enabled") : std::string("disabled")) + "\n").c_str();

        if (g_Options.misc_settings.showlogs&&g_Options.rage) {
            PrintRSHeader();
            g_CVar->ConsolePrintf(log.c_str()); 
            std::string log2 = std::string(startlog+"\001 Hit \a" + std::string(user->GetPlayerInfo().szName) + "\001 for \020" + std::to_string(event->GetInt("dmg_health")) + "\001 in the \002" + hitgroup + ((lastbt!=-1)?("\001 with \016"+std::to_string(lastbt+1)+"\001 backtrack"):"") + "\001 with Average \016" + std::to_string(avg) + "\001° Desync while Resolver was \006" + (g_Options.resolver ? std::string("enabled") : std::string("disabled")) + "\n").c_str();
            g_ClientMode->getHudChat()->printf(0, log2.c_str());
        }
        if (g_Resolver.GResolverData[i].Shots < 0)
            g_Resolver.GResolverData[i].Shots = 0;
    }
    static float lastEventTime = 0.f;
    if (!strcmp(event->GetName(), "bullet_impact"))
    {
        C_BasePlayer* shooter = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(g_EngineClient->GetPlayerForUserID(event->GetInt("userid"))));
        if(shooter == g_LocalPlayer)
        if (!(g_Ragebot.bestindex == -1||!shooter || shooter != g_LocalPlayer))
        {
            lastEventTime = g_GlobalVars->curtime;
            Vector p = Vector(event->GetFloat("x"), event->GetFloat("y"), event->GetFloat("z"));
            g_Resolver.GResolverData[g_Ragebot.bestindex].Shots++;
            g_Ragebot.bestindex = -1;
        }
    }
    if ( !strcmp ( event->GetName(), "player_death" ) )
    {
        int i = g_EngineClient->GetPlayerForUserID ( event->GetInt ( "userid" ) );
        int attacker = g_EngineClient->GetPlayerForUserID(event->GetInt("attacker"));
        bool headshot = event->GetBool("headshot");
        bool noscope = event->GetBool("noscope");
        bool wallbang = event->GetBool("penetrated");
        float distance = event->GetFloat("distance");
        g_Resolver.GResolverData[i].Shots = 0;
        C_BasePlayer* user = static_cast<C_BasePlayer*> (g_EntityList->GetClientEntity(i));

        if (!user)
            return;        
        if (attacker != g_LocalPlayer->EntIndex())
            return;
        if (user->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
            return;

        if (g_Options.misc_settings.showlogs&&g_Options.rage) {
            PrintRSHeader();
            float avg;
            GetAverageYaw(i, avg);
            g_CVar->ConsolePrintf(std::string((wallbang?"Wallbanged ":"Killed ") + std::string(user->GetPlayerInfo().szName) + (headshot?" with a Headshot":"") + (noscope?(" noscope at " + std::to_string(distance) + "m"):("")) + "\n").c_str());
            std::string log2 = std::string(startlog + std::string((wallbang ? " \x0cWallbanged \001" : " \001Killed \a") + std::string(user->GetPlayerInfo().szName) + (headshot ? "\001 with a \002Headshot\001" : "") + (noscope ? (" \003noscope\001 at \a" + std::to_string(distance) + "\001m") : ("")) + "\n")).c_str();
            g_ClientMode->getHudChat()->printf(0, log2.c_str());
        }

    }

    if ( !strcmp ( event->GetName(), "round_end" ) )
    {
        for ( int i = 1; i < g_EngineClient->GetMaxClients(); i++ )
        {
            g_Resolver.GResolverData[i].Shots = 0;
        }
    }

    if ( !strcmp ( event->GetName(), "player_disconnect" ) )
    {
        int i = g_EngineClient->GetPlayerForUserID ( event->GetInt ( "userid" ) );

        if ( i == g_EngineClient->GetLocalPlayer() )
            return;

        g_Resolver.GResolverData[i].Shots = 0;
    }
}

void Resolver::AddCurrentYaw ( C_BasePlayer* pl, int i, float ang )
{
    LastYaws[i].push_back ( ang );

    if ( LastYaws[i].size() > 8 )
        LastYaws[i].erase ( LastYaws[i].begin() );
}
bool Resolver::GetAverageYaw ( int i, float& ang )
{
    if ( LastYaws[i].size() < 3 )
        return true;

    float add = 0.f;

    for ( size_t p = 0; p < LastYaws[i].size(); p++ )
        add += LastYaws[i].at ( p );

    ang = add / LastYaws[i].size();
    return true;
}

Resolver g_Resolver;