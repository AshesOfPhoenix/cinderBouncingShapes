#pragma once

#include "cinder/Serial.h"
#include "cinder/app/App.h"
#include "cinder/CinderImGui.h"

class Shape;
enum class ShapeType_;

class BasicApp : public App {
public:
	BasicApp();
	void keyDown(KeyEvent event) override;
	void mouseDown(MouseEvent event) override;
	void draw() override;
	void setup() override;
	void SerializeToJson();
	void DeserializeFromJson(std::string fileName);
	void menuBar();
	void sendToArduino();
	void listOfAllShapes();
	void shapeProperties();

private:
	std::list<Shape*> liki;
	Shape* showShapeProp;
	int selectedShapeIndx;
	ShapeType_ selectShapeType;
	int totalNumberOfShapesCreated;
	int currentNumOfShapes;

	int currentShapeX;
	int currentShapeY;

	char output[255];

	std::string str;

	//Serial variables
	bool		mSendSerialMessage;
	SerialRef	mSerial;
	uint8_t		mCounter;
	std::string		mLastString;
	Serial::Device dev;
};