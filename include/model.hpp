#ifndef MODEL_HPP_
#define MODEL_HPP_

#include <algorithm>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <mesh.hpp>
#include <material.hpp>
#include <light.hpp>

#include <stb_image.h>

enum Axis {
	X_AXIS, Y_AXIS, Z_AXIS
};

enum Operation {
	TRANSLATE,
	ROTATE,
	SCALE,
	SCALE_SIMETRICAL
};

class Model {
public:
	Material material;
	bool isLight = false;
	int currentMaterial=1;

	Model(std::string path, Material material=MAT_JADE, bool isLight=false,
			glm::vec3 lightPos = glm::vec3(1.0f)){
		if (isLight){ 
			translate = lightPos;
			scale = glm::vec3(0.2);
			this->isLight = true;
		}
		this->material = material;
		loadModel(path);
	}
	void Draw(Shader& shader, Light light){
		shader.setMatrix4f("model", getModel());
		if (!isLight) {
			shader.setLight(light);
			shader.setMaterial(material);
		}
		for (size_t i = 0; meshes.size() > i; i++)
			meshes[i].Draw();
	}
	
	void cycleMaterial(bool up) {
		if (up){
			currentMaterial = std::min(++currentMaterial, static_cast<int>(MATERIALS.size()) - 1);
			material = MATERIALS[currentMaterial];
		}else{
			currentMaterial = std::max(--currentMaterial, 0);
			material = MATERIALS[currentMaterial];
		}
	}

	void setOperation(Operation operation){
		this->operation = operation;
	}

	void moveModel(Axis axis, float deltaTime){
		glm::vec3* chosen_op = nullptr;
		float sensitivity = getSensitivity() * deltaTime;
		switch (operation) {
			case TRANSLATE:
				chosen_op = &translate; break;
			case ROTATE:
				chosen_op = &rotate; break;
			case SCALE:
				chosen_op = &scale; break;
			case SCALE_SIMETRICAL:
				scale += sensitivity;
				return;
		};
		switch (axis) {
			case X_AXIS:
				chosen_op->x += sensitivity; break;
			case Y_AXIS:
				chosen_op->y += sensitivity; break;
			case Z_AXIS:
				chosen_op->z += sensitivity; break;
		}
	}

	void resetModelScaleAndRotation() {
		rotate = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);
	}
	
private:
	glm::vec3 translate = glm::vec3(0.0f);
	glm::vec3 rotate = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
	Operation operation = TRANSLATE;

	glm::mat4 getModel() {
		glm::mat4 model(1.0f);
		model = glm::translate(model, translate);
		model = glm::rotate(model, glm::radians(rotate.x), glm::vec3(1.0, 0.0, 0.0));
		model = glm::rotate(model, glm::radians(rotate.y), glm::vec3(0.0, 1.0, 0.0));
		model = glm::rotate(model, glm::radians(rotate.z), glm::vec3(0.0, 0.0, 1.0));
		model = glm::scale(model, scale);
		return model;
	}

	float getSensitivity() {
		float sens;
		switch (operation) {
			case TRANSLATE: sens = 5.0f; break;
			case ROTATE: sens = 20.0f; break;
			case SCALE:
			case SCALE_SIMETRICAL: 
				sens = 3.0f; break;
		}
		return sens;
	}

	std::vector<Mesh> meshes;
	std::string directory;
	
	void loadModel(std::string path){
		Assimp::Importer import;
		const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);	

		if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
		{
			std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
			return;
		}
		directory = path.substr(0, path.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void processNode(aiNode *node, const aiScene *scene){
		// process all the node's meshes (if any)
		for(unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
			meshes.push_back(processMesh(mesh, scene));			
		}
		// then do the same for each of its children
		for(unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	Mesh processMesh(aiMesh* mesh, const aiScene* scene){
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;

		for (size_t i = 0; i < mesh->mNumVertices; i++){
			Vertex vertex{
				glm::vec3(
						mesh->mVertices[i].x,
						mesh->mVertices[i].y,
						mesh->mVertices[i].z),
				glm::vec3(
						mesh->mNormals[i].x,
						mesh->mNormals[i].y,
						mesh->mNormals[i].z)
			};
			vertices.push_back(vertex);
		}

		for(size_t i = 0; i < mesh->mNumFaces; i++){
			aiFace face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++)
				indices.push_back(face.mIndices[j]);
		}

		return Mesh(vertices, indices);
	}

};

#endif
