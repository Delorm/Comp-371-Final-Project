/*
 * COMP 371: Final Project
 * October 12, 2017
 * Skeleton Adopted and Modified
 * From learnopengl.com
 */


#include "stdio.h"
#include "iostream"
#include "string"
#include "cstring"
#include "fstream"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "vertex_array_object.hpp"
#include "gl_utils.hpp"
#include "CImg.hpp"

#ifdef __linux__ 
    //linux code goes here
    #include "GL/glew.h"	// include GL Extension Wrangler
    #include "GLFW/glfw3.h"	// include GLFW helper library
#elif _WIN32
    // windows code goes here
    #include "..\glew\glew.h"	// include GL Extension Wrangler
    #include "..\glfw\glfw3.h"	// include GLFW helper library
#endif

using namespace std;

// Configurable Constant Variables
const float CHAR_SPEED = 0.3f;
const float MOUSE_SENSITIVITY = 0.1f;
const float CHAR_HEIGHT = 2.0f;
const int TERRIAN_SKIP = 5.0f;
const float TERRIAN_STEP = 0.2; 

// Constant Variables
const float PI = 3.14159265359f;
const float BACKGROUND_COLOR = 0.4f; 
const float PROJ_FAR_PLANE = 1000.0f;
const GLuint INITIAL_WIDTH = 1280;
const GLuint INITIAL_HEIGHT = 720;
const glm::mat4 IDENTITY = glm::mat4(1.0f);
const glm::vec3 ORIGIN = glm::vec3(0.0f);

// Global Variables
GLFWwindow* window;
bool close_window = false;
std::vector<VertexArrayObject> vaos;
glm::vec3 center(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);
glm::vec3 eye(0.0f, 0.0f, 5.0f);

std::vector<glm::vec3> terrian;
int terrian_width;
int terrian_height;
int terrian_width_points;
int terrian_height_points;

// Prototypes definition
void initGl(void);
void drawGl(void);
void keyCallback(GLFWwindow*, int, int, int, int);
void registerCallbacks(GLFWwindow*);
void windowSizeCallback(GLFWwindow*, int, int); 
float mapHeight(float, float); 
glm::mat4 setCameraPosition(void);
std::vector<GLuint> findIndices(int width, int height); 


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    switch (key) {
	case GLFW_KEY_ESCAPE:
	    close_window = true;
	    break;

	case GLFW_KEY_W: {
	    glm::vec3 direction = glm::normalize(center - eye);
	    glm::vec3 step = CHAR_SPEED * direction;
	    step.y = 0;
	    eye = eye + step;
	    center = center + step;
	    break;
	}

	case GLFW_KEY_S: {
	    glm::vec3 direction = glm::normalize(center - eye);
	    glm::vec3 step = CHAR_SPEED * direction;
	    eye = eye - step;
	    step.y = 0;
	    center = center - step;
	    break;
	}

	case GLFW_KEY_D: {
	    glm::vec3 forward = center - eye;
	    glm::vec3 side = glm::normalize(glm::cross(forward, up)); 
	    glm::vec3 step = side * CHAR_SPEED;
	    step.y = 0;
	    eye = eye + step;
	    center = center + step;
	    break;
	}

	case GLFW_KEY_A: {
	    glm::vec3 forward = center - eye;
	    glm::vec3 side = glm::normalize(glm::cross(forward, up)); 
	    glm::vec3 step = side * CHAR_SPEED;
	    step.y = 0;
	    eye = eye - step;
	    center = center - step;
	    break;
	}
    }
}


void initGl() {

    // Set Projection Matrix 
    glm::mat4 projection_matrix = glm::perspective(45.0f, (GLfloat)INITIAL_WIDTH / (GLfloat)INITIAL_HEIGHT, 1.0f, PROJ_FAR_PLANE);
    VertexArrayObject::setProjectionMatrix(projection_matrix);

    std::vector<glm::vec3> vertices;
    std::vector<GLuint> edges;
    VertexArrayObject vao = VertexArrayObject();

    // Load Heightmap
    GlUtilities::createTerrain(terrian, terrian_width, terrian_height); 
    terrian_width_points = terrian_width;
    terrian_height_points = terrian_height;

    // Interpolate Terrian
    GlUtilities::interpolate(terrian, TERRIAN_SKIP, TERRIAN_STEP, terrian_width_points, terrian_height_points); 
    edges = GlUtilities::findIndices(terrian_width_points, terrian_height_points);
    
    glm::mat4 model_matrix = glm::translate(IDENTITY, glm::vec3(
		(float)-terrian_width / 2.0f, 
		0.0f, 
		(float)-terrian_height / 2.0f
		));
		
    vao.setGeometry(terrian);
    vao.setTopology(edges);
    vao.setModelMatrix(model_matrix);
    vaos.push_back(vao);
    edges.clear(); 

}

void drawGl() {

    // One View Matrix per Iteration
    glm::mat4 view_matrix = setCameraPosition();
    VertexArrayObject::setViewMatrix(view_matrix);

    // Draw All Objects
    for (unsigned int i = 0; i < vaos.size(); i++) {
	vaos[i].draw();
    }
}


int main() {

    // Initialize Window
    window = GlUtilities::setupGlWindow(INITIAL_WIDTH, INITIAL_HEIGHT); 
    if (window == NULL) return -1;

    // Register Callbacks
    registerCallbacks(window);

    // Shaders
    GLuint shader_program = GlUtilities::loadShaders();
    VertexArrayObject::registerShaderProgram(shader_program);

    // Run Objects Initialization
    initGl();

    // Main OpenGL Loop 
    while (!glfwWindowShouldClose(window) && !close_window)
    {
	glfwPollEvents();
	glClearColor(BACKGROUND_COLOR, BACKGROUND_COLOR, BACKGROUND_COLOR, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Run Drawing function
	drawGl();

	glfwSwapBuffers(window);
	glfwSwapInterval(1);
    }

    glfwTerminate();
    return 0;
}

glm::mat4 setCameraPosition() {

    // Get Map Height at eye position
    float old_eye_y = eye.y;
    eye.y = mapHeight(eye.x, eye.z);
    float centerShift = eye.y - old_eye_y;
    center.y += centerShift;

    // Mouse Looking around
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    int screen_center_x = width / 2;
    int screen_center_y = height / 2;
    glfwSetCursorPos(window, screen_center_x, screen_center_y);

    double mov_x = xpos - screen_center_x;
    double mov_y = ypos - screen_center_y;

    glm::mat4 trans = glm::translate(IDENTITY, ORIGIN - (center * 2.0f) + eye);

    // Horizontal Rotation:
    float rad = (mov_x * MOUSE_SENSITIVITY) * PI / 180.0f;
    glm::mat4 rot = glm::rotate(IDENTITY, rad, up);
    glm::vec4 hom_point = glm::vec4(center, 1.0f);
    hom_point = -trans * rot * trans * hom_point;
    center = glm::vec3(hom_point);

    // Vertical Rotation:
    rad = (mov_y *  MOUSE_SENSITIVITY) * PI / 180.0f;
    glm::vec3 axis = glm::normalize(cross(up, (center - eye)));
    rot = glm::rotate(IDENTITY, -rad, axis);
    hom_point = glm::vec4(center, 1.0f);
    hom_point = -trans * rot * trans * hom_point;
    center = glm::vec3(hom_point);

    glm::mat4 view_matrix = glm::lookAt(eye, center, up);
    return view_matrix;
}

void registerCallbacks(GLFWwindow* window) {
    glfwSetKeyCallback(window, keyCallback);
    glfwSetWindowSizeCallback(window, windowSizeCallback);
}

void windowSizeCallback(GLFWwindow* window, int width, int height) {
    // Define the new Viewport Dimensions{
    glfwGetFramebufferSize(window, & width, &height);
    glViewport(0, 0, width, height);
    glm::mat4 projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, PROJ_FAR_PLANE);
    VertexArrayObject::setProjectionMatrix(projection_matrix);
}

float mapHeight(float x, float z) {

    
    x = (x + (float)terrian_width / 2.0f); 
    z = (z + (float)terrian_height / 2.0f);
    int index = terrian_width_points * (int)z + (int)x;
    float y = terrian[index].y + CHAR_HEIGHT;

    return y;
}
