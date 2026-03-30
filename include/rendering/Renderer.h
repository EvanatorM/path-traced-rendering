#pragma once

class Renderer
{
public:
    static void Init();
    static void PostWindowInit();
    static void Shutdown();

    static void SetVsync(bool state);

    static double GetTime();
};