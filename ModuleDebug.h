#pragma once

#include "Module.h"
#include "Globals.h"
#include <chrono>

#define DEBUG_BOX 225

using namespace std::chrono;

class ModuleDebug : public Module
{
public:

	ModuleDebug(Application* app, bool start_enabled = true);
	virtual ~ModuleDebug();

	bool Start();
	update_status Update();	// Switch debug on/off
	update_status PostUpdate();		// Draw if debug true

	void DrawDebug();
	void DrawPhysics();

public:
	//flags
	bool drawDebug = false;
	bool drawPhysics = false;
	bool variables = false;
	bool fpsCap = false;
	bool changePlayer = false;
	bool musicON = false;
	bool sfxON = true;
	bool hideUI = false;
	bool isGrenade = false;
	bool noGravity = false;

	microseconds timePerCycle;
	microseconds elapsedFrame;
	int desiredFPSmic;
	float desiredFPS = 60.0f;
	int saveGravity;

};