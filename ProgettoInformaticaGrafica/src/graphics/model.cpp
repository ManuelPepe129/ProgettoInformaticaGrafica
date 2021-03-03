#include "model.h"

Model::Model(BoundTypes boundType,glm::vec3 pos, glm::vec3 size, bool noTex) :
	boundType(boundType), pos(pos), size(size), noTex(noTex)
{ }

bool Model::loadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "Could not load model at " << path << std::endl << importer.GetErrorString() << std::endl;
		return false;
	}

	directory = path.substr(0, path.find_last_of("/"));
	processNode(scene->mRootNode, scene);

	return true;
}

void Model::render(Shader shader)
{
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, pos);
	model = glm::scale(model, size);
	shader.setMat4("model", model);

	shader.setFloat("material.shininess", 0.5f);

	for (Mesh mesh : meshes)
	{
		mesh.render(shader);
	}
}

void Model::cleanup()
{
	for (Mesh mesh : meshes)
	{
		mesh.cleanUp();
	}
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all meshes
	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// process all child nodes
	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	BoundingRegion br(boundType);
	glm::vec3 min(float(~0)); // min point = max float
	glm::vec3 max(-(float(~0))); // max point = min float

	// process vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++) 
	{
		Vertex vertex;

		// position
		vertex.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		for (int j = 0; j < 3; ++j)
		{
			//if smaller than min
			if (vertex.pos[j] < min[j])
				min[j] = vertex.pos[j];
			// if bigger than max
			if (vertex.pos[j] > max[j])
				max[j] = vertex.pos[j];
		}

		// normal vectors
		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		// texture coords
		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.texCoord = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	if (boundType == BoundTypes::AABB)
	{
		br.max = max;
		br.min = min;
	}
	else if (boundType == BoundTypes::SPHERE)
	{
		br.center = BoundingRegion(min, max).calculateCenter();
		float maxRadiusSquared = 0.0f;
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			float radiusSquared = 0.0f; // distance for this vertex
			for (int j = 0; j < 3; j++)
			{
				radiusSquared += (vertices[i].pos[j]-br.center[j])* (vertices[i].pos[j] - br.center[j]);
			}
			if (radiusSquared > maxRadiusSquared)
			{
				maxRadiusSquared = radiusSquared;
			}
		}
		br.radius = sqrt(maxRadiusSquared);
	}

	// process indices
	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; ++j)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// process material
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	if (noTex) {
		// diffuse color
		aiColor4D diff(1.0f);
		aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diff);

		// specular color
		aiColor4D spec(1.0f);
		aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &spec);

		return Mesh(br, vertices, indices, diff, spec);
	}

	// diffuse maps
	std::vector<Texture> diffuseMaps = loadTextures(material, aiTextureType_DIFFUSE);
	textures.insert(textures.end(),diffuseMaps.begin(), diffuseMaps.end());

	// specular maps
	std::vector<Texture> specularMaps = loadTextures(material, aiTextureType_SPECULAR);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	return Mesh(br, vertices, indices, textures);
}

std::vector<Texture> Model::loadTextures(aiMaterial* mat, aiTextureType type)
{
	std::vector<Texture> textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::cout << str.C_Str() << std::endl;

		// prevents duplicate loadings
		bool skip = false;
		for (unsigned int j = 0; j < texturesLoaded.size(); ++j)
		{
			if (std::strcmp(texturesLoaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(texturesLoaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			// texture not yet loaded
			Texture tex(directory, str.C_Str(), type);
			tex.load(false);
			textures.push_back(tex);
			texturesLoaded.push_back(tex);
		}
	}
	return textures;
}

