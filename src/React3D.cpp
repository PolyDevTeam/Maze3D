#include "React3D.hpp"

using namespace reactphysics3d;

int test_REACT(void) {
	// Gravity vector 
	rp3d::Vector3 gravity(0.0, -9.81, 0.0);

	// Create the dynamics world 
	rp3d::DynamicsWorld world(gravity);

	// Change the number of iterations of the velocity solver 
	world.setNbIterationsVelocitySolver(15);

	// Change the number of iterations of the position solver 
	world.setNbIterationsPositionSolver(8);

	// Initial position and orientation of the rigid body 
	rp3d::Vector3 initPosition(0.0, 3.0, 0.0);
	rp3d::Quaternion initOrientation = rp3d::Quaternion::identity();
	rp3d::Transform transform(initPosition, initOrientation);

	// Create a rigid body in the world 
	rp3d::RigidBody* body;
	body = world.createRigidBody(transform);

	// Change the type of the body to Kinematic 
	body->setType(rp3d::BodyType::DYNAMIC);

	// Get the current material of the body 
	rp3d::Material& material = body->getMaterial();

	// Change the bounciness of the body 
	material.setBounciness(rp3d::decimal(0.4));

	// Change the friction coefficient of the body 
	material.setFrictionCoefficient(rp3d::decimal(0.2));

	return 1;
}

