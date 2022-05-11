#include "../valve_sdk/csgostructs.hpp"
#include <deque>

#pragma once

struct ResolverAnimDataStorage
{
    void UpdateAnimationLayers(C_BasePlayer* player)
    {
        m_iLayerCount = player->GetNumAnimOverlays();
        for (int i = 0; i < m_iLayerCount; i++)
        {
            animationLayer[i] = player->GetAnimOverlays()[i];
        }
    };
    int32_t m_iLayerCount;
    AnimationLayer animationLayer[15];
};

struct SavedResolverData
{
    Vector LastPos = Vector(0, 0, 0);
    float LastVel = 0.f;
    float LastSimtime = 0.f;
};

enum class DetectionModes : int
{
    None,
    Spinning,
    Static,
    Random
};


struct ResolverDetectionData
{
    std::deque<float> AllLbys;
    std::deque<float> LastAddSimtime;
    float LastSimtime = 0.f;;
    float SpinSpeed = 0.f;
    DetectionModes mode = DetectionModes::None;
    DetectionModes BeforeMoving = DetectionModes::None;
    bool WasMoving = false;
};

enum class ResolverModes : int
{
    NONE,
    FREESTANDING,
    BACKWARDS,
    EDGE,
    MOVE_STAND_DELTA,
    FORCE_LAST_MOVING_LBY,
    FORCE_FREESTANDING,
    FORCE_BACKWARDS,
    BRUTFORCE_ALL_DISABLED, //todo
    BRUTFORCE,
    FORCE_MOVE_STAND_DELTA,
    FORCE_LBY,
    MOVING,
    LBY_BREAK,
    SPINBOT,
    AIR_FREESTANDING,
    AIR_BRUTFORCE,
    FAKEWALK_FREESTANDING,
    FAKEWALK_BRUTFORCE,
    PREDICT_FREESTANDING,
    PREDICT_BRUTFORCE,
    MAX
};

enum class ResolverDetections : int
{
    NONE,
    FAKEWALKING,
    AIR,
    MOVING,
    STANDING
};

enum class BacktrackPriorities : int
{
    none,
    good_prediction,
    normal_prediction,
    lby_prediction,
    lby_update,
    moving
};
struct GlobalResolverDataStruct
{
    bool Resolved = false;
    bool ForceBaim = false;
    int Shots = 0;
    int ResolverState = 0;
    ResolverModes mode = ResolverModes::NONE;
    ResolverDetections detection = ResolverDetections::NONE;
    int FakeWalkShots = 0;
    int InFakelag = false;
    int Fake = 0;
    bool Moving = false;
    bool InAir = false;
	bool Desync = false;
    int ShotsAtMode[(int)ResolverModes::MAX];
    int OverallShotsAtMode[(int)ResolverModes::MAX];
    int OverallShots = 0;
    bool BreakingLC = false;
    bool CanuseLbyPrediction = false;
    float NextPredictedLbyBreak = 0.f;
};
class Resolver
{
public:
    void UpdateAnims();
    void OnFramestageNotify();
    void OnFireEvent(IGameEvent* event, int lastbt);
    void Draw();
    std::array<GlobalResolverDataStruct, 128> GResolverData;
private:
    std::array<SavedResolverData, 128> SavedResolverData;
    std::deque<float> LastYaws[128];
    void AddCurrentYaw(C_BasePlayer* pl, int i, float ang);
    bool GetAverageYaw(int i, float& ang);
};

extern Resolver g_Resolver;