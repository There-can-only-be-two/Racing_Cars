#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;

#define MAX_ACCELERATION 1000.0f
#define TURN_DEGREES 25.0f * DEGTORAD
#define BRAKE_POWER 500.0f
#define BRAKE_POWER_LOW 20.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

public:
	Timer collisionTimer;

	Cube cSensor;
	PhysBody3D* pBodySensor;

	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;

	//flags
	bool airborne;
};