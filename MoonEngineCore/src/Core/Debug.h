#pragma once
constexpr auto TERMINALCOLOR_GREEN = "\033[32m";
constexpr auto TERMINALCOLOR_RED = "\033[31m";
constexpr auto TERMINALCOLOR_YELLOW = "\033[33m";
constexpr auto TERMINALCOLOR_RESET = "\033[0m";

#define DebugNor(msg) std::cout << msg << '\n'
#define DebugSuc(msg) std::cout <<  TERMINALCOLOR_GREEN << msg << TERMINALCOLOR_RESET << '\n' 
#define DebugSys(msg) std::cout << "[SYSTEM]: " << msg << '\n' 
#define DebugWar(msg) std::cout <<  TERMINALCOLOR_YELLOW << "[WARNING]: " << msg << TERMINALCOLOR_RESET << '\n'
#define DebugErr(msg) std::cout << TERMINALCOLOR_RED << "[ERROR]: " << msg << TERMINALCOLOR_RESET << '\n'

#define DebugBrk(condition, message) if(condition) { DebugWar("[BREAK]: " << message); __debugbreak(); }