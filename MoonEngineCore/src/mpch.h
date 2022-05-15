#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <functional>
#include <algorithm>	
#include <stdlib.h> 
#include <format>
#include <utility>
#include <time.h>

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

#include "Core/Debug.h"
#define EVENT_FN_POINTER(fn) [&](Event& event) { return fn(event); }
#define BIND_FN(fn) std::bind(&fn, this, std::placeholders::_1)
