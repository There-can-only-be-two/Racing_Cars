#pragma once

#include "Module.h"
#include "p2List.h"
#include "Globals.h"
#include "Timer.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleFonts.h"
#include "ModuleAudio.h"
#include "ModulePhysics3D.h"
#include "ModuleSceneIntro.h"
#include "ModulePlayer.h"
#include "ModuleDebug.h"
#include "ModuleRenderer3D.h"
#include "ModuleRenderer2D.h"


class Application
{
public:
	ModuleWindow* window;
	ModuleCamera3D* camera;
	ModuleInput* input;
	ModuleTextures* textures;
	ModuleFonts* fonts;
	ModuleAudio* audio;
	ModulePhysics3D* physics;

	ModuleSceneIntro* scene_intro;
	ModulePlayer* player;

	ModuleDebug* debug;

	ModuleRenderer3D* renderer3D;
	ModuleRenderer2D* renderer2D;


private:

	Timer	ms_timer;
	float	dt;
	p2List<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};