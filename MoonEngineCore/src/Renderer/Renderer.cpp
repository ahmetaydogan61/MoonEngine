#include "mpch.h"
#include "Renderer.h"
#include "glad/glad.h"
#include "Texture.h"
#include "Shader.h"

namespace MoonEngine
{
	const int RENDERBUFFERSIZE = 3 + 3 + 4 + 2 + 1;
	float Renderer::m_RenderBuffer[MAX_INSTANCES * RENDERBUFFERSIZE];
	int Renderer::m_Index = 0;
	int Renderer::m_TexureIds[];

	Ref<Shader> Renderer::m_DefaultShader;
	unsigned int Renderer::m_VertexArray;
	unsigned int Renderer::m_VertexBuffer;
	unsigned int Renderer::m_ElementBufffer;
	unsigned int Renderer::m_InstanceVertexBuffer;
	
	std::unordered_map<Ref<Texture>, int> Renderer::m_TextureCache;
	int Renderer::m_TextureID = 0;
	Ref<Texture> Renderer::m_WhiteTexture;
	
	glm::vec4 Renderer::m_ClearColor;

	int Renderer::CreateTextureCache(Ref<Texture> texture)
	{
		if (m_TextureID > 32)
			return 0;
		if (m_TextureCache.find(texture) != m_TextureCache.end())
			return m_TextureCache[texture];
		m_TextureID++;
		texture->Bind(m_TextureID);
		m_TextureCache[texture] = m_TextureID;
		return m_TextureID;
	}

	float Renderer::m_Vertices[] =
	{
		//Position				//Size				//Color						//TexCoords		//TexIDs
		-0.5f, -0.5f, 0.0f,		1.0f, 1.0f,	1.0f,	0.5f, 0.5f, 0.5f, 1.0f,		0.0f, 0.0f,		0.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 1.0f,	1.0f,	0.5f, 0.5f, 0.5f, 1.0f,		1.0f, 0.0f,		0.0f,
		 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,	1.0f,	0.5f, 0.5f, 0.5f, 1.0f,		1.0f, 1.0f,		0.0f,
		-0.5f,  0.5f, 0.0f,		1.0f, 1.0f, 1.0f,	0.5f, 0.5f, 0.5f, 1.0f,		0.0f, 1.0f,		0.0f
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

		//TexCoords
		glEnableVertexAttribArray(4);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, RENDERBUFFERSIZE * sizeof(float), (void*)(10 * sizeof(float)));

		//TexIndex
		glEnableVertexAttribArray(5);
		glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVertexBuffer);
		glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, RENDERBUFFERSIZE * sizeof(float), (void*)(12 * sizeof(float)));

		glVertexAttribDivisor(0, 0);
		glVertexAttribDivisor(1, 1);
		glVertexAttribDivisor(2, 1);
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 0);
		glVertexAttribDivisor(5, 1);

		glGenBuffers(1, &m_ElementBufffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBufffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_Indices), &m_Indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		m_DefaultShader = CreateRef<Shader>("res/Shaders/Default.shader");
		m_WhiteTexture = CreateRef<Texture>(1, 1);

		for (int i = 0; i < 32; i++)
			m_TexureIds[i] = i;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		DebugSys("Renderer Initialized");
	}

	void Renderer::Clear()
	{
		m_Index = 0;
		m_TextureCache.clear();
		m_TextureID = 0;
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
		m_RenderBuffer[m_Index++] = 0.0f; //TexCoords zero to ignore
		m_RenderBuffer[m_Index++] = 0.0f; //TexCoords zero to ignore
		m_RenderBuffer[m_Index++] = 0.0f; //TexID zero if not given
	}
	
	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color, Ref<Texture>& texture)
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
		m_RenderBuffer[m_Index++] = 0.0f; //TexCoords zero to ignore
		m_RenderBuffer[m_Index++] = 0.0f; //TexCoords zero to ignore
		if (texture == nullptr)
			m_RenderBuffer[m_Index++] = 0.0f;
		else
			m_RenderBuffer[m_Index++] = CreateTextureCache(texture);
	}

	void Renderer::Render(const glm::mat4& viewProjection)
	{
		m_DefaultShader->Bind();
		glBindBuffer(GL_ARRAY_BUFFER, m_InstanceVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_Index, &m_RenderBuffer[0], GL_DYNAMIC_DRAW);
		
		m_WhiteTexture->Bind(0);

		m_DefaultShader->SetUniformMat4("u_VP", viewProjection);
		m_DefaultShader->SetUniform1iv("u_Texture", 8, m_TexureIds);

		glBindVertexArray(m_VertexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ElementBufffer);
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_Index / RENDERBUFFERSIZE);
	}

	void Renderer::Destroy()
	{
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