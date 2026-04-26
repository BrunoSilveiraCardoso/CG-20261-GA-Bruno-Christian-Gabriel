#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "fileReader.hpp"
#include "material.hpp"
#include "light.hpp"

void handleId(unsigned int id, GLenum pname) {
	int success;
	char infoLog[512];

	switch (pname) {
		case GL_LINK_STATUS:
			glGetProgramiv(id, pname, &success);
			if (!success) {
				glGetProgramInfoLog(id, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			}
			break;
		case GL_COMPILE_STATUS:
			glGetShaderiv(id, pname, &success);
			if (!success) {
				glGetShaderInfoLog(id, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			break;
	}
}


class Shader{
	public:
		unsigned int ID;

		Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr){
			std::string vertexCode = read_file(vertexPath);
			std::string fragmentCode = read_file(fragmentPath);
			std::string geometryCode = (geometryPath) ? read_file(geometryPath) : "";

			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();
			const char* gShaderCode = (geometryPath) ? geometryCode.c_str() : nullptr ;

			unsigned int vertex, fragment, geometry;

			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);
			handleId(vertex, GL_COMPILE_STATUS);

			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);
			handleId(fragment, GL_COMPILE_STATUS);

			if (geometryPath) {
				geometry = glCreateShader(GL_GEOMETRY_SHADER);
				glShaderSource(geometry, 1, &gShaderCode, NULL);
				glCompileShader(geometry);
				handleId(geometry, GL_COMPILE_STATUS);
			}

			ID = glCreateProgram();
			glAttachShader(ID, vertex);
			glAttachShader(ID, fragment);
			if (geometryPath) glAttachShader(ID, geometry);
			glLinkProgram(ID);
			handleId(ID, GL_LINK_STATUS);

			glDeleteShader(vertex);
			glDeleteShader(fragment);
			if (geometryPath) glAttachShader(ID, geometry);

		};

		void use(){
			glUseProgram(ID);
		};

		void setVec3f(const std::string& name, glm::vec3 vec) const{
			GLuint location = glGetUniformLocation(ID, name.c_str());
			glUniform3f(location, vec.x, vec.y, vec.z);
		};

		void set1ui(const std::string& name, unsigned int value) const {
			GLuint location = glGetUniformLocation(ID, name.c_str());
			glUniform1ui(location, value);
		}

		void setMatrix4f(const std::string& name, glm::mat4 mat) const{
			GLuint location = glGetUniformLocation(ID, name.c_str());
			glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
		}
		void setBool(const std::string& name, bool value) const{
			GLuint location = glGetUniformLocation(ID, name.c_str());
			glUniform1i(location, value);
		};
		void setInt(const std::string& name, int value) const{
			GLuint location = glGetUniformLocation(ID, name.c_str());
			glUniform1i(location, value);
		};
		void setFloat(const std::string& name, float value) const{
			GLuint location = glGetUniformLocation(ID, name.c_str());
			glUniform1f(location, value);
		};

		void setMaterial(Material material) {
			GLuint ambientLoc = glGetUniformLocation(ID, "material.ambient");
			GLuint diffuseLoc = glGetUniformLocation(ID, "material.diffuse");
			GLuint specularLoc = glGetUniformLocation(ID, "material.specular");
			GLuint shininessLoc = glGetUniformLocation(ID, "material.shininess");
			glUniform3f(ambientLoc, material.ambient.x, material.ambient.y, material.ambient.z);
			glUniform3f(diffuseLoc, material.diffuse.x, material.diffuse.y, material.diffuse.z);
			glUniform3f(specularLoc, material.specular.x, material.specular.y, material.specular.z);
			glUniform1f(shininessLoc, material.shininess);
		}

		void setLight(Light light) {
			GLuint positionLoc = glGetUniformLocation(ID, "light.position");
			GLuint ambientLoc = glGetUniformLocation(ID, "light.ambient");
			GLuint diffuseLoc = glGetUniformLocation(ID, "light.diffuse");
			GLuint specularLoc = glGetUniformLocation(ID, "light.specular");

			GLuint constantLoc = glGetUniformLocation(ID, "light.constant");
			GLuint linearLoc = glGetUniformLocation(ID, "light.linear");
			GLuint quadraticLoc = glGetUniformLocation(ID, "light.quadratic");

			glUniform3f(ambientLoc, light.ambient.x, light.ambient.y, light.ambient.z);
			glUniform3f(diffuseLoc, light.diffuse.x, light.diffuse.y, light.diffuse.z);
			glUniform3f(specularLoc, light.specular.x, light.specular.y, light.specular.z);
			glUniform3f(positionLoc, light.position.x, light.position.y, light.position.z);

			glUniform1f(constantLoc, light.distance.constant);
			glUniform1f(linearLoc, light.distance.linear);
			glUniform1f(quadraticLoc, light.distance.quadratic);
		}

};

#endif
