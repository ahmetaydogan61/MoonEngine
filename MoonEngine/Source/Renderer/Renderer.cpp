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
	bool Renderer::s_RendererInitialized = false;
	RendererStats Renderer::s_Stats;
	RendererData Renderer::s_Data;

	uint32_t Renderer::s_TextureIndex = 0;
	int32_t Renderer::s_TextureIds[32];
	std::unordered_map<Shared<Texture>, int32_t> Renderer::s_TextureCache;

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
		if (s_RendererInitialized)
		{
			ME_LOG("Trying to initialize renderer again!");
			return;
		}

		s_RendererInitialized = true;

		s_Data.QuadShader = MakeShared<Shader>("Resource/Shaders/Default.shader");
		s_Data.DefaultTexture = MakeShared<Texture>();

		s_Data.Vertices = new Vertex[s_Data.MaxVertexCount];

		glGenVertexArrays(1, &s_Data.VertexArray);
		glGenBuffers(1, &s_Data.VertexBuffer);
		glGenBuffers(1, &s_Data.IndexBuffer);

		glBindVertexArray(s_Data.VertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, s_Data.VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * s_Data.MaxVertexCount, s_Data.Vertices, GL_DYNAMIC_DRAW);

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

		uint32_t* indices = new uint32_t[s_Data.MaxIndexCount];
		uint32_t indicesIndex = 0;

		for (uint32_t i = 0; i < s_Data.MaxIndexCount; i += 6)
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
			s_TextureIds[i] = i;
		s_TextureCache.reserve(32);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * s_Data.MaxIndexCount, indices, GL_STATIC_DRAW);

		delete[] indices;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		s_RendererInitialized = true;
	}

	void Renderer::SetRenderData(const glm::mat4& viewProjection)
	{
		s_Data.ViewProjection = viewProjection;
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
		s_Data.ClearColor = color;
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::Begin()
	{
		Clear();

		s_Stats.DrawCalls = 0;
		s_Stats.VertexCount = 0;
		s_Stats.QuadCount = 0;

		s_Data.QuadShader->SetMat4("uVP", s_Data.ViewProjection);
		s_Data.QuadShader->SetIntArray("uTexture", 32, s_TextureIds);
	}

	void Renderer::End()
	{
		s_Data.QuadShader->Bind();
		s_Data.DefaultTexture->Bind(0);

		Render();
		s_Data.VertexIndex = 0;
		s_TextureIndex = 0;
		s_TextureCache.clear();
	}

	void Renderer::Render()
	{
		if (s_Data.VertexIndex < 4)
			return;

		glBindVertexArray(s_Data.VertexArray);
		//Obsolete
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * s_VertexIndex, s_Vertices, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * s_Data.VertexIndex, s_Data.Vertices);

		glDrawElements(GL_TRIANGLES, s_Stats.QuadCount * 6, GL_UNSIGNED_INT, 0);
		s_Stats.DrawCalls++;
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
		uint32_t vertexIndex = s_Data.VertexIndex;

		if (vertexIndex >= s_Data.MaxVertexCount || s_TextureIndex >= 32)
			End();

		for (int i = 0; i < 4; i++)
		{
			s_Data.Vertices[vertexIndex + i].Position = transform * VertexPositions[i];
			s_Data.Vertices[vertexIndex + i].Color = color;
			s_Data.Vertices[vertexIndex + i].TextureCoord = TexCoords[i];
			s_Data.Vertices[vertexIndex + i].Tiling = tiling;
			s_Data.Vertices[vertexIndex + i].EntityId = -1;

			if (!texture)
				s_Data.Vertices[vertexIndex + i].TextureId = 0;
			else
				s_Data.Vertices[vertexIndex + i].TextureId = GetTextureFromCache(texture);
		}
		s_Data.VertexIndex += 4;
		s_Stats.VertexCount = s_Data.VertexIndex;
		s_Stats.QuadCount++;
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
		uint32_t vertexIndex = s_Data.VertexIndex;

		if (vertexIndex >= s_Data.MaxVertexCount || s_TextureIndex >= 32)
			End();

		for (int i = 0; i < 4; i++)
		{
			s_Data.Vertices[vertexIndex + i].Position = transform * VertexPositions[i];
			s_Data.Vertices[vertexIndex + i].Color = color;
			s_Data.Vertices[vertexIndex + i].TextureCoord = TexCoords[i];
			s_Data.Vertices[vertexIndex + i].Tiling = tiling;
			s_Data.Vertices[vertexIndex + i].EntityId = entityId;

			if (!texture)
				s_Data.Vertices[vertexIndex + i].TextureId = 0;
			else
				s_Data.Vertices[vertexIndex + i].TextureId = GetTextureFromCache(texture);
		}
		s_Data.VertexIndex += 4;
		s_Stats.VertexCount = s_Data.VertexIndex;
		s_Stats.QuadCount++;
	}

	int32_t Renderer::GetTextureFromCache(const Shared<Texture>& texture)
	{
		if (s_TextureCache.find(texture) != s_TextureCache.end())
			return s_TextureCache.at(texture);

		s_TextureIndex++;
		texture->Bind(s_TextureIndex);
		s_TextureCache[texture] = s_TextureIndex;
		return s_TextureIndex;
	}

	void Renderer::SetLineWidth(float width)
	{
		glLineWidth(width);
	}

	void Renderer::Terminate()
	{
		if (!s_RendererInitialized)
		{
			ME_SYS_WAR("Trying to terminate non initialized Renderer!");
			return;
		}

		glDeleteVertexArrays(1, &s_Data.VertexArray);
		glDeleteBuffers(1, &s_Data.VertexBuffer);
		glDeleteBuffers(1, &s_Data.IndexBuffer);
		s_Data.VertexIndex = 0;

		delete[] s_Data.Vertices;
		s_TextureIndex = 0;
		s_TextureCache.clear();

		s_RendererInitialized = false;
	}
}
