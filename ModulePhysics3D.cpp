#include "Globals.h"
#include "Application.h"
#include "ModulePhysics3D.h"
#include "PhysBody3D.h"
#include "PhysVehicle3D.h"
#include "Primitive.h"

#ifdef _DEBUG
	#pragma comment (lib, "Bullet/libx86/BulletDynamics_debug.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision_debug.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath_debug.lib")
#else
	#pragma comment (lib, "Bullet/libx86/BulletDynamics.lib")
	#pragma comment (lib, "Bullet/libx86/BulletCollision.lib")
	#pragma comment (lib, "Bullet/libx86/LinearMath.lib")
#endif

ModulePhysics3D::ModulePhysics3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;

	collision_conf = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_conf);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	debug_draw = new DebugDrawer();
}

// Destructor
ModulePhysics3D::~ModulePhysics3D()
{
	delete debug_draw;
	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_conf;
}

// Render not available yet----------------------------------
bool ModulePhysics3D::Init()
{
	LOG("Creating 3D Physics simulation");
	bool ret = true;

	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics3D::Start()
{
	LOG("Creating Physics environment");

	//world
	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_conf);
	world->setDebugDrawer(debug_draw);
	world->setGravity(GRAVITY);
	vehicle_raycaster = new btDefaultVehicleRaycaster(world);

	//var
	gravity = GRAVITY;
	//clayDragForce = 300;
	coeficientDragClay = 30;
	dragOn = false;
	//aeroLiftForce = 300;
	coeficientLiftAero = 50;
	liftOn = false;

	goalcount = 0;
	goal = false;

	bodyMass = 220.0f;

	freeCamera = false;


	// Big plane as ground - MUST DELETE
	/*{
		btCollisionShape* colShape = new btStaticPlaneShape(btVector3(0, 1, 0), 0);

		btDefaultMotionState* myMotionState = new btDefaultMotionState();
		btRigidBody::btRigidBodyConstructionInfo rbInfo(0.0f, myMotionState, colShape);

		btRigidBody* body = new btRigidBody(rbInfo);
		world->addRigidBody(body);
	}*/

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PreUpdate(float dt)
{
	world->stepSimulation(dt, 15);

	int numManifolds = world->getDispatcher()->getNumManifolds();
	for(int i = 0; i<numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();
		if(numContacts > 0)
		{
			PhysBody3D* pbodyA = (PhysBody3D*)obA->getUserPointer();
			PhysBody3D* pbodyB = (PhysBody3D*)obB->getUserPointer();

			if(pbodyA && pbodyB)
			{
				p2List_item<Module*>* item = pbodyA->collision_listeners.getFirst();
				while(item)
				{
					item->data->OnCollision(pbodyA, pbodyB);
					item = item->next;
				}

				item = pbodyB->collision_listeners.getFirst();
				while(item)
				{
					item->data->OnCollision(pbodyB, pbodyA);
					item = item->next;
				}
			}
		}
	}

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::Update(float dt)
{
	// DEBUG ON/OFF
	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
	{
		debug = !debug;

		if (!debug)
		{
			//Reset stuff
			world->setGravity(GRAVITY);
			gravity = GRAVITY;

			p2List_item<PhysVehicle3D*>* vehicle = vehicles.getFirst();
			vehicle->data->body->setMassProps(bodyMass = 220, vehicle->data->body->getLocalInertia());
		}
	}

	if(debug == true)
	{
		//Draw stuff
		if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
			drawWorld = !drawWorld;
		if (drawWorld)
		{
			world->debugDrawWorld();

			// Render vehicles
			p2List_item<PhysVehicle3D*>* item = vehicles.getFirst();
			while (item)
			{
				item->data->Render();
				item = item->next;
			}
		}		

		//Camera		
		if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
			freeCamera = !freeCamera;

		//Gravity
		if (App->input->GetKey(SDL_SCANCODE_Y) == KEY_REPEAT) { gravity.setY(gravity.getY() - 0.1); }			
		if (App->input->GetKey(SDL_SCANCODE_U) == KEY_REPEAT) { gravity.setY(gravity.getY() + 0.1); }
		if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN) { gravityON = !gravityON; }

		int gON = gravityON ? 1 : 0;
		world->setGravity(btVector3(0, gravity.getY() * gON, 0));

		//Mass - TODO
		p2List_item<PhysVehicle3D*>* vehicle = vehicles.getFirst();

		if (App->input->GetKey(SDL_SCANCODE_H) == KEY_REPEAT) 
		{ 
			bodyMass--;
			if (bodyMass == 0)
				bodyMass = 1;
			vehicle->data->body->setMassProps(bodyMass, vehicle->data->body->getLocalInertia());
		}
		if (App->input->GetKey(SDL_SCANCODE_J) == KEY_REPEAT) 
		{ 
			bodyMass++;
			vehicle->data->body->setMassProps(bodyMass, vehicle->data->body->getLocalInertia());
		}
		if (App->input->GetKey(SDL_SCANCODE_K) == KEY_DOWN) { vehicle->data->body->setMassProps(bodyMass = 1,vehicle->data->body->getLocalInertia()); }

		//Forces

		//Drag Force Clay
		if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN) //function for debug
		{
			dragOn = !dragOn;
			LOG("Activated DRAGGGGGGGGGGGGG");
		}
		if (dragOn)
		{
			if (vehicle->data->body->getLinearVelocity().getZ() > 0)
			{
				clayDragForce = -coeficientDragClay * vehicle->data->body->getLinearVelocity().getZ();
				vehicle->data->body->applyForce(btVector3(0, 0, clayDragForce), btVector3(0, 0, 0));
			}
			if (vehicle->data->body->getLinearVelocity().getZ() < 0)
			{
				clayDragForce = -coeficientDragClay * vehicle->data->body->getLinearVelocity().getZ();
				vehicle->data->body->applyForce(btVector3(0, 0, clayDragForce), btVector3(0, 0, 0));
			}
			if (vehicle->data->body->getLinearVelocity().getX() > 0)
			{
				clayDragForce = -coeficientDragClay * vehicle->data->body->getLinearVelocity().getX();
				vehicle->data->body->applyForce(btVector3(clayDragForce, 0, 0), btVector3(0, 0, 0));
			}
			if (vehicle->data->body->getLinearVelocity().getX() < 0)
			{
				clayDragForce = -coeficientDragClay * vehicle->data->body->getLinearVelocity().getX();
				vehicle->data->body->applyForce(btVector3(clayDragForce, 0, 0), btVector3(0, 0, 0));
			}
		}

		//Aerodynamic drag force
		if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN) //function for debug
		{
			liftOn = !liftOn;
			LOG("Activated AERODYNAMICCCCCCCC");
		}
		if (liftOn)
		{
			if (vehicle->data->body->getLinearVelocity().getZ() > 10)
			{
				aeroLiftForce = -coeficientLiftAero * vehicle->data->body->getLinearVelocity().getZ();
				vehicle->data->body->applyForce(btVector3(0, aeroLiftForce, 0), btVector3(0, 0, 0));
			}
			if (vehicle->data->body->getLinearVelocity().getZ() < -10)
			{
				aeroLiftForce = coeficientLiftAero * vehicle->data->body->getLinearVelocity().getZ();
				vehicle->data->body->applyForce(btVector3(0, aeroLiftForce, 0), btVector3(0, 0, 0));
			}
			if (vehicle->data->body->getLinearVelocity().getX() > 10)
			{
				aeroLiftForce = -coeficientLiftAero * vehicle->data->body->getLinearVelocity().getX();
				vehicle->data->body->applyForce(btVector3(0, aeroLiftForce, 0), btVector3(0, 0, 0));
			}
			if (vehicle->data->body->getLinearVelocity().getX() < -10)
			{
				aeroLiftForce = coeficientLiftAero * vehicle->data->body->getLinearVelocity().getX();
				vehicle->data->body->applyForce(btVector3(0, aeroLiftForce, 0), btVector3(0, 0, 0));
			}
		}

		btVector3 getP = vehicle->data->body->getWorldTransform().getOrigin();
		
		vec3 pos;
		pos.x = getP.x();
		pos.y = getP.y();
		pos.z = getP.z();

		LOG("Z = %d", pos.z);
		LOG("X = %d", pos.x);
		LOG("Y = %d", pos.y);


		if (vehicle->data->body->getCenterOfMassPosition().getZ() > 150 && vehicle->data->body->getCenterOfMassPosition().getZ() < 350 &&
			vehicle->data->body->getCenterOfMassPosition().getX() > 50  && vehicle->data->body->getCenterOfMassPosition().getX() < 100)
		{
			goal = true;
		}

		if (goalcount == 3)
			return UPDATE_STOP;

		//nice balls
		if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) 
		{
			Sphere s(1);
			s.SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);
			float force = 30.0f;
			AddBody(s)->Push(-(App->camera->Z.x * force), -(App->camera->Z.y * force), -(App->camera->Z.z * force));
		}
	}

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics3D::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics3D::CleanUp()
{
	LOG("Destroying 3D Physics simulation");

	// Remove from the world all collision bodies
	for(int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	for(p2List_item<btTypedConstraint*>* item = constraints.getFirst(); item; item = item->next)
	{
		world->removeConstraint(item->data);
		delete item->data;
	}
	
	constraints.clear();

	for(p2List_item<btDefaultMotionState*>* item = motions.getFirst(); item; item = item->next)
		delete item->data;

	motions.clear();

	for(p2List_item<btCollisionShape*>* item = shapes.getFirst(); item; item = item->next)
		delete item->data;

	shapes.clear();

	for(p2List_item<PhysBody3D*>* item = bodies.getFirst(); item; item = item->next)
		delete item->data;

	bodies.clear();

	for(p2List_item<PhysVehicle3D*>* item = vehicles.getFirst(); item; item = item->next)
		delete item->data;

	vehicles.clear();

	delete vehicle_raycaster;
	delete world;

	return true;
}

// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const Sphere& sphere, float mass)
{
	btCollisionShape* colShape = new btSphereShape(sphere.radius);
	shapes.add(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&sphere.transform);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.add(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.add(pbody);

	return pbody;
}


// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const Cube& cube, float mass, bool sensor)
{
	btCollisionShape* colShape = new btBoxShape(btVector3(cube.size.x*0.5f, cube.size.y*0.5f, cube.size.z*0.5f));
	shapes.add(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cube.transform);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.add(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	if (sensor)
	{
		pbody->SetAsSensor(true);
	}

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.add(pbody);

	return pbody;
}

// ---------------------------------------------------------
PhysBody3D* ModulePhysics3D::AddBody(const Cylinder& cylinder, float mass)
{
	btCollisionShape* colShape = new btCylinderShapeX(btVector3(cylinder.height*0.5f, cylinder.radius, 0.0f));
	shapes.add(colShape);

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(&cylinder.transform);

	btVector3 localInertia(0, 0, 0);
	if(mass != 0.f)
		colShape->calculateLocalInertia(mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	motions.add(myMotionState);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	PhysBody3D* pbody = new PhysBody3D(body);

	body->setUserPointer(pbody);
	world->addRigidBody(body);
	bodies.add(pbody);

	return pbody;
}

// ---------------------------------------------------------
PhysVehicle3D* ModulePhysics3D::AddVehicle(const VehicleInfo& info)
{
	btCompoundShape* comShape = new btCompoundShape();
	shapes.add(comShape);

	btCollisionShape* colShape = new btBoxShape(btVector3(info.chassisList[0].size.x*0.5f, info.chassisList[0].size.y*0.5f, info.chassisList[0].size.z*0.5f));
	shapes.add(colShape);

	btTransform trans;
	trans.setIdentity();
	trans.setOrigin(btVector3(info.chassisList[0].offset.x, info.chassisList[0].offset.y, info.chassisList[0].offset.z));

	comShape->addChildShape(trans, colShape);

	btTransform startTransform;
	startTransform.setIdentity();

	btVector3 localInertia(0, 0, 0);
	comShape->calculateLocalInertia(info.mass, localInertia);

	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(info.mass, myMotionState, comShape, localInertia);

	btRigidBody* body = new btRigidBody(rbInfo);
	body->setContactProcessingThreshold(BT_LARGE_FLOAT);
	body->setActivationState(DISABLE_DEACTIVATION);

	world->addRigidBody(body);

	btRaycastVehicle::btVehicleTuning tuning;
	tuning.m_frictionSlip = info.frictionSlip;
	tuning.m_maxSuspensionForce = info.maxSuspensionForce;
	tuning.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
	tuning.m_suspensionCompression = info.suspensionCompression;
	tuning.m_suspensionDamping = info.suspensionDamping;
	tuning.m_suspensionStiffness = info.suspensionStiffness;

	btRaycastVehicle* vehicle = new btRaycastVehicle(tuning, body, vehicle_raycaster);

	vehicle->setCoordinateSystem(0, 1, 2);

	for(int i = 0; i < info.num_wheels; ++i)
	{
		btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
		btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z);
		btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);

		vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
	}
	// ---------------------

	PhysVehicle3D* pvehicle = new PhysVehicle3D(body, vehicle, info);
	world->addVehicle(vehicle);
	vehicles.add(pvehicle);

	return pvehicle;
}

// ---------------------------------------------------------
void ModulePhysics3D::AddConstraintP2P(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB)
{
	btTypedConstraint* p2p = new btPoint2PointConstraint(
		*(bodyA.body), 
		*(bodyB.body), 
		btVector3(anchorA.x, anchorA.y, anchorA.z), 
		btVector3(anchorB.x, anchorB.y, anchorB.z));
	world->addConstraint(p2p);
	constraints.add(p2p);
	p2p->setDbgDrawSize(2.0f);
}

void ModulePhysics3D::AddConstraintHinge(PhysBody3D& bodyA, PhysBody3D& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisA, const vec3& axisB, bool disable_collision)
{
	btHingeConstraint* hinge = new btHingeConstraint(
		*(bodyA.body), 
		*(bodyB.body), 
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z),
		btVector3(axisA.x, axisA.y, axisA.z), 
		btVector3(axisB.x, axisB.y, axisB.z));

	world->addConstraint(hinge, disable_collision);
	constraints.add(hinge);
	hinge->setDbgDrawSize(2.0f);
}

// =============================================
void DebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	line.origin.Set(from.getX(), from.getY(), from.getZ());
	line.destination.Set(to.getX(), to.getY(), to.getZ());
	line.color.Set(color.getX(), color.getY(), color.getZ());
	line.Render();
}

void DebugDrawer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	point.transform.translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	point.color.Set(color.getX(), color.getY(), color.getZ());
	point.Render();
}

void DebugDrawer::reportErrorWarning(const char* warningString)
{
	LOG("Bullet warning: %s", warningString);
}

void DebugDrawer::draw3dText(const btVector3& location, const char* textString)
{
	LOG("Bullet draw text: %s", textString);
}

void DebugDrawer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes) debugMode;
}

int	 DebugDrawer::getDebugMode() const
{
	return mode;
}
