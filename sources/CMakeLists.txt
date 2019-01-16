cmake_minimum_required(VERSION 3.10.2)

if(WIN32)
   set(CMAKE_GENERATOR "Visual Studio 17")
elseif(UNIX AND NOT APPLE)
   set(CMAKE_GENERATOR "make")
elseif(APPLE)
   set(CMAKE_GENERATOR "make")
endif()

if(WIN32)
   set(CMAKE_C_COMPILER "clang-cl")
   set(CMAKE_CXX_COMPILER "clang-cl")
elseif(UNIX AND NOT APPLE)
   set(CMAKE_C_COMPILER "clang")
   set(CMAKE_CXX_COMPILER "clang++")
elseif(APPLE)
   set(CMAKE_C_COMPILER "clang")
   set(CMAKE_CXX_COMPILER "clang++")
endif()

project(App)

# file(GLOB_RECURSE SOURCES "*.cpp")

# add_definitions(-std=c++17 -g -Wall -Wextra)
# add_definitions(-std=c++17 -g0 -w)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../includes /usr/local/include)
LINK_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/../libraries /usr/local/lib)

# add_executable(${PROJECT_NAME} ${SOURCES})
add_executable(${PROJECT_NAME} "main.cpp" "glad.c")

if(WIN32)
   target_link_libraries(${PROJECT_NAME} opengl32 glfw3 assimp-vc140-mt)
elseif(UNIX AND NOT APPLE)
   target_link_libraries(${PROJECT_NAME} pthread GL glfw dl assimp)
elseif(APPLE)
   find_package(OpenGL REQUIRED)
   include_directories(${OPENGL_INCLUDE_DIR})

   target_link_libraries(${PROJECT_NAME} pthread ${OPENGL_LIBRARIES} glfw dl assimp)
endif()