#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	//collisionTimer.Start();

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.num_chassis = 11;

	car.chassisList[0].size.Set(2, 0.7, 4);
	car.chassisList[1].size.Set(1.6, 0.3, 3.5);
	car.chassisList[2].size.Set(1.2, 0.5, 2);
	car.chassisList[3].size.Set(0.7, 0.2, 0.6);
	car.chassisList[4].size.Set(0.8, 0.2, 0.8);
	car.chassisList[5].size.Set(0.8, 0.2, 0.8);

	car.chassisList[0].offset.Set(0, 0.8, 0);
	car.chassisList[1].offset.Set(0, 1.3, -0.6);
	car.chassisList[2].offset.Set(0, 0.7, 3);
	car.chassisList[3].offset.Set(0, 0.7, 4.3);
	car.chassisList[4].offset.Set(1.4, 0.55, 1.2);
	car.chassisList[5].offset.Set(-1.4, 0.55, 1.2);

	car.mass = 220.0f;
	car.suspensionStiffness = 20.0f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 20;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.6f;
	float wheel_radius = 0.8f;
	float wheel_width = 0.8f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------ I'll fkin' do it again :)

	float half_width = car.chassisList[0].size.x * 0.5f;
	float half_length = car.chassisList[0].size.z *0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width + wheel_width, connection_height, half_length * 2 - 0.2);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width - wheel_width, connection_height, half_length * 2 - 0.2);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width + wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width - wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;


	// LISTENER --------------------------
	vehicle = App->physics->AddVehicle(car);
	vehicle->collision_listeners.add(this);
	vehicle->body->setUserPointer(vehicle);
	vehicle->SetPos(0, 1, 0);
	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;

	// NITRO
	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		acceleration = App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT ? MAX_ACCELERATION * 2 : MAX_ACCELERATION;
		nitro = true;
	}
	else
	{
		nitro = false;
	}

	// JUMP
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && !airborne)
	{
		//jumpTime->Start();
		//isJumped = true;
		vehicle->vehicle->getRigidBody()->applyCentralForce({ 0,69420,0 });
	}

	// TURN
	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if(turn < TURN_DEGREES)
			turn +=  TURN_DEGREES;
	}

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if(turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
	}

	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	vehicle->Render();

	char title[96];
	sprintf_s(title, " Awesome Epic CarGame    ||    airborne: %s   |   nitro: %s   |   %6.1f Km/h   |  ", airborne ? "true" : "false", airborne ? "on" : "off", vehicle->GetKmh());

	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}

void ModulePlayer::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	switch (body2->ctype)
	{
	case ColliderType::GROUND:
		LOG("Collision GROUND");
		airborne = false;
		break;
	}
}