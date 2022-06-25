cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set a project name.
project(GOF)

# Find the packages we need.
find_package(OpenGL REQUIRED)
find_package(glfw3  REQUIRED)

# Add source files
file(GLOB_RECURSE SOURCE_FILES lib/*.c lib/*.cpp src/*.cpp )
# Add resource files
file(GLOB_RECURSE RESOURCE_FILES resources/*)
# Copy resource files to the current binary directory (relative paths)
file(COPY ${RESOURCE_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Add executable (Voxel-Engine)
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_FILES})

# Include directories
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link link_libraries to target 
target_link_libraries(${CMAKE_PROJECT_NAME} PUBLIC ${OPENGL_LIBRARIES} glfw)
