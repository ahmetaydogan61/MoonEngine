#pragma once
#include <cstdint>
#include <array>
#include <random>
#include <iomanip>
#include <sstream>
#include <string>

namespace MoonEngine
{
	class UUID64Generator
	{
	public:
		UUID64Generator() :
			m_RandomEngine(std::random_device{}()), m_Distribution(std::numeric_limits<uint64_t>::min(), std::numeric_limits<uint64_t>::max()) {}

		uint64_t Generate() { return m_Distribution(m_RandomEngine); }
	private:
		std::mt19937_64 m_RandomEngine;
		std::uniform_int_distribution<uint64_t> m_Distribution;
	};

	class UUID
	{
	public:
		UUID();
		UUID(const UUID&) = default;
		UUID(uint64_t uuid)
			:m_UUID(uuid)
		{}

		UUID(const std::string& uuidStr);
		std::string ToString() const;

		operator uint64_t() const { return m_UUID; }

		bool operator==(const UUID& other) const { return m_UUID == other.m_UUID; }
		bool operator!=(const UUID& other) const { return m_UUID != other.m_UUID; }

		size_t hash() const { return (m_UUID >> 32) ^ (m_UUID & 0xFFFFFFFF); }
	private:
		uint64_t m_UUID;
	};
}

namespace std 
{
	template <> struct hash<MoonEngine::UUID>
	{
		size_t operator()(const MoonEngine::UUID& uuid) const
		{
			return uuid.hash();
		}
	};
}
