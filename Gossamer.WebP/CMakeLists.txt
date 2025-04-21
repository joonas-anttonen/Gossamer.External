cmake_minimum_required (VERSION 3.10)

project (Gossamer.WebP)

add_library (Gossamer.WebP SHARED "Gossamer.WebP.cpp")
set_target_properties(Gossamer.WebP PROPERTIES CXX_STANDARD 20)

find_package(WebP REQUIRED)
target_link_libraries(Gossamer.WebP WebP::webp WebP::webpdecoder)

install(TARGETS Gossamer.WebP
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})