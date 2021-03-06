#ifndef _ITEM_H
#define _ITEM_H

#include "stdio.h"
#include "stdlib.h"
#include "iostream"
#include "vector"
#include "vertex_array_object.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

class Item {

    public:
	// Gemoetry & Topology
	std::vector<glm::vec3> vertices;    
	std::vector<unsigned int> edges;    // Edges
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<float> d_coeff;	    // Of containing Plane(to speed up collision)
	glm::mat4 model_matrix;
	bool collidable = false;
	glm::vec3 position;		    // in World Coordinates
	int type;
	
	// VAO
	VertexArrayObject vao;

    public:
	Item(int);
	static void setLightDirection(glm::vec4 &);
	static void setEyeLocation(glm::vec4 &);
	void loadObject(char*);
	void setGeometry(std::vector<glm::vec3>);
	void setTopology(std::vector<unsigned int>);
	void setColors(std::vector<glm::vec3>);
	void setUVs(std::vector<glm::vec2>);
	void setNormals(std::vector<glm::vec3>);
	void setTexture(char*);
	void setTexture(char*, char*, int);
	void setNumOfTexture(int);
	void setModelMatrix(glm::mat4 &);
	void setShaderProgram(GLuint);
	void setCollidable(bool);
	bool isCollidable();
	void draw();
	void clear(int);		// Clears for resuse
	void recycle(int);		// Clears while keeping texture(for repeated objects)

	// Getters
	std::vector<glm::vec3> & getVertices();
};

#endif
