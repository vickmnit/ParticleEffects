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


#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
//#include <ctime>

#if defined(WIN32) || defined(_WIN32)
#include <windows.h>
#endif
//#include <GL/gl.h>
//#include <GL/glu.h>
#include <chrono>

#include "Core/SPK_Shader.h"
#include <glad/glad.h>
#define GLFW_INCLUDE_ES2
#include <GLFW/glfw3.h>
GLFWwindow* window = nullptr;
#include <errno.h>

#include "SPK.h"
#include "SPK_GLES.h"
#include "stb_image.h"

using namespace std;
using namespace SPK;
using namespace SPK::GLES;

static const GLuint WIDTH = 800;
static const GLuint HEIGHT = 600;

float angleZ = 0.0f;
float angleY = 0.0f;
float angleX = 0.0f;
float camPosZ = -10.0f;
float camPosY = 0.0f;
float camPosX = 0.0f;
float fov = 45.0f;
float near = 0.01f;
float far = 80.0f;

int deltaTime = 0;

//FTGLTextureFont* fontPtr;

System* particleSystem;
const float PI = 3.14159265358979323846f;

const string STR_NB_PARTICLES = "NB PARTICLES : ";
const string STR_FPS = "FPS : ";

string strNbParticles = STR_NB_PARTICLES;
string strFps = STR_FPS;

int screenWidth;
int screenHeight;
float screenRatio;

int drawText = 2;

GLuint textureWall;
GLuint textureFloor;
GLuint textureCrate;
GLuint textureParticle;

const GLuint DISPLAY_LIST_WORLD = 1;

const float ROOM_X = 7.0f;
const float ROOM_Y = 3.0f;
const float ROOM_Z = 9.0f;

const float CRATE_DIM2 = 0.5f;
const float CRATE_DIM = CRATE_DIM2 * 0.5f;

const size_t NB_CRATES = 5;
//SDL_Window* Window = nullptr;
AABox* boxes[NB_CRATES + 3];
float forceMin = 3.5f, forceMax = 3.5f;
int frames = 0;
// Obstacles
Obstacle* obstacles[1 + NB_CRATES];

Vector3D CRATE_POSITIONS[NB_CRATES] =
{
	Vector3D(0.0f,CRATE_DIM,0.0f),
	Vector3D(0.0f,CRATE_DIM + CRATE_DIM2,0.0f),
	Vector3D(0.0f,CRATE_DIM + CRATE_DIM2 * 2.0f,0.0f),
	Vector3D(0.0f,CRATE_DIM + CRATE_DIM2 * 3.0f,0.0f),
	Vector3D(0.0f,CRATE_DIM + CRATE_DIM2 * 4.0f,0.0f),
};

Vector3D vMin0(10000,10000,10000);
Vector3D vMin1(10000,10000,10000);
Vector3D vMax0(-10000,-10000,-10000);
Vector3D vMax1(-10000,-10000,-10000);

bool enableBoxDrawing = false;
bool renderEnv = true;

// Converts an int into a string
string int2Str(int a)
{
    ostringstream stm;
    stm << a;
    return stm.str();
}
/*
// Loads a texture
bool loadTexture(GLuint& index,char* path,GLuint type,GLuint clamp,bool mipmap)
{
	SDL_Surface *particleImg; 
	particleImg = SDL_LoadBMP(path);
	if (particleImg == NULL)
	{
		cout << "Unable to load bitmap :" << SDL_GetError() << endl;
		return false;
	}

	// converts from BGR to RGB
	if ((type == GL_RGB)||(type == GL_RGBA))
	{
		const int offset = (type == GL_RGB ? 3 : 4);
		unsigned char* iterator = static_cast<unsigned char*>(particleImg->pixels);
		unsigned char *tmp0,*tmp1;
		for (int i = 0; i < particleImg->w * particleImg->h; ++i)
		{
			tmp0 = iterator;
			tmp1 = iterator + 2;
			swap(*tmp0,*tmp1);
			iterator += offset;
		}
	}

	glGenTextures(1,&index);
	glBindTexture(GL_TEXTURE_2D,index);

	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,clamp);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,clamp);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	if (mipmap)
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

		gluBuild2DMipmaps(GL_TEXTURE_2D,
			type,
			particleImg->w,
			particleImg->h,
			type,
			GL_UNSIGNED_BYTE,
			particleImg->pixels);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D,
			0,
			type,
			particleImg->w,
			particleImg->h,
			0,
			type,
			GL_UNSIGNED_BYTE,
			particleImg->pixels);
	}

	SDL_FreeSurface(particleImg);

	return true;
}

void drawBox(const Vector3D& AABBMin,const Vector3D& AABBMax,float r,float g,float b)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(r,g,b);

	glVertex3f(AABBMin.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMax.x,AABBMin.y,AABBMin.z);
	
	glVertex3f(AABBMin.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMin.x,AABBMax.y,AABBMin.z);

	glVertex3f(AABBMin.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMin.x,AABBMin.y,AABBMax.z);

	glVertex3f(AABBMax.x,AABBMax.y,AABBMax.z);
	glVertex3f(AABBMin.x,AABBMax.y,AABBMax.z);

	glVertex3f(AABBMax.x,AABBMax.y,AABBMax.z);
	glVertex3f(AABBMax.x,AABBMin.y,AABBMax.z);

	glVertex3f(AABBMax.x,AABBMax.y,AABBMax.z);
	glVertex3f(AABBMax.x,AABBMax.y,AABBMin.z);

	glVertex3f(AABBMin.x,AABBMin.y,AABBMax.z);
	glVertex3f(AABBMax.x,AABBMin.y,AABBMax.z);

	glVertex3f(AABBMin.x,AABBMin.y,AABBMax.z);
	glVertex3f(AABBMin.x,AABBMax.y,AABBMax.z);

	glVertex3f(AABBMin.x,AABBMax.y,AABBMin.z);
	glVertex3f(AABBMax.x,AABBMax.y,AABBMin.z);

	glVertex3f(AABBMin.x,AABBMax.y,AABBMin.z);
	glVertex3f(AABBMin.x,AABBMax.y,AABBMax.z);

	glVertex3f(AABBMax.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMax.x,AABBMax.y,AABBMin.z);

	glVertex3f(AABBMax.x,AABBMin.y,AABBMin.z);
	glVertex3f(AABBMax.x,AABBMin.y,AABBMax.z);
	glEnd();
}

// Draws the bounding box for a particle group
void drawBoundingBox(const System& system)
{
	if (!system.isAABBComputingEnabled())
		return;

	drawBox(system.getAABBMin(),system.getAABBMax(),1.0f,0.0f,0.0f);
}

// Creates the display lists for the world
void createDisplayLists()
{
	const float TEXTURE_SIZE = 2.0f;

	float X = ROOM_X * 0.5f;
	float Y = ROOM_Y;
	float Z = ROOM_Z * 0.5f;

	glNewList(DISPLAY_LIST_WORLD,GL_COMPILE);

	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	// floor
	glBindTexture(GL_TEXTURE_2D,textureFloor);
	glBegin(GL_QUADS);
	glColor3f(0.6f,0.6f,0.6f);
	glTexCoord2f(X / TEXTURE_SIZE,Z / TEXTURE_SIZE);
	glVertex3f(X,0.0f,Z);
	glTexCoord2f(X / TEXTURE_SIZE,-Z / TEXTURE_SIZE);
	glVertex3f(X,0.0f,-Z);
	glTexCoord2f(-X / TEXTURE_SIZE,-Z / TEXTURE_SIZE);
	glVertex3f(-X,0.0f,-Z);
	glTexCoord2f(-X / TEXTURE_SIZE,Z / TEXTURE_SIZE);
	glVertex3f(-X,0.0f,Z);
	glEnd();

	// walls and ceiling
	glBindTexture(GL_TEXTURE_2D,textureWall);
	glBegin(GL_QUADS);

	glColor3f(0.2f,0.2f,0.2f);
	glTexCoord2f(X / TEXTURE_SIZE,Y / TEXTURE_SIZE);
	glVertex3f(X,Y,Z);
	glTexCoord2f(X / TEXTURE_SIZE,0.0F);
	glVertex3f(X,0.0f,Z);
	glTexCoord2f(-X / TEXTURE_SIZE,0.0F);
	glVertex3f(-X,0.0f,Z);
	glTexCoord2f(-X / TEXTURE_SIZE,Y / TEXTURE_SIZE);
	glVertex3f(-X,Y,Z);

	glColor3f(0.6f,0.6f,0.6f);
	glTexCoord2f(-X / TEXTURE_SIZE,Y / TEXTURE_SIZE);
	glVertex3f(-X,Y,-Z);
	glTexCoord2f(-X / TEXTURE_SIZE,0.0F);
	glVertex3f(-X,0.0f,-Z);
	glTexCoord2f(X / TEXTURE_SIZE,0.0F);
	glVertex3f(X,0.0f,-Z);
	glTexCoord2f(X / TEXTURE_SIZE,Y / TEXTURE_SIZE);
	glVertex3f(X,Y,-Z);

	glColor3f(0.4f,0.4f,0.4f);
	glTexCoord2f(Y / TEXTURE_SIZE,-Z / TEXTURE_SIZE);
	glVertex3f(X,Y,-Z);
	glTexCoord2f(0.0f,-Z / TEXTURE_SIZE);
	glVertex3f(X,0.0f,-Z);
	glTexCoord2f(0.0f,Z / TEXTURE_SIZE);
	glVertex3f(X,0.0f,Z);
	glTexCoord2f(Y / TEXTURE_SIZE,Z / TEXTURE_SIZE);
	glVertex3f(X,Y,Z);

	glColor3f(0.4f,0.4f,0.4f);
	glTexCoord2f(Y / TEXTURE_SIZE,Z / TEXTURE_SIZE);
	glVertex3f(-X,Y,Z);
	glTexCoord2f(0.0f,Z / TEXTURE_SIZE);
	glVertex3f(-X,0.0f,Z);
	glTexCoord2f(0.0f,-Z / TEXTURE_SIZE);
	glVertex3f(-X,0.0f,-Z);
	glTexCoord2f(Y / TEXTURE_SIZE,-Z / TEXTURE_SIZE);
	glVertex3f(-X,Y,-Z);

	glColor3f(0.3f,0.3f,0.3f);
	glTexCoord2f(-X / TEXTURE_SIZE,Z / TEXTURE_SIZE);
	glVertex3f(-X,Y,Z);
	glTexCoord2f(-X / TEXTURE_SIZE,-Z / TEXTURE_SIZE);
	glVertex3f(-X,Y,-Z);
	glTexCoord2f(X / TEXTURE_SIZE,-Z / TEXTURE_SIZE);
	glVertex3f(X,Y,-Z);
	glTexCoord2f(X / TEXTURE_SIZE,Z / TEXTURE_SIZE);
	glVertex3f(X,Y,Z);
	glEnd();

	glEndList();
}


void DrawCrates()
{
	float X = ROOM_X * 0.5f;
	float Y = ROOM_Y;
	float Z = ROOM_Z * 0.5f;
// crate
	glBindTexture(GL_TEXTURE_2D,textureCrate);
	glBegin(GL_QUADS);
	for (size_t i = 0; i < NB_CRATES; ++i)
	{
		X = CRATE_POSITIONS[i].x;
		Y = CRATE_POSITIONS[i].y;
		Z = CRATE_POSITIONS[i].z;
		
		glColor3f(0.2f,0.2f,0.2f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(X + CRATE_DIM,Y + CRATE_DIM,Z - CRATE_DIM);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(X + CRATE_DIM,Y - CRATE_DIM,Z - CRATE_DIM);
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(X - CRATE_DIM,Y - CRATE_DIM,Z - CRATE_DIM);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(X - CRATE_DIM,Y + CRATE_DIM,Z - CRATE_DIM);

		glColor3f(1.0f,1.0f,1.0f);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(X - CRATE_DIM,Y + CRATE_DIM,Z + CRATE_DIM);
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(X -CRATE_DIM,Y - CRATE_DIM,Z + CRATE_DIM);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(X + CRATE_DIM,Y - CRATE_DIM,Z + CRATE_DIM);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(X + CRATE_DIM,Y + CRATE_DIM,Z + CRATE_DIM);

		glColor3f(0.6f,0.6f,0.6f);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(X - CRATE_DIM,Y + CRATE_DIM,Z - CRATE_DIM);
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(X - CRATE_DIM,Y - CRATE_DIM,Z - CRATE_DIM);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(X - CRATE_DIM,Y - CRATE_DIM,Z + CRATE_DIM);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(X - CRATE_DIM,Y + CRATE_DIM,Z + CRATE_DIM);

		glColor3f(0.6f,0.6f,0.6f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(X + CRATE_DIM,Y + CRATE_DIM,Z + CRATE_DIM);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(X + CRATE_DIM,Y - CRATE_DIM,Z + CRATE_DIM);
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(X + CRATE_DIM,Y - CRATE_DIM,Z - CRATE_DIM);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(X + CRATE_DIM,Y + CRATE_DIM,Z - CRATE_DIM);

		glColor3f(0.8f,0.8f,0.8f);
		glTexCoord2f(1.0f,1.0f);
		glVertex3f(X + CRATE_DIM,Y + CRATE_DIM,Z + CRATE_DIM);
		glTexCoord2f(1.0f,0.0f);
		glVertex3f(X + CRATE_DIM,Y + CRATE_DIM,Z - CRATE_DIM);
		glTexCoord2f(0.0f,0.0f);
		glVertex3f(X - CRATE_DIM,Y + CRATE_DIM,Z - CRATE_DIM);
		glTexCoord2f(0.0f,1.0f);
		glVertex3f(X - CRATE_DIM,Y + CRATE_DIM,Z + CRATE_DIM);
	}
	glEnd();
}

// Renders the scene
void render()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45,screenRatio,0.01f,80.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();                       

	glTranslatef(0.0f,0.0f,camPosZ);
	glRotatef(angleX,1.0f,0.0f,0.0f);
	glRotatef(angleY,0.0f,1.0f,0.0f);

	if (renderEnv)
	{
		glDisable(GL_BLEND);
		glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
		glCallList(DISPLAY_LIST_WORLD);
	}

	DrawCrates();
	drawBoundingBox(*particleSystem);

	if (enableBoxDrawing)
	{
		for (int i = 0; i < NB_CRATES + 1; ++i)
			drawBox(boxes[i]->getPosition() - boxes[i]->getDimension() * 0.5f,boxes[i]->getPosition() + boxes[i]->getDimension() * 0.5f,0.0f,0.0f,1.0f);
		for (int i = NB_CRATES + 1; i < NB_CRATES + 3; ++i)
			drawBox(boxes[i]->getPosition() - boxes[i]->getDimension() * 0.5f,boxes[i]->getPosition() + boxes[i]->getDimension() * 0.5f,0.0f,1.0f,0.0f);
	}

	particleSystem->render();

	if (drawText != 0)
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluOrtho2D(0,screenWidth,0,screenHeight);

		// Renders info strings
		glDisable(GL_DEPTH_TEST);
		glColor3f(1.0f,1.0f,1.0f);
		if (drawText == 2)
			fontPtr->Render(strNbParticles.c_str(),-1,FTPoint(4.0f,40.0f));
		fontPtr->Render(strFps.c_str(),-1,FTPoint(4.0f,8.0f));
	}

	SDL_GL_SwapWindow(Window);
}
*/

void renderFirstFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glfwSwapBuffers(window);
	glfwSwapBuffers(window);
}

// This call back will assign a color to a particle (either red,green,blue or yellow)
void assignColor(Particle& particle)
{
	float alpha = atan(0.3f);
	float theta = atan(abs(CRATE_POSITIONS[0].x));
	//cout<<"theta: " <<theta << endl << "alpha: " << alpha << endl;
	float dx = 0.0f;
	if(theta > alpha)
		dx = abs(abs(CRATE_POSITIONS[0].x) - abs(tan(theta - alpha)));
	else
		dx = abs(abs(CRATE_POSITIONS[0].x) + abs(tan(alpha - theta)));
	// float min = CRATE_POSITIONS[0].x - dx;
	// min = min < -1.0f ? -1.0f : min;
	// min = min > 1.0f ? 1.0f : min;
	// float max = CRATE_POSITIONS[0].x + dx;
	// max = max < -1.0f ? -1.0f : max;
	// max = max > 1.0f ? 1.0f : max;
	float norm;
	//do 
	{
		particle.velocity().set(0.0f,-0.5f,random(1.0f,0.8f));
		//particle.velocity().set(random(CRATE_POSITIONS[0].x - dx, CRATE_POSITIONS[0].x + dx), 0.0f, random(-1.0f,-0.8f));
		norm = particle.velocity().getNorm();
	}
	//while((norm > 1.0f) || (norm == 0.0f));
	float speed = random(forceMin,forceMax) / particle.getParamCurrentValue(PARAM_MASS);
	particle.velocity() *= speed / norm;

	//particle.setParamCurrentValue(PARAM_RED,1.0f);
	//particle.setParamCurrentValue(PARAM_GREEN,0.0f);
	//particle.setParamCurrentValue(PARAM_BLUE,0.0f);
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

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void error_callback( int error, const char *msg ) {
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
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
	// screenWidth = screen.w;
	// screenHeight = screen.h;
	// screenRatio = (float)screen.w / (float)screen.h;
	
	glClearColor(0.3f,0.3f,0.3f,1.0f);
	glViewport(0,0,WIDTH, HEIGHT);

    /*
	// Loads texture font
	FTGLTextureFont font = FTGLTextureFont("res/font.ttf");
	if(font.Error())
		return 1;
	font.FaceSize(24);
	fontPtr = &font;

	// Loads crate texture
	if (!loadTexture(textureCrate,"res/crate.bmp",GL_RGB,GL_REPEAT,true))
		return 1;

	// Loads floor texture
	if (!loadTexture(textureFloor,"res/floor.bmp",GL_RGB,GL_REPEAT,true))
		return 1;

	// Loads wall texture
	if (!loadTexture(textureWall,"res/wall.bmp",GL_RGB,GL_REPEAT,true))
		return 1;

	// Creates display lists
	createDisplayLists();
    */

	// Inits Particle Engine
	Vector3D gravity(0.0f,-0.3f,0.0f);
    
	// Renderer
	//GLESPointRenderer* basicRenderer = GLESPointRenderer::create();

	//GLRenderer* particleRenderer = NULL;
	//if (!loadTexture(textureParticle,"res/explosion.bmp",GL_ALPHA,GL_CLAMP,false))
	//		return 1;

	GLESQuadRenderer* particleRenderer = GLESQuadRenderer::create();
	particleRenderer->setScale(0.5f,0.5f);
	particleRenderer->setTexturingMode(TEXTURE_2D);
	//particleRenderer->setTexture(textureParticle);
	//particleRenderer->setTextureBlending(GL_MODULATE);
	particleRenderer->setBlending(BLENDING_ALPHA);
	particleRenderer->enableRenderingHint(DEPTH_TEST,false);
	particleRenderer->setAtlasDimensions(2,2);
	particleRenderer->setTransformationParams(camPosX, camPosY, camPosZ, angleX, angleY, angleZ, fov, WIDTH, HEIGHT, near, far);
	particleRenderer->setResourcePaths("res/air.png", GL_RGBA, "res/Shaders/Splash.vert.glsl", "res/Shaders/Splash.frag.glsl");

	Model* particleModel = Model::create(FLAG_ALPHA | FLAG_SIZE | FLAG_ANGLE | FLAG_TEXTURE_INDEX, FLAG_SIZE | FLAG_ANGLE,
		FLAG_TEXTURE_INDEX | FLAG_ANGLE,
		FLAG_ALPHA);

	particleModel->setParam(PARAM_ALPHA,1.0f,1.0f);
	particleModel->setParam(PARAM_SIZE,1.0,1.0f);
	particleModel->setParam(PARAM_TEXTURE_INDEX,0.0f,4.0f);
	particleModel->setParam(PARAM_ANGLE,0.0f,2.0f * PI,0.0f,2.0f * PI);
	particleModel->setLifeTime(3.5f, 3.5f);

	// Emitter
	int n = 15; 
	if(n%2==0) n-=1; //to keep n odd number
	int mid = (n/2);
	RandomEmitter* particleEmitter = RandomEmitter::create();
	particleEmitter->setZone(Line::create(Vector3D(-0.1f * (float)mid,2.0f,4.475f), Vector3D(0.1f * (float)mid,2.0f,4.475f)));
	particleEmitter->setFlow(500);
	particleEmitter->setForce(forceMin,forceMax);

	// the room
	boxes[0] = AABox::create();
	boxes[0]->setPosition(Vector3D(0.0f,ROOM_Y * 0.5f,0.0f));
	boxes[0]->setDimension(Vector3D(ROOM_X - 0.05f,ROOM_Y - 0.05f,ROOM_Z - 0.05f));
	
	obstacles[0] = Obstacle::create();
	obstacles[0]->setZone(boxes[0]);
	obstacles[0]->setBouncingRatio(0.1f);
	obstacles[0]->setFriction(0.1f);

	for (int i = 1; i < 1 + NB_CRATES; ++i)
	{
		boxes[i] = AABox::create();
		boxes[i]->setPosition(CRATE_POSITIONS[i - 1]);
		boxes[i]->setDimension(Vector3D(CRATE_DIM2 + 0.05f,CRATE_DIM2 + 0.05f,CRATE_DIM2 + 0.05f));
		
		obstacles[i] = Obstacle::create();
		obstacles[i]->setZone(boxes[i]);
		obstacles[i]->setBouncingRatio(0.1f);
		obstacles[i]->setFriction(0.1f);
	}

	for (int i = 0; i < NB_CRATES - 1; ++i)
	{
		if (CRATE_POSITIONS[i].x < vMin0.x)
			vMin0.x = CRATE_POSITIONS[i].x;
		if (CRATE_POSITIONS[i].x > vMax0.x)
			vMax0.x = CRATE_POSITIONS[i].x;

		if (CRATE_POSITIONS[i].y < vMin0.y)
			vMin0.y = CRATE_POSITIONS[i].y;
		if (CRATE_POSITIONS[i].y > vMax0.y)
			vMax0.y = CRATE_POSITIONS[i].y;

		if (CRATE_POSITIONS[i].z < vMin0.z)
			vMin0.z = CRATE_POSITIONS[i].z;
		if (CRATE_POSITIONS[i].z > vMax0.z)
			vMax0.z = CRATE_POSITIONS[i].z;
	}

	vMin1.x = min(vMin0.x,CRATE_POSITIONS[NB_CRATES - 1].x);
	vMax1.x = max(vMax0.x,CRATE_POSITIONS[NB_CRATES - 1].x);

	vMin1.y = min(vMin0.y,CRATE_POSITIONS[NB_CRATES - 1].y);
	vMax1.y = max(vMax0.y,CRATE_POSITIONS[NB_CRATES - 1].y);

	vMin1.z = min(vMin0.z,CRATE_POSITIONS[NB_CRATES - 1].z);
	vMax1.z = max(vMax0.z,CRATE_POSITIONS[NB_CRATES - 1].z);

	// partition zones
	boxes[NB_CRATES + 1] = AABox::create();
	boxes[NB_CRATES + 1]->setPosition(Vector3D((vMin0 + vMax0) / 2.0f));
	boxes[NB_CRATES + 1]->setDimension(Vector3D(vMax0 - vMin0 + CRATE_DIM2 + 0.05f));
	ModifierGroup* partition0 = ModifierGroup::create(boxes[NB_CRATES + 1],INSIDE_ZONE);

	boxes[NB_CRATES + 2] = AABox::create();
	boxes[NB_CRATES + 2]->setPosition(Vector3D((vMin1 + vMax1) / 2.0f));
	boxes[NB_CRATES + 2]->setDimension(Vector3D(vMax1 - vMin1 + CRATE_DIM2 + 0.05f));
	ModifierGroup* partition1 = ModifierGroup::create(boxes[NB_CRATES + 2],INSIDE_ZONE);

	partition0->addModifier(obstacles[1]);
	partition0->addModifier(obstacles[2]);
	partition0->addModifier(obstacles[3]);
	partition0->addModifier(obstacles[4]);

	partition1->addModifier(partition0);
	partition1->addModifier(obstacles[NB_CRATES]);

	// Group
	Group* particleGroup = Group::create(particleModel,31000);

	PointMass* pointMasses[NB_CRATES];
	for (int i = 0; i < NB_CRATES; ++i)
	{
		pointMasses[i] = PointMass::create();
		pointMasses[i]->setZone(boxes[i+1], true);
		pointMasses[i]->setMass(-40.0f);
		pointMasses[i]->setMinDistance(0.0f);
		//particleGroup->addModifier(pointMasses[i]);
	}

	particleGroup->addEmitter(particleEmitter);
	particleGroup->setRenderer(particleRenderer);
	particleGroup->setCustomBirth(&assignColor);
	particleGroup->setGravity(gravity);
	particleGroup->setFriction(0.2f);
	
	particleSystem = System::create();
	particleSystem->addGroup(particleGroup);
	
	bool exit = false;
	bool paused = false;

	// renderValue :
	// 0 : normal
	// 1 : basic render
	// 2 : no render
	unsigned int renderValue = 0;

	cout << "\nSPARK FACTORY AFTER INIT :" << endl;
	SPKFactory::getInstance().traceAll();
	
	std::chrono::time_point<std::chrono::system_clock> start, end, prev;
  
    start = std::chrono::system_clock::now();
	prev = start;

	cout << "Going into render loop!" <<endl;

	while(!glfwWindowShouldClose(window))
	{
		/*
		while (SDL_PollEvent(&event))
		{
			// if esc is pressed, exit
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_ESCAPE))
				exit = true;

			// if del is pressed, reinit the system
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_DELETE))
				particleSystem->empty();

			// if F1 is pressed, we display or not the text
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_F1))
			{
				--drawText;
				if (drawText < 0)
					drawText = 2;
			}

			// if F2 is pressed, we display or not the bounding boxes
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_F2))
			{
				particleGroup->enableAABBComputing(!particleGroup->isAABBComputingEnabled());
				
				if (paused)
					particleSystem->computeAABB();
			}
	
			// if F4 is pressed, the renderers are changed
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_F4))
			{
				renderValue = (renderValue + 1) % 3;

				switch (renderValue)
				{
				case 0 :
					particleGroup->setRenderer(particleRenderer);
					break;

				case 1 :
					particleGroup->setRenderer(basicRenderer);
					break;

				case 2 :
					particleGroup->setRenderer(NULL);
					break;
				}
			}

			// if F5 is pressed, environment is rendered are not
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_F5))
				renderEnv = !renderEnv;

			// if F6 is pressed, boxes are drawn or not
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_F6))
				enableBoxDrawing = !enableBoxDrawing;

			// if pause is pressed, the system is paused
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_PAUSE))
				paused = !paused;

			// Moves the camera with the mouse
			// if ((event.type == SDL_MOUSEMOTION))
			// {
			// 	angleY += event.motion.xrel * 0.05f;
			// 	angleX += event.motion.yrel * 0.05f;
			// 	angleX = min(179.0f,max(1.0f,angleX));	// we cannot look under the ground
			// }
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_RIGHT))
			{
				angleY += event.motion.xrel * 0.005f;
			}
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_LEFT))
			{	
				angleY -= event.motion.xrel * 0.005f;
			}
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_UP))
			{
				angleX -= event.motion.xrel * 0.005f;
				angleX = min(179.0f,max(1.0f,angleX));	// we cannot look under the ground
			}
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_DOWN))
			{
				angleX += event.motion.xrel * 0.005f;
				angleX = min(179.0f,max(1.0f,angleX));	// we cannot look under the ground
			}

			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_LCTRL))
			{
				for (size_t i = 0; i < NB_CRATES; ++i)
				{
					//CRATE_POSITIONS[i].z += 0.03f;
					CRATE_POSITIONS[i].x += 0.03f;
					boxes[i+1]->setPosition(CRATE_POSITIONS[i]);
					obstacles[i+1]->setZone(boxes[i+1]);
				}
			}
			if ((event.type == SDL_KEYDOWN)&&(event.key.keysym.sym == SDLK_RCTRL))
			{
				for (size_t i = 0; i < NB_CRATES; ++i)
				{
					//CRATE_POSITIONS[i].z -= 0.03f;
					CRATE_POSITIONS[i].x -= 0.03f;
					boxes[i+1]->setPosition(CRATE_POSITIONS[i]);
					obstacles[i+1]->setZone(boxes[i+1]);
				}
			}

			// Zoom in and out
			// if (event.type == SDL_MOUSEBUTTONDOWN)
			// {
			// 	if (event.button.button == SDL_BUTTON_WHEELDOWN)
			// 		camPosZ = min(18.0f,camPosZ + 0.5f);
			// 	if (event.button.button == SDL_BUTTON_WHEELUP)
			// 		camPosZ = max(0.5f,camPosZ - 0.5f);
			// }
		}
		*/

		if (!paused)
		{
			// Updates particle system
			particleSystem->update(deltaTime * 0.001f); // 1 defined as a second
		}

		// Renders scene
		//render();
		//glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// glMatrixMode(GL_PROJECTION);
		// glLoadIdentity();
		// gluPerspective(45,screenRatio,0.01f,80.0f);

		// glMatrixMode(GL_MODELVIEW);
		// glLoadIdentity();                       

		// glTranslatef(0.0f,0.0f,camPosZ);
		// glRotatef(angleX,1.0f,0.0f,0.0f);
		// glRotatef(angleY,0.0f,1.0f,0.0f);
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