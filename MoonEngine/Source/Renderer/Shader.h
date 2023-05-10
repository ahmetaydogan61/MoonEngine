#pragma once

namespace MoonEngine
{
	class Shader
	{
	public:
		Shader(const std::string& filepath);
		~Shader();

		void Bind() const;
		void Unbind() const;

		void SetMat4(const std::string& key, const glm::mat4& val);
		void SetIntArray(const std::string& key, uint32_t size, const int32_t* val);

		uint32_t GetShaderProgram() const { return m_ShaderBuffer; }
	private:
		uint32_t m_ShaderBuffer = 0;
		uint32_t CompileShader(unsigned int type, const std::string& source);
		void CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	};
}