#ifndef PLAYGROUND_H
#define PLAYGROUND_H

// Include GLEW
#include <GL/glew.h>

// Include GLM
#include <glm/glm.hpp>

#include <vector>
#include <playground/parse_stl.h>

#include "../../bin/RenderingObject.h"

//program ID of the shaders, required for handling the shaders with OpenGL
GLuint programID;

//global variables to handle the MVP matrix
GLuint View_Matrix_ID;
glm::mat4 V;
GLuint Projection_Matrix_ID;
glm::mat4 P;
GLuint Model_Matrix_ID;

RenderingObject board;
RenderingObject sphere;
//RenderingObject obj3;

bool xStop = false;
bool yStop = false;
float gForce;
float gForceX;
float gForceY;
float gForceZ;
float angleX;
float angleY;
float oldX_angle=0.0f;
float oldY_angle= 0.0f;
//bool isBallinPit = false;

std::vector< glm::vec3 > boardWalls;
std::vector< glm::vec3 > ballPits;
glm::vec3 ballCenter;
glm::vec3 currentBallCenter;
float ballRadius;


double cursorX;
double cursorY;
double oldCursorX;
double oldCursorY;
bool isMouseClick = false;

glm::vec3 camPos;
glm::vec3 camPos2;
GLfloat camRadius;
GLint camAngleAlpha;
GLint camAngleBeta;


int main(void); //<<< main function, called at startup
void updateAnimationLoop(); //<<< updates the animation loop
bool initializeWindow(); //<<< initializes the window using GLFW and GLEW
bool initializeMVPTransformation();
bool updateMVPTransformation();
bool initializeVertexbuffer(); //<<< initializes the vertex buffer array and binds it OpenGL
bool cleanupVertexbuffer(); //<<< frees all recources from the vertex buffer
bool closeWindow(); //<<< Closes the OpenGL window and terminates GLFW

void updateMovingObjectTransformation();


#endif
