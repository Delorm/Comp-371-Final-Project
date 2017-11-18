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
#include "terrian.hpp"
#include "objloader.hpp"
#include "item.hpp"
#include "time.h"

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
const float MOUSE_SENSITIVITY = 0.1f;
const float CHAR_HEIGHT = 1.0f;
const float WALK_SPEED = 0.1f;
const float FLY_SPEED = 1.0f;

// Terrian
const int T_WIDTH = 256;
const int T_HEIGHT = 256;
const int T_MAX = 10.0f;	// Highest & Lowest point in terrian
const int T_SHIFT = 2;		// Increases land to water ratio

// Rocks
const int R_NUMBER = 500;
const int R_MAX_RADIUS = 3;
const int R_POINTS = 20;

// Constant Variables
const float PI = 3.14159265359f;
const float BACKGROUND_COLOR = 0.4f; 
const float PROJ_NEAR_PLANE = 0.1f;
const float PROJ_FAR_PLANE = 2000.0f;
const GLuint INITIAL_WIDTH = 1280;
const GLuint INITIAL_HEIGHT = 720;
const glm::mat4 IDENTITY = glm::mat4(1.0f);
const glm::vec3 ORIGIN = glm::vec3(0.0f);

// Global Variables
GLFWwindow* window;
bool close_window = false;
std::vector<Item> items;
//std::vector<VertexArrayObject> vaos;
glm::vec3 center(0.0f, 0.0f, 0.0f);
glm::vec3 up(0.0f, 1.0f, 0.0f);
glm::vec3 eye(0.0f, 0.0f, 5.0f);
bool free_look = false;
bool wireframe = false; 
Terrian terrian;
float mov_speed = WALK_SPEED;
float teapot_ang = 0.0f;


//std::vector<glm::vec3> terrian;
int terrian_width;
int terrian_height;

// Prototypes definition
void initGl(void);
void drawGl(void);
void keyCallback(GLFWwindow*, int, int, int, int);
void registerCallbacks(GLFWwindow*);
void windowSizeCallback(GLFWwindow*, int, int); 
float mapHeight(float, float); 
glm::mat4 setCameraPosition(void);
std::vector<GLuint> findIndices(int width, int height); 
bool validMove(glm::vec3);
void move(glm::vec3);


void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    switch (key) {
	case GLFW_KEY_ESCAPE:
	    close_window = true;
	    break;

	case GLFW_KEY_W: {
	    glm::vec3 direction = glm::normalize(center - eye);
	    glm::vec3 step = direction * mov_speed;
	    move(step);
	    break;
	}

	case GLFW_KEY_S: {
	    glm::vec3 direction = glm::normalize(center - eye);
	    glm::vec3 step = -direction * mov_speed;
	    move(step);
	    break;
	}

	case GLFW_KEY_D: {
	    glm::vec3 forward = center - eye;
	    glm::vec3 side = glm::normalize(glm::cross(forward, up)); 
	    glm::vec3 step = side * mov_speed;
	    move(step);
	    break;
	}

	case GLFW_KEY_A: {
	    glm::vec3 forward = center - eye;
	    glm::vec3 side = glm::normalize(glm::cross(forward, up)); 
	    glm::vec3 step = -side * mov_speed;
	    move(step);
	    break;
	}

	case GLFW_KEY_V: {
	    if (action == GLFW_PRESS) {
		free_look = !free_look;
		mov_speed = (free_look == false) ? WALK_SPEED : FLY_SPEED;
	    }
	    break;
	}

	case GLFW_KEY_T: {
	    if (action == GLFW_PRESS) {
		wireframe = !wireframe;
		if (wireframe) {
		    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		} else {
		    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	    }
	    break;
	}



    }
}


void initGl() {

    // Set Projection Matrix 
    glm::mat4 projection_matrix = glm::perspective(45.0f, (GLfloat)INITIAL_WIDTH / (GLfloat)INITIAL_HEIGHT, PROJ_NEAR_PLANE, PROJ_FAR_PLANE);
    VertexArrayObject::setProjectionMatrix(projection_matrix);
    glm::mat4 model_matrix = IDENTITY;

    // Terrain
    Item item(1);
    terrian = Terrian(T_WIDTH, T_HEIGHT, T_MAX, T_SHIFT);
    item.setGeometry(terrian.generateMap());
    item.setTopology(terrian.findIndices());
    item.setTexture("resources/grass.png");
    item.setShaderProgram(GlUtilities::loadShaders("resources/terrain_vertex.shader", "resources/terrain_fragment.shader"));
    model_matrix = glm::translate(IDENTITY, glm::vec3( (float)-T_WIDTH / 2.0f, 0.0f, (float)-T_HEIGHT / 2.0f));
    item.setModelMatrix(model_matrix);
    items.push_back(item);

    // Cube #1
    item.clear(2);
    item.loadObject("resources/cube.obj");
    model_matrix = glm::translate(IDENTITY, glm::vec3(0, 10, 0));
    item.setModelMatrix(model_matrix);
    item.setTexture("resources/grass.png");
    item.setShaderProgram(GlUtilities::loadShaders("resources/tex_vertex.shader", "resources/tex_fragment.shader"));
    items.push_back(item);

    // Cube #2
    item.clear(2);
    item.loadObject("resources/cube.obj");
    model_matrix = glm::translate(IDENTITY, glm::vec3(10, 10, 0));
    item.setModelMatrix(model_matrix);
    item.setTexture("resources/image2.png");
    item.setShaderProgram(GlUtilities::loadShaders("resources/tex_vertex.shader", "resources/tex_fragment.shader"));
    items.push_back(item);

    // Rock
    item.clear(2);
    item.loadObject("resources/capsule.obj");
    model_matrix = glm::translate(IDENTITY, glm::vec3(6, 8, 0));
    item.setModelMatrix(model_matrix);
    item.setTexture("resources/rocky.jpg");
    item.setShaderProgram(GlUtilities::loadShaders("resources/tex_vertex.shader", "resources/tex_fragment.shader"));
    items.push_back(item);


    // Trees
    int num_of_trees = 100;

    item.clear(2);
    item.loadObject("resources/tree.obj");
    item.setTexture("resources/trunc.jpg");
    item.setShaderProgram(GlUtilities::loadShaders("resources/tex_vertex.shader", "resources/tex_fragment.shader"));

    for (int i = 0; i < num_of_trees; i++) {

	int x_loc = (rand() % T_WIDTH) - T_WIDTH / 2.0f;
	int z_loc = (rand() % T_HEIGHT) - T_HEIGHT / 2.0f;
	float y_loc = mapHeight(x_loc, z_loc);
	if (y_loc < 0) continue;
	float rad = rand() / RAND_MAX * 2 * PI;
	model_matrix = glm::rotate(IDENTITY, rad, up);
	model_matrix = glm::translate(model_matrix, glm::vec3(x_loc, y_loc, z_loc));
	item.setModelMatrix(model_matrix);
	items.push_back(item);
    }

    
	// Fern
	item.clear(2);
	item.loadObject("resources/fern.obj");
	model_matrix = glm::translate(IDENTITY, glm::vec3(32, 4, 0));
	item.setModelMatrix(model_matrix);
	item.setTexture("resources/fern.png");
	item.setShaderProgram(GlUtilities::loadShaders("resources/tex_vertex.shader", "resources/tex_fragment.shader"));
	items.push_back(item);

	// Tree
	item.clear(2);
	item.loadObject("resources/tree.obj");
	model_matrix = glm::translate(IDENTITY, glm::vec3(16, 4, 0));
	item.setModelMatrix(model_matrix);
	item.setTexture("resources/tree.png");
	item.setShaderProgram(GlUtilities::loadShaders("resources/tex_vertex.shader", "resources/tex_fragment.shader"));
	items.push_back(item);

	// Grass
	glm::mat4 trans;
	trans = glm::rotate(trans, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	item.clear(2);
	item.loadObject("resources/grassModel.obj");
	model_matrix = glm::translate(IDENTITY, glm::vec3(2, 2, 0));
	item.setModelMatrix(model_matrix*trans);
	item.setTexture("resources/grassTexture.png");
	item.setShaderProgram(GlUtilities::loadShaders("resources/tex_vertex.shader", "resources/tex_fragment.shader"));
	items.push_back(item);
	
    // Water plane
    item.clear(1);
    item.loadObject("resources/water.obj");
    model_matrix = glm::scale(IDENTITY, glm::vec3(T_WIDTH, 0, T_HEIGHT));
    item.setModelMatrix(model_matrix);
    item.setTexture("resources/water.png");
    item.setShaderProgram(GlUtilities::loadShaders("resources/terrain_vertex.shader", "resources/terrain_fragment.shader"));
    items.push_back(item);
    


    // Random Rock
    srand(time(NULL));
    item.clear(2);
    item.setTexture("resources/rocky.jpg");
    item.setShaderProgram(GlUtilities::loadShaders("resources/tex_vertex.shader", "resources/tex_fragment.shader"));

    for (int i = 0; i < R_NUMBER; i++) {

	int x_loc = (rand() % T_WIDTH) - T_WIDTH / 2.0f;
	int z_loc = (rand() % T_HEIGHT) - T_HEIGHT / 2.0f;
	float y_loc = mapHeight(x_loc, z_loc);
	if (y_loc < 0) continue;

	std::vector<glm::vec3> vertices = GlUtilities::genRandomRock(R_MAX_RADIUS, R_POINTS);
	std::vector<unsigned int> indices;
	GlUtilities::convexHull(vertices, indices);
	std::vector<glm::vec2> uvs = GlUtilities::genSphericalUVs(vertices);

	item.recycle(2);
	item.setGeometry(vertices);
	item.setTopology(indices);
	item.setUVs(uvs);

	model_matrix = glm::translate(IDENTITY, glm::vec3(x_loc, y_loc, z_loc));
	item.setModelMatrix(model_matrix);
	items.push_back(item);

    }

}

void drawGl() {

    // One View Matrix per Iteration
    glm::mat4 view_matrix = setCameraPosition();
    VertexArrayObject::setViewMatrix(view_matrix);

    // Draw All Objects
    for (unsigned int i = 0; i < items.size(); i++) {
	items[i].draw();
    }
}


int main() {

    // Initialize Window
    window = GlUtilities::setupGlWindow(INITIAL_WIDTH, INITIAL_HEIGHT); 
    if (window == NULL) return -1;

    // Register Callbacks
    registerCallbacks(window);

    // Shaders
    //GLuint shader_program = GlUtilities::loadShaders();
    //VertexArrayObject::registerShaderProgram(shader_program);

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

    // Get Map Height at eye position to clip the character to ground
    if (!free_look) {
	float old_eye_y = eye.y;
	eye.y = mapHeight(eye.x, eye.z) + CHAR_HEIGHT;
	float centerShift = eye.y - old_eye_y;
	center.y += centerShift;
    }

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
    glm::mat4 projection_matrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, PROJ_NEAR_PLANE, PROJ_FAR_PLANE);
    VertexArrayObject::setProjectionMatrix(projection_matrix);
}

float mapHeight(float x, float z) {
    float char_x = x + T_WIDTH / 2.0f;
    float char_z = z + T_HEIGHT / 2.0f;
    return terrian.getHeight(char_x, char_z);     

}

bool validMove(glm::vec3 step) {

    if (free_look) return true;

    // Water Collistion
    glm::vec3 char_pos = eye - glm::vec3(0, CHAR_HEIGHT, 0);
    glm::vec3 next_pos = char_pos + step;
    if (mapHeight(next_pos.x, next_pos.z) <= 0) {
	return false;
    }

    return true;
}

void move(glm::vec3 step) {
    if (validMove(step)) {
	center = center + step;
	eye = eye + step;
    }
}
