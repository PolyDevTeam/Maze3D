#include "bullet3D.hpp"
#include "GL.hpp"

int testBullet() {
	///-----includes_end-----

	int i;
	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -10, 0));

	///-----initialization_end-----

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	///create a few basic rigid bodies

	//the ground is a cube of side 100 at position y = -56.
	//the sphere will hit it at y = -6, with center at -5
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -56, 0));

		btScalar mass(0.);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			groundShape->calculateLocalInertia(mass, localInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}

	{
		//create a dynamic rigidbody

		//btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
		btCollisionShape* colShape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(colShape);

		/// Create Dynamic Objects
		btTransform startTransform;
		startTransform.setIdentity();

		btScalar mass(1.f);

		//rigidbody is dynamic if and only if mass is non zero, otherwise static
		bool isDynamic = (mass != 0.f);

		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);

		startTransform.setOrigin(btVector3(2, 10, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		dynamicsWorld->addRigidBody(body);
	}

	/// Do some simulation

	///-----stepsimulation_start-----
	for (i = 0; i < 150; i++)
	{
		dynamicsWorld->stepSimulation(1.f / 60.f, 10);

		//print positions of all objects
		for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
		{
			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
			btRigidBody* body = btRigidBody::upcast(obj);
			btTransform trans;
			if (body && body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
			{
				trans = obj->getWorldTransform();
			}
			printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
		}
	}

	///-----stepsimulation_end-----

	//cleanup in the reverse order of creation/initialization

	///-----cleanup_start-----

	//remove the rigidbodies from the dynamics world and delete them
	for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();

	return 1;
}


int testBullet2() {
	GLFWwindow* win = init_GL(640, 480);


	//	Echequier
	int blockSize = 40;
	int imageSize = blockSize * 8;
	cv::Mat chessBoard(imageSize, imageSize, CV_8UC3, cv::Scalar::all(0));
	unsigned char color = 0;

	for (int i = 0; i < imageSize; i = i + blockSize) {
		color = ~color;
		for (int j = 0; j < imageSize; j = j + blockSize) {
			cv::Mat ROI = chessBoard(cv::Rect(i, j, blockSize, blockSize));
			ROI.setTo(cv::Scalar::all(color));
			color = ~color;
		}
	}

	//	Creer un Monde physique
	btVector3 gravite(0, -9.8, 0);
	btDiscreteDynamicsWorld* world = createWorld(gravite);

	//	Creer une Balle Physique
	float radius = 20;
	btVector3 origine(0, 100, 0);
	btScalar ballMass = 1;
	btRigidBody* ball = createBall(radius, origine, ballMass, world);

	//	Creer un sol physique
	btVector3 groundDims(500, 1, 300);
	btVector3 origineGround(0, 0, 0);
	btRigidBody* ground = createGround(groundDims, origineGround, world);

	//	Creer les pillier physique
	btVector3 pillardDims(0.1, 1, 0.1);
	//std::vector<btRigidBody*> pillards = createWalls(chessBoard, pillardDims, chessBoard.rows/2, world);


	//	Scale to window size
	GLint windowWidth, windowHeight;
	glfwGetWindowSize(win, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//	Set PROJECTION
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(100, (double)windowWidth / (double)windowHeight, 0.1f, 10000.0f);

	//	Set MODELVIEW
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 500, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	while (!glfwWindowShouldClose(win)) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		world->stepSimulation(1.f / 60.f, 10);
		/*
		*	DRAW STUFF
		*/

		//std::cout << world->getNumCollisionObjects() << std::endl;
		for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
		{
			btCollisionObject* obj = world->getCollisionObjectArray()[i];
			btRigidBody* body = btRigidBody::upcast(obj);
			btTransform trans;
			if (body && body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
			{
				trans = obj->getWorldTransform();
			}
			//printf("world pos object %d = %f,%f,%f\n", i, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
			btScalar mat[16];
			
			if (i == 1) {
				//btQuaternion quat(1, 0, 0, -0.79);
				//trans.setRotation(quat);
				//body->getMotionState()->setWorldTransform(trans);
			}

			trans.getOpenGLMatrix(mat);

			//for (int k = 0; k < 16; k++) {
			//	std::cout << mat[k] << "  ";
			//}
			std::cout << std::endl;

			glPushMatrix();
			//glTranslatef(-chessBoard.cols/2, -chessBoard.rows/2, 0);
			//glRotatef(45, 1, 0, 0);
			//glRotatef(40, 0, 1, 0);
			glMultMatrixf(mat);


			//	BALL
			if (i == 0) {
				//draw 3d
				glColor3f(1.0, 1.0, 0);
				GLUquadric* params = gluNewQuadric();
				gluSphere(params, radius, 20, 20);
			}

			//	GROUND
			if (i == 1) {
				glBegin(GL_POLYGON);
				{

					glColor3f(1, 0, 0);

					float dimx = groundDims[0];
					float dimz = groundDims[2];

					//glVertex3f(-dimx/2, 0.5, -dimz/2);
					//glVertex3f(dimx/2, 0.5, -dimz/2);
					//glVertex3f(dimx / 2, 0.5, dimz / 2);
					//glVertex3f(-dimx / 2, 0.5, dimz / 2);

					glVertex3f(-dimx, 0.5, -dimz);
					glVertex3f(dimx, 0.5, -dimz);
					glVertex3f(dimx, 0.5, dimz);
					glVertex3f(-dimx, 0.5, dimz);

				}
				glEnd();

				
			}

		
			glPopMatrix();
		}

		//glPointSize(40.0f);
		//glColor3f(1, 0, 0);
		//glBegin(GL_POINTS);
		//glVertex3f(0.0f, 0.0f, 1.0f);
		//glEnd();



		// Update Screen
		glfwSwapBuffers(win);

		// Check for any input, or window movement
		glfwPollEvents();


	}


	return 1;
};
















int clearPhysics(btDynamicsWorld* dynamicsWorld) {
	///-----cleanup_start-----

//remove the rigidbodies from the dynamics world and delete them
	for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	return 1;
}

btDiscreteDynamicsWorld* createWorld(btVector3 gravite) {
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	//	Create the World
	btDiscreteDynamicsWorld* world = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	//	Set Gravity
	world->setGravity(gravite);
	return world;
}



btCompoundShape* createWalls(cv::Mat cloud,btVector3 pillardDims, float z, btDiscreteDynamicsWorld* world) {
	
	btCompoundShape* wallShape = new btCompoundShape();
	std::vector<btRigidBody*> vect;

	for (int row = 0; row < cloud.rows; row++) {
		for (int col = 0; col < cloud.cols; col++) {
			uchar intensity = cloud.at<uchar>(row, col);

			if (intensity != 0) {

				//	Creation de la Forme
				btCollisionShape* pillardShape = new btBoxShape(pillardDims);

				btTransform myTransform;
				myTransform.setIdentity();

				// Position du pillier
				myTransform.setOrigin(btVector3(col - cloud.cols/2, -row + cloud.rows/2, z));
				btVector3 localInertiaPillard(0, 0, 0);

				btDefaultMotionState *myMotionState;
				myMotionState = new btDefaultMotionState(myTransform);


				wallShape->addChildShape(myTransform, pillardShape);


			}

		}
	}

	btTransform wallTransform;
	wallTransform.setIdentity();
	wallTransform.setOrigin(btVector3(0, 0, 0));
	btVector3 localInertia(0, 0, 0);

	btDefaultMotionState *wallMotionState;
	wallMotionState = new btDefaultMotionState(wallTransform);


	btRigidBody::btRigidBodyConstructionInfo infoWall(0, wallMotionState, wallShape, localInertia);
	btRigidBody* bodyWall = new btRigidBody(infoWall);

	//	Activation du mode KINEMATIC pour le body
	bodyWall->setCollisionFlags(bodyWall->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	bodyWall->setActivationState(DISABLE_DEACTIVATION);




	// On ajoute le mur dans le monde Bullet
	world->addRigidBody(bodyWall);



	return wallShape;
}

btRigidBody* createBall(float radius, btVector3 origine, btScalar mass, btDiscreteDynamicsWorld* world) {
	//	Creation de la Forme
	btCollisionShape* ballShape = new btSphereShape(radius);
	
	//	Position & Rotation de l'objet
	btTransform myTransform;
	myTransform.setIdentity();
	myTransform.setOrigin(origine);
	btVector3 localInertia(0, 0, 0);
	
	//	Objet Dynamique
	if (mass) {
		ballShape->calculateLocalInertia(mass, localInertia);
	}

	btDefaultMotionState *myMotionState;
	myMotionState = new btDefaultMotionState(myTransform);
	

	//	Regroupement des informations pour creer le body
	btRigidBody::btRigidBodyConstructionInfo myBoxRigidBodyConstructionInfo(mass, myMotionState, ballShape, localInertia);
	myBoxRigidBodyConstructionInfo.m_restitution = 0.0f;
	//	On creer la balle physique
	btRigidBody *body;
	body = new btRigidBody(myBoxRigidBodyConstructionInfo);

	world->addRigidBody(body);

	return body;
}


btRigidBody* createGround(cv::Mat cloud, btVector3 origine, btDiscreteDynamicsWorld* world) {
	//	Creation de la Forme
	btVector3 dims(cloud.cols/2, cloud.rows/2, 50);
	btCollisionShape* groundShape = new btBoxShape(dims);

	btTransform myTransform;
	myTransform.setIdentity();

	// Position du sol
	myTransform.setOrigin(origine);
	btVector3 localInertiaGround(0, 0, 0);

	btDefaultMotionState *myMotionState;
	myMotionState = new btDefaultMotionState(myTransform);

	btRigidBody::btRigidBodyConstructionInfo infoGround(0, myMotionState, groundShape, localInertiaGround);
	//infoGround.m_restitution = 0.0f;
	btRigidBody* bodyGround;

	//	Création du body 
	bodyGround = new btRigidBody(infoGround);

	//	Activation du mode KINEMATIC pour le body
	bodyGround->setCollisionFlags(bodyGround->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	bodyGround->setActivationState(DISABLE_DEACTIVATION);

	// On ajoute le sol dans le monde Bullet
	world->addRigidBody(bodyGround);
	return bodyGround;
}


btRigidBody* createGround(btVector3 groundDims, btVector3 origine, btDiscreteDynamicsWorld* world) {
	//	Creation de la Forme
	btCollisionShape* groundShape = new btBoxShape(groundDims);

	btTransform myTransform;
	myTransform.setIdentity();

	// Position du sol
	myTransform.setOrigin(origine);
	btVector3 localInertiaGround(0, 0, 0);

	btDefaultMotionState *myMotionState;
	myMotionState = new btDefaultMotionState(myTransform);

	btRigidBody::btRigidBodyConstructionInfo infoGround(0, myMotionState, groundShape, localInertiaGround);

	btRigidBody* bodyGround;

	//	Création du body 
	bodyGround = new btRigidBody(infoGround);

	//	Activation du mode KINEMATIC pour le body
	bodyGround->setCollisionFlags(bodyGround->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	bodyGround->setActivationState(DISABLE_DEACTIVATION);

	// On ajoute le sol dans le monde Bullet
	world->addRigidBody(bodyGround);
	return bodyGround;
}
