#include "mpch.h"
#include "Core/Debug.h"

namespace MoonEngine
{
	std::shared_ptr<spdlog::logger> Debug::s_Logger;
	std::shared_ptr<spdlog::logger> Debug::s_SystemLogger;

	void Debug::Init()
	{
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		consoleSink->set_pattern("%^[%T.%e] %v%$");
		std::vector<spdlog::sink_ptr> sinks{ consoleSink };

		s_Logger = std::make_shared<spdlog::logger>("Logger", sinks.begin(), sinks.end());
		s_Logger->set_level(spdlog::level::trace);
		s_Logger->flush_on(spdlog::level::trace);
		spdlog::register_logger(s_Logger);

		auto systemSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		systemSink->set_pattern("%^[%T.%e] [SYSTEM]: %v%$");
		std::vector<spdlog::sink_ptr> systemSinks{ systemSink };

		s_SystemLogger = std::make_shared<spdlog::logger>("SystemLogger", systemSinks.begin(), systemSinks.end());
		s_SystemLogger->set_level(spdlog::level::trace);
		s_SystemLogger->flush_on(spdlog::level::trace);
		spdlog::register_logger(s_SystemLogger);
	}

	void Debug::Terminate()
	{
		spdlog::shutdown();
	}
}
