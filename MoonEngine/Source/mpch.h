#pragma once
#include "Core/Debug.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <functional>
#include <memory>
#include <utility>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <map>
#include <vector>

template<typename T>
using Shared = std::shared_ptr<T>;
template<typename T, typename ... Args>
constexpr Shared<T> MakeShared(Args&& ... args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Unique = std::unique_ptr<T>;
template<typename T, typename ... Args>
constexpr Unique<T> MakeUnique(Args&& ... args)
{
	return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T>
using Weak = std::weak_ptr<T>;

#define REFLECT(x) template<class R> void reflect(R& r) { r x; }