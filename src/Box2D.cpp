#include "Box2D.hpp"
#include "opencv2/opencv.hpp"

b2World initBox2D(float g_x, float g_y)
{
	return 	b2World(b2Vec2(g_x, g_y));
}


void createCircle(b2World& world, int type, float x, float y, float radius) {
	//	On definit le body phisique
	b2BodyDef body_def;
	body_def.position.Set(x, y);
	switch (type) {
	case 0: body_def.type = b2_staticBody; break;
	case 1: body_def.type = b2_dynamicBody; break;
	case 2: body_def.type = b2_kinematicBody; break;
	default: body_def.type = b2_staticBody; break;
	}

	//	avec le world phisique on peut creer le body selon sa definition
	b2Body* body = world.CreateBody(&body_def);

	//	on creer le shape de notre cercle
	b2CircleShape circle_shape;
	//	position relative to the body position
	circle_shape.m_p.Set(0, 0);
	circle_shape.m_radius = radius;

	//	on creer le fixture qui va correspondre au cercle
	b2FixtureDef fixture_def;
	fixture_def.shape = &circle_shape;
	fixture_def.density = 1;
	body->CreateFixture(&fixture_def);

}

void makePhysicWalls(b2World& world, cv::Mat cloudWalls) {

	for (int row = 0; row < cloudWalls.rows; row++) {
		for (int col = 0; col < cloudWalls.cols; col++) {
			uchar intensity = cloudWalls.at<uchar>(row, col);
			//	Points Blanc
			if (intensity != 0) {
				createCircle(world, 1, col, row, 2);
			}

		}
	}
}

int test_Box2D(void) {
	b2World world = initBox2D();
	createCircle(world, 1, 0, 1, 1);
	createCircle(world, 1, 1, 1, 1);
	std::cout << "test creation de body, nombre creer : " << world.GetBodyCount() << std::endl;
	if (world.GetBodyCount() == 2) {
		return 1;
	}
	else return -1;
}

int test_box2D_Walls(void) {
	b2World world = initBox2D();
	//echequier
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

	makePhysicWalls(world, chessBoard);
	std::cout << world.GetBodyCount() << std::endl;
	return 1;
}