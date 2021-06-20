#include "model.h"

#include <iostream>
#include <limits>
#include "../algorithms/states.hpp"


Model::Model(std::string id, BoundTypes BoundType, unsigned int flags)
	: id(id), boundType(boundType), switches(flags), currentNoInstances(0)
{ }

Model::~Model()
{
	cleanup();
}

void Model::init()
{

}

RigidBody* Model::generateInstance(glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot)
{
	instances.push_back(new RigidBody(id, size, mass, pos, rot));
	//std::cout << "Model: " << id << " now has " << currentNoInstances+1<<" instancies" << std::endl;
	return instances[currentNoInstances++];
}

void Model::initInstances()
{
	GLenum usage = GL_DYNAMIC_DRAW;

	glm::mat4* modelData = nullptr;
	glm::mat3* normalModelData = nullptr;

	std::vector<glm::mat4>* models = new std::vector<glm::mat4>();
	std::vector<glm::mat3>* normalModels = new std::vector<glm::mat3>();

	unsigned int size = UPPER_BOUND;

	if (States::isActive(&switches, CONST_INSTANCES))
	{
		// set data pointers
		for (unsigned int i = 0; i < instances.size(); i++)
		{
			models->push_back(instances[i]->model);
			normalModels->push_back(instances[i]->normalModel);
		}

		if (currentNoInstances) {
			modelData = models->data();
			normalModelData = normalModels->data();
		}

		usage = GL_STATIC_DRAW;
		size = instances.size();
	}

	// generate positions VBO
	modelVBO = BufferObject(GL_ARRAY_BUFFER);
	modelVBO.generate();
	modelVBO.bind();
	modelVBO.setData<glm::mat4>(size, modelData, usage);

	// generate size VBO
	normalModelVBO = BufferObject(GL_ARRAY_BUFFER);
	normalModelVBO.generate();
	normalModelVBO.bind();
	normalModelVBO.setData<glm::mat3>(size, normalModelData, usage);

	// set attribute pointers for each mesh
	for (unsigned int i = 0; i < meshes.size(); ++i) {

		meshes[i].VAO.bind();

		// set vertex attrib pointers

		// positions
		modelVBO.bind();
		modelVBO.setAttPointer<glm::vec4>(3, 4, GL_FLOAT, 4, 0, 1);
		modelVBO.setAttPointer<glm::vec4>(4, 4, GL_FLOAT, 4, 1, 1);
		modelVBO.setAttPointer<glm::vec4>(5, 4, GL_FLOAT, 4, 2, 1);
		modelVBO.setAttPointer<glm::vec4>(6, 4, GL_FLOAT, 4, 3, 1);
		// size
		normalModelVBO.bind();
		normalModelVBO.setAttPointer<glm::vec3>(7, 3, GL_FLOAT, 3, 0, 1);
		normalModelVBO.setAttPointer<glm::vec3>(8, 3, GL_FLOAT, 3, 1, 1);
		normalModelVBO.setAttPointer<glm::vec3>(9, 3, GL_FLOAT, 3, 2, 1);

		ArrayObject::clear();
	}
}

void Model::loadModel(const std::string& path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cerr << "Could not load model at " << path << std::endl << import.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of("/"));

	processNode(scene->mRootNode, scene);
}

void Model::setMaterial(Material material)
{
	for (unsigned int i = 0; i < meshes.size(); ++i)
	{
		meshes[i].setMaterial(material);
	}
}

void Model::update(double dt)
{
	if (!States::isActive(&switches, CONST_INSTANCES))
	{
		bool doUpdate = States::isActive(&switches, DYNAMIC);

		for (int i = 0; i < currentNoInstances; ++i)
		{
			if (doUpdate && !States::isActive(&instances[i]->state, INSTANCE_DEAD))
			{
				instances[i]->update(dt);
				States::activate(&instances[i]->state, INSTANCE_MOVED);
			}
			else
			{
				States::deactivate(&instances[i]->state, INSTANCE_MOVED);
			}
		}
	}
}


void Model::render(Shader shader, bool setModel)
{
	if (currentNoInstances > 0)
	{
		if (setModel)
		{
			shader.setMat4("model", glm::mat4(1.0f));
		}

		if (!States::isActive(&switches, CONST_INSTANCES))
		{
			// update VBO data
			std::vector<glm::mat4> models(currentNoInstances);
			std::vector<glm::mat3> normalModels(currentNoInstances);

			for (int i = 0; i < currentNoInstances; ++i)
			{
				// add updated matrices
				models[i] = instances[i]->model;
				normalModels[i] = instances[i]->normalModel;
			}

			// set transformation data
			modelVBO.bind();
			modelVBO.updateData<glm::mat4>(0, currentNoInstances, &models[0]);
			normalModelVBO.bind();
			normalModelVBO.updateData<glm::mat3>(0, currentNoInstances, &normalModels[0]);

		}

		shader.setFloat("material.shininess", 0.5f);

		for (unsigned int i = 0; i < meshes.size(); ++i)
		{
			meshes[i].render(shader, currentNoInstances);
		}
	}

}

void Model::removeInstance(unsigned int idx)
{
	if (idx < currentNoInstances)
	{
		instances.erase(instances.begin() + idx);
		currentNoInstances--;
	}
}

void Model::removeInstance(std::string instanceId)
{
	unsigned int idx = getIdx(instanceId);
	if (idx != -1)
		removeInstance(idx);
}

const unsigned int Model::getIdx(std::string id)const
{
	for (int i = 0; i < currentNoInstances; ++i)
	{
		if (instances[i]->instanceId == id)
		{
			return i;
		}
	}
	return -1;
}

const unsigned int Model::getNoInstances() const
{
	return currentNoInstances;
}

const std::string Model::getId() const
{
	return id;
}

void Model::cleanup()
{
	// free all instances
	for (unsigned int i = 0, len = instances.size(); i < len; i++) {
		if (instances[i]) {
			free(instances[i]);
		}
	}

	instances.clear();

	for (unsigned int i = 0; i < meshes.size(); i++)
	{
		meshes[i].cleanup();
	}

	normalModelVBO.cleanup();
	modelVBO.cleanup();
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh newMesh = processMesh(mesh, scene);
		meshes.push_back(newMesh);
		boundingRegions.push_back(newMesh.br);
	}

	// process all child nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	//std::cout << "Processing mesh " << mesh->mName.C_Str() << std::endl;

	BoundingRegion br(boundType);
	glm::vec3 min(std::numeric_limits<float>::max());		// min point = max float
	glm::vec3 max(std::numeric_limits<float>::lowest());	// max point = min float

	// vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{

		Vertex vertex;

		// position
		vertex.pos = glm::vec3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);

		for (int j = 0; j < 3; j++) {
			// if smaller than min
			if (vertex.pos[j] < min[j]) min[j] = vertex.pos[j];
			// if larger than max
			if (vertex.pos[j] > max[j]) max[j] = vertex.pos[j];
		}

		// normal vectors
		vertex.normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);

		// textures
		if (mesh->mTextureCoords[0])
		{

			vertex.texCoord = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);
		}
		else
		{
			vertex.texCoord = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	//std::cout << "max.x: " << max[0] << " max.y: " << max[1] << " max.z: " << max[2] << std::endl;
	//std::cout << "min.x: " << min[0] << " min.y: " << min[1] << " min.z: " << min[2] << std::endl;

	// process min/max for BR
	if (boundType == BoundTypes::AABB)
	{
		// assign max and min
		br.min = min;
		br.ogMin = br.min;
		br.max = max;
		br.ogMax = br.max;
	}
	else
	{
		// calculate max distance from the center
		br.center = BoundingRegion(min, max).calculateCenter();
		br.ogCenter = br.center;
		float maxRadiusSquared = 0.0f;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			float radiusSquared = 0.0f; // distance for this vertex
			for (int j = 0; j < 3; j++)
			{
				radiusSquared += (vertices[i].pos[j] - br.center[j]) * (vertices[i].pos[j] - br.center[j]);
			}
			if (radiusSquared > maxRadiusSquared)
			{
				// found new squared radius
				// a^2 > b^2 --> |a| > |b|
				maxRadiusSquared = radiusSquared;
			}
		}

		br.radius = sqrt(maxRadiusSquared);
		br.ogRadius = br.radius;
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	Mesh ret;

	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		if (States::isActive<unsigned int>(&switches, NO_TEX))
		{
			// 1. diffuse colors
			aiColor4D diff(1.0f);
			aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diff);
			// 2. specular colors
			aiColor4D spec(1.0f);
			aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &spec);

			ret = Mesh(br, diff, spec);
		}
		else
		{
			// 1. diffuse maps
			std::vector<Texture> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			std::vector<Texture> specularMaps = loadTextures(material, aiTextureType_SPECULAR);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

			ret = Mesh(br, textures);

			/*
			for (Texture tex : textures) {
				std::cout << tex.dir <<"/"<< tex.path <<  std::endl;
			}
			*/
		}
	}

	ret.loadData(vertices, indices);
	return ret;
}

std::vector<Texture> Model::loadTextures(aiMaterial* mat, aiTextureType type)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		// prevent duplicate loading
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip) {
			// not loaded yet
			Texture tex(directory, str.C_Str(), type);
			tex.load(false);
			textures.push_back(tex);
			textures_loaded.push_back(tex);
		}
	}

	return textures;
}