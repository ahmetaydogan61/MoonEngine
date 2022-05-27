#pragma once
namespace MoonEngine
{
	struct ShaderProgramSource
	{
		std::string VertexSource;
		std::string FragmentSource;
	};
	
	class Shader
	{
	private:
		std::string m_FilePath;
		unsigned int m_ShaderBuffer;
		std::unordered_map<std::string, int> m_UniformLocationCache;
	public:
		Shader(const std::string& filepath);
		~Shader();
	
		void Bind() const;
		void Unbind() const;
	
		void SetUniform1i(const std::string& name, int value);
		void SetUniform1f(const std::string& name, float f);
		void SetUniform4f(const std::string& name, float f, float f1, float f2, float f3);
		void SetUniformVec3(const std::string& name, const glm::vec3& matrix);
		void SetUniformMat4(const std::string& name, const glm::mat4& matrix);
		void SetUniform1iv(const std::string& name,int size, const int i[]);
	private:
		ShaderProgramSource ParseShader(const std::string& filepath);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
		int GetUnfiformLocation(const std::string& name);
	};
}