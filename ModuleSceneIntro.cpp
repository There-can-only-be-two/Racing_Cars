#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	// CREATE MAP
	//Test zone
	CreatePlatform(vec3(100, 1, 100), vec3(50, 0, 50), 0, Red);
	CreatePlatform(vec3(100, 1, 100), vec3(50, 0, -50), 0, Magenta);
	CreatePlatform(vec3(100, 1, 100), vec3(-50, 0, 50), 0, Green);
	CreatePlatform(vec3(100, 1, 100), vec3(-50, 0, -50), 0, Yellow);
	//CreateCorner(vec3(15, 1, 30), vec3(0, 0, 0), 0, 15, 3, true, Red);

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}