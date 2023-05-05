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

	static uint32_t s_TextureIndex;
	static int32_t s_TextureIds[32];
	static std::unordered_map<Shared<Texture>, int32_t> s_TextureCache;

	static int32_t GetTextureFromCache(const Shared<Texture>& texture)
	{
		if (s_TextureCache.find(texture) != s_TextureCache.end())
			return s_TextureCache.at(texture);

		s_TextureIndex++;
		texture->Bind(s_TextureIndex);
		s_TextureCache[texture] = s_TextureIndex;
		return s_TextureIndex;
	}

	void Renderer::Init()
	{
		if (s_RendererInitialized)
		{
			ME_LOG("Trying to initialize renderer again!");
			return;
		}

		//Quad Renderer
		s_Data.QuadShader = MakeShared<Shader>("Resource/Shaders/Default.shader");
		s_Data.DefaultTexture = MakeShared<Texture>();

		s_Data.QuadVertices = new QuadVertex[s_Data.MaxVertexCount];

		glGenVertexArrays(1, &s_Data.QuadVertexArray);
		glGenBuffers(1, &s_Data.QuadVertexBuffer);
		glGenBuffers(1, &s_Data.QuadIndexBuffer);

		glBindVertexArray(s_Data.QuadVertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, s_Data.QuadVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertex) * s_Data.MaxVertexCount, s_Data.QuadVertices, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, Color));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, TextureCoord));
		glEnableVertexAttribArray(2);

		glVertexAttribIPointer(3, 1, GL_INT, sizeof(QuadVertex), (void*)offsetof(QuadVertex, TextureId));
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(QuadVertex), (void*)offsetof(QuadVertex, Tiling));
		glEnableVertexAttribArray(4);

		glVertexAttribIPointer(5, 1, GL_INT, sizeof(QuadVertex), (void*)offsetof(QuadVertex, EntityId));
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

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.QuadIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices) * s_Data.MaxIndexCount, indices, GL_STATIC_DRAW);

		delete[] indices;

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		//Line Renderer
		s_Data.LineShader = MakeShared<Shader>("Resource/Shaders/Line.shader");

		s_Data.LineVertices = new LineVertex[s_Data.MaxVertexCount];

		glGenVertexArrays(1, &s_Data.LineVertexArray);
		glGenBuffers(1, &s_Data.LineVertexBuffer);
		
		glBindVertexArray(s_Data.LineVertexArray);
		
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.LineVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertex) * s_Data.MaxVertexCount, s_Data.LineVertices, GL_DYNAMIC_DRAW);
		
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), 0);
		glEnableVertexAttribArray(0);
		
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, Color));
		glEnableVertexAttribArray(1);
		
		glVertexAttribIPointer(2, 1, GL_INT, sizeof(LineVertex), (void*)offsetof(LineVertex, EntityId));
		glEnableVertexAttribArray(2);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_LINE_SMOOTH);

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
	}

	void Renderer::End()
	{
		if (s_Data.QuadVertexIndex > 3)
		{
			s_Data.QuadShader->Bind();
			s_Data.QuadShader->SetMat4("uVP", s_Data.ViewProjection);
			s_Data.QuadShader->SetIntArray("uTexture", 32, s_TextureIds);

			s_Data.DefaultTexture->Bind(0);
			
			RenderIndexed();

			s_Data.QuadVertexIndex = 0;
			s_TextureIndex = 0;
			s_TextureCache.clear();
		}

		if (s_Data.LineVertexIndex > 1)
		{
			s_Data.LineShader->Bind();
			s_Data.LineShader->SetMat4("uVP", s_Data.ViewProjection);

			RenderLines();

			s_Data.LineVertexIndex = 0;
		}
	}

	void Renderer::RenderIndexed()
	{
		glBindVertexArray(s_Data.QuadVertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.QuadVertexBuffer);

		//Obsolete
		//glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * s_VertexIndex, s_Vertices, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(QuadVertex) * s_Data.QuadVertexIndex, s_Data.QuadVertices);

		glDrawElements(GL_TRIANGLES, s_Data.QuadVertexIndex * 1.5f, GL_UNSIGNED_INT, 0);
		s_Stats.DrawCalls++;
	}

	void Renderer::RenderLines()
	{
		glBindVertexArray(s_Data.LineVertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data.LineVertexBuffer);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(LineVertex) * s_Data.LineVertexIndex, s_Data.LineVertices);

		glDrawArrays(GL_LINES, 0, s_Data.LineVertexIndex);
		s_Stats.DrawCalls++;
	}

	void Renderer::DrawQuad(const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale,
							const glm::vec4& color, const Shared<Texture>& texture, const glm::vec2& tiling)
	{
		const glm::mat4& rotationMat = glm::toMat4(glm::quat(rotation));
		const glm::mat4& transform = glm::translate(glm::mat4(1.0f), position) * rotationMat * glm::scale(glm::mat4(1.0f), scale);

		DrawQuad(transform, color, texture, tiling);
	}

	void Renderer::DrawQuad(const glm::mat4& transform, const glm::vec4& color, const Shared<Texture>& texture, const glm::vec2& tiling)
	{
		uint32_t vertexIndex = s_Data.QuadVertexIndex;

		if (vertexIndex >= s_Data.MaxVertexCount || s_TextureIndex >= 32)
		{
			End();
			vertexIndex = 0;
		}

		for (int i = 0; i < 4; i++)
		{
			s_Data.QuadVertices[vertexIndex + i].Position = transform * VertexPositions[i];
			s_Data.QuadVertices[vertexIndex + i].Color = color;
			s_Data.QuadVertices[vertexIndex + i].TextureCoord = TexCoords[i];
			s_Data.QuadVertices[vertexIndex + i].Tiling = tiling;
			s_Data.QuadVertices[vertexIndex + i].EntityId = -1;

			if (!texture)
				s_Data.QuadVertices[vertexIndex + i].TextureId = 0;
			else
				s_Data.QuadVertices[vertexIndex + i].TextureId = GetTextureFromCache(texture);
		}
		s_Data.QuadVertexIndex += 4;
		s_Stats.VertexCount = s_Data.QuadVertexIndex;
		s_Stats.QuadCount++;
	}

	void Renderer::DrawEntity(const glm::mat4& transform, const SpriteComponent& spriteComponent, int entityId)
	{
		DrawEntity(transform, spriteComponent.Color, spriteComponent.Texture, spriteComponent.Tiling, entityId);
	}

	void Renderer::DrawEntity(const TransformComponent& transformComponent, const SpriteComponent& spriteComponent, int entityId)
	{
		const glm::mat4& rotationMat = glm::toMat4(glm::quat(transformComponent.Rotation));
		const glm::mat4& transform = glm::translate(glm::mat4(1.0f), transformComponent.Position)
			* rotationMat * glm::scale(glm::mat4(1.0f), transformComponent.Scale);

		DrawEntity(transform, spriteComponent, entityId);
	}

	void Renderer::DrawEntity(const glm::mat4& transform, const glm::vec4& color, const Shared<Texture>& texture, const glm::vec2& tiling, int entityId)
	{
		uint32_t vertexIndex = s_Data.QuadVertexIndex;

		if (vertexIndex >= s_Data.MaxVertexCount || s_TextureIndex >= 32)
		{
			End();
			vertexIndex = 0;
		}

		for (int i = 0; i < 4; i++)
		{
			s_Data.QuadVertices[vertexIndex + i].Position = transform * VertexPositions[i];
			s_Data.QuadVertices[vertexIndex + i].Color = color;
			s_Data.QuadVertices[vertexIndex + i].TextureCoord = TexCoords[i];
			s_Data.QuadVertices[vertexIndex + i].Tiling = tiling;
			s_Data.QuadVertices[vertexIndex + i].EntityId = entityId;

			if (!texture)
				s_Data.QuadVertices[vertexIndex + i].TextureId = 0;
			else
				s_Data.QuadVertices[vertexIndex + i].TextureId = GetTextureFromCache(texture);
		}
		s_Data.QuadVertexIndex += 4;
		s_Stats.VertexCount = s_Data.QuadVertexIndex;
		s_Stats.QuadCount++;
	}

	void Renderer::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityId)
	{
		uint32_t vertexIndex = s_Data.LineVertexIndex;

		if (vertexIndex >= s_Data.MaxVertexCount)
			End();

		s_Data.LineVertices[vertexIndex].Position = p0;
		s_Data.LineVertices[vertexIndex].Color = color;
		s_Data.LineVertices[vertexIndex].EntityId = entityId;
		vertexIndex++;

		s_Data.LineVertices[vertexIndex].Position = p1;
		s_Data.LineVertices[vertexIndex].Color = color;
		s_Data.LineVertices[vertexIndex].EntityId = entityId;
		vertexIndex++;

		s_Data.LineVertexIndex = vertexIndex;
		s_Stats.LineCount++;
	}

	void Renderer::DrawRect(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color, int entityId)
	{
		glm::vec3 p0 = glm::vec3(position.x - scale.x * 0.5f, position.y - scale.y * 0.5f, position.z);
		glm::vec3 p1 = glm::vec3(position.x + scale.x * 0.5f, position.y - scale.y * 0.5f, position.z);
		glm::vec3 p2 = glm::vec3(position.x + scale.x * 0.5f, position.y + scale.y * 0.5f, position.z);
		glm::vec3 p3 = glm::vec3(position.x - scale.x * 0.5f, position.y + scale.y * 0.5f, position.z);

		DrawLine(p0, p1, color, entityId);
		DrawLine(p1, p2, color, entityId);
		DrawLine(p2, p3, color, entityId);
		DrawLine(p3, p0, color, entityId);
	}

	void Renderer::DrawRect(const glm::mat4& transform, const glm::vec4& color, int entityId)
	{
		glm::vec3 lineVertices[4];
		for (size_t i = 0; i < 4; i++)
			lineVertices[i] = transform * VertexPositions[i];

		DrawLine(lineVertices[0], lineVertices[1], color, entityId);
		DrawLine(lineVertices[1], lineVertices[2], color, entityId);
		DrawLine(lineVertices[2], lineVertices[3], color, entityId);
		DrawLine(lineVertices[3], lineVertices[0], color, entityId);
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

		glDeleteVertexArrays(1, &s_Data.QuadVertexArray);
		glDeleteBuffers(1, &s_Data.QuadVertexBuffer);
		glDeleteBuffers(1, &s_Data.QuadIndexBuffer);
		s_Data.QuadVertexIndex = 0;

		delete[] s_Data.QuadVertices;
		s_TextureIndex = 0;
		s_TextureCache.clear();

		s_RendererInitialized = false;
	}
}
