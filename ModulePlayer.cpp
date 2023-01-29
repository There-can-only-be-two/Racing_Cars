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
	car.num_chassis = 6;

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

		// SENSORS ---------------------------
	VehicleInfo sensor;

	sensor.sensorList[0].size.Set(0.75f, 0.5f, 0.75f);
	sensor.sensorList[0].offset.Set(0, 0, 0);
	sensor.mass = 0.001f;
	sensor.num_wheels = 0;

	vehicleSensor = App->physics->AddVehicle(sensor);
	vehicleSensor->color = White;
	vehicleSensor->body->setGravity({ 0,0,0 });
	//vehicleSensor->collision_listeners.add(this);

	vehicleSensor->SetAsSensor(true);
	vehicleSensor->body->setUserPointer(vehicleSensor);
	vehicleSensor->body->setCollisionFlags(vehicleSensor->body->getCollisionFlags() | btCollisionObject::CO_GHOST_OBJECT);


	//cubeVehicleSensor.SetPos(0, 10, 0);
	//cubeVehicleSensor.size = { 0.25,0.25,0.25 };
	//cubeVehicleSensor.color = White;
	//bodySensor = App->physics->AddBody(cubeVehicleSensor, 0);

	////App->physics->world->addCollisionObject(bodySensor);

	//bodySensor->collision_listeners.add(this);
	////bodySensor->body->setUserPointer(bodySensor);
	//bodySensor->SetAsSensor(true);
	//bodySensor->body->setCollisionFlags(bodySensor->body->getCollisionFlags() | btCollisionObject::CO_GHOST_OBJECT);

	//bodySensor->SetPos(0, 10, 0);

	
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

	// TP TO DEATH
	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN)
	{
		vehicle->SetPos(0, -90, 0);
	}

	// DEATH
	if (vehicle->body->getCenterOfMassPosition().getY() < -100)
	{
		const float matrixRot[16] = { 0,0,0,0,
									  0,0,0,0,
									  0,0,1,0,
									  0,0,0,1 };
		vehicle->body->setLinearVelocity(btVector3(0, 0, 0));
		vehicle->body->setAngularVelocity(btVector3(0, 0, 0));
		vehicle->SetTransform(matrixRot);
		vehicle->SetPos(0, 1, 0);
	}

	//SNESOR
#pragma region SENSOR_POS

	/*btQuaternion q = vehicle->vehicle->getChassisWorldTransform().getRotation();

	cubeVehicleSensor.SetPos(positionCM.getX(), positionCM.getY() - 0.55, positionCM.getZ());
	vehicle->vehicle->getChassisWorldTransform().getOpenGLMatrix(&cubeVehicleSensor.transform);
	btVector3 offset(0, -0.55, 0);
	offset = offset.rotate(q.getAxis(), q.getAngle());

	cubeVehicleSensor.transform.M[12] += offset.getX();
	cubeVehicleSensor.transform.M[13] += offset.getY();
	cubeVehicleSensor.transform.M[14] += offset.getZ();
	float* pos = cubeVehicleSensor.transform.M;
	bodySensor->SetTransform(pos);
	sensorV->SetTransform(pos);	*/

#pragma endregion SENSOR_POS


	// FORWARD
	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		// NITRO
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) 
		{
			acceleration = MAX_ACCELERATION * 2;
			nitro = true;
		}
		else
		{
			acceleration = MAX_ACCELERATION;
			nitro = false;
		}	
	}	
	// BACKWARD
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
	}

	// LEFT
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (turn < TURN_DEGREES)
			turn += TURN_DEGREES;
	}
	// RIGHT
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}

	// JUMP
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && airborne)
	{
		//jumpTime->Start();
		//isJumped = true;
		vehicle->vehicle->getRigidBody()->applyCentralForce({ 0,69420,0 });
	}


	vehicle->ApplyEngineForce(acceleration);
	vehicle->Turn(turn);
	vehicle->Brake(brake);

	vehicle->Render();

	char title[128];
	sprintf_s(title, " Awesome Epic CarGame    ||    airborne: %s  |  nitro: %s  |  %6.1f Km/h  |  gravity (%s): %5.1f m/s^2  ",
		airborne ? "true" : "false", nitro ? "on" : "off", vehicle->GetKmh(), App->physics->gravityON ? "on" : "off" , App->physics->gravity.getY());

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