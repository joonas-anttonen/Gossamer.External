cmake_minimum_required (VERSION 3.10)

project (Gossamer.FreeType)

add_library (Gossamer.FreeType SHARED "Gossamer.FreeType.cpp")
set_target_properties(Gossamer.FreeType PROPERTIES CXX_STANDARD 20)
target_include_directories(Gossamer.FreeType PRIVATE "freetype/build/install/include/freetype2")
target_link_directories(Gossamer.FreeType PRIVATE "freetype/build/install/lib")

find_package(freetype REQUIRED)
target_link_libraries(Gossamer.FreeType freetype)

install(TARGETS Gossamer.FreeType
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})