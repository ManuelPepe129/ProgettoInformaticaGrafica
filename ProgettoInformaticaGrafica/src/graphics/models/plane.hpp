#ifndef PLANE_H
#define PLANE_H

#include "../model.h"

class Plane : public Model {
public:
	Plane(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f))
		: Model(BoundTypes::AABB, pos, size) {};

    void init() {
        int noVertices = 7;

        float vertices[] = {
            // position			   normal				texcoord
            0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // top right
           -0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	0.0f, 1.0f, // top left
           -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // bottom left
            0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 0.0f  // bottom right
          - 0.5f,  0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	0.0f, 1.0f, // top left
           -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // bottom left
            0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,	1.0f, 0.0f  // bottom right
        };

        std::vector<unsigned int> indices(noVertices);
        for (unsigned int i = 0; i < noVertices; i++) {
            indices[i] = i;
        }

        BoundingRegion br(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f));

        meshes.push_back(Mesh(br, Vertex::genList(vertices, noVertices), indices));
    }

};
#endif // !PLANE_H
