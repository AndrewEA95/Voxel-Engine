cmake_minimum_required(VERSION 3.16)
project(VoxelEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# GLFW
add_subdirectory(extern/glfw)

# GLAD
add_library(glad STATIC extern/glad/src/glad.c)
target_include_directories(glad PUBLIC extern/glad/include)

# GLM(header-only)
add_subdirectory(extern/glm-1.0.2)

# Enabel warnings
if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(voxel_engine
    src/main.cpp
    src/core/Log.cpp
    src/core/CLI.cpp
    src/core/Engine.cpp
    src/core/Time.cpp
    src/core/Input.cpp
    src/core/ChunkManager.cpp
    src/game/Game.cpp
    src/platform/Window.cpp
    src/render/Camera.cpp
    src/render/Render.cpp
    src/render/Shader.cpp
    src/render/IndexBuffer.cpp
    src/render/Mesh.cpp
    src/render/VertexArray.cpp
    src/render/VertexBuffer.cpp
    src/voxel/Chunk.cpp
    src/voxel/ChunkMesher.cpp
    src/voxel/TerrainGenerator.cpp
)

target_link_libraries(voxel_engine
    PRIVATE
        glad
        glfw
        glm
)

if(APPLE)
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
    find_library(OpenGL_LIBRARY OpenGL REQUIRED)

    target_link_libraries(voxel_engine PRIVATE
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
        ${OpenGL_LIBRARY}
    )
endif()