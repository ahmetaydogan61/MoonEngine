#include "mpch.h"
#include "Renderer/Shader.h"

#include "Core/Debug.h"

#include <glad/glad.h>

namespace MoonEngine
{
	Shader::Shader(const std::string& filepath)
		:m_ShaderBuffer(0)
	{
		uint32_t IsFragment = 0;

		std::ifstream stream(filepath);
		ME_ASSERT(stream, "Shader path not found!")

		std::string line;
		std::stringstream ss[2];
		
		while (getline(stream, line))
		{
			if (line.find("#Vertex") != std::string::npos)
			{
				IsFragment = 0;
			}
			else if (line.find("#Fragment") != std::string::npos)
			{
				IsFragment = 1;
			}
			else
			{
				ss[IsFragment] << line << '\n';
			}
		}

		CreateShader(ss[0].str(), ss[1].str());
	}

	uint32_t Shader::CompileShader(unsigned int type, const std::string& source)
	{
		uint32_t id = glCreateShader(type);
		const char* src = source.c_str();
		glShaderSource(id, 1, &src, nullptr);
		glCompileShader(id);

		int success;
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			char infoLog[512];
			glGetShaderInfoLog(id, 512, NULL, infoLog);
			ME_SYS_ERR("{0} Shader Compile Error: {1}", type == GL_VERTEX_SHADER ? "Vertex " : "Fragment ", infoLog);
			glDeleteShader(id);
			return 0;
		}
		return id;
	}

	void Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
	{
		m_ShaderBuffer = glCreateProgram();
		uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
		uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

		glAttachShader(m_ShaderBuffer, vs);
		glAttachShader(m_ShaderBuffer, fs);
		glLinkProgram(m_ShaderBuffer);
		glValidateProgram(m_ShaderBuffer);

		glDeleteShader(vs);
		glDeleteShader(fs);
	}

	void Shader::Bind() const
	{
		glUseProgram(m_ShaderBuffer);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	void Shader::SetMat4(const std::string& key, const glm::mat4& val)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ShaderBuffer, key.c_str()), 1, GL_FALSE, &val[0][0]);
	}

	void Shader::SetIntArray(const std::string& key, uint32_t size, const int32_t* val)
	{
		glUniform1iv(glGetUniformLocation(m_ShaderBuffer, key.c_str()), size, val);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_ShaderBuffer);
	}
}