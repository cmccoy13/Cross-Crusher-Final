/*
ZJ Wood CPE 471 Lab 3 base code
*/

#include <iostream>
#include <glad/glad.h>
#include <cstdlib>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include <math.h>
#include "vector"
#include <Windows.h>
#include <mmsystem.h>

#include "WindowManager.h"
#include "Shape.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

shared_ptr<Shape> shape;
float ballSpeed = 0.127;
static float xVel1 = ballSpeed;
static float yVel1 = ballSpeed;
static float xBall1 = 0.0;
static float yBall1 = 0.34;

static float xVel2 = -ballSpeed;
static float yVel2 = ballSpeed;
static float xBall2 = -0.34;
static float yBall2 = 0.0;

static float xVel3 = -ballSpeed;
static float yVel3 = -ballSpeed;
static float xBall3 = 0.0;
static float yBall3 = -0.34;

static float xVel4 = ballSpeed;
static float yVel4 = -ballSpeed;
static float xBall4 = 0.34;
static float yBall4 = 0.0;

vec4 ball1 = vec4(xVel1, yVel1, xBall1, yBall1);
vec4 ball2 = vec4(xVel2, yVel2, xBall2, yBall2);
vec4 ball3 = vec4(xVel3, yVel3, xBall3, yBall3);
vec4 ball4 = vec4(xVel4, yVel4, xBall4, yBall4);

std::vector<vec4> balls;

static bool uBricks[14][6];
static bool dBricks[14][6];
static bool lBricks[14][6];
static bool rBricks[14][6];

bool cheat = false;
bool cheatLose = false;

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime = glfwGetTime();
	double difference = actualtime - lasttime;
	lasttime = actualtime;
	return difference;
}

class Controls
{
public:
	int w, a, s, d, up, down, left, right;
	Controls()
	{
		w = a = s = d = up = down = left = right = 0;
	}
};

Controls controls;

class Matrix
{
public:
	float data[4][4];
	void createIdentityMatrix();
	void createTranslateMat(float x, float y, float z);
	void createScaleMat(float x, float y, float z);
	void createRotationMatX(float rot);
	void createRotationMatY(float rot);
	void createRotationMatZ(float rot);
	void multMat(Matrix A, Matrix B);
};

void Matrix::createIdentityMatrix()
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			data[i][j] = 0.0;
		}
	}

	data[0][0] = 1.0;
	data[1][1] = 1.0;
	data[2][2] = 1.0;
	data[3][3] = 1.0;
}

void Matrix::createTranslateMat(float x, float y, float z)
{
	createIdentityMatrix();

	data[3][0] = x;
	data[3][1] = y;
	data[3][2] = z;
}

void Matrix::createScaleMat(float x, float y, float z)
{
	createIdentityMatrix();

	data[0][0] = x;
	data[1][1] = y;
	data[2][2] = z;
}

void Matrix::createRotationMatX(float rot)
{
	createIdentityMatrix();

	data[1][1] = cos(rot);
	data[1][2] = -sin(rot);
	data[2][1] = sin(rot);
	data[2][2] = cos(rot);
}

void Matrix::createRotationMatY(float rot)
{
	createIdentityMatrix();

	data[0][0] = cos(rot);
	data[0][2] = sin(rot);
	data[2][0] = -sin(rot);
	data[2][2] = cos(rot);
}

void Matrix::createRotationMatZ(float rot)
{
	createIdentityMatrix();

	data[0][0] = cos(rot);
	data[0][1] = -sin(rot);
	data[1][0] = sin(rot);
	data[1][1] = cos(rot);
}

void Matrix::multMat(Matrix A, Matrix B)
{
	createIdentityMatrix();

	for (int k = 0; k < 4; k++)
	{
		for (int j = 0; j < 4; j++)
		{
			for (int i = 0; i < 4; i++)
			{
				data[j][k] += (A.data[i][k]) * (B.data[j][i]);
			}
		}
	}
}

class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> courtProg;
	std::shared_ptr<Program> brickProg;
	std::shared_ptr<Program> paddleProg;
	std::shared_ptr<Program> floorProg;
	std::shared_ptr<Program> ballProg;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexColorIDBox, IndexBufferIDBox, VertexNormDBox, VertexTexBox;

	//Texture Data
	GLuint Texture, Texture2, Texture3, Texture4;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_0 && action == GLFW_PRESS)
		{
			cheat = true;
		}
		if (key == GLFW_KEY_MINUS && action == GLFW_PRESS)
		{
			cheatLose = true;
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			controls.w = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			controls.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			controls.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			controls.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			controls.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			controls.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			controls.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			controls.d = 0;
		}
		if (key == GLFW_KEY_UP && action == GLFW_PRESS)
		{
			controls.up = 1;
			controls.down = 0;
			controls.left = 0;
			controls.right = 0;
		}
		if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
		{
			controls.down = 1;
			controls.up = 0;
			controls.left = 0;
			controls.right = 0;
		}
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		{
			controls.left = 1;
			controls.up = 0;
			controls.down = 0;
			controls.right = 0;
		}
		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		{
			controls.right = 1;
			controls.up = 0;
			controls.down = 0;
			controls.left = 0;
		}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;
		float newPt[2];
		if (action == GLFW_PRESS)
		{

		}
	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		//SPHERE
		string resourceDirectory = "../resources";
		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/sphere.obj");
		shape->resize();
		shape->init();
		//END SPHERE

		//Initialize balls vector
		balls.push_back(ball1);
		balls.push_back(ball2);
		balls.push_back(ball3);
		balls.push_back(ball4);

		//Initialize brick locations
		for (int i = 0; i < 14; i++)
		{
			for (int j = 0; j < 6; j++)
			{
				if (i < 5)
				{
					uBricks[i][j] = true;
					dBricks[i][j] = true;
					lBricks[i][j] = true;
					rBricks[i][j] = true;
				}
				else
				{
					uBricks[i][j] = false;
					dBricks[i][j] = false;
					lBricks[i][j] = false;
					rBricks[i][j] = false;
				}

			}
		}

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		GLfloat cube_vertices[] = {
			// front
			-1.0, -1.0,  1.0,
			1.0, -1.0,  1.0,
			1.0,  1.0,  1.0,
			-1.0,  1.0,  1.0,
			// back
			-1.0, -1.0, -1.0,
			1.0, -1.0, -1.0,
			1.0,  1.0, -1.0,
			-1.0,  1.0, -1.0,
		};
		//make it a bit smaller
		for (int i = 0; i < 24; i++)
			cube_vertices[i] *= 0.5;
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		//color
		GLfloat cube_colors[] = {
			// front colors
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 1.0,
			1.0, 1.0, 1.0,
			// back colors
			1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, 0.0, 1.0,
			1.0, 1.0, 1.0,
		};
		glGenBuffers(1, &VertexColorIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexColorIDBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		GLushort cube_elements[] = {
			// front
			0, 1, 2,
			2, 3, 0,
			// top
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// bottom
			4, 0, 3,
			3, 7, 4,
			// left
			4, 5, 1,
			1, 0, 4,
			// right
			3, 2, 6,
			6, 7, 3,
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

		
		//color
		GLfloat cube_norm[] = {
			// front colors
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,
			0.0, 0.0, 1.0,

		};
		glGenBuffers(1, &VertexNormDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexNormDBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_norm), cube_norm, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		

		//color
		glm::vec2 cube_tex[] = {
			// front colors
			glm::vec2(0.0, 1.0),
			glm::vec2(1.0, 1.0),
			glm::vec2(1.0, 0.0),
			glm::vec2(0.0, 0.0),
			glm::vec2(0.0, 0.0),
			glm::vec2(0.0, 0.0),
			glm::vec2(0.0, 0.0),
			glm::vec2(0.0, 0.0)
		};
		glGenBuffers(1, &VertexTexBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexTexBox);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_tex), cube_tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);

		int width, height, channels;
		char filepath[1000];

		//texture 1
		//string str = resourceDirectory + "/05-Breakout-Tiles.png";
		string str = resourceDirectory + "/metal1.png";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture 2
		//str = resourceDirectory + "/15-Breakout-Tiles.png";
		str = resourceDirectory + "/metal1.png";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture 3
		//str = resourceDirectory + "/07-Breakout-Tiles.png";
		str = resourceDirectory + "/metal1.png";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture3);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture3);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//texture 4
		//str = resourceDirectory + "/11-Breakout-Tiles.png";
		str = resourceDirectory + "/metal1.png";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture4);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture4);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
		GLuint Tex1Location = glGetUniformLocation(brickProg->pid, "tex");//tex, tex2... sampler in the fragment shader
		GLuint Tex2Location = glGetUniformLocation(brickProg->pid, "tex2");
		GLuint Tex3Location = glGetUniformLocation(brickProg->pid, "tex3");
		GLuint Tex4Location = glGetUniformLocation(brickProg->pid, "tex4");

		// Then bind the uniform samplers to texture units:
		glUseProgram(brickProg->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 0);
		glUniform1i(Tex3Location, 0);
		glUniform1i(Tex4Location, 0);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		courtProg = std::make_shared<Program>();
		courtProg->setVerbose(true);
		courtProg->setShaderNames(resourceDirectory + "/shader_vertex_court.glsl", resourceDirectory + "/shader_fragment_court.glsl");
		courtProg->init();
		courtProg->addUniform("P");
		courtProg->addUniform("V");
		courtProg->addUniform("M");
		courtProg->addAttribute("vertPos");
		courtProg->addAttribute("vertColor");

		brickProg = std::make_shared<Program>();
		brickProg->setVerbose(true);
		brickProg->setShaderNames(resourceDirectory + "/shader_vertex_brick.glsl", resourceDirectory + "/shader_fragment_brick.glsl");
		brickProg->init();
		brickProg->addUniform("P");
		brickProg->addUniform("V");
		brickProg->addUniform("M");
		brickProg->addAttribute("vertPos");
		brickProg->addAttribute("vertNor");
		brickProg->addAttribute("vertTex");

		paddleProg = std::make_shared<Program>();
		paddleProg->setVerbose(true);
		paddleProg->setShaderNames(resourceDirectory + "/shader_vertex_paddle.glsl", resourceDirectory + "/shader_fragment_paddle.glsl");
		paddleProg->init();
		paddleProg->addUniform("P");
		paddleProg->addUniform("V");
		paddleProg->addUniform("M");
		paddleProg->addAttribute("vertPos");
		paddleProg->addAttribute("vertColor");

		floorProg = std::make_shared<Program>();
		floorProg->setVerbose(true);
		floorProg->setShaderNames(resourceDirectory + "/shader_vertex_floor.glsl", resourceDirectory + "/shader_fragment_floor.glsl");
		floorProg->init();
		floorProg->addUniform("P");
		floorProg->addUniform("V");
		floorProg->addUniform("M");
		floorProg->addAttribute("vertPos");
		floorProg->addAttribute("vertColor");

		ballProg = std::make_shared<Program>();
		ballProg->setVerbose(true);
		ballProg->setShaderNames(resourceDirectory + "/shader_vertex_ball.glsl", resourceDirectory + "/shader_fragment_ball.glsl");
		ballProg->init();
		ballProg->addUniform("P");
		ballProg->addUniform("V");
		ballProg->addUniform("M");
		ballProg->addAttribute("vertPos");
		ballProg->addAttribute("vertNor");
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void drawCourt(glm::mat4 P, glm::mat4 V, glm::mat4 M)
	{
		float w = 3.141592769 / 2;

		courtProg->bind();

		glm::mat4 Rotate;
		glm::mat4 Scale;
		glm::mat4 Trans;
		glm::mat4 TransView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3));

		glUniformMatrix4fv(courtProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);

		glBindVertexArray(VertexArrayID);

		//TOP COURT
		//Top
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.1f, 0.2f));
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.1f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//L
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.35f, 0.75f, 0.0f));
		Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//R
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.35f, 0.75f, 0.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);



		//BOTTOM COURT
		//Bot
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.1f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//L
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.35f, -0.75f, 0.0f));
		Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//R
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.35f, -0.75f, 0.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//LEFT Court
		//L
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-1.1f, 0.0f, 0.0f));
		Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//Top
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.7f, 0.35f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//Bot
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.7f, -0.35f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);



		//RIGHT COURT
		//R
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(1.1f, 0.0f, 0.0f));
		Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//Top
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, 0.35f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//Bot
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.7f, -0.35f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		courtProg->unbind();



		floorProg->bind();

		glUniformMatrix4fv(courtProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);

		glBindVertexArray(VertexArrayID);

		//TOP FLOOR
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.90f, 0.01f));
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.764f, -0.2f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//BOTTOM FLOOR
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.764f, -0.2f));
		M = Trans*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//LEFT FLOOR
		Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.764f, 0.0f, -0.2f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//RIGHT FLOOR
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.764f, 0.0f, -0.2f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		floorProg->unbind();
	}

	void processCollisions(float upad, float dpad, float lpad, float rpad, double frametime, float frameCounter)
	{
		for (int i = 0; i < 4; i++)
		{
			if (frameCounter >= i)
			{
				if (frameCounter < i +.005)
				{
					PlaySound(TEXT("../resources/ballSpawn.wav"), NULL, SND_ASYNC);
				}
				balls[i][2] += balls[i][0] * frametime;
				balls[i][3] += balls[i][1] * frametime;

				/* FOR TESTING, ASDF MOVES BALL 
				if (controls.a == 1)
				{
					balls[i][2] -= balls[i][0] * frametime;
				}
				if (controls.d == 1)
				{
					balls[i][2] += balls[i][0] * frametime;
				}
				if (controls.w == 1)
				{
					balls[i][3] += balls[i][1] * frametime;
				}
				if (controls.s == 1)
				{
					balls[i][3] -= balls[i][1] * frametime;
				}
				*/

				//TOP SECTION
				if (balls[i][3] > 0.31)
				{
					//COURT
					if (balls[i][2] > 0.28)
					{
						balls[i][2] = 0.28;
						balls[i][0] *= -1;
						PlaySound(TEXT("../resources/wallBounce.wav"), NULL, SND_ASYNC);
					}
					else if (balls[i][2] < -0.28)
					{
						balls[i][2] = -0.28;
						balls[i][0] *= -1;
						PlaySound(TEXT("../resources/wallBounce.wav"), NULL, SND_ASYNC);
					}
					if (balls[i][3] > 1.03)
					{
						balls[i][3] = 1.03;
						balls[i][1] *= -1;
						PlaySound(TEXT("../resources/wallBounce.wav"), NULL, SND_ASYNC);
					}

					//PADDLE
					if (balls[i][3] < 0.33)
					{
						if (balls[i][2] < upad + .1  && balls[i][2] > upad - .1)
						{
							balls[i][1] *= -1;
							balls[i][3] = 0.33;
							balls[i][0] += (balls[i][2] - upad) * 4;
							balls[i][0] = clamp(balls[i][0], -2 * ballSpeed, 2 * ballSpeed);

							PlaySound(TEXT("../resources/paddleHit.wav"), NULL, SND_ASYNC);
						}
					}

					//BRICKS
					for (int row = 0; row < 14; row++)
					{
						for (int col = 0; col < 6; col++)
						{
							if (uBricks[row][col] == true)
							{
								float top = .97 - 0.045*row;  //top of original spawn (bot layer) is .79, add .5 for each row
								float bot = top - 0.08;  //top -0.08
								float left = -0.32 + .1*col; //LEFT begins at -0.32, add .1 for each iteration.
								float right = left + .14; //RIGHT is left +.14

								if (balls[i][3] > bot && balls[i][3] < top && balls[i][2] < right && balls[i][2] > left) //HITTING THE BRICK
								{
									if (balls[i][3] < bot + .01) //Hits bottom
									{
										balls[i][3] = bot - .001;
										balls[i][1] *= -1;
									}
									if (balls[i][3] > top - .01) //Hits top
									{
										balls[i][3] = top + .001;;
										balls[i][1] *= -1;
									}
									if (balls[i][2] > right - .01) //Hits right
									{
										balls[i][2] = right + .001;
										balls[i][0] *= -1;
									}
									if (balls[i][2] < left + .01) //Hits left
									{
										balls[i][2] = left + .001;
										balls[i][0] *= -1;
									}

									uBricks[row][col] = false;
									PlaySound(TEXT("../resources/brickBreak.wav"), NULL, SND_ASYNC);
								}
							}
						}
					}
				}

				//BOT SECTION
				else if (balls[i][3] < -0.31)
				{
					if (balls[i][2] > 0.28)
					{
						balls[i][2] = 0.28;
						balls[i][0] *= -1;
						PlaySound(TEXT("../resources/wallBounce.wav"), NULL, SND_ASYNC);
					}
					else if (balls[i][2] < -0.28)
					{
						balls[i][2] = -0.28;
						balls[i][0] *= -1;
						PlaySound(TEXT("../resources/wallBounce.wav"), NULL, SND_ASYNC);
					}
					if (balls[i][3] < -1.03)
					{
						balls[i][3] = -1.03;
						balls[i][1] *= -1;
						PlaySound(TEXT("../resources/wallBounce.wav"), NULL, SND_ASYNC);
					}

					//PADDLE
					if (balls[i][3] > -0.33 && balls[i][3] < -0.3)
					{
						if (balls[i][2] < dpad + .09  && balls[i][2] > dpad - .09)
						{
							balls[i][1] *= -1;
							balls[i][3] = -0.33;
							balls[i][0] += (balls[i][2] - dpad) * 4;
							balls[i][0] = clamp(balls[i][0], -2 * ballSpeed, 2 * ballSpeed);
							PlaySound(TEXT("../resources/paddleHit.wav"), NULL, SND_ASYNC);
						}
					}

					//BRICKS
					for (int row = 0; row < 14; row++)
					{
						for (int col = 0; col < 6; col++)
						{
							if (dBricks[row][col] == true)
							{
								float bot = -.97 + 0.045*row;  //top of original spawn (bot layer) is .79, add .5 for each row
								float top = bot + 0.08;  //top -0.08
								float left = -0.32 + .1*col; //LEFT begins at -0.32, add .1 for each iteration.
								float right = left + .14; //RIGHT is left +.14

								if (balls[i][3] > bot && balls[i][3] < top && balls[i][2] < right && balls[i][2] > left) //HITTING THE BRICK
								{
									if (balls[i][3] < bot + .01) //Hits bottom
									{
										//balls[i][3] -= frametime*balls[i][1];
										balls[i][3] = bot - .001;
										balls[i][1] *= -1;
									}
									if (balls[i][3] > top - .01) //Hits top
									{
										//balls[i][3] += frametime*balls[i][1];
										balls[i][3] = top + .001;
										balls[i][1] *= -1;
									}
									if (balls[i][2] > right - .01) //Hits right
									{
										balls[i][2] = right + .001;
										balls[i][0] *= -1;
									}
									if (balls[i][2] < left + .01) //Hits left
									{
										balls[i][2] = left - .001;
										balls[i][0] *= -1;
									}

									dBricks[row][col] = false;
									PlaySound(TEXT("../resources/brickBreak.wav"), NULL, SND_ASYNC);
								}
							}
						}
					}
				}

				//LEFT SECTION
				else if (balls[i][2] < -0.31)
				{
					if (balls[i][3] > 0.28)
					{
						balls[i][3] = 0.28;
						balls[i][1] *= -1;
						PlaySound(TEXT("../resources/wallBounce.wav"), NULL, SND_ASYNC);
					}
					else if (balls[i][3] < -0.28)
					{
						balls[i][3] = -0.28;
						balls[i][1] *= -1;
						PlaySound(TEXT("../resources/wallBounce.wav"), NULL, SND_ASYNC);
					}
					if (balls[i][2] < -1.03)
					{
						balls[i][2] = -1.03;
						balls[i][0] *= -1;
						PlaySound(TEXT("../resources/wallBounce.wav"), NULL, SND_ASYNC);
					}

					//PADDLE
					if (balls[i][2] > -0.33)
					{
						if (balls[i][3] < lpad + .09  && balls[i][3] > lpad - .09)
						{
							balls[i][0] *= -1;
							balls[i][2] = -0.33;
							balls[i][1] += 4 * (balls[i][3] - lpad);
							balls[i][1] = clamp(balls[i][1], -2 * ballSpeed, 2 * ballSpeed);
							PlaySound(TEXT("../resources/paddleHit.wav"), NULL, SND_ASYNC);
						}
					}

					//BRICKS
					for (int row = 0; row < 14; row++)
					{
						for (int col = 0; col < 6; col++)
						{
							if (lBricks[row][col] == true)
							{
								float top = -.97 + 0.045*row;
								float bot = top + 0.08;
								float left = -0.32 + .1*col;
								float right = left + .14;

								if (balls[i][3] > left && balls[i][3] < right && balls[i][2] < bot && balls[i][2] > top) //HITTING THE BRICK
								{
									if (balls[i][3] < left + .01)
									{
										//balls[i][3] -= frametime*balls[i][1];
										balls[i][3] = left - .001;
										balls[i][1] *= -1;
									}
									if (balls[i][3] > right - .01) 
									{
										//balls[i][3] += frametime*balls[i][1];
										balls[i][3] = right + .001;
										balls[i][1] *= -1;
									}
									if (balls[i][2] < top + .01)
									{
										//balls[i][2] += frametime*balls[i][0];
										balls[i][2] = top - .001;
										balls[i][0] *= -1;
									}
									if (balls[i][2] > bot - .01)
									{
										//balls[i][2] -= frametime*balls[i][0];
										balls[i][2] = bot + .001;
										balls[i][0] *= -1;
									}

									lBricks[row][col] = false;
									PlaySound(TEXT("../resources/brickBreak.wav"), NULL, SND_ASYNC);
								}
							}
						}
					}
				}

				//RIGHT SECTION
				else if (balls[i][2] > 0.31)
				{
					if (balls[i][3] > 0.28)
					{
						balls[i][3] = 0.28;
						balls[i][1] *= -1;
						PlaySound(TEXT("../resources/wallBounce.wav"), NULL, SND_ASYNC);
					}
					else if (balls[i][3] < -0.28)
					{
						balls[i][3] = -0.28;
						balls[i][1] *= -1;
						PlaySound(TEXT("../resources/wallBounce.wav"), NULL, SND_ASYNC);
					}
					if (balls[i][2] > 1.03)
					{
						balls[i][2] = 1.03;
						balls[i][0] *= -1;
						PlaySound(TEXT("../resources/wallBounce.wav"), NULL, SND_ASYNC);
					}

					//PADDLE
					if (balls[i][2] < 0.33)
					{
						if (balls[i][3] < rpad + .09  && balls[i][3] > rpad - .09)
						{
							balls[i][0] *= -1;
							balls[i][2] = 0.33;
							balls[i][1] += 4 * (balls[i][3] - rpad);
							balls[i][1] = clamp(balls[i][1], -2 * ballSpeed, 2 * ballSpeed);
							PlaySound(TEXT("../resources/paddleHit.wav"), NULL, SND_ASYNC);
						}
					}

					//BRICKS
					for (int row = 0; row < 14; row++)
					{
						for (int col = 0; col < 6; col++)
						{
							if (rBricks[row][col] == true)
							{
								float top = .97 - 0.045*row;
								float bot = top - 0.08;
								float left = 0.32 - .1*col;
								float right = left - .14;

								if (balls[i][3] < left && balls[i][3] > right && balls[i][2] > bot && balls[i][2] < top) //HITTING THE BRICK
								{
									if (balls[i][3] > left - .01)
									{
										//balls[i][3] += frametime*balls[i][1];
										balls[i][3] = left + .001;
										balls[i][1] *= -1;
									}
									if (balls[i][3] < right + .01)
									{
										//balls[i][3] -= frametime*balls[i][1];
										balls[i][3] = right - .001;
										balls[i][1] *= -1;
									}
									if (balls[i][2] > top - .01)
									{
										//balls[i][2] += frametime*balls[i][0];
										balls[i][2] = top + .001;
										balls[i][0] *= -1;
									}
									if (balls[i][2] < bot + .01)
									{
										//balls[i][2] -= frametime*balls[i][0];
										balls[i][2] = bot - .001;
										balls[i][0] *= -1;
									}

									rBricks[row][col] = false;
									PlaySound(TEXT("../resources/brickBreak.wav"), NULL, SND_ASYNC);
								}
							}
						}
					}
				}

				//MIDDLE SECTION PADDLES
				else
				{
					if (balls[i][3] > 0.27) //Top Paddle
					{
						if (balls[i][2] < upad + .105  && balls[i][2] > upad - .105)
						{
							if (balls[i][3] > 0.28)
							{
								if (balls[i][2] > upad) //Going left, aka right side of paddle
								{
									balls[i][2] = upad + .105;
									balls[i][0] *= -1;
								}
								else
								{
									balls[i][2] = upad - .105;
									balls[i][0] *= -1;
								}
							}
							else
							{
								balls[i][1] *= -1;
								balls[i][3] = 0.27;
								balls[i][0] += (balls[i][2] - upad) * 4;
								balls[i][0] = clamp(balls[i][0], -2 * ballSpeed, 2 * ballSpeed);
							}
							PlaySound(TEXT("../resources/paddleHit.wav"), NULL, SND_ASYNC);
						}
					}
					else if (balls[i][3] < -0.27) //Bot Paddle
					{
						if (balls[i][2] < dpad + .105  && balls[i][2] > dpad - .105)
						{
							if (balls[i][3] < -0.28)
							{
								if (balls[i][2] > dpad)
								{
									balls[i][2] = dpad + .105;
									balls[i][0] *= -1;
								}
								else
								{
									balls[i][2] = dpad - .105;
									balls[i][0] *= -1;
								}
							}
							else
							{
								balls[i][1] *= -1;
								balls[i][3] = -0.27;
								balls[i][0] += (balls[i][2] - dpad) * 4;
								balls[i][0] = clamp(balls[i][0], -2 * ballSpeed, 2 * ballSpeed);
							}
							PlaySound(TEXT("../resources/paddleHit.wav"), NULL, SND_ASYNC);
						}
					}
					if (balls[i][2] < -0.27) //Left Paddle
					{
						if (balls[i][3] < lpad + .105  && balls[i][3] > lpad - .105)
						{
							if (balls[i][2] < -0.28)
							{
								if (balls[i][3] > lpad)
								{
									balls[i][3] = lpad + .105;
									balls[i][0] *= -1;
								}
								else
								{
									balls[i][3] = lpad - .105;
									balls[i][0] *= -1;
								}
							}
							else
							{
								balls[i][0] *= -1;
								balls[i][2] = -0.27;
								balls[i][1] += 4 * (balls[i][3] - lpad);
								balls[i][1] = clamp(balls[i][1], -2 * ballSpeed, 2 * ballSpeed);
							}		
							PlaySound(TEXT("../resources/paddleHit.wav"), NULL, SND_ASYNC);
						}
					}
					else if (balls[i][2] > 0.27) //Right Paddle
					{
						if (balls[i][3] < rpad + .105  && balls[i][3] > rpad - .105)
						{
							if (balls[i][2] > 0.28)
							{
								if (balls[i][3] > rpad)
								{
									balls[i][3] = rpad + .105;
									balls[i][0] *= -1;
								}
								else
								{
									balls[i][3] = rpad - .105;
									balls[i][0] *= -1;
								}
							}
							else
							{
								balls[i][0] *= -1;
								balls[i][2] = 0.27;
								balls[i][1] += 4 * (balls[i][3] - rpad);
								balls[i][1] = clamp(balls[i][1], -2 * ballSpeed, 2 * ballSpeed);
							}		
							PlaySound(TEXT("../resources/paddleHit.wav"), NULL, SND_ASYNC);
						}
					}
				}
			}
		}
	}

	bool addBricks(bool bricks[][6])
	{
		PlaySound(TEXT("../resources/brickSpawn.wav"), NULL, SND_ASYNC);

		for (int j = 0; j < 6; j++) //Check to find any bricks in bottom row
		{
			if (bricks[13][j] == true)
			{
				return false;
			}
		}

		for (int i = 13; i > 0; i--)
		{
			for (int j = 0; j < 6; j++)
			{
				bricks[i][j] = bricks[i - 1][j];
			}
		}

		for (int j = 0; j < 6; j++)
		{
			bricks[0][j] = true;
		}

		return true;
	}

	void checkQuadrants(bool* top, bool* left, bool* bot, bool*right)
	{
		*top = false;
		*bot = false;
		*left = false;
		*right = false;

		for (int i = 0; i < 4; i++)
		{
			if (balls[i][3] > 0.31)
			{
				*top = true;
			}
			else if (balls[i][3] < -0.31)
			{
				*bot = true;
			}
			else if (balls[i][2] < -0.31)
			{
				*left = true;
			}
			else if (balls[i][2] > 0.31)
			{
				*right = true;
			}
		}
	}

	void crushBrick(vec3 pos)
	{
		glm::mat4 M = glm::mat4(1);
		glm::mat4 Trans = glm::translate(glm::mat4(1.0f), pos);
		glm::mat4 Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.001f, 0.001f, 0.001f));
		M = Trans * Scale;
		glUniformMatrix4fv(ballProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape->draw(ballProg);
	}

	bool checkWin()
	{
		if (cheat == false)
		{
			for (int i = 13; i > 0; i--)
			{
				for (int j = 0; j < 6; j++)
				{
					if (uBricks[i][j] == true || dBricks[i][j] == true || lBricks[i][j] == true || rBricks[i][j] == true)
					{
						return false;
					}
				}
			}
		}

		balls[0][0] = 0;
		balls[0][1] = 0;
		balls[1][0] = 0;
		balls[1][1] = 0;
		balls[2][0] = 0;
		balls[2][1] = 0;
		balls[3][0] = 0;
		balls[3][1] = 0;

		PlaySound(TEXT("../resources/winner2.wav"), NULL, SND_ASYNC);

		if (cheat == true)
		{
			cheat = false;
		}
		return true;
	}

	void gameOver()
	{
		balls[0][0] = 0;
		balls[0][1] = 0;
		balls[1][0] = 0;
		balls[1][1] = 0;
		balls[2][0] = 0;
		balls[2][1] = 0;
		balls[3][0] = 0;
		balls[3][1] = 0;

		PlaySound(TEXT("../resources/gameOver.wav"), NULL, SND_ASYNC);
	}

	void render()
	{
		double frametime = get_last_elapsed_time();
		static float frameCounter = 0.0;
		static float spawnCounter = 0.0;
		static float spawnRate = 0.0;
		static float baseIncrement = 0.0005;
		static int numSpawns = 0;

		static bool ballTop = true;
		static bool ballLeft = true;
		static bool ballBot = true;
		static bool ballRight = true;
		static bool win = false;
		static bool lose = false;

		spawnRate += .00000004;
		frameCounter += baseIncrement + spawnRate;
		spawnCounter += baseIncrement + spawnRate;

		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Create the matrix stacks - please leave these alone for now

		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = glm::mat4(1);
		M = glm::mat4(1);
		// Apply orthographic projection....
		P = glm::ortho(-1 * aspect, 1 * aspect, -1.0f, 1.0f, -2.0f, 100.0f);
		if (width < height)
		{
			P = glm::ortho(-1.0f, 1.0f, -1.0f / aspect, 1.0f / aspect, -2.0f, 100.0f);
		}
		// ...but we overwrite it (optional) with a perspective projection.
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width / (float)height), 0.1f, 1000.0f); //so much type casting... GLM metods are quite funny ones

		static float w = 0.0;
		static float lpad = 0.0;
		static float rpad = 0.0;
		static float upad = 0.0;
		static float dpad = 0.0;
		float paddleSpeed = 0.6;
		float paddleDist = 0.2;

		if (controls.up == 1)
		{
			if (controls.a == 1 && upad > -paddleDist)
			{
				upad -= frametime*paddleSpeed;
			}
			if (controls.d == 1 && upad < paddleDist)
			{
				upad += frametime*paddleSpeed;
			}
		}
		if (controls.down == 1)
		{
			if (controls.a == 1 && dpad > -paddleDist)
			{
				dpad -= frametime*paddleSpeed;
			}
			if (controls.d == 1 && dpad < paddleDist)
			{
				dpad += frametime*paddleSpeed;
			}
		}
		if (controls.left == 1)
		{
			if (controls.w == 1 && lpad < paddleDist)
			{
				lpad += frametime*paddleSpeed;
			}
			if (controls.s == 1 && lpad > -paddleDist)
			{
				lpad -= frametime*paddleSpeed;
			}
		}
		if (controls.right == 1)
		{
			if (controls.w == 1 && rpad < paddleDist)
			{
				rpad += frametime*paddleSpeed;
			}
			if (controls.s == 1 && rpad > -paddleDist)
			{
				rpad -= frametime*paddleSpeed;
			}
		}

		processCollisions(upad, dpad, lpad, rpad, frametime, frameCounter);


		w = 3.141592769 / 2;

		glm::mat4 Rotate;
		glm::mat4 Scale;
		glm::mat4 Trans;
		glm::mat4 TransView = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3));

		V = TransView*V;


		drawCourt(P, V, M);



		paddleProg->bind();

		glUniformMatrix4fv(courtProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);

		//PADDLES
		//Top
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.17f, 0.03f, 0.1f));
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(upad, 0.3f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//Bot
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(dpad, -0.3f, 0.0f));
		M = Trans*Scale;
		glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//L
		Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f, lpad, 0.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		//R
		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.3f, rpad, 0.0f));
		M = Trans*Rotate*Scale;
		glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);

		paddleProg->unbind();



		brickProg->bind();

		glUniformMatrix4fv(courtProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(courtProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glBindVertexArray(VertexArrayID);

		//BRICKS
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.10f, 0.04f, 0.05f));

		int spawnOffset = floor(frameCounter * 3);

		if(spawnCounter >= 1.0)
		{
			numSpawns++;
			spawnCounter = 0;
			checkQuadrants(&ballTop, &ballLeft, &ballBot, &ballRight);

			if (numSpawns > 1 && ballTop == false)
			{
				if (addBricks(uBricks) == false)
				{
					gameOver();
					lose = true;					
				}
			}
			if (numSpawns > 2 && ballLeft == false)
			{
				if (addBricks(lBricks) == false)
				{
					gameOver();
					lose = true;
				}
			}
			if (numSpawns > 3 && ballBot == false)
			{
				if (addBricks(dBricks) == false)
				{
					gameOver();
					lose = true;
				}
			}
			if (numSpawns > 4 && ballRight == false)
			{
				if (addBricks(rBricks) == false)
				{
					gameOver();
					lose = true;
				}
			}
		}

		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 14; j++)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Texture);
				//TOP
				if (uBricks[j][i] == true)
				{
					Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.25f + (.1*i), 0.93 - (.045*j), 0.0f));
					M = Trans*Scale;
					glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
				}

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Texture2);

				//BOTTOM
				if (dBricks[j][i] == true)
				{
					Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-0.25f + (.1*i), -0.93 + (.045*j), 0.0f));
					glm::mat4 Rotate1 = glm::rotate(glm::mat4(1.0f), 2*w, glm::vec3(0.0f, 0.0f, 1.0f));
					M = Trans*Scale*Rotate1;
					glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
				}

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Texture3);

				//LEFT
				if (lBricks[j][i] == true)
				{
					Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
					Trans = glm::translate(glm::mat4(1.0f), glm::vec3(-.93f + (0.045*j), -.25f + (.1*i), 0.0f));
					M = Trans*Rotate*Scale;
					glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
				}

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, Texture4);

				//RIGHT
				if (rBricks[j][i] == true)
				{
					Rotate = glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 0.0f, 1.0f));
					Trans = glm::translate(glm::mat4(1.0f), glm::vec3(.93f - (0.045*j), 0.25f - (.1*i), 0.0f));
					glm::mat4 Rotate1 = glm::rotate(glm::mat4(1.0f), 2 * w, glm::vec3(0.0f, 0.0f, 1.0f));
					M = Trans*Rotate*Scale*Rotate1;
					glUniformMatrix4fv(brickProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
					glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
				}
			}
		}

		glBindVertexArray(0);

		brickProg->unbind();


		//BALLS
		ballProg->bind();

		glUniformMatrix4fv(ballProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(ballProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(ballProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);

		Trans = glm::translate(glm::mat4(1.0f), glm::vec3(balls[0][2], balls[0][3], 0.0f));
		Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f));
		M = Trans * Scale;
		glUniformMatrix4fv(ballProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		shape->draw(ballProg);

		if (frameCounter > 0.8)
		{
			Trans = glm::translate(glm::mat4(1.0f), glm::vec3(balls[1][2], balls[1][3], 0.0f));
			Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f));
			M = Trans * Scale;
			glUniformMatrix4fv(ballProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			shape->draw(ballProg);
		}

		if (frameCounter > 1.8)
		{
			Trans = glm::translate(glm::mat4(1.0f), glm::vec3(balls[2][2], balls[2][3], 0.0f));
			Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f));
			M = Trans * Scale;
			glUniformMatrix4fv(ballProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			shape->draw(ballProg);
		}

		if (frameCounter > 2.8)
		{
			Trans = glm::translate(glm::mat4(1.0f), glm::vec3(balls[3][2], balls[3][3], 0.0f));
			Scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.02f, 0.02f, 0.02f));
			M = Trans * Scale;
			glUniformMatrix4fv(ballProg->getUniform("M"), 1, GL_FALSE, &M[0][0]);
			shape->draw(ballProg);
		}

		ballProg->unbind();

		if (win == false)
		{
			if (checkWin() == true)
			{
				win = true;
				spawnRate = 0;
				baseIncrement = 0;
			}
		}

		if (cheatLose == true && lose == false)
		{
			gameOver();
			lose = true;
		}

		if (lose == true)
		{
			baseIncrement = 0;
			spawnRate = 0;
		}
	}
};
//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
	and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1500, 1000);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
	may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while (!glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
