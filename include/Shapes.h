#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"
#include "cinder/Vector.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;

enum class ShapeType_ { CIRCLE = 0, SQUARE = 1, RECTANGLE = 2, NONE = 3 };

class Shape {
public:
	Shape() : shape(ShapeType_::NONE), disableMovement(false), size(100.f) {};
	~Shape() {};

	virtual void generateShape(MouseEvent event) {}; // Generate all properties for selected shape type
	virtual void drawShape() {}; // Draw shape on screen
	virtual bool clickInsideShape(glm::vec2 clickPos); // Detect if mouse click was inside of shape
	//virtual bool isOverlapping(Shape* shape1, Shape* shape2);
	virtual bool checkWindowCollisionX(); // Check collision with window on X axis
	virtual bool checkWindowCollisionY(); // Check collision with window on Y axis
	virtual void updateDirection() {}; // Update object direction and movement
	void moveUp();
	void moveDown();
	void moveLeft();
	void moveRight();

	glm::vec2 location;
	glm::vec3 color;
	glm::vec2 direction;
	ShapeType_ shape;
	std::string id;
	glm::vec2 size;
	bool disableMovement;
};

class Circle : public Shape
{
public:
	Circle() {};
	~Circle() {};
	void generateShape(MouseEvent event) override;
	void drawShape() override;
	void updateDirection() override;
	bool checkWindowCollisionX() override;
	bool checkWindowCollisionY() override;
	float distanceBetweenCenters(glm::vec2 pos1, glm::vec2 pos2);
	bool clickInsideShape(glm::vec2 clickPos) override;
};

class Square : public Shape
{
public:
	Square() {}
	~Square() {}
	void generateShape(MouseEvent event) override;
	void drawShape() override;
	void updateDirection() override;
	bool checkWindowCollisionX() override;
	bool checkWindowCollisionY() override;
	bool clickInsideShape(glm::vec2 clickPos) override;
};

class RectangleShape : public Shape
{
public:
	RectangleShape() {}
	~RectangleShape() {}
	void generateShape(MouseEvent event) override;
	void drawShape() override;
	void updateDirection() override;
	bool checkWindowCollisionX() override;
	bool checkWindowCollisionY() override;
	bool clickInsideShape(glm::vec2 clickPos);
};