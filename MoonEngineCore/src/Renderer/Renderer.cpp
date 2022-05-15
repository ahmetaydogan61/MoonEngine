#include "mpch.h"
#include "Renderer.h"
#include "glad/glad.h"
#include "Utils/OrthographicCamera.h"

namespace MoonEngine
{
	float Renderer::m_RenderBuffer[MAX_INSTANCES * 7];
	int Renderer::m_Index = 0;
	Shader* Renderer::m_DefaultShader;
	unsigned int Renderer::m_VertexArray;
	unsigned int Renderer::m_VertexBuffer;
	unsigned int Renderer::m_IndexBuffer;
	unsigned int Renderer::m_InstanceVertexBuffer;

	float Renderer::m_Vertices[] =
	{
		//Position		//Size			//Color			
		-0.5f, -0.5f,	1.0f, 1.0f,		0.5f, 0.5f, 0.5f,
		 0.5f, -0.5f,	1.0f, 1.0f,		0.5f, 0.5f, 0.5f,
		 0.5f,  0.5f,	1.0f, 1.0f,		0.5f, 0.5f, 0.5f,
		-0.5f,  0.5f,	1.0f, 1.0f,		0.5f, 0.5f, 0.5f
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
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);

		//Position Offset
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVertexBuffer);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);

		//Size
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));

		//Color
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(4 * sizeof(float)));

		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);

		glGenBuffers(1, &m_IndexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices), &m_Indices[0], GL_STATIC_DRAW);

		m_DefaultShader = new Shader("res/Shaders/Default.shader");
		DebugSys("Renderer Initialized");
	}

	void Renderer::Clear()
	{
		m_Index = 0;
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void Renderer::DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec3& color)
	{
		if ((m_Index / 2 + 2 + 3) >= MAX_INSTANCES) return;
		m_RenderBuffer[m_Index++] = position.x;
		m_RenderBuffer[m_Index++] = position.y;
		m_RenderBuffer[m_Index++] = size.x;
		m_RenderBuffer[m_Index++] = size.y;
		m_RenderBuffer[m_Index++] = color.x;
		m_RenderBuffer[m_Index++] = color.y;
		m_RenderBuffer[m_Index++] = color.z;
	}

	void Renderer::Render(OrthographicCamera& camera)
	{
		m_DefaultShader->Bind();
		glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_Index, &m_RenderBuffer[0], GL_DYNAMIC_DRAW);

		m_DefaultShader->SetUniformMat4("u_VP", camera.GetViewProjection());

		glBindVertexArray(m_VertexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_Index / (2 + 2 + 3));
	}

	void Renderer::Destroy()
	{
		delete m_DefaultShader;
		glDeleteBuffers(1, &m_VertexArray);
		glDeleteBuffers(1, &m_VertexBuffer);
		glDeleteBuffers(1, &m_IndexBuffer);
		glDeleteBuffers(1, &m_InstanceVertexBuffer);
	}
}