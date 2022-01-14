#include "playground.h"

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

// Include GLM
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>

#define PI 3.1415926535897932384626433832795


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (yoffset != 0) {
        camRadius += yoffset * -0.5;
        //camAngleBeta += yoffset * -0.5;
        //camPos.z += yoffset * -0.5;
    }
    //std::cout << "x: " << camPos2.x << " y: " << camPos2.y << " z: " << camPos2.z << std::endl;
    //std::cout << "x: " << camPos.x << " y: " << camPos.y << " z: " << camPos.z<<std::endl;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        isMouseClick = true;
        glfwGetCursorPos(window, &oldCursorX, &oldCursorY);
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        isMouseClick = false;
    }
}

int main(void)
{
    //hard coded positions of specific vertexes
    boardWalls.push_back(glm::vec3(1.0f,  1.0f, 0.4f));
    boardWalls.push_back(glm::vec3(-1.0f, 1.0f, 0.4f));
    boardWalls.push_back(glm::vec3(-1.0f,-1.0f, 0.4f));
    boardWalls.push_back(glm::vec3(1.0f, -1.0f, 0.4f));
    ballCenter = glm::vec3(0.3f, 0.1f, 0.4f);
    currentBallCenter = glm::vec3(0.3f, 0.1f, 0.407f);
    ballRadius = 0.07f;
    ballPits.push_back(glm::vec3(-0.1f, -0.3f, 0.4f));
    ballPits.push_back(glm::vec3(-0.7f, -0.6f, 0.4f));
    ballPits.push_back(glm::vec3(-0.5f, 0.6f,  0.4f));
    ballPits.push_back(glm::vec3( 0.8f, 0.5f,  0.4f));
    ballPits.push_back(glm::vec3( 0.4f, -0.7f, 0.4f));


    //Initialize window
    bool windowInitialized = initializeWindow();
    if (!windowInitialized) return -1;

    //Initialize vertex buffer
    bool vertexbufferInitialized = initializeVertexbuffer();
    if (!vertexbufferInitialized) return -1;

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

    //initializeMVPTransformation();

    //initialize pose variables
    gForce = -0.01;
    gForceX = 0.0;
    gForceY = 0.0;
    gForceZ = 0.0;

    angleY = 0;
    angleX = 0;
    //camPos = vec3(5.0f, 5.0f, 5.0f);
    camAngleAlpha = 30;
    camAngleBeta = 0;
    camRadius = 5.0f;

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);


    initializeMVPTransformation();


    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    //start animation loop until escape key is pressed
    do {

        updateAnimationLoop();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);


    //Cleanup and close window
    cleanupVertexbuffer();
    glDeleteProgram(programID);
closeWindow();

return 0;
}

void updateAnimationLoop()
{
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
    glUseProgram(programID);

    if (isMouseClick) {
        glfwGetCursorPos(window, &cursorX, &cursorY);
        if (cursorX - oldCursorX > 10) {
            camAngleBeta -= 5.0;
            oldCursorX += 5.0;
        }
        else if (cursorX - oldCursorX < -10) {
            camAngleBeta += 5.0;
            oldCursorX -= 5.0;
        }
        if (cursorY - oldCursorY > 10) {
            camAngleAlpha -= 5.0;
            oldCursorY += 5.0;
        }
        else if (cursorY - oldCursorY < -10) {
            camAngleAlpha += 5.0;
            oldCursorY -= 5.0;
        }
        //std::cout << "old cursor x: " << oldCursorX << "old cursor y: " << oldCursorY << std::endl;
        //std::cout << "current cursor x: " << cursorX << "current cursor y: " << cursorY << std::endl;
        //std::cout << "Angle Alpha " << camAngleAlpha << " Angle Beta " << camAngleBeta << std::endl;
    }

    // Update the variables for movement / rotation if a key was pressed
    if (glfwGetKey(window, GLFW_KEY_W) && angleX < 0.5) {
        angleX += 0.01;
    }
    else if (glfwGetKey(window, GLFW_KEY_S) && angleX > -0.5) {
        angleX -= 0.01;
    }
    else if (glfwGetKey(window, GLFW_KEY_A) && angleY < 0.5) {
        angleY += 0.01;
    }
    else if (glfwGetKey(window, GLFW_KEY_D) && angleY > -0.5) {
        angleY -= 0.01;
    }

    //initializeMVPTransformation();
    updateMVPTransformation();

    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    glUniformMatrix4fv(View_Matrix_ID, 1, GL_FALSE, &V[0][0]);
    glUniformMatrix4fv(Projection_Matrix_ID, 1, GL_FALSE, &P[0][0]);
    glUniformMatrix4fv(Model_Matrix_ID, 1, GL_FALSE, &board.M[0][0]);
    glUniform1i(colorId, 0);
    board.DrawObject();


    glUniformMatrix4fv(Model_Matrix_ID, 1, GL_FALSE, &sphere.M[0][0]);
    glUniform1i(colorId, 1);
    sphere.DrawObject();


    updateMovingObjectTransformation();

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();
}

void updateMovingObjectTransformation()
{
    //std::cout <<"y angle: "<< angleY<<std::endl;
    //std::cout << "x angle: " << angleX << std::endl;
    bool isBallinPit = false;
    xStop = false;
    yStop = false;
    board.M = glm::rotate(glm::mat4(1.0f), angleY, glm::vec3(1.0f, 0.0f, 0.0f));
    board.M = glm::rotate(board.M, angleX, glm::vec3(0.0f, 1.0f, 0.0f));
    sphere.M = glm::rotate(glm::mat4(1.0f), angleY, glm::vec3(1.0f, 0.0f, 0.0f));
    sphere.M = glm::rotate(sphere.M, angleX, glm::vec3(0.0f, 1.0f, 0.0f));

    std::vector<glm::vec3> currentBoardWalls;
    std::vector<glm::vec3> currentBallPits;

    for (uint i = 0; i < boardWalls.size(); i++)
    {
        glm::vec4 tempV4 = glm::vec4(boardWalls[i], 1.0);
        glm::vec4 rotated = board.M * tempV4;
        currentBoardWalls.push_back(glm::vec3(rotated));
        //std::cout<< "index: " << i << " Wall X : " << currentBoardWalls[i].x << "   Wall Y : " << currentBoardWalls[i].y << "   Wall Z : " << currentBoardWalls[i].z << std::endl;
    }

    for (uint i = 0; i < ballPits.size(); i++)
    {
        glm::vec4 tempV4 = glm::vec4(ballPits[i], 1.0);
        glm::vec4 rotated = board.M * tempV4;
        currentBallPits.push_back(glm::vec3(rotated));
        //std::cout<< "index: " << i << " Wall X : " << currentBoardWalls[i].x << "   Wall Y : " << currentBoardWalls[i].y << "   Wall Z : " << currentBoardWalls[i].z << std::endl;
    }

    /*glm::vec4 tempV4 = glm::vec4(ballCenter, 1.0);
    glm::vec4 rotated = sphere.M * tempV4;
    currentBallCenter = glm::vec3(rotated);*/

	for (uint i = 0; i < currentBoardWalls.size(); i++)
	{
		if (currentBoardWalls[i].x > 0 && currentBallCenter.x + ballRadius >= currentBoardWalls[i].x) {
			if (angleX > 0) {

				xStop = true;
			}
			//std::cout << "index: " << i << " Wall X : " << currentBoardWalls[i].x << "   Wall Y : " << currentBoardWalls[i].y << "   Wall Z : " << currentBoardWalls[i].z << std::endl;
		}
		else if (currentBoardWalls[i].x < 0 && currentBallCenter.x - ballRadius <= currentBoardWalls[i].x) {
			if (angleX < 0) {

				xStop = true;
			}
			//std::cout << "index: " << i << " Wall X : " << currentBoardWalls[i].x << "   Wall Y : " << currentBoardWalls[i].y << "   Wall Z : " << currentBoardWalls[i].z << std::endl;
		}
		if (currentBoardWalls[i].y > 0 && currentBallCenter.y + ballRadius >= currentBoardWalls[i].y) {

			if (angleY < 0) {
				yStop = true;
			}
			//std::cout << "index: " << i << " Wall X : " << currentBoardWalls[i].x << "   Wall Y : " << currentBoardWalls[i].y << "   Wall Z : " << currentBoardWalls[i].z << std::endl;
		}
		else if (currentBoardWalls[i].y < 0 && currentBallCenter.y - ballRadius <= currentBoardWalls[i].y) {
			if (angleY > 0) {
				yStop = true;
			}
			//std::cout << "index: " << i << " Wall X : " << currentBoardWalls[i].x << "   Wall Y : " << currentBoardWalls[i].y << "   Wall Z : " << currentBoardWalls[i].z << std::endl;
		}

	}
    float zForce = 0.0f;
    for (uint i = 0; i < currentBallPits.size(); i++)
    {
        float distance = sqrt((currentBallPits[i].x - currentBallCenter.x) * (currentBallPits[i].x - currentBallCenter.x) + (currentBallPits[i].y - currentBallCenter.y) * (currentBallPits[i].y - currentBallCenter.y));
        if (!isBallinPit && distance <= 0.1 - ballRadius) {
            zForce = -distance * 3.2f;//-0.1 / distance;            
            isBallinPit = true;
        }       

    }

    /*if (yStop) {
        if(oldY_angle != 0.0f && (oldY_angle < 0 && angleY >= 0) || (oldY_angle > 0 && angleY <= 0))
        oldY_angle = angleY;
    }*/
    if (isBallinPit) {
        gForce = (gForce*2) / 3;
    }
    else {
        gForce = -0.01;
    }
    
    if ( !yStop){// || !((oldY_angle >= 0 && angleY >= 0) || (oldY_angle < 0 && angleY < 0))) {
        //yStop = false;
        gForceY += sin(angleY) * gForce;
        //gForceZ += cos(angleY) * gForce;
    }
    if (!xStop ){//|| !((oldX_angle >= 0 && angleX >= 0) || (oldX_angle < 0 && angleX < 0))) {
        //xStop = false;
        gForceX -= sin(angleX) * gForce;
        //std::cout << gForceX << std::endl;
        //gForceZ += cos(angleX) * gForce;
    }
    
   
    sphere.M = glm::translate(sphere.M, { gForceX, gForceY, zForce });
    //tempV4 = glm::vec4(currentBallCenter, 1.0);
    /*rotated = sphere.M * tempV4;
    currentBallCenter = glm::vec3(rotated);*/

    glm::vec4 tempV4 = glm::vec4(ballCenter, 1.0);
    glm::vec4 rotated = sphere.M * tempV4;
    currentBallCenter = glm::vec3(rotated);
    
    //std::cout << "x: " << currentBallCenter.x << " y: " << currentBallCenter.y << " z: " << currentBallCenter.z << std::endl;

    //std::cout << boardWalls[0].z << std::endl;

    //board.M = glm::translate(Model, glm::vec3(curr_x, curr_y, 0.0f)); //apply translation

    //sphere.M = glm::rotate(glm::mat4(1.0f), curr_angle, { 1.0f,0.0f,0.0f });
    /*if (ballCenter.z - ballRadius > boardWalls[0].z) {
        sphere.M = glm::translate(sphere.M, { 0.0f, 0.0f, gForce });
        ballCenter.z += gForce;
    }*/
}

bool initializeWindow()
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return false;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(1024, 768, "Balance Balls", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return false;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
    return true;
}

bool initializeMVPTransformation()
{
    // Get a handle for our "MVP" uniform
    Model_Matrix_ID = glGetUniformLocation(programID, "M");
    Projection_Matrix_ID = glGetUniformLocation(programID, "P");
    View_Matrix_ID = glGetUniformLocation(programID, "V");
    colorId = glGetUniformLocation(programID, "myColor");
    // Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    P = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 10000.0f);

    // Camera matrix (modify this to let the camera move)
    float alphaInRadians = camAngleAlpha * PI / 180;
    float betaInRadians = camAngleBeta * PI / 180;
    //std::cout << " alfa: " << alphaInRadians << " beta: " << betaInRadians << std::endl;
    camPos2 = vec3(sin(alphaInRadians) * cos(betaInRadians) * camRadius,
        sin(alphaInRadians) * sin(betaInRadians) * camRadius,
        cos(alphaInRadians) * camRadius);
    //std::cout << "x: " << camPos2.x << " y: " << camPos2.y << " z: " << camPos2.z << std::endl;

    V = glm::lookAt(
        camPos2, // Camera is at (0,0,0), in World Space
        glm::vec3(0.0f, 0.0f, 0.0f), // and looks at the origin
        glm::vec3(0.0f, 0.0f, 1.0f)  // Head is up (set to 0,-1,0 to look upside-down)
    );


    return true;

}

bool updateMVPTransformation() 
{
    // Camera matrix (modify this to let the camera move)
    float alphaInRadians = camAngleAlpha * PI / 180;
    float betaInRadians = camAngleBeta * PI / 180;
    //std::cout << " alfa: " << alphaInRadians << " beta: " << betaInRadians << std::endl;
    camPos2 = vec3(sin(alphaInRadians) * cos(betaInRadians) * camRadius,
        sin(alphaInRadians) * sin(betaInRadians) * camRadius,
        cos(alphaInRadians) * camRadius);
    //std::cout << "x: " << camPos2.x << " y: " << camPos2.y << " z: " << camPos2.z << std::endl;

    V = glm::lookAt(
        camPos2, // Camera is at (0,0,0), in World Space
        glm::vec3(0.0f, 0.0f, 0.0f), // and looks at the origin
        glm::vec3(0.0f, 0.0f, 1.0f)  // Head is up (set to 0,-1,0 to look upside-down)
    );


    return true;
}

bool initializeVertexbuffer()
{
    //####################### FIRST OBJECT: GROUND ###################
    board = RenderingObject();
    board.InitializeVAO();
    board.LoadSTL("last.stl");

    
    //####################### SECOND OBJECT: BALL ###################
    sphere = RenderingObject();
    sphere.InitializeVAO();
    sphere.LoadSTL("sphereLast.stl");


    /*obj3 = RenderingObject();
    obj3.InitializeVAO();
    obj3.LoadSTL("Bunny-LowPoly.stl");*/

    return true;
}

bool cleanupVertexbuffer()
{
    // Cleanup VBO
    glDeleteVertexArrays(1, &board.VertexArrayID);
    return true;
}

bool closeWindow()
{
    glfwTerminate();
    return true;
}

void calculateBallTranslation() 
{
    
}

