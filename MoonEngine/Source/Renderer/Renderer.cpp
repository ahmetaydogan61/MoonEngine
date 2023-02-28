#include "mpch.h"
#include "Renderer/Renderer.h"

#include "Core/Application.h"
#include "Core/Debug.h"

#include "Engine/Components.h"

#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

#include <glad/glad.h>

namespace MoonEngine
{
	bool Renderer::m_RendererInitialized = false;
	
	uint32_t Renderer::m_VertexArray = 0;
	uint32_t Renderer::m_VertexBuffer = 0;
	uint32_t Renderer::m_IndexBuffer = 0;
	uint32_t Renderer::m_VertexIndex = 0;
	Vertex* Renderer::m_Vertices = nullptr;
	
	Shared<Shader> Renderer::m_Shader = nullptr;
	Shared<Texture> Renderer::m_DefaultTexture = nullptr;
	RendererData Renderer::m_RendererData = {};

	uint32_t Renderer::m_TextureIndex = 0;
	int32_t Renderer::m_TextureIds[32];
	std::unordered_map<Shared<Texture>, int32_t> Renderer::m_TextureCache;

	const glm::vec4 VertexPositions[4] =
	{
		{ -0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f, -0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, 0.0f, 1.0f },
		{ -0.5f,  0.5f, 0.0f, 1.0f },
	};

	const glm::vec2 TexCoords[] =
	{
		{ 0.0f, 0.0f },
		{ 1.0f, 0.0f },
		{ 1.0f, 1.0f },
		{ 0.0f, 1.0f }
	};

	void Renderer::Init()
	{
		m_Shader = MakeShared<Shader>("Resource/Shaders/Default.shader");
		m_DefaultTexture = MakeShared<Texture>();

		m_Vertices = new Vertex[m_RendererData.MaxVertexCount];

		glGenVertexArrays(1, &m_VertexArray);
		glGenBuffers(1, &m_VertexBuffer);
		glGenBuffers(1, &m_IndexBuffer);

		glBindVertexArray(m_VertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_RendererData.MaxVertexCount, m_Vertices, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoord));
		glEnableVertexAttribArray(2);

		glVertexAttribIPointer(3, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, TextureId));
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tiling));
		glEnableVertexAttribArray(4);

		glVertexAttribIPointer(5, 1, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, EntityId));
		glEnableVertexAttribArray(5);

		uint32_t* indices = new uint32_t[m_RendererData.MaxIndexCount];
		uint32_t indicesIndex = 0;

		for (uint32_t i = 0; i < m_RendererData.MaxIndexCount; i += 6)
		{
			indices[i + 0] = 0 + indicesIndex;
			indices[i + 1] = 1 + indicesIndex;
			indices[i + 2] = 2 + indicesIndex;

			indices[i + 3] = 0 + indicesIndex;
			indices[i + 4] = 2 + indicesIndex;
			indices[i + 5] = 3 + indicesIndex;

			indicesIndex += 4;
		}

		for (int i = 0; i < 32; i++)
			m_TextureIds[i] = i;
		m_TextureCache.reserve(32);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * m_RendererData.MaxIndexCount, indices, GL_STATIC_DRAW);

		delete[] indices;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		m_RendererInitialized = true;
	}

	void Renderer::SetRenderData(const glm::mat4& viewProjection)
	{
		m_RendererData.ViewProjection = viewProjection;
	}

	void Renderer::SetRenderMode(RenderMode renderMode)
	{
		switch (renderMode)
		{
			case RenderMode::Solid:
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				break;
			case RenderMode::Wireframe:
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				break;
		}
	}

	void Renderer::SetClearColor(const glm::vec3& color)
	{
		glClearColor(color.x, color.y, color.z, 1.0f);
		m_RendererData.ClearColor = color;
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::Begin()
	{
		Clear();

		m_RendererData.DrawCalls = 0;
		m_RendererData.VertexCount = 0;
		m_RendererData.QuadCount = 0;

		m_Shader->SetMat4("uVP", m_RendererData.ViewProjection);
		m_Shader->SetIntArray("uTexture", 32, m_TextureIds);
	}

	void Renderer::End()
	{
		m_Shader->Bind();
		m_DefaultTexture->Bind(0);

		Render();
		m_VertexIndex = 0; 
		m_TextureIndex = 0;
		m_TextureCache.clear();
	}

	void Renderer::Render()
	{
		if (m_VertexIndex < 4)
			return;

		glBindVertexArray(m_VertexArray);
		//Obsolete
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * s_VertexIndex, s_Vertices, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * m_VertexIndex, m_Vertices);

		glDrawElements(GL_TRIANGLES, m_RendererData.QuadCount * 6, GL_UNSIGNED_INT, 0);

		m_RendererData.DrawCalls++;
	}

	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
							const Shared<Texture>& texture, const glm::vec4& color, const glm::vec2& tiling)
	{
		const glm::mat4& rotationMat = glm::toMat4(glm::quat(rotation));
		const glm::mat4& transform = glm::translate(glm::mat4(1.0f), position) * rotationMat * glm::scale(glm::mat4(1.0f), scale);

		DrawQuad(transform, texture, color, tiling);
	}

	void Renderer::DrawQuad(const glm::mat4& transform, const Shared<Texture>& texture, const glm::vec4& color, const glm::vec2& tiling)
	{
		if (m_VertexIndex >= m_RendererData.MaxVertexCount || m_TextureIndex >= 32)
			End();

		for (int i = 0; i < 4; i++)
		{
			m_Vertices[m_VertexIndex + i].Position = transform * VertexPositions[i];
			m_Vertices[m_VertexIndex + i].Color = color;
			m_Vertices[m_VertexIndex + i].TextureCoord = TexCoords[i];
			m_Vertices[m_VertexIndex + i].Tiling = tiling;
			m_Vertices[m_VertexIndex + i].EntityId = -1;

			if (!texture)
				m_Vertices[m_VertexIndex + i].TextureId = 0;
			else
				m_Vertices[m_VertexIndex + i].TextureId = GetTextureFromCache(texture);
		}
		m_VertexIndex += 4;
		m_RendererData.VertexCount = m_VertexIndex;
		m_RendererData.QuadCount++;
	}

	void Renderer::DrawEntity(const glm::mat4& transform, const SpriteComponent& spriteComponent, int entityId)
	{
		DrawEntity(transform, spriteComponent.Texture, spriteComponent.Color, spriteComponent.Tiling, entityId);
	}

	void Renderer::DrawEntity(const TransformComponent& transformComponent, const SpriteComponent& spriteComponent, int entityId)
	{
		const glm::mat4& rotationMat = glm::toMat4(glm::quat(transformComponent.Rotation));
		const glm::mat4& transform = glm::translate(glm::mat4(1.0f), transformComponent.Position) 
			* rotationMat * glm::scale(glm::mat4(1.0f), transformComponent.Scale);

		DrawEntity(transform, spriteComponent, entityId);
	}

	void Renderer::DrawEntity(const glm::mat4& transform, const Shared<Texture>& texture, const glm::vec4& color, const glm::vec2& tiling, int entityId)
	{
		if (m_VertexIndex >= m_RendererData.MaxVertexCount || m_TextureIndex >= 32)
			End();

		for (int i = 0; i < 4; i++)
		{
			m_Vertices[m_VertexIndex + i].Position = transform * VertexPositions[i];
			m_Vertices[m_VertexIndex + i].Color = color;
			m_Vertices[m_VertexIndex + i].TextureCoord = TexCoords[i];
			m_Vertices[m_VertexIndex + i].Tiling = tiling;
			m_Vertices[m_VertexIndex + i].EntityId = entityId;

			if (!texture)
				m_Vertices[m_VertexIndex + i].TextureId = 0;
			else
				m_Vertices[m_VertexIndex + i].TextureId = GetTextureFromCache(texture);
		}
		m_VertexIndex += 4;
		m_RendererData.VertexCount = m_VertexIndex;
		m_RendererData.QuadCount++;
	}

	int32_t Renderer::GetTextureFromCache(const Shared<Texture>& texture)
	{
		if (m_TextureCache.find(texture) != m_TextureCache.end())
			return m_TextureCache.at(texture);

		m_TextureIndex++;
		texture->Bind(m_TextureIndex);
		m_TextureCache[texture] = m_TextureIndex;
		return m_TextureIndex;
	}

	void Renderer::Terminate()
	{
		glDeleteVertexArrays(1, &m_VertexArray);
		glDeleteBuffers(1, &m_VertexBuffer);
		glDeleteBuffers(1, &m_IndexBuffer);

		delete[] m_Vertices;
		m_TextureIndex = 0;
		m_VertexIndex = 0;
		m_TextureCache.clear();
	}
}
