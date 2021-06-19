#ifndef BOX_HPP
#define BOX_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector>

#include "../glmemory.hpp"

#include "../../algorithms/bounds.h"
#include "../shader.h"

#define UPPER_BOUND 200

class Box {
public:
	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> sizes;
	std::vector<glm::vec3> rotations;

	void init() {
		vertices = {
			// position             x   y   z   i
			 0.5f,  0.5f,  0.5f, // +   +   +   0
			-0.5f,  0.5f,  0.5f, // -   +   +   1
			-0.5f, -0.5f,  0.5f, // -   -   +   2
			 0.5f, -0.5f,  0.5f, // +   -   +   3
			 0.5f, 0.5f, -0.5f,  // +   +   -   4
			-0.5f, 0.5f, -0.5f,  // -   +   -   5
			-0.5f, -0.5f, -0.5f, // -   -   -   6
			 0.5f, -0.5f, -0.5f  // +   -   -   7
		};

		indices = { // 12 lines
			// front face (+ve z)
			0, 1,
			1, 2,
			2, 3,
			3, 0,
			// back face (-ve z)
			4, 5,
			5, 6,
			6, 7,
			7, 4,
			// right face (+ve x)
			0, 4,
			3, 7,
			// left face (-ve x)
			1, 5,
			2, 6
		};

		// generate VAO
		VAO.generate();
		VAO.bind();

		// generate EBO
		VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
		VAO["EBO"].generate();
		VAO["EBO"].bind();
		VAO["EBO"].setData<GLuint>(indices.size(), &indices[0], GL_STATIC_DRAW);

		// generate VBO
		VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["VBO"].generate();
		VAO["VBO"].bind();
		VAO["VBO"].setData<GLfloat>(vertices.size(), &vertices[0], GL_STATIC_DRAW);
		VAO["VBO"].setAttPointer<GLfloat>(0, 3, GL_FLOAT, 3, 0);

		// position VBO - dynamic
		VAO["posVBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["posVBO"].generate();
		VAO["posVBO"].bind();
		VAO["posVBO"].setData<glm::vec3>(UPPER_BOUND, NULL, GL_DYNAMIC_DRAW);
		VAO["posVBO"].setAttPointer<glm::vec3>(1, 3, GL_FLOAT, 1, 0, 1);

		// size VBO - dynamic
		VAO["sizeVBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["sizeVBO"].generate();
		VAO["sizeVBO"].bind();
		VAO["sizeVBO"].setData<glm::vec3>(UPPER_BOUND, NULL, GL_DYNAMIC_DRAW);
		VAO["sizeVBO"].setAttPointer<glm::vec3>(2, 3, GL_FLOAT, 1, 0, 1);

		// size VBO - dynamic
		VAO["rotVBO"] = BufferObject(GL_ARRAY_BUFFER);
		VAO["rotVBO"].generate();
		VAO["rotVBO"].bind();
		VAO["rotVBO"].setData<glm::vec3>(UPPER_BOUND, NULL, GL_DYNAMIC_DRAW);
		VAO["rotVBO"].setAttPointer<glm::mat4>(3, 4, GL_FLOAT, 4, 0, 1);
		VAO["rotVBO"].setAttPointer<glm::mat4>(4, 4, GL_FLOAT, 4, 1, 1);
		VAO["rotVBO"].setAttPointer<glm::mat4>(5, 4, GL_FLOAT, 4, 2, 1);
		VAO["rotVBO"].setAttPointer<glm::mat4>(6, 4, GL_FLOAT, 4, 3, 1);
		VAO["rotVBO"].clear();

		ArrayObject::clear();
	}

	void render(Shader shader) {
		shader.setMat4("model", glm::mat4(1.0f));

		// update data
		int instances = std::min(UPPER_BOUND, (int)positions.size()); // if more than UPPER_BOUND instances, only render UPPER_BOUND

		// update data
		if (instances != 0) {
			// if instances exist

			std::vector<glm::mat4> rots(instances);
			// update data

			// update data
			VAO["posVBO"].bind();
			VAO["posVBO"].updateData<glm::vec3>(0, instances, &positions[0]);

			VAO["sizeVBO"].bind();
			VAO["sizeVBO"].updateData<glm::vec3>(0, instances, &sizes[0]);

			for (unsigned int i = 0; i < instances; ++i)
			{
				// calculate rotation matrix
				glm::mat4 rotMat = glm::toMat4(glm::quat(rotations[i]));

				rots[i] = rotMat;
			}

			VAO["rotVBO"].bind();
			VAO["rotVBO"].updateData<glm::mat4>(0, instances, &rots[0]);
		}

		// render data
		VAO.bind();
		VAO.draw(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0, instances);
		ArrayObject::clear();
	}

	void addInstance(RigidBody* instance) {

	}

	void cleanup() {
		VAO.cleanup();
	}

private:
	ArrayObject VAO;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;
};

#endif