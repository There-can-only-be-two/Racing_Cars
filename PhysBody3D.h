#ifndef __PhysBody3D_H__
#define __PhysBody3D_H__

#include "p2List.h"

class btRigidBody;
class Module;

// =================================================
enum class ColliderType
{
	GROUND,
	CHECKPOINT,

	//SENSORS
	SENSOR_DEATH,

	UNKNOWN
};
struct PhysBody3D
{
	friend class ModulePhysics3D;
public:
	PhysBody3D(btRigidBody* body);
	~PhysBody3D();

	void Push(float x, float y, float z);
	void GetTransform(float* matrix) const;
	void SetTransform(const float* matrix) const;
	void SetPos(float x, float y, float z);
	void SetAsSensor(bool pIsSensor);
	bool GetIsSensor() { return isSensor; }

private:
	bool isSensor;

public:
	btRigidBody* body = nullptr;
	ColliderType ctype;
	p2List<Module*> collision_listeners;
};

#endif // __PhysBody3D_H__