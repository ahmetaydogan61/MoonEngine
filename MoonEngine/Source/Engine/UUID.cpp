#include "mpch.h"
#include "UUID.h"

namespace MoonEngine
{
	static UUID64Generator s_UUIDGenerator;

	UUID::UUID()
	{
		m_UUID = s_UUIDGenerator.Generate();
	}

	UUID::UUID(const std::string& uuidStr)
		:m_UUID(0)
	{
		auto str = uuidStr;
		str.erase(std::remove(str.begin(), str.end(), '-'), str.end());

		std::stringstream ss(str);
		uint64_t value = 0;
		ss >> std::hex >> value;

		if (!ss)
			throw std::invalid_argument("Invalid UUID string");

		m_UUID = value;
	}

	std::string UUID::ToString() const
	{
		std::stringstream ss;
		ss << std::hex << std::setfill('0') << std::setw(16) << m_UUID;

		std::string ssstr = ss.str();
		ssstr.insert(8, "-");
		ssstr.insert(13, "-");

		return ssstr;
	}
}