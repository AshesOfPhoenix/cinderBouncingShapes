#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/filereadstream.h"

#include <stdlib.h>
#include <iostream>
#include "fstream"
#include "iostream"
#include <direct.h>
#include <shlobj.h>
#include <stdio.h>

#include "Shapes.h"
#include "cinder2LCDDisplay.h"

using namespace ci;
using namespace ci::app;

#define BUFSIZE 80
#define READ_INTERVAL 0.25
//char* port = "\\\\.\\COM4";

std::ofstream outp;

// Initialize basic variables
BasicApp::BasicApp() {
	if (ImGui::Initialize()) {
		console() << "ImGui Initialized" << std::endl;
	}

	//Variables
	selectedShapeIndx = 50000;
	currentNumOfShapes = 0;
	totalNumberOfShapesCreated = 0;
	showShapeProp = nullptr;
	selectShapeType = ShapeType_::CIRCLE;
	currentShapeX = 0;
	currentShapeY = 0;
	//---------

	wchar_t my_documents[1024];
	std::wstring nm = L"\\BouncingShapes\\jsonfiles";
	HRESULT hr = SHGetFolderPathW(0, CSIDL_MYDOCUMENTS, 0, 0, my_documents);
	const wchar_t* nm2 = nm.c_str();
	std::wstring s(my_documents);
	s += std::wstring(nm2);
	
	std::transform(s.begin(), s.end(), std::back_inserter(str), [](wchar_t c) {
		return (char)c;
		});

}

void BasicApp::setup() {
	// Redirect console output to file cout.txt
	outp.open("cout.txt");
	std::streambuf* sbuf = std::cout.rdbuf();
	std::cout.rdbuf(outp.rdbuf());
	//-----------------------------------------
	
	//Serial connection setup
	mCounter = 0;
	mSendSerialMessage = true;

	// print the devices
	for (const auto& dev : Serial::getDevices())
		console() << "Device: " << dev.getName() << std::endl;

	try {
		dev = Serial::findDeviceByNameContains("USB-SERIAL CH340");
		mSerial = Serial::create(dev, 115200); // Specify baudRate, has to be the same as specified in arduino program
		mSerial->flush();
		console() << "Serial connection with " + dev.getName() + " initialized!" << std::endl;
	}
	catch (SerialExc& exc) {
		console() << "Serial connection with Arduino couldn't be initialized!" << std::endl;
		//exit(-1);
	}
	//-----------------------
}

// Save shapes to json file
void BasicApp::SerializeToJson() {
	if (liki.size() == 0) {
		return;
	}
	// Declare root of json as object
	rapidjson::Document d;
	rapidjson::Document::AllocatorType& allocator = d.GetAllocator();
	d.SetObject();

	// Declare an array of shapes
	rapidjson::Value arrValue;
	arrValue.SetArray();

	// Loop through all shapes on screen and write them into the array
	for each (Shape * shape in liki) {
		rapidjson::Value objValue;
		objValue.SetObject();

		rapidjson::Value id;
		id.SetString(shape->id.c_str(), allocator);
		objValue.AddMember("ID", id, allocator);

		if (shape->shape == ShapeType_::RECTANGLE) {
			rapidjson::Value size(rapidjson::kArrayType);;
			size.PushBack(shape->size.x, allocator);
			size.PushBack(shape->size.y, allocator);
			objValue.AddMember("Size", size, allocator);
		}
		else {
			rapidjson::Value size;
			size.SetFloat(shape->size.x);
			objValue.AddMember("Size", size, allocator);
		}

		rapidjson::Value shapeId;
		shapeId.SetInt(static_cast<int>(shape->shape));
		objValue.AddMember("Shape", shapeId, allocator);

		rapidjson::Value disableMovement;
		disableMovement.SetBool(shape->disableMovement);
		objValue.AddMember("DisableMovement", disableMovement, allocator);

		rapidjson::Value color(rapidjson::kArrayType);
		rapidjson::Value colorR;
		colorR.SetFloat(shape->color.x);
		rapidjson::Value colorG;
		colorG.SetFloat(shape->color.y);
		rapidjson::Value colorB;
		colorB.SetFloat(shape->color.z);
		color.PushBack(colorR, allocator);
		color.PushBack(colorG, allocator);
		color.PushBack(colorB, allocator);
		objValue.AddMember("ColorRGB", color, allocator);

		rapidjson::Value location(rapidjson::kArrayType);
		rapidjson::Value locx;
		locx.SetFloat(shape->location.x);
		rapidjson::Value locy;
		locy.SetFloat(shape->location.y);
		location.PushBack(locx, allocator);
		location.PushBack(locy, allocator);
		objValue.AddMember("LocationXY", location, allocator);

		rapidjson::Value direction(rapidjson::kArrayType);
		rapidjson::Value dirx;
		dirx.SetFloat(shape->direction.x);
		rapidjson::Value diry;
		diry.SetFloat(shape->direction.y);
		direction.PushBack(dirx, allocator);
		direction.PushBack(diry, allocator);
		objValue.AddMember("DirectionXY", direction, allocator);

		arrValue.PushBack(objValue, allocator);
	}
	d.AddMember("Shapes", arrValue, allocator);

	// Save json object to file
	FILE* fp;
	std::time_t t = std::time(0);   // get time now
	std::tm* now = std::localtime(&t);
	std::string temp = str + "\\project_";
	std::string date = std::to_string(totalNumberOfShapesCreated) + "_" + std::to_string(now->tm_sec) + "_" + std::to_string(now->tm_min) + "_" + std::to_string(now->tm_hour) + "_" + std::to_string(now->tm_mday) + "_" + std::to_string(now->tm_mon + 1) + "_" + std::to_string(now->tm_year) + ".json";
	std::string fileName = temp + date;
	
	// If file called project.json already exists created a new one with date and time prefix else create project.json
	_mkdir(str.c_str());
	std::string nwName = str + "\\jsonObject.json";
	if (FILE* file = fopen(nwName.c_str(), "r")) {
		fp = fopen(fileName.c_str(), "wb");
	}
	else {
		fp = fopen(nwName.c_str(), "wb");
	}
	char* writeBuffer;
	writeBuffer = (char*)malloc(65536);

	rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	d.Accept(writer);
	fclose(fp);
}

// Load shapes from json file
void BasicApp::DeserializeFromJson(std::string fileName) {
	liki.clear(); // Overwrite current shapes on screen
	FILE* fp = fopen(fileName.c_str(), "r");

	char* readBuffer;
	readBuffer = (char*)malloc(65536);
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

	rapidjson::Document d;
	d.ParseStream(is);
	fclose(fp);

	assert(d.IsObject());
	rapidjson::Value& savedShapes = d["Shapes"];

	assert(savedShapes.IsArray());

	// Loop through an array of shapes and add them to list
	for (rapidjson::Value::ConstValueIterator itr = savedShapes.Begin(); itr != savedShapes.End(); ++itr) {
		Shape* sh{};
		ShapeType_ shapeId{};
		if (itr->HasMember("Shape")) {
			rapidjson::Value::ConstMemberIterator currentAttribute = itr->FindMember("Shape");
			shapeId = static_cast<ShapeType_>(currentAttribute->value.GetInt());
			if (shapeId == ShapeType_::CIRCLE) {
				sh = new Circle;
				sh->shape = ShapeType_::CIRCLE;
			} else if (shapeId == ShapeType_::SQUARE) {
				sh = new Square;
				sh->shape = ShapeType_::SQUARE;
			} else if (shapeId == ShapeType_::RECTANGLE) {
				sh = new Circle;
				sh->shape = ShapeType_::RECTANGLE;
			} else {
				continue;
			}
		}
		if (itr->HasMember("ID")) {
			rapidjson::Value::ConstMemberIterator currentAttribute = itr->FindMember("ID");
			sh->id = currentAttribute->value.GetString();
		}
		if (itr->HasMember("Size")) {
			if (shapeId == ShapeType_::RECTANGLE) {
				rapidjson::Value::ConstMemberIterator currentAttribute = itr->FindMember("Size");
				rapidjson::GenericArray arr = currentAttribute->value.GetArray();
				sh->size.x = arr.operator[](0).GetFloat();
				sh->size.y = arr.operator[](1).GetFloat();
			} else {
				rapidjson::Value::ConstMemberIterator currentAttribute = itr->FindMember("Size");
				sh->size.x = currentAttribute->value.GetFloat();
			}
		}
		if (itr->HasMember("DisableMovement")) {
			rapidjson::Value::ConstMemberIterator currentAttribute = itr->FindMember("DisableMovement");
			sh->disableMovement = currentAttribute->value.GetBool();
		}
		if (itr->HasMember("LocationXY")) {
			rapidjson::Value::ConstMemberIterator currentAttribute = itr->FindMember("LocationXY");
			rapidjson::GenericArray arr = currentAttribute->value.GetArray();
			sh->location.x = arr.operator[](0).GetFloat();
			sh->location.y = arr.operator[](1).GetFloat();
		}
		if (itr->HasMember("DirectionXY")) {
			rapidjson::Value::ConstMemberIterator currentAttribute = itr->FindMember("DirectionXY");
			rapidjson::GenericArray arr = currentAttribute->value.GetArray();
			sh->direction.x = arr.operator[](0).GetFloat();
			sh->direction.y = arr.operator[](1).GetFloat();
		}
		if (itr->HasMember("ColorRGB")) {
			rapidjson::Value::ConstMemberIterator currentAttribute = itr->FindMember("ColorRGB");
			rapidjson::GenericArray arr = currentAttribute->value.GetArray();
			sh->color.x = arr.operator[](0).GetFloat();
			sh->color.y = arr.operator[](1).GetFloat();
			sh->color.z = arr.operator[](2).GetFloat();
		}
		liki.push_back(sh);
	}
	showShapeProp = liki.front();
	totalNumberOfShapesCreated = liki.size();
	showShapeProp = nullptr;

	fclose(fp);
}

// Read key presses
void BasicApp::keyDown(KeyEvent event) {
	if (event.getChar() == 'f') {
		// Toggle full screen when the user presses the 'f' key.
		setFullScreen(!isFullScreen());
	} else if (event.getCode() == KeyEvent::KEY_SPACE) {
		// Clear the list of shapes on press
		liki.clear();
		totalNumberOfShapesCreated = 0;
		showShapeProp = nullptr;
		selectedShapeIndx = 50000;
	} else if (event.getCode() == KeyEvent::KEY_ESCAPE) {
		showShapeProp = nullptr;
		selectedShapeIndx = 50000;
	}
	// Movement functions
	else if (event.getChar() == 'w') {
		liki.back()->moveUp();
	} else if (event.getChar() == 's') {
		liki.back()->moveDown();
	} else if (event.getChar() == 'a') {
		liki.back()->moveLeft();
	} else if (event.getChar() == 'd') {
		liki.back()->moveRight();
	}
}

// Read mouse presses
void BasicApp::mouseDown(MouseEvent event) {
	if (event.isLeftDown()) { // Create a new selected shape
		if (selectShapeType == ShapeType_::CIRCLE) {
			Circle* newShape = new Circle;
			newShape->shape = ShapeType_::CIRCLE;
			newShape->generateShape(event);	// Generate shape and shape specific properties
			newShape->id = "Circle_" + std::to_string(totalNumberOfShapesCreated);  // Set custom id to created shape
			showShapeProp = newShape;
			totalNumberOfShapesCreated++; // Increase shape count
			liki.push_back(newShape); // Add shape to list of created shapes
		} else if (selectShapeType == ShapeType_::SQUARE) {
			Square* newShape = new Square;
			newShape->shape = ShapeType_::SQUARE;
			newShape->generateShape(event);	// Generate shape and shape specific properties
			newShape->id = "Square_" + std::to_string(totalNumberOfShapesCreated);  // Set custom id to created shape
			showShapeProp = newShape;
			totalNumberOfShapesCreated++; // Increase shape count
			liki.push_back(newShape); // Add shape to list of created shapes
		} else if (selectShapeType == ShapeType_::RECTANGLE) {
			RectangleShape* newShape = new RectangleShape;
			newShape->shape = ShapeType_::RECTANGLE;
			newShape->generateShape(event);	// Generate shape and shape specific properties
			newShape->id = "Rectangle_" + std::to_string(totalNumberOfShapesCreated);  // Set custom id to created shape
			showShapeProp = newShape;
			totalNumberOfShapesCreated++; // Increase shape count
			liki.push_back(newShape); // Add shape to list of created shapes
		}
	}
	else if (event.isRightDown())
	{
		std::list<Shape*>::reverse_iterator i = liki.rbegin();
		for (i; i != liki.rend(); i++) {
			if ((*i)->clickInsideShape(event.getPos())) {
				liki.remove(*i);  // Remove last created shape
				break;
			}
		}
	}
}

// Draw loop
void BasicApp::draw() {
	gl::clear(Color::gray(0.1f));

	// Menu Bar
	menuBar();
	// Window that displays a list of all shapes on screen
	listOfAllShapes();
	// Display properties of currently selected shape
	shapeProperties();

	ImGui::ShowDemoWindow();

	sendToArduino();

	// Draw shapes
	for (Shape* lik : liki) {
		lik->drawShape();
	}
}

void BasicApp::shapeProperties() {
	ImGuiColorEditFlags misc_flags = ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoOptions;

	// Properties window for selected object, if no shape is selected don't display widget
	if (showShapeProp != nullptr && selectedShapeIndx != 50000) {
		ImGui::Begin("Properties");
		// Location settings
		ImGui::DragFloat("LocationX", &showShapeProp->location.x, 0.f, getWindowWidth());
		ImGui::DragFloat("LocationY", &showShapeProp->location.y, 0.f, getWindowHeight());

		// Color settings
		ImGui::ColorEdit3("Color", &showShapeProp->color.x, misc_flags);
		// Size settings
		if (showShapeProp->shape == ShapeType_::RECTANGLE) {
			ImGui::DragFloat("SizeX", &showShapeProp->size.x, 5.0f, 300.0f);
			ImGui::DragFloat("SizeY", &showShapeProp->size.y, 5.0f, 300.0f);
		} else {
			ImGui::DragFloat("Size", &showShapeProp->size.x, 5.0f, 300.0f);
		}
		// Freeze shape
		ImGui::Checkbox("Disable Movement", &showShapeProp->disableMovement);
		ImGui::End();
	}
}

// Display a list of all shapes in a widget
void BasicApp::listOfAllShapes() {
	// Select which shape type you want to create
	ImGui::Text("Select shape type");
	if (ImGui::Button("Circle"))
		selectShapeType = ShapeType_::CIRCLE;
	if (ImGui::Button("Square"))
		selectShapeType = ShapeType_::SQUARE;
	if (ImGui::Button("Rectangle"))
		selectShapeType = ShapeType_::RECTANGLE;

	// Show list of all objects created
	if (ImGui::BeginListBox("", ImVec2(-FLT_MIN, 700))) { // ImVec2(120, 700)
		int n = 0;
		for each (Shape * var in liki) {
			const bool is_selected = (selectedShapeIndx == n);
			if (ImGui::Selectable(var->id.c_str(), is_selected)) { // Select which shape you want to edit from the list x
				this->selectedShapeIndx = n;
				this->showShapeProp = var;
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
			n++;
		}
		ImGui::EndListBox();
	}
}

void BasicApp::menuBar() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save")) { 
				SerializeToJson(); 
			}
			// Show list of json files in directory
			if (ImGui::BeginMenu("Open")) { // Select which json file you want to load shapes from
				_mkdir(str.c_str());
				std::string ext(".json");
				for (auto& p : std::filesystem::directory_iterator(str)) {
					if (p.path().extension() == ext) {
						if (ImGui::MenuItem(p.path().stem().string().c_str())) { DeserializeFromJson(p.path().string()); }
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Delete")) { // Delete a json file in directory		
				std::string ext(".json");
				for (auto& p : std::filesystem::directory_iterator(str)) {
					if (p.path().extension() == ext) {
						if (ImGui::MenuItem(p.path().stem().string().c_str())) {
							if (remove(p.path().string().c_str()) != 0)
								perror("Error deleting file");
							else
								puts("File successfully deleted");
						}
					}
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

// Display Arduino window only if connection with Arduino is established
void BasicApp::sendToArduino() {	
	if (mSerial) {
		if (ImGui::Begin("Arduino LCD Display")) {
			static char stri[17];
			ImGui::InputText("Enter text", stri, IM_ARRAYSIZE(stri));
			if (ImGui::Button("Apply text")) {
				memset(output, 0, 255);

				mSerial->writeBytes(stri, IM_ARRAYSIZE(stri));
				mSerial->readAvailableBytes(output, 255);
				//mSerial->readBytes(output, 13);

				console() << "Text send to arduino: " + std::string(stri) << std::endl;
				console() << std::string(output) << std::endl;
				
				memset(stri, 0, 16);
			}
		}
		ImGui::End();
	}
}

// This line tells Cinder to actually create and run the application.
CINDER_APP(BasicApp, RendererGl,
	[&](App::Settings* settings) {
		settings->setWindowSize(1280, 720);
		settings->setMultiTouchEnabled(false);
	});