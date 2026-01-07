#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include <glm/glm.hpp>

#include "PakReader.h"

template <typename T>
struct UniformData {
	int location = 0;
	T data;
};

class Shader {
public:
	// Reference ID of the Shader Program
	GLuint ID;
	// Constructor that build the Shader Program from 2 different shaders
	Shader(const char* vertexFile, const char* fragmentFile);
	~Shader();

	// Activates the Shader Program
	void Activate();
	// Deletes the Shader Program
	void Delete();

	void setMat4(std::string key, glm::mat4 value);
	void setMat3(std::string key, glm::mat3 value);
	void setVec4(std::string key, glm::vec4 value);
	void setVec3(std::string key, glm::vec3 value);
	void setVec2(std::string key, glm::vec2 value);
	void setInt(std::string key, int value);
	void setFloat(std::string key, float value);

	static void Init();
	// Call after finishing create all the shaders for the program
	static void CleanUp();

private:
	// Checks if the different Shaders have compiled properly
	void compileErrors(unsigned int shader, const char* type);

	int GetUniformLocation(const std::string& key);

	std::unordered_map<std::string, int> uniformCache;
};


#endif