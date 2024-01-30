cmake_minimum_required(VERSION 3.15)
project(Engine CXX)

add_compile_definitions(PROJECT_NAME="${PROJECT_NAME}" PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

add_library(engine
        ColorMaterial.h ColorMaterial.cpp
        Light.h
        Mesh.h Mesh.cpp
        Material.h
        mesh_loader.h mesh_loader.h
        mesh_loader.cpp mesh_loader.cpp
        PhongMaterial.h PhongMaterial.cpp
        texture.h texture.cpp
        )

target_link_libraries(engine PUBLIC spdlog::spdlog)
target_link_libraries(engine PUBLIC objreader)