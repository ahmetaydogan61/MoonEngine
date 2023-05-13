#include "mpch.h"
#include "Renderer/Renderer.h"

#include "Engine/Components.h"

#include "Renderer/Shader.h"
#include "Renderer/Texture.h"

#include <glad/glad.h>

namespace MoonEngine
{
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

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoord;
		int32_t TextureId;
		glm::vec2 Tiling;
		int EntityId;
	};

	struct LineVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		int EntityId;
	};

	struct QuadLayerArray
	{
		QuadVertex* QuadVertices = nullptr;
		uint32_t QuadVertexIndex = 0;
	};

	struct RenderData
	{
		const static uint32_t MaxLayers = 16;
		const static uint32_t MaxQuads = 256;
		const static uint32_t MaxVertices = MaxQuads * 4;
		const static uint32_t MaxIndices = MaxQuads * 6;

		//Renderer Data
		glm::vec3 ClearColor = glm::vec3(0.0f);
		glm::mat4 ViewProjection = glm::mat4(1.0f);

		//Quad Renderer
		uint32_t QuadVertexArray = 0;
		uint32_t QuadVertexBuffer = 0;
		uint32_t QuadIndexBuffer = 0;
		QuadLayerArray* QLayerArray = nullptr;

		Shared<Shader> QuadShader = nullptr;
		Shared<Texture> QuadTexture = nullptr;

		int32_t TextureIds[32];
		uint32_t TextureIndex = 0;
		std::unordered_map<Shared<Texture>, int32_t> TextureCache;

		//Line Renderer
		uint32_t LineVertexArray = 0;
		uint32_t LineVertexBuffer = 0;

		LineVertex* LineVertices = nullptr;
		uint32_t LineVertexIndex = 0;

		Shared<Shader> LineShader = nullptr;

		int32_t GetTextureFromCache(const Shared<Texture>& texture)
		{
			if (TextureCache.find(texture) != TextureCache.end())
				return TextureCache.at(texture);

			TextureIndex++;
			texture->Bind(TextureIndex);
			TextureCache[texture] = TextureIndex;
			return TextureIndex;
		}
	};

	static RenderData* s_Data;
	RendererStats* Renderer::s_Stats = nullptr;

	void Renderer::SetClearColor(const glm::vec3& color)
	{
		glClearColor(color.x, color.y, color.z, 1.0f);
		s_Data->ClearColor = color;
	}

	void Renderer::Init()
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_LINE_SMOOTH);

		s_Data = new RenderData();
		s_Stats = new RendererStats();
		s_Stats->MaxLayers = s_Data->MaxLayers;

		//+Quad Renderer Init

		s_Data->QLayerArray = new QuadLayerArray[s_Data->MaxLayers];

		for (int i = 0; i < s_Data->MaxLayers; i++)
			s_Data->QLayerArray[i].QuadVertices = new QuadVertex[s_Data->MaxVertices];

		uint32_t* indices = new uint32_t[s_Data->MaxIndices];
		uint32_t indicesIndex = 0;

		for (uint32_t i = 0; i < s_Data->MaxIndices; i += 6)
		{
			indices[i + 0] = 0 + indicesIndex;
			indices[i + 1] = 1 + indicesIndex;
			indices[i + 2] = 2 + indicesIndex;

			indices[i + 3] = 0 + indicesIndex;
			indices[i + 4] = 2 + indicesIndex;
			indices[i + 5] = 3 + indicesIndex;

			indicesIndex += 4;
		}

		glGenVertexArrays(1, &s_Data->QuadVertexArray);
		glGenBuffers(1, &s_Data->QuadVertexBuffer);
		glGenBuffers(1, &s_Data->QuadIndexBuffer);

		glBindVertexArray(s_Data->QuadVertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, s_Data->QuadVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(QuadVertex) * s_Data->MaxVertices, nullptr, GL_DYNAMIC_DRAW);

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

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data->QuadIndexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * s_Data->MaxIndices, indices, GL_STATIC_DRAW);

		delete[] indices;

		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		s_Data->QuadShader = MakeShared<Shader>("Resource/Shaders/Default.shader");

		s_Data->QuadTexture = MakeShared<Texture>();
		for (int32_t i = 0; i < 32; i++)
			s_Data->TextureIds[i] = i;
		s_Data->TextureCache.reserve(32);

		//-Quad Renderer Init
		//+Line Renderer Init

		s_Data->LineVertices = new LineVertex[s_Data->MaxVertices];

		glGenVertexArrays(1, &s_Data->LineVertexArray);
		glGenBuffers(1, &s_Data->LineVertexBuffer);

		glBindVertexArray(s_Data->LineVertexArray);

		glBindBuffer(GL_ARRAY_BUFFER, s_Data->LineVertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertex) * s_Data->MaxVertices, nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)offsetof(LineVertex, Color));
		glEnableVertexAttribArray(1);

		glVertexAttribIPointer(2, 1, GL_INT, sizeof(LineVertex), (void*)offsetof(LineVertex, EntityId));
		glEnableVertexAttribArray(2);

		s_Data->LineShader = MakeShared<Shader>("Resource/Shaders/Line.shader");

		//+Line Renderer Init

	}

	void Renderer::Begin(const glm::mat4& viewProjection)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		s_Data->ViewProjection = viewProjection;

		s_Data->TextureCache.clear();
		s_Data->TextureIndex = 0;

		s_Stats->DrawCalls = 0;
	}

	void Renderer::End()
	{
		s_Data->QuadShader->Bind();
		s_Data->QuadShader->SetMat4("uVP", s_Data->ViewProjection);
		s_Data->QuadShader->SetIntArray("uTexture", 32, s_Data->TextureIds);
		s_Data->QuadTexture->Bind(0);
		RenderIndexed();

		if (s_Data->LineVertexIndex > 1)
		{
			s_Data->LineShader->Bind();
			s_Data->LineShader->SetMat4("uVP", s_Data->ViewProjection);

			RenderLines();

			s_Data->LineVertexIndex = 0;
		}
	}

	void Renderer::RenderIndexed()
	{
		glBindVertexArray(s_Data->QuadVertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data->QuadVertexBuffer);

		for (uint32_t i = 0; i < s_Data->MaxLayers; i++)
		{
			if (s_Data->QLayerArray[i].QuadVertexIndex >= 4)
			{
				uint32_t quadQuadVertexIndex = s_Data->QLayerArray[i].QuadVertexIndex;

				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(QuadVertex) * quadQuadVertexIndex, s_Data->QLayerArray[i].QuadVertices);
				glDrawElements(GL_TRIANGLES, quadQuadVertexIndex * 1.5f, GL_UNSIGNED_INT, 0);
				s_Stats->DrawCalls++;
			}
			s_Data->QLayerArray[i].QuadVertexIndex = 0;
		}
	}

	void Renderer::RenderLines()
	{
		glBindVertexArray(s_Data->LineVertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, s_Data->LineVertexBuffer);

		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(LineVertex) * s_Data->LineVertexIndex, s_Data->LineVertices);

		glDrawArrays(GL_LINES, 0, s_Data->LineVertexIndex);
		s_Stats->DrawCalls++;
	}

	//+Quad Renderer

	void Renderer::DrawEntity(const TransformComponent& tC, const SpriteComponent& sC, int entityId)
	{
		const glm::mat4& rotationMat = glm::toMat4(glm::quat(tC.Rotation));
		const glm::mat4& transform = glm::translate(glm::mat4(1.0f), tC.Position) * rotationMat * glm::scale(glm::mat4(1.0f), tC.Scale);

		DrawEntity(transform, sC.Color, sC.Texture, sC.Layer, sC.Tiling, entityId);
	}

	void Renderer::DrawEntity(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& rotation, const glm::vec4& color, const Shared<Texture>& texture, int layer, const glm::vec2& tiling, int entityId)
	{
		const glm::mat4& rotationMat = glm::toMat4(glm::quat(rotation));
		const glm::mat4& transform = glm::translate(glm::mat4(1.0f), position) * rotationMat * glm::scale(glm::mat4(1.0f), scale);

		DrawEntity(transform, color, texture, layer, tiling, entityId);
	}

	void Renderer::DrawEntity(const glm::mat4& transform, const glm::vec4& color, const Shared<Texture>& texture, int layer, const glm::vec2& tiling, int entityId)
	{
		bool layerException = layer < 0 || layer >= s_Data->MaxLayers;
		ME_ASSERT(!layerException, "Layer out of bounds!");

		if (s_Data->QLayerArray[layer].QuadVertexIndex >= s_Data->MaxVertices)
			End();

		if (s_Data->TextureIndex >= 32)
		{
			End();
			s_Data->TextureCache.clear();
			s_Data->TextureIndex = 0;
		}

		uint32_t vertexIndex = s_Data->QLayerArray[layer].QuadVertexIndex;

		const QuadLayerArray& layerArray = s_Data->QLayerArray[layer];

		for (int i = 0; i < 4; i++)
		{
			layerArray.QuadVertices[vertexIndex + i].Position = transform * VertexPositions[i];
			layerArray.QuadVertices[vertexIndex + i].Color = color;
			layerArray.QuadVertices[vertexIndex + i].TextureCoord = TexCoords[i];
			layerArray.QuadVertices[vertexIndex + i].Tiling = tiling;
			layerArray.QuadVertices[vertexIndex + i].EntityId = entityId;

			if (texture)
				layerArray.QuadVertices[vertexIndex + i].TextureId = s_Data->GetTextureFromCache(texture);
			else
				layerArray.QuadVertices[vertexIndex + i].TextureId = 0;
		}

		s_Data->QLayerArray[layer].QuadVertexIndex += 4;
	}

	//-Quad Renderer
	//+Line Renderer

	void Renderer::DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec4& color, int entityId)
	{
		uint32_t vertexIndex = s_Data->LineVertexIndex;

		if (vertexIndex >= s_Data->MaxVertices)
			End();

		s_Data->LineVertices[vertexIndex].Position = p0;
		s_Data->LineVertices[vertexIndex].Color = color;
		s_Data->LineVertices[vertexIndex].EntityId = entityId;
		vertexIndex++;

		s_Data->LineVertices[vertexIndex].Position = p1;
		s_Data->LineVertices[vertexIndex].Color = color;
		s_Data->LineVertices[vertexIndex].EntityId = entityId;
		vertexIndex++;

		s_Data->LineVertexIndex = vertexIndex;
	}

	void Renderer::DrawRect(const glm::vec3& position, const glm::vec2& scale, const glm::vec4& color, int entityId)
	{
		const glm::vec3& p0 = glm::vec3(position.x - scale.x * 0.5f, position.y - scale.y * 0.5f, position.z);
		const glm::vec3& p1 = glm::vec3(position.x + scale.x * 0.5f, position.y - scale.y * 0.5f, position.z);
		const glm::vec3& p2 = glm::vec3(position.x + scale.x * 0.5f, position.y + scale.y * 0.5f, position.z);
		const glm::vec3& p3 = glm::vec3(position.x - scale.x * 0.5f, position.y + scale.y * 0.5f, position.z);

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

	//-Line Renderer

	void Renderer::Terminate()
	{
		glDeleteVertexArrays(1, &s_Data->QuadVertexArray);
		glDeleteBuffers(1, &s_Data->QuadVertexBuffer);
		glDeleteBuffers(1, &s_Data->QuadIndexBuffer);

		for (int i = 0; i < s_Data->MaxLayers; i++)
		{
			s_Data->QLayerArray[i].QuadVertexIndex = 0;
			delete[] s_Data->QLayerArray[i].QuadVertices;
			s_Data->QLayerArray[i].QuadVertices = nullptr;
		}

		delete[] s_Data->QLayerArray;

		delete s_Data;
		s_Data = nullptr;

		delete s_Stats;
		s_Stats = nullptr;
	}
}