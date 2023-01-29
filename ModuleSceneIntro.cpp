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

	//track
	CreateRamp(vec3(50, 1, 100), vec3(0, 21, 145), vec3(1, 0, 0), -25, 1, true, 0, Orange);
	CreatePlatform(vec3(150, 1, 50), vec3(0, 42, 215), 0, Orange);
	/*CreatePlatform(vec3(100, 1, 100), vec3(50, 0, 50), 0, Orange);
	CreatePlatform(vec3(100, 1, 100), vec3(50, 0, 50), 0, Orange);
	CreatePlatform(vec3(100, 1, 100), vec3(50, 0, 50), 0, Orange);*/

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
	/*Plane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();*/

	//render all cubes on cubeList
	for (p2List_item<Cube*>* cube = cubeList.getFirst(); cube; cube = cube->next)
	{
		cube->data->Render();
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}



// MAP STUFF =======================================================================

void ModuleSceneIntro::CreatePlatform(vec3 size, vec3 pos, int alpha, Color color)
{
	Cube* cube;

	cube = new Cube(size.x, size.y, size.z);
	cube->SetPos(pos.x, pos.y, pos.z);
	cube->SetRotation(alpha, { 0, 1, 0 });
	cube->color = color;

	cubeList.add(cube);
	App->physics->AddBody(*cube, 0);
	//App->physics->GetPhysBodyList().getLast()->data->body->setFriction(1000.0f);
}

void ModuleSceneIntro::CreateCorner(vec3 size, vec3 pos, int alpha, int alphaStep, int steps, bool right, Color color)
{
	Cube* cube;
	//lastPos = cube->GetPos(cube->transform);

	//diplacement
	float r = sqrtf(pow(size.x / 2, 2) + pow(size.z / 2, 2));

	vec3 disp;
	
	disp.x = r * cos(alpha);
	disp.z = r * sin(alpha);

	float newPosX = -disp.x;
	float newPosZ = -pos.z + disp.z;

	//15 alpha x 6 steps = 90 degrees
	for (int i = 0; i < steps; i++)
	{
		disp.x = r * cos(alpha);
		disp.z = r * sin(alpha);

		if (i > 0)
		{
			newPosX += -disp.x;
			newPosZ += -pos.z + disp.z;
		}
	
		cube = new Cube(size.x, size.y, size.z);
		cube->SetPos(newPosX, pos.y, newPosZ);
		cube->SetRotation(alpha, { 0, 1, 0 });

		switch (i)
		{
		case 0: cube->color = Red; break;
		case 1: cube->color = Orange; break;
		case 2: cube->color = Yellow; break;
		}
		
		cubeList.add(cube);
		App->physics->AddBody(*cube, 0);

		alpha += alphaStep;
	}
}

void ModuleSceneIntro::CreateRamp(vec3 size, vec3 pos, vec3 rot, int alpha, int steps, bool up, int slope, Color color)
{
	Cube* cube;

	for (int i = 0; i < steps; i++)
	{
		cube = new Cube(size.x, size.y, size.z);
		cube->SetPos(pos.x, pos.y, pos.z);
		cube->SetRotation(alpha, { rot.x, rot.y, rot.z });
		cube->color = color;

		cubeList.add(cube);
		App->physics->AddBody(*cube, 0);
	}
}