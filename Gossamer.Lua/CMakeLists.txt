cmake_minimum_required (VERSION 3.10)

project (Gossamer.Lua)

#add include directories from Lua/src
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Lua/src)

add_library(Gossamer.Lua SHARED 
        "Lua/src/lapi.c" "Lua/src/lauxlib.c" "Lua/src/lbaselib.c" "Lua/src/lutf8lib.c" "Lua/src/lcode.c" "Lua/src/lcorolib.c"
        "Lua/src/lctype.c" "Lua/src/ldblib.c" "Lua/src/ldebug.c" "Lua/src/ldo.c" "Lua/src/ldump.c" "Lua/src/lfunc.c" "Lua/src/lgc.c"
        "Lua/src/linit.c" "Lua/src/liolib.c" "Lua/src/llex.c" "Lua/src/lmathlib.c" "Lua/src/lmem.c" "Lua/src/loadlib.c" "Lua/src/lobject.c"
        "Lua/src/lopcodes.c" "Lua/src/loslib.c" "Lua/src/lparser.c" "Lua/src/lstate.c" "Lua/src/lstring.c" "Lua/src/lstrlib.c"
        "Lua/src/ltable.c" "Lua/src/ltablib.c" "Lua/src/ltm.c" "Lua/src/lundump.c" "Lua/src/lvm.c" "Lua/src/lzio.c"
        "Gossamer.Lua.cpp")

set_target_properties(Gossamer.Lua PROPERTIES CXX_STANDARD 20)

install(TARGETS Gossamer.Lua
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})