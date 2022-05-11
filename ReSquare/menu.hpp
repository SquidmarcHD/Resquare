#pragma once

#include <string>
#include "singleton.hpp"
#include "imgui/imgui.h"
#include <ctime>
struct IDirect3DDevice9;

class Menu
    : public Singleton<Menu>
{
public:
    float menurender = 1;
    clock_t lastTime = clock();
    bool              _visible;
    void Initialize();
    void Shutdown();

    void OnDeviceLost();
    void OnDeviceReset();

    void Dump();

    void Render();

    void Toggle();

    bool IsVisible() const { return _visible; }
private:

    ImGuiStyle        _style;
};