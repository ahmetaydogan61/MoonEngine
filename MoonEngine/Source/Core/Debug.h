#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace MoonEngine
{
	class Debug
	{
	public:
		static void Init();
		static void Terminate();

		static std::shared_ptr<spdlog::logger>& GetLogger() { return s_Logger; }
		static std::shared_ptr<spdlog::logger>& GetSystemLogger() { return s_SystemLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_Logger;
		static std::shared_ptr<spdlog::logger> s_SystemLogger;
	};
}

#ifdef ENGINE_DEBUG

#define ME_LOG(...) MoonEngine::Debug::GetLogger()->trace(__VA_ARGS__)
#define ME_SUC(...) MoonEngine::Debug::GetLogger()->info(__VA_ARGS__)
#define ME_WAR(...) MoonEngine::Debug::GetLogger()->warn(__VA_ARGS__)
#define ME_ERR(...) MoonEngine::Debug::GetLogger()->error(__VA_ARGS__)
#define ME_CRT(...) MoonEngine::Debug::GetLogger()->critical(__VA_ARGS__)

#define ME_SYS_LOG(...) MoonEngine::Debug::GetSystemLogger()->trace(__VA_ARGS__)
#define ME_SYS_SUC(...) MoonEngine::Debug::GetSystemLogger()->info(__VA_ARGS__)
#define ME_SYS_WAR(...) MoonEngine::Debug::GetSystemLogger()->warn(__VA_ARGS__)
#define ME_SYS_ERR(...) MoonEngine::Debug::GetSystemLogger()->error(__VA_ARGS__)
#define ME_SYS_CRT(...) MoonEngine::Debug::GetSystemLogger()->critical(__VA_ARGS__)

#elif ENGINE_RELEASE

#define ME_LOG(...)
#define ME_SUC(...)
#define ME_WAR(...)
#define ME_ERR(...)
#define ME_CRT(...)

#define ME_SYS_LOG(...) 
#define ME_SYS_SUC(...) 
#define ME_SYS_WAR(...) 
#define ME_SYS_ERR(...) 
#define ME_SYS_CRT(...)

#endif // ENGINE_DEBUG

#define ME_ASSERT(condition, message) if(!condition) { ME_CRT("ALERT: {} (file: {} line: {})", message, __FILE__, __LINE__); __debugbreak(); } 
