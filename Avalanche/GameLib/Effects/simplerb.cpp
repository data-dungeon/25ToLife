/////////////////////////////////////////////////////////////////////////////
// Exceptionally simple self-contained rigid body simulation
/////////////////////////////////////////////////////////////////////////////
#include "Effects/EffectsPCH.h"

// World physics quantities
Vector3 WorldPhysicsMetrics::gravityVector( 0.0f, -9.8f, 0.0f );
float WorldPhysicsMetrics::gravity = 9.8f;
Vector3 WorldPhysicsMetrics::gravityDir( 0.0f, -1.0f, 0.0f );
float WorldPhysicsMetrics::linearDampning = 0.0f; //-0.01f;
float WorldPhysicsMetrics::angularDampning = 0.00f;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void RigidBodyState::Clear(void)
{
	velocity.Clear();
	angularMomentum.Clear();
	force.Clear();
	torque.Clear();

	invWorldInertiaTensor.Identity();
	angularVelocity.Clear();

	atRest = false;
}

/////////////////////////////////////////////////////////////////////////////
// Setup stuff
/////////////////////////////////////////////////////////////////////////////
void RigidBodyProps::SetBody(
Vector3CRef size,
float density)
{
	halfSize = size * 0.5f;
	this->density = density;
	Update();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void RigidBodyProps::Set(
float coeffOfRestitution,
float groundImpulseRatio)
{
	this->coeffOfRestitution = coeffOfRestitution;
	this->groundImpulseRatio = groundImpulseRatio;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void RigidBodyProps::Defaults(void)
{
	// setup all the defaults
	halfSize.Set(0.5f, 0.5f, 0.5f);
	density = 1.0f;
	coeffOfRestitution = 0.3f;
	groundImpulseRatio = 0.2f;
	Update();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void RigidBodyProps::Update(void)
{
	// Sanity check on halfsize
	if (Math::Zero(halfSize.x()))
		halfSize.x( 0.01f );
	if (Math::Zero(halfSize.y()))
		halfSize.y( 0.01f );
	if (Math::Zero(halfSize.z()))
		halfSize.z( 0.01f );

	// compute raidus
	radius = halfSize.x();
	if (halfSize.y() > radius)
		radius = halfSize.y();
	if (halfSize.z() > radius)
		radius = halfSize.z();

	// Assume a box, of 2 * halfSize size
	mass = 8.0f * density * halfSize.x() * halfSize.y() * halfSize.z();

	// Save this for later
	invMass = 1.0f / mass;

	// Figure out the non-inverted tensor
	Vector3 tensor;
	tensor.Set((halfSize.y() * halfSize.y() + halfSize.z() * halfSize.z()),
					(halfSize.x() * halfSize.x() + halfSize.z() * halfSize.z()),
					(halfSize.x() * halfSize.x() + halfSize.y() * halfSize.y()));
	inertiaTensor.Diagonalize(tensor * (mass / 3.0f));

	// Figure out our inverted inertal tensor
	Vector3 invTensor;
	invTensor.Set(1.0f / tensor.x(), 1.0f / tensor.y(), 1.0f / tensor.z());
	invInertiaTensor.Diagonalize(invTensor * (3.0f * invMass));
}

/////////////////////////////////////////////////////////////////////////////
// Doh
/////////////////////////////////////////////////////////////////////////////
RigidBodyProps &RigidBodyProps::operator=(const RigidBodyProps &p)
{ 
	halfSize = p.halfSize; 
	density = p.density;		

	mass = p.mass;
	invMass = p.invMass;
	inertiaTensor = p.inertiaTensor;
	invInertiaTensor = p.inertiaTensor;
	radius = p.radius;

	coeffOfRestitution = p.coeffOfRestitution;
	groundImpulseRatio = p.groundImpulseRatio;
	
	return *this; 
}	

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
RigidBody::RigidBody(void)
{
	destination = NULL;
	props = NULL;
	state.Clear();
	ClearExternalForces();
}

/////////////////////////////////////////////////////////////////////////////
// Setup this bad boy
/////////////////////////////////////////////////////////////////////////////
void RigidBody::Initialize(
RigidBodyProps &properties,
Graphics4x4 &destinationMatrix)
{
	// Save our properties
	props = &properties;

	// Who are we writing to?
	destination = &destinationMatrix;

	// Initialize the state vector
	state.FromGraphics4x4(*destination);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void RigidBody::Place(
const Vector3 &pos,
const DirCos3x3 &orientation,
Vector3 *velocity)
{
	// Reset
	state.Clear();
	state.orientation = orientation;
	state.pos = pos;
	if (!velocity)
		state.atRest = true;
	else
	{
		state.atRest = false;
		state.velocity = *velocity;
	}
}

/////////////////////////////////////////////////////////////////////////////
// Reset
/////////////////////////////////////////////////////////////////////////////
void RigidBody::Reset(
bool atRest)
{
	// reset the state
	state.Clear();
	state.atRest = atRest;

	// Clear the external forces
	ClearExternalForces();

	// set the position
	if (destination)
		state.FromGraphics4x4(*destination);
}

/////////////////////////////////////////////////////////////////////////////
// Run for one display frame
/////////////////////////////////////////////////////////////////////////////
void RigidBody::Run(
float timeStep)
{
	// If we are at rest, get out!
	if (state.atRest)
		return;

	// Compute the forces
	ComputeForces();

	// Do the integration
	Integrate(timeStep);

	// Update the destination
	state.ToGraphics4x4(*destination);
}

/////////////////////////////////////////////////////////////////////////////
// Run for one display frame
/////////////////////////////////////////////////////////////////////////////
void RigidBody::RunCollide(
float timeStep)
{
	// If we are at rest, get out!
	if (state.atRest)
		return;

	// start point
	CLineEntity entity;
	entity.SetPositiveInclude(WORLD_CATEGORY);
	entity.Line().SetFindMode(CLineCollisionObject::FIND_CLOSEST);
	entity.Line().Origin() = state.pos;

	// Compute the forces
	ComputeForces();

	// Do the integration
	Integrate(timeStep);

	// Test for a ground collision
	entity.Line().Basis() = state.pos - entity.Line().Origin();
	Vector3 intersection, normal;
	if ((g_collisionSystem.Test(&entity) != NULL) &&
			CLineQuery::Contact(entity.Line(), intersection, normal))
	{
		state.pos = intersection;
		state.pos.y(state.pos.y() + props->radius);
		ResolveImpulseTerrain(intersection, normal);
	}

	// Update the destination
	state.ToGraphics4x4(*destination);
}

/////////////////////////////////////////////////////////////////////////////
// Compute forces on me
/////////////////////////////////////////////////////////////////////////////
void RigidBody::ComputeForces(void)
{
	// Apply externals
	state.force = externalForce;
	state.torque = externalTorque;

	// Damping
	state.force += WorldPhysicsMetrics::linearDampning * state.velocity;
	state.torque += WorldPhysicsMetrics::angularDampning * state.angularVelocity;

	// Gravity
	state.force += WorldPhysicsMetrics::gravityVector * props->mass;
}

/////////////////////////////////////////////////////////////////////////////
// Integrate the forces on me
/////////////////////////////////////////////////////////////////////////////
void RigidBody::Integrate(
float timeStep)
{
	// integrate primary quantities
	state.pos += state.velocity * timeStep;

	Matrix3x3 skewAV;
	skewAV.SkewSymmetric(state.angularVelocity);
	state.orientation += (state.orientation * skewAV) * timeStep;

	state.velocity += state.force * (props->invMass * timeStep);

	state.angularMomentum += state.torque * timeStep;

	// Make sure this looks good
	((DirCos3x3 &)state.orientation).Orthonormalize();

	// Need a transposed orientation
	Matrix3x3 transO;
	state.orientation.Transpose(transO);

	// compute auxiliary quantities
	state.invWorldInertiaTensor = transO * props->invInertiaTensor * state.orientation;

	state.angularVelocity = state.angularMomentum * state.invWorldInertiaTensor;
}

/////////////////////////////////////////////////////////////////////////////
// Apply an angular impulse
/////////////////////////////////////////////////////////////////////////////
void RigidBody::ApplyAngularImpulse(
Vector3CRef velocity)
{
	// Need a transposed orientation
	Matrix3x3 transO;
	state.orientation.Transpose(transO);

	// compute auxiliary quantities
	Matrix3x3 worldInertiaTensor = transO * props->inertiaTensor * state.orientation;

	state.angularMomentum += velocity * worldInertiaTensor;
	state.atRest = false;
}

/////////////////////////////////////////////////////////////////////////////
// Apply an impulse from a collision with immovable object
/////////////////////////////////////////////////////////////////////////////
void RigidBody::ResolveImpulse(
Vector3CRef velocity,
Vector3CRef point,
Vector3CRef normal)
{
	// Reset these
	state.atRest = false;

	// Figure out all the velocities
	Vector3 padot = PointVelocity(point);
	Vector3 pbdot = velocity;
	Vector3 ra = point - state.pos;
	float vrel = Vector3::Dot(normal, padot - pbdot);

	// Impulse numerator
	float impulseNumerator = -(1.0f + props->coeffOfRestitution) * vrel;

	// Denominator is in 4 parts
	float term1 = props->invMass;
	Vector3 aTensorRCN = Vector3::Cross(ra, normal) * state.invWorldInertiaTensor;
	float term3 = Vector3::Dot(normal, Vector3::Cross(aTensorRCN, ra));

	// Impulse magnitude
	float j = impulseNumerator / (term1 + term3);
	Vector3 impulse = normal * j;

	// apply impulse to primary quantities
	state.velocity += impulse * props->invMass;

	state.angularMomentum += Vector3::Cross(ra, impulse);

	// compute affected auxiliary quantities
	state.angularVelocity = state.angularMomentum * state.invWorldInertiaTensor;
}

/////////////////////////////////////////////////////////////////////////////
// Apply an impulse from a collision with immovable object (terrain)
/////////////////////////////////////////////////////////////////////////////
void RigidBody::ResolveImpulseTerrain(
Vector3CRef point,
Vector3CRef normal)
{
	// Figure out all the velocities
	Vector3 padot = PointVelocity(point);
	Vector3 ra = point - state.pos;
	float vrel = Vector3::Dot(normal, padot);

	// Impulse numerator
	float impulseNumerator = -(1.0f + props->coeffOfRestitution) * vrel;

	// Denominator is in 4 parts
	float term1 = props->invMass;
	Vector3 aTensorRCN = Vector3::Cross(ra, normal) * state.invWorldInertiaTensor;
	float term3 = Vector3::Dot(normal, Vector3::Cross(aTensorRCN, ra));

	// Impulse magnitude
	float j = impulseNumerator / (term1 + term3);

#if 1
	// This is the usual impulse calculation (frictionless)
	Vector3 impulse = normal * j;
#else
	// This makes things get sideways forces when they hit the ground
	Vector3 rdir = ra.Normalized() * -1.0f;
	Vector3 oddNormal = normal + (rdir - normal) * props->groundImpulseRatio;
	Vector3 impulse = oddNormal * j;
#endif

	// apply impulse to primary quantities
	state.velocity += impulse * props->invMass;
	state.angularMomentum += Vector3::Cross(ra, impulse);

	// compute affected auxiliary quantities
	state.angularVelocity = state.angularMomentum * state.invWorldInertiaTensor;
}


