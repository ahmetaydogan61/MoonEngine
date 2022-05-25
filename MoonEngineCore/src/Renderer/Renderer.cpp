#include "mpch.h"
#include "Renderer.h"
#include "glad/glad.h"

namespace MoonEngine
{
	const int RENDERBUFFERSIZE = 3 + 3 + 4;
	glm::vec4 Renderer::m_ClearColor;
	float Renderer::m_RenderBuffer[MAX_INSTANCES * RENDERBUFFERSIZE];
	int Renderer::m_Index = 0;
	Shader* Renderer::m_DefaultShader;
	unsigned int Renderer::m_VertexArray;
	unsigned int Renderer::m_VertexBuffer;
	unsigned int Renderer::m_ElementBufffer;
	unsigned int Renderer::m_InstanceVertexBuffer;

	float Renderer::m_Vertices[] =
	{
		//Position				//Size				//Color			
		-0.5f, -0.5f, 0.0f,		1.0f, 1.0f,	1.0f,	0.5f, 0.5f, 0.5f, 1.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 1.0f,	1.0f,	0.5f, 0.5f, 0.5f, 1.0f,
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,	1.0f,	0.5f, 0.5f, 0.5f, 1.0f,
		-0.5f,  0.5f, 0.0f,		1.0f, 1.0f, 1.0f,	0.5f, 0.5f, 0.5f, 1.0f
	};

	unsigned int Renderer::m_Indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	void Renderer::Init()
	{
		glGenBuffers(1, &m_InstanceVertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_RenderBuffer), &m_RenderBuffer[0], GL_DYNAMIC_DRAW);

		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);

		glGenBuffers(1, &m_VertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), &m_Vertices[0], GL_DYNAMIC_DRAW);

		//Position
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, RENDERBUFFERSIZE * sizeof(float), 0);

		//Position Offset
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVertexBuffer);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, RENDERBUFFERSIZE * sizeof(float), 0);

		//Size
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, RENDERBUFFERSIZE * sizeof(float), (void*)(3 * sizeof(float)));

		//Color
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, RENDERBUFFERSIZE * sizeof(float), (void*)(6 * sizeof(float)));

		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);

		glGenBuffers(1, &m_ElementBufffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBufffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices), &m_Indices[0], GL_STATIC_DRAW);

		m_DefaultShader = new Shader("res/Shaders/Default.shader");
		DebugSys("Renderer Initialized");
	}

	void Renderer::Clear()
	{
		m_Index = 0;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color)
	{
		if ((m_Index / RENDERBUFFERSIZE) >= MAX_INSTANCES) return;
		m_RenderBuffer[m_Index++] = position.x;
		m_RenderBuffer[m_Index++] = position.y;
		m_RenderBuffer[m_Index++] = position.z;
		m_RenderBuffer[m_Index++] = size.x;
		m_RenderBuffer[m_Index++] = size.y;
		m_RenderBuffer[m_Index++] = size.z;
		m_RenderBuffer[m_Index++] = color.x;
		m_RenderBuffer[m_Index++] = color.y;
		m_RenderBuffer[m_Index++] = color.z;
		m_RenderBuffer[m_Index++] = color.w;
	}

	void Renderer::Render(const glm::mat4& viewProjection)
	{
		m_DefaultShader->Bind();
		glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_Index, &m_RenderBuffer[0], GL_DYNAMIC_DRAW);

		m_DefaultShader->SetUniformMat4("u_VP", viewProjection);

		glBindVertexArray(m_VertexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBufffer);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_Index / RENDERBUFFERSIZE);
	}

	void Renderer::Destroy()
	{
		delete m_DefaultShader;
		glDeleteBuffers(1, &m_VertexArray);
		glDeleteBuffers(1, &m_VertexBuffer);
		glDeleteBuffers(1, &m_ElementBufffer);
		glDeleteBuffers(1, &m_InstanceVertexBuffer);
		DebugSys("Renderer Destroyed");
	}

	void Renderer::SetClearColor(glm::vec4& color)
	{
		m_ClearColor = color; 
		glClearColor(m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w);
	};
}