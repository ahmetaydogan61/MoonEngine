#include "mpch.h"
#include "Renderer.h"
#include "glad/glad.h"
#include "Texture.h"
#include "Shader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace MoonEngine
{
	glm::vec4 Renderer::m_ClearColor;

	unsigned int va;
	unsigned int vb;
	unsigned int ib;

	const uint32_t maxQuads = 200;
	const uint32_t maxVertex = maxQuads * 4;
	const uint32_t maxIndex = maxQuads * 6;

	glm::vec4 vertices[] =
	{
		{ -0.5f, -0.5f, 0.0f, 1.0f},
		{  0.5f, -0.5f, 0.0f, 1.0f},
		{  0.5f,  0.5f, 0.0f, 1.0f},
		{ -0.5f,  0.5f, 0.0f, 1.0f}
	};

	glm::vec2 texCoords[] =
	{
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
	};

	struct Vertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec2 texCoords;
		float texID;
	};
	int strideLenght = 10;

	Vertex* verts;
	int index = 0;
	int quadCount = 0;

	Ref<Shader> Renderer::m_DefaultShader;
	Ref<Texture> Renderer::m_WhiteTexture;
	std::unordered_map<Ref<Texture>, int> Renderer::m_TextureCache;
	int Renderer::m_TextureID = 0;
	int Renderer::m_TextureIDs[32];

	int Renderer::CreateTextureCache(const Ref<Texture>& texture)
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

	void Renderer::Init()
	{
		verts = new Vertex[maxVertex * sizeof(Vertex)];

		glGenVertexArrays(1, &va);
		glBindVertexArray(va);

		glGenBuffers(1, &vb);
		glBindBuffer(GL_ARRAY_BUFFER, vb);
		glBufferData(GL_ARRAY_BUFFER, index * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideLenght * sizeof(float), 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, strideLenght * sizeof(float), (void*)(offsetof(Vertex, color)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, strideLenght * sizeof(float), (void*)(offsetof(Vertex, texCoords)));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, strideLenght * sizeof(float), (void*)(offsetof(Vertex, texID)));


		unsigned int* quadIndices = new unsigned int[maxIndex];
		int indicesOffset = 0;
		for (int i = 0; i < maxIndex; i += 6)
		{
			quadIndices[i + 0] = indicesOffset + 0;
			quadIndices[i + 1] = indicesOffset + 1;
			quadIndices[i + 2] = indicesOffset + 2;

			quadIndices[i + 3] = indicesOffset + 0;
			quadIndices[i + 4] = indicesOffset + 2;
			quadIndices[i + 5] = indicesOffset + 3;

			indicesOffset += 4;
		}

		glGenBuffers(1, &ib);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, maxIndex * sizeof(unsigned int), &quadIndices[0], GL_STATIC_DRAW);

		m_DefaultShader = CreateRef<Shader>("res/Shaders/Default.shader");
		m_WhiteTexture = CreateRef<Texture>(1, 1);
		
		for (int i = 0; i < 32; i++)
			m_TextureIDs[i] = i;

		delete[] quadIndices;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		DebugSys("Renderer Initialized");
	}

	void Renderer::Clear()
	{
		index = 0;
		quadCount = 0;

		m_TextureCache.clear();
		m_TextureID = 0;
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& size, const glm::vec4& color)
	{
		glm::mat4 rotationMat = glm::toMat4(glm::quat(rotation));
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * rotationMat * glm::scale(glm::mat4(1.0f), size);

		for (int i = 0; i < 4; i++)
		{
			verts[i + (quadCount * 4)].position = transform * vertices[i];
			verts[i + (quadCount * 4)].color = color;
			verts[i + (quadCount * 4)].texCoords = texCoords[i];
			verts[i + (quadCount * 4)].texID = 0;
			index += strideLenght;
		}
		quadCount++;
	}

	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& size, const glm::vec4& color,const Ref<Texture>& texture)
	{
		glm::mat4 rotationMat = glm::toMat4(glm::quat(rotation));
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * rotationMat * glm::scale(glm::mat4(1.0f), size);

		for (int i = 0; i < 4; i++)
		{
			verts[i + (quadCount * 4)].position = transform * vertices[i];
			verts[i + (quadCount * 4)].color = color;
			verts[i + (quadCount * 4)].texCoords = texCoords[i];
			if(texture == nullptr)
				verts[i + (quadCount * 4)].texID = 0;
			else	
				verts[i + (quadCount * 4)].texID = CreateTextureCache(texture);
			index += strideLenght;
		}
		quadCount++;
	}

	void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& color)
	{
		for (int i = 0; i < 4; i++)
		{
			verts[i + (quadCount * 4)].position = transform * vertices[i];
			verts[i + (quadCount * 4)].color = color;
			verts[i + (quadCount * 4)].texCoords = texCoords[i];
			verts[i + (quadCount * 4)].texID = 0;
			index += strideLenght;
		}
		quadCount++;
	}

	void Renderer::Render(const glm::mat4& viewProjection)
	{
		m_WhiteTexture->Bind(0);

		m_DefaultShader->Bind();
		m_DefaultShader->SetUniformMat4("uVP", viewProjection);
		m_DefaultShader->SetUniform1iv("uTexture", 32, m_TextureIDs);

		glBindVertexArray(va);
		glBufferData(GL_ARRAY_BUFFER, index * sizeof(float), &verts[0], GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
		glDrawElements(GL_TRIANGLES, 6 * quadCount, GL_UNSIGNED_INT, nullptr);
	}

	void Renderer::Destroy()
	{
		DebugSys("Renderer Destroyed");
	}

	void Renderer::SetClearColor(glm::vec4& color)
	{
		m_ClearColor = color;
		glClearColor(m_ClearColor.x, m_ClearColor.y, m_ClearColor.z, m_ClearColor.w);
	};
}