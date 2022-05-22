#include "mpch.h"
#include "Shader.h"
#include "glad/glad.h"

namespace MoonEngine
{
    Shader::Shader(const std::string& filepath)
        : m_FilePath(filepath), m_ShaderBuffer(0)
    {
        ShaderProgramSource source = ParseShader(filepath);
        m_ShaderBuffer = CreateShader(source.VertexSource, source.FragmentSource);
    }
    
    Shader::~Shader()
    {
        glDeleteProgram(m_ShaderBuffer);
    }
    
    ShaderProgramSource Shader::ParseShader(const std::string& filepath)
    {
        std::ifstream stream(filepath);
        enum class ShaderType
        {
            NONE = -1,
            VERTEX = 0,
            FRAGMENT = 1,
        };
    
        ShaderType type = ShaderType::NONE;
        std::string line;
        std::stringstream ss[2];
        while (getline(stream, line))
        {
            if (line.find("#shader") != std::string::npos)
            {
                if (line.find("vertex") != std::string::npos)
                {
                    type = ShaderType::VERTEX;
                }
                else if (line.find("fragment") != std::string::npos)
                {
                    type = ShaderType::FRAGMENT;
                }
            }
            else
            {
                ss[(int)type] << line << '\n';
            }
        }
    
        return { ss[0].str(), ss[1].str() };
    }
    
    unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
    {
        unsigned int id = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);
    
        int success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            DebugErr((type == GL_VERTEX_SHADER ? "Vertex " : "Fragment ") << "Shader Compile Error: " << infoLog);
            glDeleteShader(id);
            return 0;
        }
        return id;
    }
    
    
    unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
    {
        unsigned int program = glCreateProgram();
        unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
    
        glAttachShader(program, vs);
        glAttachShader(program, fs);
        glLinkProgram(program);
        glValidateProgram(program);
    
        glDeleteShader(vs);
        glDeleteShader(fs);
        return program;
    }
    
    void Shader::Bind() const
    {
        glUseProgram(m_ShaderBuffer);
    }
    
    void Shader::Unbind() const
    {
        glUseProgram(0);
    }
    
    void Shader::SetUniform4f(const std::string& name, float f, float f1, float f2, float f3)
    {
        glUniform4f(GetUnfiformLocation(name), f, f1, f2, f3);
    }

    void Shader::SetUniformVec3(const std::string& name, const glm::vec3& vector)
    {
        glUniform3fv(GetUnfiformLocation(name), 1, &vector[0]);
    }
    
    void Shader::SetUniform1i(const std::string& name, int value)
    {
        glUniform1i(GetUnfiformLocation(name), value);
    }

    void Shader::SetUniform1f(const std::string& name, float f)
    {
        glUniform1f(GetUnfiformLocation(name), f);
    }
    
    void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
    {
        glUniformMatrix4fv(GetUnfiformLocation(name), 1, GL_FALSE, &matrix[0][0]);
    }
    
    int Shader::GetUnfiformLocation(const std::string& name)
    {
        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];
    
        int location = glGetUniformLocation(m_ShaderBuffer, name.c_str());
        if (location == -1)
            DebugWar("Uniform: " << name << " does not exist");
    
        m_UniformLocationCache[name] = location;
        return location;
    }
}