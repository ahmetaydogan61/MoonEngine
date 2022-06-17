#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <functional>
#include <algorithm>	
#include <stdlib.h> 
#include <format>
#include <utility>
#include <time.h>
#include <memory>
#include <random>

#include <string>
#include <list>
#include <map>
#include <vector>
#include <array>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <stack>

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Core/Debug.h"
#include "Utils/Maths.h"

#define EVENT_FN_POINTER(fn) [&](Event& event) { return fn(event); }
#define BIND_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace MoonEngine
{
	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Weak = std::weak_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Weak<T> CreateWeak(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}