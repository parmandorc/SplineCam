#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <string>
#include <sstream>

class Shader
{
	GLuint program = 0;

	enum class ShaderType
	{
		VERTEX,
		FRAGMENT
	};

public:
	Shader() {}
	Shader(const char* vertexShaderFile, const char* fragmentShaderFile)
	{
		Load(vertexShaderFile, fragmentShaderFile);
	}

	virtual ~Shader()
	{
		glDeleteProgram(program);
	}

	bool Load(const char* vertexShaderFile, const char* fragmentShaderFile)
	{
		// load vertex shader
		GLuint vertexShader;
		if (!LoadShader(ShaderType::VERTEX, vertexShaderFile, vertexShader))
		{
			return false;
		}

		// load fragment shader
		GLuint fragmentShader;
		if (!LoadShader(ShaderType::FRAGMENT, fragmentShaderFile, fragmentShader))
		{
			return false;
		}

		// create the program, attach the shaders and link
		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		// delete created shaders as they are already linked to the program
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		// check link status
		GLint result;
		glGetProgramiv(program, GL_LINK_STATUS, &result);
		if (!result)
		{
			GLchar programInfoLog[512];
			glGetProgramInfoLog(program, sizeof(programInfoLog), nullptr, programInfoLog);
			printf("Shader program failed to link.\n %s", programInfoLog);
			Clear();
			return false;
		}

		return true;
	}

	void Use()
	{
		glUseProgram(program);
	}

protected:

	void Clear()
	{
		glDeleteProgram(program);
	}

	bool LoadShader(ShaderType type, const char* shaderFile, GLuint& outShader)
	{
		std::string shaderStr;
		if (!FileToString(shaderFile, shaderStr))
		{
			return false;
		}

		switch (type)
		{
		case ShaderType::VERTEX:
			outShader = glCreateShader(GL_VERTEX_SHADER);
			break;
		case ShaderType::FRAGMENT:
			outShader = glCreateShader(GL_FRAGMENT_SHADER);
			break;
		default:	
			return false;
		}
		
		const GLchar* shaderSrc = shaderStr.c_str();
		glShaderSource(outShader, 1, &shaderSrc, nullptr);
		glCompileShader(outShader);

		GLint result;
		glGetShaderiv(outShader, GL_COMPILE_STATUS, &result);
		if (!result)
		{
			GLchar shaderInfoLog[512];
			glGetShaderInfoLog(outShader, sizeof(shaderInfoLog), nullptr, shaderInfoLog);
			printf("Shader  %s failed to compile.\n %s", shaderFile, shaderInfoLog);

			return false;
		}

		return true;
	}

	bool FileToString(const char* fileName, std::string& outSrc)
	{
		std::stringstream ss;
		std::ifstream file(fileName);

		if (file.bad() || !file.is_open())
		{
			printf("Unable to load file %s", fileName);
			return false;
		}

		ss << file.rdbuf();
		outSrc = ss.str();

		file.close();

		return true;
	}

};

#endif SHADER_H
