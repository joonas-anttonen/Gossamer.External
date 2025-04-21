cmake_minimum_required (VERSION 3.10)

project (Gossamer.Vma)

add_library (Gossamer.Vma SHARED "Gossamer.Vma.cpp")
set_target_properties(Gossamer.Vma PROPERTIES CXX_STANDARD 20)

find_package(Vulkan REQUIRED)
target_link_libraries(Gossamer.Vma PUBLIC Vulkan::Vulkan)

install(TARGETS Gossamer.Vma
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})