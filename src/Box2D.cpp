#include "Box2D.hpp"

b2World initBox2D(float g_x, float g_y)
{
	return 	b2World(b2Vec2(g_x, g_y));
}

void createCircle(b2World& world, float x, float y, float radius) {
	//	On definit le body phisique
	b2BodyDef body_def;
	body_def.position.Set(x, y);
	body_def.type = b2_dynamicBody;

	//	avec le world phisique on peut creer le body selon sa definition
	b2Body* body = world.CreateBody(&body_def);

	//	on creer le shape de notre cercle
	b2CircleShape circle_shape;
	circle_shape.m_p.Set(0, 0);
	circle_shape.m_radius = radius;

	//	on creer le fixture qui va correspondre au cercle
	b2FixtureDef fixture_def;
	fixture_def.shape = &circle_shape;
	fixture_def.density = 1;
	body->CreateFixture(&fixture_def);

}