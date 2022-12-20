//////////////////////////////////////////////////////////////////////////////////
// SPARK particle engine														//
// Copyright (C) 2008-2009 - Julien Fryer - julienfryer@gmail.com				//
//																				//
// This software is provided 'as-is', without any express or implied			//
// warranty.  In no event will the authors be held liable for any damages		//
// arising from the use of this software.										//
//																				//
// Permission is granted to anyone to use this software for any purpose,		//
// including commercial applications, and to alter it and redistribute it		//
// freely, subject to the following restrictions:								//
//																				//
// 1. The origin of this software must not be misrepresented; you must not		//
//    claim that you wrote the original software. If you use this software		//
//    in a product, an acknowledgment in the product documentation would be		//
//    appreciated but is not required.											//
// 2. Altered source versions must be plainly marked as such, and must not be	//
//    misrepresented as being the original software.							//
// 3. This notice may not be removed or altered from any source distribution.	//
//////////////////////////////////////////////////////////////////////////////////

//#define _POSIX_C_SOURCE 199309L
//#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
//#include <ctime>
#include <chrono>

#include "Core/SPK_Shader.h"
#include <glad/glad.h>

#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
GLFWwindow* window = nullptr;
#include <errno.h>

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "SPK.h"
#include "SPK_GLES.h"
#include "stb_image.h"

using namespace std;
using namespace SPK;
using namespace SPK::GLES;

static const GLuint WIDTH = 800;
static const GLuint HEIGHT = 600;
float fov = 45.0f;
float near = 0.01f;
float far = 30.0f;

float rainRatio = 0.5f; //0.5

double prevMouseX = 0.0;
double prevMouseY = 0.0;

int deltaTime = 0;

float posX = 0.0f;
float posY = 1.0f;
float posZ = 0.0f;
float angleX = 12.0f;
float angleY = 0.0f;
float angleZ = 0.0f;

float boxposX = 1.0f; 
float boxposY = -1.0f; 
float boxposZ = -3.0f;
float boxangleX = 12.0f;
float boxangleY = 45.0f;
float boxangleZ = 30.0f;

float obstacleposX = 1.0f; 
float obstacleposY = -1.0f; 
float obstacleposZ = -3.0f;
float obstacleangleX = 12.0f;
float obstacleangleY = 45.0f;
float obstacleangleZ = 30.0f;

GLuint texturePaving = 0;

Group* rainGroup = NULL;
Group* dropGroup = NULL;
Group* splashGroup = NULL;
Group* splashGroup1 = NULL;
System* particleSystem = NULL;
Emitter* dropEmitter = NULL;

const float PI = 3.14159265358979323846f;

const string STR_RAIN_RATE = "RAIN INTENSITY : ";
const string STR_NB_PARTICLES = "NB PARTICLES : ";
const string STR_FPS = "FPS : ";

string strRainRate = STR_RAIN_RATE;
string strNbParticles = STR_NB_PARTICLES;
string strFps = STR_FPS;

int drawText = 2;
bool renderEnv = true;
int frames = 0;

const float CRATE_DIM2 = 1.0f;
const float CRATE_DIM = CRATE_DIM2 * 0.5f;
Vector3D CRATE_POSITIONS = Vector3D(0.0f,CRATE_DIM,0.0f);

AABox* box;
Plane* roof;
ZoneIntersection* windShield;
// Obstacles
Obstacle* obstacle;

glm::mat4 modelview_floor = glm::mat4(1.0f);
glm::mat4 projectionMat = glm::perspective(glm::radians(fov), (float)WIDTH/(float)HEIGHT, near, far);

glm::mat4 modelview_box = glm::mat4(1.0f);
glm::mat4 modelviewUnit = glm::mat4(1.0f);

void DestroySPKFactory();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow *window);

// Converts an int into a string
string int2Str(int a)
{
    ostringstream stm;
    stm << a;
    return stm.str();
}

// Gets the values of a param function of the rain intensity
template<class T>
T param(T min,T max)
{
	return static_cast<T>(min + rainRatio * (max - min));
}

void renderFirstFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(window);
	glfwSwapBuffers(window);
}

//Renders the scene
void render(Shader &floorShader)
{
	floorShader.use();
	floorShader.bindVBO();
	floorShader.bindTBO();
	floorShader.bindEBO();
	floorShader.setAttribute("aPos", 3, 5, 0);
	floorShader.setAttribute("aTexCoord", 2, 5, 3);
	floorShader.setInt("ourTexture", 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void renderCrate(Shader &Shader)
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	Shader.use();
	Shader.bindVBO();
	Shader.bindEBO();
	Shader.bindTBO();
	Shader.setAttribute("aPos", 3, 5, 0);
	Shader.setAttribute("aTexCoord", 2, 5, 3);
	Shader.setInt("crateTexture", 0);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void renderBox(Shader &boxShader)
{
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	boxShader.use();
	boxShader.bindVBO();
	boxShader.setAttribute("aPos", 3, 3, 0);
	glDrawArrays(GL_LINES,0,24);
}

void setFloorTransformations()
{
	// create transformations
	modelview_floor = glm::translate(modelview_floor, glm::vec3(-posX,-posY,-posZ));
	modelview_floor = glm::rotate(modelview_floor, glm::radians(angleZ), glm::vec3(0.0f,0.0f,1.0f));
	modelview_floor = glm::rotate(modelview_floor, glm::radians(angleY), glm::vec3(0.0f,1.0f,0.0f));
	modelview_floor = glm::rotate(modelview_floor, glm::radians(angleX), glm::vec3(1.0f,0.0f,0.0f));
}

void setBoxTransformations()
{
	// create transformations
	modelview_box = glm::translate(modelview_box, glm::vec3(boxposX, boxposY-(boxposZ * glm::tan(glm::radians(boxangleX))),boxposZ));
	modelview_box = glm::rotate(modelview_box, glm::radians(boxangleZ), glm::vec3(0.0f,0.0f,1.0f));
	modelview_box = glm::rotate(modelview_box, glm::radians(boxangleX), glm::vec3(1.0f,0.0f,0.0f));
	modelview_box = glm::rotate(modelview_box, glm::radians(boxangleY), glm::vec3(0.0f,1.0f,0.0f));
}

void setConfig(Shader &shadervalue)
{
	if (renderEnv)
	{
			float dist = param(20.0f,5.0f); // = 12.5

float vertices[] = {
         	dist + posX, 0.0f, dist + posZ, dist + posX,dist + posZ,  // top right
        	dist + posX, 0.0f, -dist + posZ, dist + posX,-dist + posZ,  // bottom right
        	-dist + posX, 0.0f, -dist + posZ, -dist + posX,-dist + posZ, // bottom left
        	-dist + posX, 0.0f, dist + posZ, -dist + posX,dist + posZ   // top left
    	};

    unsigned int indices[] = {  // note that we start from 0!
        	0, 1, 3,  // first Triangle
        	1, 2, 3   // second Triangle
    	};

	shadervalue.generateTBO();
	shadervalue.bindTBO();
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load("res/paving.bmp", &width, &height, &nrChannels, 0);
	if (data)
	{
		shadervalue.setTBOData(GL_RGB, width, height, data);
		shadervalue.genMipMap();
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	shadervalue.generateVBO();
	shadervalue.generateEBO();

	shadervalue.bindVBO();
	shadervalue.setVBOData(sizeof(vertices), vertices);
	
	shadervalue.bindEBO();
	shadervalue.setEBOData(sizeof(indices), indices);

	shadervalue.use();

	shadervalue.setAttribute("aPos", 3, 5, 0);

	shadervalue.setAttribute("aTexCoord", 2, 5, 3);
	shadervalue.setMatrix("modelview", modelview_floor);

	shadervalue.setMatrix("projection", projectionMat);
	shadervalue.setInt("ourTexture", 0);

	shadervalue.unbindTBO();
	shadervalue.unbindVBO();
	shadervalue.unbindEBO();
	}
}

// Call back function to destroy rain particles when it touches the ground and create splash
bool killRain(Particle& particle,float deltaTime)
{
	// Vector3D v0 = particle.position();
	// Vector3D v1 = v0 + Vector3D(0.0f, -0.1f, 0.0f);
	// Vector3D intersection = Vector3D(0.0f, 0.0f, 0.0f);
	// Vector3D normal = Vector3D(0.0f, 0.0f, 0.0f);
	// bool collision = box->intersects(v0, v1, &intersection, &normal);
	bool kill = false;
	if (particle.position() == obstacle->intersection)
	{
		particle.position().set(obstacle->intersection.x, obstacle->intersection.y + 0.05f, obstacle->intersection.z);
		splashGroup1->addParticles(1,particle.position(),Vector3D());
		//dropGroup->addParticles(param(2,8),particle.position(),dropEmitter);
		//return true;
		kill = true;
	}
	else if (particle.position().y <= 0.0f)
	{
		particle.position().set(particle.position().x,0.01f,particle.position().z);
		splashGroup->addParticles(1,particle.position(),Vector3D());
		//dropGroup->addParticles(param(2,8),particle.position(),dropEmitter);
		//return true;
		kill = true;
	}
	return kill;
}

void DestroySPKFactory()
{
	cout << "\nSPARK FACTORY BEFORE DESTRUCTION :" << endl;
	SPKFactory::getInstance().traceAll();
	SPKFactory::getInstance().destroyAll();
	cout << "\nSPARK FACTORY AFTER DESTRUCTION :" << endl;
	SPKFactory::getInstance().traceAll();

#ifdef USE_WINDOWS
	winMan.DestroyWindow();
#endif

	cout << endl;
	system("pause"); // Waits for the user to close the console
}

void error_callback( int error, const char *msg ) {
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
}

void setCrateConfig(Shader &shadervalue)
{
	float X = CRATE_POSITIONS.x;
	float Y = CRATE_POSITIONS.y;
	float Z = CRATE_POSITIONS.z;

	float vertices[] = {
		X-CRATE_DIM, Y+CRATE_DIM, Z+CRATE_DIM, 0.0f, 1.0f,
		X-CRATE_DIM, Y-CRATE_DIM, Z+CRATE_DIM, 0.0f, 0.0f,
		X+CRATE_DIM, Y-CRATE_DIM, Z+CRATE_DIM, 1.0f, 0.0f,
		X+CRATE_DIM, Y+CRATE_DIM, Z+CRATE_DIM, 1.0f, 1.0f,
		X-CRATE_DIM, Y-CRATE_DIM, Z-CRATE_DIM, 0.0f, 0.0f,
		X-CRATE_DIM, Y+CRATE_DIM, Z-CRATE_DIM, 0.0f, 1.0f,
		X+CRATE_DIM, Y+CRATE_DIM, Z-CRATE_DIM, 1.0f, 1.0f,
		X+CRATE_DIM, Y-CRATE_DIM, Z-CRATE_DIM, 1.0f, 0.0f
	};

	unsigned int indices[] = {  // note that we start from 0!
		0, 1, 2,   // first triangle
		2, 3, 0,    // second triangle
		3, 2, 7, 
		7, 6, 3,
		6, 7, 4,
		4, 5, 6,
		5, 4, 1,
		1, 0, 5,
		0, 3, 6,
		6, 5, 0,
		1, 4, 7,
		7, 2, 1
	};

	shadervalue.generateTBO();
	shadervalue.bindTBO();
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
	int width, height, nrChannels;
	unsigned char *data = stbi_load("res/crate.bmp", &width, &height, &nrChannels, 0);
	if (data)
	{
		shadervalue.setTBOData(GL_RGB, width, height, data);
		shadervalue.genMipMap();
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	shadervalue.generateVBO();
	shadervalue.bindVBO();
	shadervalue.setVBOData(sizeof(vertices), vertices);

	shadervalue.generateEBO();
	shadervalue.bindEBO();
	shadervalue.setEBOData(sizeof(indices), indices);

	shadervalue.use();
	shadervalue.setAttribute("aTexCoord", 2, 5, 3);
	shadervalue.setAttribute("aPos", 3, 5, 0);
	shadervalue.setMatrix("modelview", modelview_box);
	shadervalue.setMatrix("projection", projectionMat);
	shadervalue.setInt("crateTexture", 0);

	shadervalue.unbindTBO();
	shadervalue.unbindVBO();
	shadervalue.unbindEBO();

	//shadervalue.deleteVBO();
	//shadervalue.deleteEBO();
}

void setBoxConfig(Shader &shadervalue, const Vector3D& AABBMin,const Vector3D& AABBMax,float r,float g,float b)
{
	// float vertices1[] = {
    //      	-0.5f, 0.0f, 0.0f, // top right
    //     	0.0f, 1.0f, 0.0f, // bottom right
    //     	0.5f, 0.0f, 0.0f // bottom left
    // 	};

		// float vertices2[] = {
		// 	0.5f,  0.5f, 0.0f,  // top right
		// 	0.5f, -0.5f, 0.0f,  // bottom right
		// 	-0.5f, -0.5f, 0.0f,  // bottom left
		// 	-0.5f,  0.5f, 0.0f   // top left 
		// };
		unsigned int indices[] = {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};

	float vertices[] = {
         	AABBMin.x,AABBMin.y,AABBMin.z,
			AABBMax.x,AABBMin.y,AABBMin.z,
			AABBMin.x,AABBMin.y,AABBMin.z,
			AABBMin.x,AABBMax.y,AABBMin.z,
			AABBMin.x,AABBMin.y,AABBMin.z,
			AABBMin.x,AABBMin.y,AABBMax.z,
			AABBMax.x,AABBMax.y,AABBMax.z,
			AABBMin.x,AABBMax.y,AABBMax.z,
			AABBMax.x,AABBMax.y,AABBMax.z,
			AABBMax.x,AABBMin.y,AABBMax.z,
			AABBMax.x,AABBMax.y,AABBMax.z,
			AABBMax.x,AABBMax.y,AABBMin.z,
			AABBMin.x,AABBMin.y,AABBMax.z,
			AABBMax.x,AABBMin.y,AABBMax.z,
			AABBMin.x,AABBMin.y,AABBMax.z,
			AABBMin.x,AABBMax.y,AABBMax.z,
			AABBMin.x,AABBMax.y,AABBMin.z,
			AABBMax.x,AABBMax.y,AABBMin.z,
			AABBMin.x,AABBMax.y,AABBMin.z,
			AABBMin.x,AABBMax.y,AABBMax.z,
			AABBMax.x,AABBMin.y,AABBMin.z,
			AABBMax.x,AABBMax.y,AABBMin.z,
			AABBMax.x,AABBMin.y,AABBMin.z,
			AABBMax.x,AABBMin.y,AABBMax.z
    	};

	shadervalue.generateVBO();
	shadervalue.bindVBO();
	shadervalue.setVBOData(sizeof(vertices), vertices);

	shadervalue.use();

	shadervalue.setAttribute("aPos", 3, 3, 0);
	shadervalue.setMatrix("modelview", modelview_box);
	shadervalue.setMatrix("projection", projectionMat);

	shadervalue.unbindVBO();

	//shadervalue.deleteVBO();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// Main function
int main(int argc, char *argv[])
{
	cout << "Entering to main! Execution statrts here!" <<endl;

	std::chrono::time_point<std::chrono::system_clock> mainStart, mainEnd;
  
    mainStart = std::chrono::system_clock::now();

	// random seed
	randomSeed = static_cast<unsigned int>(time(NULL));

	// Sets the update step
	System::setClampStep(true,0.1f);			// clamp the step to 100 ms
	System::useAdaptiveStep(0.001f,0.01f);		// use an adaptive step from 1ms to 10ms (1000fps to 100fps)


	glfwSetErrorCallback( error_callback );

    if(!glfwInit()) 
		return -1;

    window = glfwCreateWindow(WIDTH, HEIGHT, "SPARK-RAIN", NULL, NULL);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwMakeContextCurrent(window);
	if (window == NULL) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return -1;
    }
	if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress))
	{
    	std::cout << "Failed to initialize GLAD" << std::endl;
    	return -1;
	}

	cout << "Window and context creation completed!" <<endl;
	renderFirstFrame();

	// inits openGL
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	//glDisable(GL_DEPTH);
	glDisable(GL_DEPTH_TEST);

	// Inits Particle Engine
	Vector3D gravity = Vector3D(0.0f,-2.0f,0.0f);

	// Renderers
	// the size ratio is used with renderers whose size are defined in pixels. This is to adapt to any resolution
	float sizeRatio = (float)WIDTH / (float)HEIGHT;

	//GLPointRenderer* basicRenderer = GLPointRenderer::create();

	// point renderer
	// GLPointRenderer* dropRenderer = GLPointRenderer::create();
	// dropRenderer->setType(POINT_CIRCLE);
	// dropRenderer->setSize(2.0f * sizeRatio);
	// dropRenderer->enableBlending(true);

	// line renderer
	GLESLineRenderer* rainRenderer = GLESLineRenderer::create();
	rainRenderer->setLength(-0.1f);
	rainRenderer->enableBlending(true);
	rainRenderer->setTransformations(modelview_floor, projectionMat);
	rainRenderer->setResourcePaths("", GL_RGB, "res/Shaders/Particles.vert.glsl", "res/Shaders/Particles.frag.glsl");

	setFloorTransformations();
	setBoxTransformations();
	// quad renderer
	GLESQuadRenderer* splashRenderer = GLESQuadRenderer::create();
	splashRenderer->setScale(0.05f,0.05f);
	splashRenderer->setTexturingMode(TEXTURE_2D);
	//splashRenderer->setTexture(textureSplash);
	splashRenderer->enableBlending(true);
	splashRenderer->enableRenderingHint(DEPTH_WRITE,false);
	splashRenderer->setTransformations(modelview_floor, projectionMat);
	splashRenderer->setResourcePaths("res/waterdrops.bmp", GL_RGB, "res/Shaders/Splash.vert.glsl", "res/Shaders/Splash.frag.glsl");

	GLESQuadRenderer* splashRenderer1 = GLESQuadRenderer::create();
	splashRenderer1->setScale(0.05f,0.05f);
	splashRenderer1->setTexturingMode(TEXTURE_2D);
	//splashRenderer1->setTexture(textureSplash);
	splashRenderer1->enableBlending(true);
	splashRenderer1->enableRenderingHint(DEPTH_WRITE,false);
	splashRenderer1->setTransformations(modelview_floor, projectionMat);
	splashRenderer1->setResourcePaths("res/waterdrops.bmp", GL_RGB, "res/Shaders/Splash.vert.glsl", "res/Shaders/Splash.frag.glsl");

	// Models
	// rain model
	Model* rainModel = Model::create(FLAG_GREEN | FLAG_RED | FLAG_BLUE | FLAG_ALPHA | FLAG_MASS,
		0,
		FLAG_MASS);

	rainModel->setParam(PARAM_ALPHA,0.2f);
	rainModel->setImmortal(true);

	// drop model
	Model* dropModel = Model::create(FLAG_GREEN | FLAG_RED | FLAG_BLUE | FLAG_ALPHA | FLAG_MASS,
		0,
		FLAG_MASS);

	dropModel->setParam(PARAM_ALPHA,0.6f);

	// splash model
	Model* splashModel = Model::create(FLAG_GREEN | FLAG_RED | FLAG_BLUE | FLAG_ALPHA | FLAG_SIZE | FLAG_ANGLE,
		FLAG_SIZE | FLAG_ALPHA,
		FLAG_SIZE | FLAG_ANGLE);

	splashModel->setParam(PARAM_ANGLE,0.0f,2.0f * PI);
	splashModel->setParam(PARAM_ALPHA,1.0f,0.0f);

	// rain emitter
	//AABox* rainZone = AABox::create(Vector3D(0.0f,5.0f,0.0f));
	Ring* rainZone = Ring::create(Vector3D(-2.0f,5.0f,-1.0f));
	SphericEmitter* rainEmitter = SphericEmitter::create(Vector3D(0.0f,-1.0f,0.0f),0.0f,0.03f * PI);
	rainEmitter->setZone(rainZone);

	// drop emitter
	dropEmitter = SphericEmitter::create(Vector3D(0.0f,1.0f,0.0f),0.0f,0.2f * PI);

	// the room
	box = AABox::create();
	box->setPosition(CRATE_POSITIONS);
	box->setDimension(Vector3D(CRATE_DIM2 + 0.05f,CRATE_DIM2 + 0.05f,CRATE_DIM2 + 0.05f));
	box->setTransformPosition(Vector3D(obstacleposX, obstacleposY-(obstacleposZ * glm::tan(glm::radians(obstacleangleX))),obstacleposZ));
	box->setTransformOrientationZ(obstacleangleZ);
	box->setTransformOrientationX(obstacleangleX);
	box->setTransformOrientationY(obstacleangleY);
	box->updateTransform(obstacle);

	// roof = Plane::create(Vector3D(0.0f, CRATE_DIM2, 0.0f), Vector3D(0.0f, 1.0f, 0.0f));
	// windShield = ZoneIntersection::create(2);
	// windShield->addZone(box);
	// windShield->addZone(roof);

	obstacle = Obstacle::create();
	obstacle->setZone(box);
	obstacle->setBouncingRatio(0.8f);
	obstacle->setFriction(0.95f);
	
	// obstacle->setTransformOrientationY(obstacleangleY);
	// obstacle->setTransformOrientationX(obstacleangleX);
	// obstacle->setTransformOrientationZ(obstacleangleZ);
	// obstacle->setTransformPosition(Vector3D(obstacleposX, obstacleposY-(obstacleposZ * glm::tan(glm::radians(obstacleangleX))),obstacleposZ));

	//obstacle->setTransformPosition(Vector3D(obstacleposX, obstacleposY-(obstacleposZ * glm::tan(glm::radians(obstacleangleX))),obstacleposZ));
	// obstacle->setTransformOrientationZ(obstacleangleZ);
	// obstacle->setTransformOrientationX(obstacleangleX);
	// obstacle->setTransformOrientationY(obstacleangleY);
	
	//obstacle->updateTransform(rainGroup);

	//ModifierGroup* partition = ModifierGroup::create(box,INSIDE_ZONE);
	//partition->addModifier(obstacle);
	// Groups
	// rain group
	rainGroup = Group::create(rainModel,16000);
	rainGroup->setCustomUpdate(&killRain);
	rainGroup->setRenderer(rainRenderer);
	rainGroup->addEmitter(rainEmitter);
	rainGroup->setFriction(0.7f);
	rainGroup->setGravity(gravity);
	rainGroup->addModifier(obstacle);

	// drop group
	//dropGroup = Group::create(dropModel,16000);
	//dropGroup->setRenderer(dropRenderer);
	//dropGroup->setFriction(0.7f);
	//dropGroup->setGravity(gravity);

	// splash group
	splashGroup = Group::create(splashModel,4200);
	splashGroup->setRenderer(splashRenderer);
	// splashGroup->setTransformPosition(Vector3D(-posX, -posY, -posZ));
	// splashGroup->setTransformOrientationZ(angleZ);
	// splashGroup->setTransformOrientationY(angleY);
	// splashGroup->setTransformOrientationX(angleX);
	// splashGroup->updateTransform(particleSystem);

	splashGroup1 = Group::create(splashModel,4200);
	splashGroup1->setRenderer(splashRenderer1);
	// splashGroup1->setTransformPosition(Vector3D(boxposX, boxposY-(boxposZ * glm::tan(glm::radians(boxangleX))),boxposZ));
	// splashGroup1->setTransformOrientationZ(boxangleZ);
	// splashGroup1->setTransformOrientationX(boxangleX);
	// splashGroup1->setTransformOrientationY(boxangleY);
	// splashGroup1->updateTransform(particleSystem);

	// System

	float bgColor = param(0.8f,0.2f);
	glClearColor(0.0f,0.0f,0.0f,1.0f);
	glDepthMask(GL_TRUE);

	particleSystem = System::create();
	particleSystem->addGroup(splashGroup);
	particleSystem->addGroup(splashGroup1);
	//particleSystem->addGroup(dropGroup);
	particleSystem->addGroup(rainGroup);
	
	Shader FloorShader("res/Shaders/Floor.vert.glsl", "res/Shaders/Floor.frag.glsl");
	setConfig(FloorShader);

	Shader BoxShader("res/Shaders/Box.vert.glsl", "res/Shaders/Box.frag.glsl");
	setBoxConfig(BoxShader, box->getPosition() - box->getDimension() * 0.5f,box->getPosition() + box->getDimension() * 0.5f,0.0f,1.0f,0.0f);

	Shader CrateShader("res/Shaders/Crate.vert.glsl", "res/Shaders/Crate.frag.glsl");
	setCrateConfig(CrateShader);

	bool exit = false;
	bool recompute = true;
	bool paused = false;

	// renderValue :
	// 0 : normal
	// 1 : basic render
	// 2 : no render
	unsigned int renderValue = 0;

	cout << "\nSPARK FACTORY AFTER INIT :" << endl;
	SPKFactory::getInstance().traceAll();
	// clock_gettime(CLOCK_REALTIME, &to);
	// last_t = timespec2nsec(&to);

	std::chrono::time_point<std::chrono::system_clock> start, end, prev;
  
    start = std::chrono::system_clock::now();
	prev = start;

	cout << "Going into render loop!" <<endl;

	float delta = 0.005f;
	while(!glfwWindowShouldClose(window)/*1*/)
	{
		if (!paused)
		{
			// if the ratio rate has changed, recompute the particule system parameters
			if (recompute)
			{
				strRainRate = STR_RAIN_RATE + int2Str((int)(rainRatio * 100.0f)) + "%";

				if (renderEnv)
				{
					//float bgColor = param(0.8f,0.2f);
					//glClearColor(bgColor,bgColor,bgColor,1.0f);
				}

				rainModel->setParam(PARAM_RED,param(1.0f,0.40f));
				rainModel->setParam(PARAM_GREEN,param(1.0f,0.40f));
				rainModel->setParam(PARAM_BLUE,param(1.0f,0.42f));
				rainModel->setParam(PARAM_MASS,param(0.4f,0.8f),param(0.8f,1.6f));

				dropModel->setParam(PARAM_RED,param(1.0f,0.40f));
				dropModel->setParam(PARAM_GREEN,param(1.0f,0.40f));
				dropModel->setParam(PARAM_BLUE,param(1.0f,0.42f));
				dropModel->setParam(PARAM_MASS,param(0.4f,0.8f),param(3.0f,4.0f));
				dropModel->setLifeTime(param(0.05f,0.3f),param(0.1f,0.5f));

				splashModel->setParam(PARAM_RED,param(1.0f,0.40f));
				splashModel->setParam(PARAM_GREEN,param(1.0f,0.40f));
				splashModel->setParam(PARAM_BLUE,param(1.0f,0.42f));
				splashModel->setParam(PARAM_SIZE,0.0f,0.0f,param(0.375f,2.25f),param(0.75f,3.78f));
				splashModel->setLifeTime(param(0.2f,0.3f),param(0.4f,0.5f));

				rainEmitter->setFlow(param(0.0f,9600.0f));
				rainEmitter->setForce(param(3.0f,5.0f),param(6.0f,10.0f));
				//rainZone->setRadius(0.0f,param(20.0f,5.0f));
				rainZone->setRadius(0.0f,3.0f);
				//rainZone->setDimension(Vector3D(param(40.0f,10.0f),0.0f,param(40.0f,10.0f)));

				dropEmitter->setForce(param(0.1f,1.0f),param(0.2f,2.0f));

				//dropRenderer->setSize(param(1.0f,3.0f) * sizeRatio);
				rainRenderer->setWidth(param(1.0f,4.0f) * sizeRatio);
				recompute = false;
			}

			CRATE_POSITIONS.x -= delta;
			box->setPosition(CRATE_POSITIONS);
			setBoxConfig(BoxShader, box->getPosition() - box->getDimension() * 0.5f,box->getPosition() + box->getDimension() * 0.5f,0.0f,1.0f,0.0f);
			setCrateConfig(CrateShader);

			obstacleposX -= delta;
			if(abs(obstacleposX) >= 1.0f) 
			{
				obstacleposX = obstacleposX < 0.0f ? -1.0f : 1.0f;
				delta *= -1.0f;
			}

			box->setTransformPosition(Vector3D(obstacleposX, obstacleposY-(obstacleposZ * glm::tan(glm::radians(obstacleangleX))),obstacleposZ));
			box->setTransformOrientationZ(obstacleangleZ);
			box->setTransformOrientationX(obstacleangleX);
			box->setTransformOrientationY(obstacleangleY);
			box->updateTransform(obstacle);

			//obstacle->setTransformPosition(Vector3D(obstacleposX, obstacleposY-(obstacleposZ * glm::tan(glm::radians(obstacleangleX))),obstacleposZ));
			// obstacle->setTransformOrientationZ(obstacleangleZ);
			// obstacle->setTransformOrientationX(obstacleangleX);
			// obstacle->setTransformOrientationY(obstacleangleY);
			//obstacle->updateTransform(rainGroup);

			// Updates particle system
			particleSystem->update(deltaTime * 0.001f); // 1 defined as a second
		}

		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//drawBox(box->getPosition() - box->getDimension() * 0.5f,box->getPosition() + box->getDimension() * 0.5f,0.0f,1.0f,0.0f);
		//cout << "Rain model creation/system update done!" <<endl;
		// Renders scene
		//cout << "Calling acatual render routine to draw floor and rain!" <<endl;
		render(FloorShader);
		renderBox(BoxShader);
		renderCrate(CrateShader);
		particleSystem->render();
		glfwSwapBuffers(window);

		frames++;

		end = std::chrono::system_clock::now();
		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
		deltaTime = (std::chrono::duration_cast<std::chrono::milliseconds>(end - prev)).count();
		prev = end;
		strNbParticles = STR_NB_PARTICLES + int2Str(particleSystem->getNbParticles());
		if (milliseconds.count() >= 5000LL) 
		{
			cout << endl << "Milliseconds per frame: "<< deltaTime << endl;
			cout << endl << strNbParticles<< endl;
			printf("%d frames in %6.3f seconds = %6.3f FPS\n",
			frames, 0.001f * milliseconds.count(), 1000.0f * frames / milliseconds.count());			
			start = end;
			frames = 0;
		}
	}

	DestroySPKFactory();

		mainEnd = std::chrono::system_clock::now();
		auto TotalTimeInMS = std::chrono::duration_cast<std::chrono::milliseconds>(mainEnd - mainStart);

		cout << "\nTotal execution time in seconds is: "<< TotalTimeInMS.count() * 0.001f << endl;
	glfwTerminate();
	return 0;
}