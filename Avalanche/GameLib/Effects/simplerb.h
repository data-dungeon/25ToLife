/////////////////////////////////////////////////////////////////////////////
// Simple rigid body
/////////////////////////////////////////////////////////////////////////////
#ifndef SIMPLERB_H
#define SIMPLERB_H

#include "Math/Matrix.h"

// Some simple properties about the world
class WorldPhysicsMetrics
{
public:
	// Setup stuff
	static void Setup(Vector3 &gravity, float linearDampning, float angularDampning);

	// The quantities
	static Vector3	gravityVector;
	static float 	gravity;
	static Vector3	gravityDir;
	static float 	linearDampning;
	static float 	angularDampning;
};

// A physical state for a rigid body
class RigidBodyState
{
public:
	RigidBodyState(void) {}

	// Clear everything
	void Clear(void);

	// primary quantities
	Vector3		pos;
	Matrix3x3	orientation;

	Vector3		velocity;
	Vector3		angularMomentum;

	Vector3 		force;
	Vector3 		torque;

	// auxiliary quantities
	Matrix3x3	invWorldInertiaTensor;
	Vector3 		angularVelocity;

	// states
	bool			atRest;

	// Convert to/from a pos/orientation
	inline void ToGraphics4x4(Graphics4x4 &m);
	inline void FromGraphics4x4(const Graphics4x4 &m);
};

// A rigid body setup
class RigidBodyProps
{
public:
	RigidBodyProps(void) {}

	// Setup some defaults
	void Defaults(void);

	// Setup stuff
	void SetBody(Vector3CRef size, float density);
	void Set(float coeffOfRestitution, float groundImpulseRatio);

	// Call this after you setup stuff manually
	void Update(void);

	// Doh
	RigidBodyProps &operator=(const RigidBodyProps &p);

	// The stuff setup externally
	Vector3				halfSize;
	float					density;

	// The computed quantitiues
	float					mass, invMass;
	Matrix3x3			inertiaTensor;
	Matrix3x3			invInertiaTensor;
	float					radius;

	// Other quantities
	float					coeffOfRestitution;
	float					groundImpulseRatio;
};

// The rigid body
class RigidBody
{
public:
	RigidBody(void);

	// Initialize (takes initial position from destination)
	// updates destination every frame
	void Initialize(RigidBodyProps &properties, Graphics4x4 &destinationMatrix);

	// Place, if velocity is NULL it is considered "at rest"
	void Place(const Vector3 &pos, const DirCos3x3 &orientation, Vector3 *velocity = NULL);

	// Reset everything (loads from destination matrix)
	void Reset(bool atRest = true);

	// Run for a frame wihtout collisions
	void Run(float timeStep);

	// Run for a frame wiht terrain collisions
	void RunCollide(float timeStep);

	// Apply an impulse from external source
	void ResolveImpulse(Vector3CRef velocity, Vector3CRef point, Vector3CRef normal);

	// Peek at the orientation and pos (DO NOT CHANGE)
	Vector3CRef Position(void)
		{ return(state.pos); }
	const Matrix3x3 &Orientation(void)
		{ return(state.orientation); }

	// Is the body at rest?
	bool AtRest(void)
		{ return(state.atRest); }

	// What it the body's veloctity	
	Vector3CRef Velocity(void)
		{ return(state.velocity); }
	Vector3CRef AngularVelocity(void)
		{ return(state.angularVelocity); }

	// Displace
	inline void Displace(Vector3CRef moveBy)
		{ state.pos += moveBy; }

	// apply an impulse
	inline void ApplyImpulse(Vector3CRef velocity)
		{ state.velocity += velocity; state.atRest = false; }

	// Apply an angular impulse
	void ApplyAngularImpulse(Vector3CRef velocity);

	// Set the external force
	void SetExternalForce(Vector3CRef force)
		{ externalForce = force; state.atRest = false; }
	void SetExternalTorque(Vector3CRef torque)
		{ externalTorque = torque; state.atRest = false; }
	void AddExternalForce(Vector3CRef force)
		{ externalForce += force; state.atRest = false; }
	void AddExternalTorque(Vector3CRef torque)
		{ externalTorque += torque; state.atRest = false; }
	void ClearExternalForce(void)
		{ externalForce.Clear(); }
	void ClearExternalTorque(void)
		{ externalTorque.Clear(); }
	void ClearExternalForces(void)
		{ externalForce.Clear();  externalTorque.Clear(); }

private:
	// static quantities
	Graphics4x4			*destination;
	RigidBodyProps		*props;
	Vector3				externalForce;
	Vector3				externalTorque;

	// Compute forces on me
	void ComputeForces(void);

	// Integrate the forces on me
	void Integrate(float timeStep);

	// Get the velocity of a point on the body
	inline Vector3 PointVelocity(Vector3CRef p);

	// Apply an impulse from a collision with immovable object (terrain)
	void ResolveImpulseTerrain(Vector3CRef point, Vector3CRef normal);

	// Our state
	RigidBodyState		state;
};

// Inlines
#include "Effects/simplerb.hpp"

#endif
