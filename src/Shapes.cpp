#include "Shapes.h"

bool Shape::clickInsideShape(glm::vec2 clickPos) { return false; };
//bool Shape::isOverlapping(Shape* shape1, Shape* shape2) { return false; };
bool Shape::checkWindowCollisionX() { return false; };
bool Shape::checkWindowCollisionY() { return false; };
void Shape::moveUp()
{
	if (!checkWindowCollisionY())
	{
		if (!disableMovement)
			location.y -= 5.0f;
	}
}
void Shape::moveDown()
{
	if (!checkWindowCollisionY())
	{
		if (!disableMovement)
			location.y += 5.0f;
	}
}
void Shape::moveLeft()
{
	if (!checkWindowCollisionX())
	{
		if (!disableMovement)
			location.x -= 5.0f;
	}
}
void Shape::moveRight()
{
	if (!checkWindowCollisionX())
	{
		if (!disableMovement)
			location.x += 5.0f;
	}
}

void Circle::generateShape(MouseEvent event)
{
	Rand rnd;
	size.x = (rnd.randFloat() * 300)/2.f;
	color = glm::vec3(rnd.randFloat(), rnd.randFloat(), rnd.randFloat());
	location = event.getPos();
	direction.x = std::round(rnd.randPosNegFloat(-3.f, 3.f));
	direction.y = std::round(rnd.randPosNegFloat(-3.f, 3.f));
	console() << "Generated Circle" << std::endl;
}
void Circle::drawShape()
{
	gl::begin(GL_LINE_LOOP);
	gl::color(color.x, color.y, color.z);
	for (int ii = 0; ii < 100; ii++) {
		float theta = 2.0f * 3.1415926f * float(ii) / float(100);//get the current angle
		float x = size.x * cosf(theta);//calculate the x component
		float y = size.x * sinf(theta);//calculate the y component
		gl::vertex(x + location.x, y + location.y);//output vertex
	}
	gl::end();
	if (!disableMovement)
		updateDirection();
}
void Circle::updateDirection()
{
	location.x += direction.x;
	location.y += direction.y;
	if (checkWindowCollisionX())
	{
		direction.x *= -1;
	}
	if (checkWindowCollisionY())
	{
		direction.y *= -1;
	}
}
bool Circle::checkWindowCollisionX()
{
	if (location.x + size.x >= getWindowWidth() || location.x - size.x <= 0) {
		return true;
	}
	return false;
}
bool Circle::checkWindowCollisionY()
{
	if (location.y + size.x >= getWindowHeight() || location.y - size.x <= 0) {
		return true;
	}
	return false;
}
float Circle::distanceBetweenCenters(glm::vec2 pos1, glm::vec2 pos2)
{
	return std::sqrt(std::pow(pos1.x - pos2.x, 2) + std::pow(pos1.y - pos2.y, 2));
}
bool Circle::clickInsideShape(glm::vec2 clickPos)
{
	if (distanceBetweenCenters(clickPos, location) < size.x)
	{
		return true;
	}
	return false;
}

void Square::generateShape(MouseEvent event)
{
	Rand rnd;
	size.x = rnd.randFloat() * 300;
	color = glm::vec3(rnd.randFloat(), rnd.randFloat(), rnd.randFloat());
	location = event.getPos();
	direction.x = std::round(rnd.randPosNegFloat(-3.f, 3.f));
	direction.y = std::round(rnd.randPosNegFloat(-3.f, 3.f));
	console() << "Generated Square" << std::endl;
}

void Square::drawShape()
{
	gl::begin(GL_LINE_LOOP);
	gl::color(color.x, color.y, color.z);

	gl::vertex(location.x - size.x / 2.f, location.y - size.x / 2.f);
	gl::vertex(location.x + size.x / 2.f, location.y - size.x / 2.f);
	gl::vertex(location.x + size.x / 2.f, location.y + size.x / 2.f);
	gl::vertex(location.x - size.x / 2.f, location.y + size.x / 2.f);

	gl::end();
	if (!disableMovement)
		updateDirection();
}

void Square::updateDirection()
{
	location.x += direction.x;
	location.y += direction.y;
	if (checkWindowCollisionX())
	{
		direction.x *= -1;
	}
	if (checkWindowCollisionY())
	{
		direction.y *= -1;
	}
}

bool Square::checkWindowCollisionX()
{
	if (location.x + size.x / 2.f >= getWindowWidth() || location.x - size.x / 2.f <= 0) {
		return true;
	}
	return false;
}

bool Square::checkWindowCollisionY()
{
	if (location.y + size.x / 2.f >= getWindowHeight() || location.y - size.x / 2.f <= 0) {
		return true;
	}
	return false;
}

bool Square::clickInsideShape(glm::vec2 clickPos)
{
	if (clickPos.x < location.x + size.x / 2.f && clickPos.x > location.x - size.x / 2.f)
	{
		if (clickPos.y < location.y + size.x / 2.f && clickPos.y > location.y - size.x / 2.f)
		{
			return true;
		}
	}
	return false;
}

void RectangleShape::generateShape(MouseEvent event)
{
	Rand rnd;
	size.x = rnd.randFloat() * 300;
	size.y = rnd.randFloat() * 300;
	color = glm::vec3(rnd.randFloat(), rnd.randFloat(), rnd.randFloat());
	location = event.getPos();
	direction.x = std::round(rnd.randPosNegFloat(-3.f, 3.f));
	direction.y = std::round(rnd.randPosNegFloat(-3.f, 3.f));
	console() << "Generated Rectangle" << std::endl;
}

void RectangleShape::drawShape()
{
	gl::begin(GL_LINE_LOOP);
	gl::color(color.x, color.y, color.z);

	gl::vertex(location.x - size.x / 2.f, location.y - size.y / 2.f);
	gl::vertex(location.x + size.x / 2.f, location.y - size.y / 2.f);
	gl::vertex(location.x + size.x / 2.f, location.y + size.y / 2.f);
	gl::vertex(location.x - size.x / 2.f, location.y + size.y / 2.f);

	gl::end();
	if (!disableMovement)
		updateDirection();
}

void RectangleShape::updateDirection()
{
	location.x += direction.x;
	location.y += direction.y;
	if (checkWindowCollisionX())
	{
		direction.x *= -1;
	}
	if (checkWindowCollisionY())
	{
		direction.y *= -1;
	}
}

bool RectangleShape::checkWindowCollisionX()
{
	if (location.x + size.x / 2.f >= getWindowWidth() || location.x - size.x / 2.f <= 0) {
		return true;
	}
	return false;
}

bool RectangleShape::checkWindowCollisionY()
{
	if (location.y + size.y / 2.f >= getWindowHeight() || location.y - size.y / 2.f <= 0) {
		return true;
	}
	return false;
}

bool RectangleShape::clickInsideShape(glm::vec2 clickPos)
{
	if (clickPos.x < location.x + size.x / 2.f && clickPos.x > location.x - size.x / 2.f)
	{
		if (clickPos.y < location.y + size.y / 2.f && clickPos.y > location.y - size.y / 2.f)
		{
			return true;
		}
	}
	return false;
}