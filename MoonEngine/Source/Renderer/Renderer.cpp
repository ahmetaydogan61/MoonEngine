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
	
	uint32_t Renderer::s_VertexArray = 0;
	uint32_t Renderer::s_VertexBuffer = 0;
	uint32_t Renderer::s_IndexBuffer = 0;
	uint32_t Renderer::s_VertexIndex = 0;
	Vertex* Renderer::s_Vertices = nullptr;
	
	Shared<Shader> Renderer::s_Shader = nullptr;
	Shared<Texture> Renderer::s_DefaultTexture = nullptr;
	RendererData Renderer::s_RendererData = {};

	uint32_t Renderer::s_TextureIndex = 0;
	int32_t Renderer::s_TextureIds[32];
	std::unordered_map<Shared<Texture>, int32_t> Renderer::s_TextureCache
		;

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

		s_Shader = MakeShared<Shader>("Resource/Shaders/Default.shader");
		s_DefaultTexture = MakeShared<Texture>();

		s_Vertices = new Vertex[s_RendererData.MaxVertexCount];

		glGenVertexArrays(1, &s_VertexArray);
		glGenBuffers(1, &s_VertexBuffer);
		glGenBuffers(1, &s_IndexBuffer);

		glBindVertexArray(s_VertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, s_VertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * s_RendererData.MaxVertexCount, s_Vertices, GL_DYNAMIC_DRAW);

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

		uint32_t* indices = new uint32_t[s_RendererData.MaxIndexCount];
		uint32_t indicesIndex = 0;

		for (uint32_t i = 0; i < s_RendererData.MaxIndexCount; i += 6)
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

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_IndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * s_RendererData.MaxIndexCount, indices, GL_STATIC_DRAW);

		delete[] indices;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		s_RendererInitialized = true;
	}

	void Renderer::SetRenderData(const glm::mat4& viewProjection)
	{
		s_RendererData.ViewProjection = viewProjection;
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
		s_RendererData.ClearColor = color;
	}

	void Renderer::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void Renderer::Begin()
	{
		Clear();

		s_RendererData.DrawCalls = 0;
		s_RendererData.VertexCount = 0;
		s_RendererData.QuadCount = 0;

		s_Shader->SetMat4("uVP", s_RendererData.ViewProjection);
		s_Shader->SetIntArray("uTexture", 32, s_TextureIds);
	}

	void Renderer::End()
	{
		s_Shader->Bind();
		s_DefaultTexture->Bind(0);

		Render();
		s_VertexIndex = 0; 
		s_TextureIndex = 0;
		s_TextureCache.clear();
	}

	void Renderer::Render()
	{
		if (s_VertexIndex < 4)
			return;

		glBindVertexArray(s_VertexArray);
		//Obsolete
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * s_VertexIndex, s_Vertices, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * s_VertexIndex, s_Vertices);

		glDrawElements(GL_TRIANGLES, s_RendererData.QuadCount * 6, GL_UNSIGNED_INT, 0);

		s_RendererData.DrawCalls++;
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
		if (s_VertexIndex >= s_RendererData.MaxVertexCount || s_TextureIndex >= 32)
			End();

		for (int i = 0; i < 4; i++)
		{
			s_Vertices[s_VertexIndex + i].Position = transform * VertexPositions[i];
			s_Vertices[s_VertexIndex + i].Color = color;
			s_Vertices[s_VertexIndex + i].TextureCoord = TexCoords[i];
			s_Vertices[s_VertexIndex + i].Tiling = tiling;
			s_Vertices[s_VertexIndex + i].EntityId = -1;

			if (!texture)
				s_Vertices[s_VertexIndex + i].TextureId = 0;
			else
				s_Vertices[s_VertexIndex + i].TextureId = GetTextureFromCache(texture);
		}
		s_VertexIndex += 4;
		s_RendererData.VertexCount = s_VertexIndex;
		s_RendererData.QuadCount++;
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
		if (s_VertexIndex >= s_RendererData.MaxVertexCount || s_TextureIndex >= 32)
			End();

		for (int i = 0; i < 4; i++)
		{
			s_Vertices[s_VertexIndex + i].Position = transform * VertexPositions[i];
			s_Vertices[s_VertexIndex + i].Color = color;
			s_Vertices[s_VertexIndex + i].TextureCoord = TexCoords[i];
			s_Vertices[s_VertexIndex + i].Tiling = tiling;
			s_Vertices[s_VertexIndex + i].EntityId = entityId;

			if (!texture)
				s_Vertices[s_VertexIndex + i].TextureId = 0;
			else
				s_Vertices[s_VertexIndex + i].TextureId = GetTextureFromCache(texture);
		}
		s_VertexIndex += 4;
		s_RendererData.VertexCount = s_VertexIndex;
		s_RendererData.QuadCount++;
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

	void Renderer::Terminate()
	{
		if (!s_RendererInitialized)
		{
			ME_SYS_WAR("Trying to terminate non initialized Renderer!");
			return;
		}

		glDeleteVertexArrays(1, &s_VertexArray);
		glDeleteBuffers(1, &s_VertexBuffer);
		glDeleteBuffers(1, &s_IndexBuffer);

		delete[] s_Vertices;
		s_TextureIndex = 0;
		s_VertexIndex = 0;
		s_TextureCache.clear();

		s_RendererInitialized = false;
	}
}
