cmake_minimum_required(VERSION 3.3)

set(DEFAULT_BUILD_TYPE "Debug")
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

set(LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

project(3D)

set(LIB_GLEW_LIBRARY "Path to your GLEW library glew32.lib or glew32.a file" CACHE FILEPATH "Path to your glew32.lib or glew32.a file")
set(LIB_GLEW_INCLUDE_DIR "Path to your GLEW library 'include' directory (should contain folder GL)" CACHE PATH "Path to your glew include folder (should contain folder GL)")

set(LIB_BULLET_INCLUDE_DIR "Path to your bullet 'src' directory" CACHE PATH "Path to your bullet 'src' directory")
set(LIB_BULLET_DYNAMICS_LIBRARY "Path to your bullet dynamics library file" CACHE FILEPATH "Path to your bullet dynamics library file")
set(LIB_BULLET_COLLISION_LIBRARY "Path to your bullet collision library file" CACHE FILEPATH "Path to your bullet collision library file")
set(LIB_BULLET_MATH_LIBRARY "Path to your bullet linear math file" CACHE FILEPATH "Path to your bullet linear math file")

set(LIB_FREETYPE_INCLUDE_DIR "Path to your freetype library 'include' directory" CACHE PATH "Path to your freetype library 'include' directory")
set(LIB_FREETYPE_LIBRARY "Path to your freetype 2.8.1 library file (freetype281.lib or freetype281.a)" CACHE FILEPATH "Path to your freetype 2.8.1 library file (freetype281.lib or freetype281.a)")

set(LIB_SOIL_INCLUDE_DIR "Path to your SOIL library 'src' directory" CACHE PATH "Path to your SOIL library 'src' directory")
set(LIB_SOIL_LIBRARY "Path to your SOIL library soil.lib or soil.a file" CACHE FILEPATH "Path to your SOIL library soil.lib or soil.a file")

set(LIB_ASSIMP_INCLUDE_DIR "Path to your assimp library 'include' directory" CACHE PATH "Path to your assimp library 'include' directory")
set(LIB_ASSIMP_LIBRARY "Path to your assimp library assimp.lib or assimp.a file" CACHE FILEPATH "Path to your assimp library assimp.lib or assimp.a file")

set(LIB_GLM_INCLUDE_DIR "Path to your GLM include folder, should be 'glm'" CACHE PATH "Path to your GLM include folder, should be 'glm'")

set(BULLET_LIBRARIES ${LIB_BULLET_DYNAMICS_LIBRARY} ${LIB_BULLET_COLLISION_LIBRARY} ${LIB_BULLET_MATH_LIBRARY})

set(XML_INCLUDE_DIR "${LIBS_DIR}/rapidxml")

find_package(OpenGL REQUIRED)

file(GLOB CORE_INCLUDES ${INCLUDE_DIR}/core/*.hpp)
file(GLOB RENDERER_INCLUDES ${INCLUDE_DIR}/renderer/*.hpp)
file(GLOB OS_INCLUDES ${INCLUDE_DIR}/os/*.hpp)
file(GLOB OBJECTSASSETS_INCLUDES ${INCLUDE_DIR}/objectsassets/*.chpppp)
file(GLOB UI_INCLUDES ${INCLUDE_DIR}/ui/*.hpp)
file(GLOB SHADERS_INCLUDES ${INCLUDE_DIR}/shaders/*.hpp)
file(GLOB UTIL_INCLUDES ${INCLUDE_DIR}/util/*.hpp)

file(GLOB CORE_SOURCES ${SRC_DIR}/core/*.cpp)
file(GLOB RENDERER_SOURCES ${SRC_DIR}/renderer/*.cpp)
file(GLOB OS_SOURCES ${SRC_DIR}/os/*.cpp)
file(GLOB OBJECTSASSETS_SOURCES ${SRC_DIR}/objectsassets/*.cpp)
file(GLOB UI_SOURCES ${SRC_DIR}/ui/*.cpp)
file(GLOB SHADERS_SOURCES ${SRC_DIR}/shaders/*.cpp)
file(GLOB UTIL_SOURCES ${SRC_DIR}/util/*.cpp)

set(MY_SOURCES ${CORE_SOURCES} ${OBJECTSASSETS_SOURCES} ${OS_SOURCES} ${RENDERER_SOURCES} ${UI_SOURCES} ${UTIL_SOURCES} ${SHADERS_SOURCES})
set(MY_INCLUDES ${CORE_INCLUDES} ${OBJECTSASSETS_INCLUDES} ${OS_INCLUDES} ${RENDERER_INCLUDES} ${UI_INCLUDES} ${UTIL_INCLUDES} ${SHADERS_INCLUDES})

set(ALL_INCLUDE_DIRS ${INCLUDE_DIR}/core ${INCLUDE_DIR}/objectsassets ${INCLUDE_DIR}/os ${INCLUDE_DIR}/renderer ${INCLUDE_DIR}/shaders ${INCLUDE_DIR}/ui ${INCLUDE_DIR}/util
		${OPENGL_INCLUDE_DIR} ${BOOST_INCLUDE_DIR} ${LIB_FREETYPE_INCLUDE_DIR} ${LIB_BULLET_INCLUDE_DIR} ${LIB_GLEW_INCLUDE_DIR} ${LIB_GLM_INCLUDE_DIR} ${LIB_SOIL_INCLUDE_DIR} ${LIB_ASSIMP_INCLUDE_DIR} ${XML_INCLUDE_DIR}
		${LIBS_DIR})

file(GLOB XML_INCLUDES ${XML_INCLUDE_DIR}/*.hpp)
file(GLOB BOOST_INCLUDES ${BOOST_INCLUDE_DIR}/*.hpp)
file(GLOB_RECURSE GLM_INCLUDES ${LIB_GLM_INCLUDE_DIR}/*.hpp)

set(CMAKE_CXX_STANDARD 11)
list(APPEND CMAKE_CXX_FLAGS "-fpermissive -w -m32")

SET(GCC_COVERAGE_LINK_FLAGS "--verbose")

include_directories(${ALL_INCLUDE_DIRS})

add_executable(3D ${MY_SOURCES} ${MY_INCLUDES})

set_target_properties(3D PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

target_link_libraries(3D ${OPENGL_LIBRARIES} ${LIB_GLEW_LIBRARY} ${LIB_FREETYPE_LIBRARY} ${BULLET_LIBRARIES} ${LIB_ASSIMP_LIBRARY} ${LIB_SOIL_LIBRARY} ${LIB_ASSIMP_LIBRARY})

source_group("Header Files\\Core" FILES ${CORE_INCLUDES})
source_group("Header Files\\Renderer" FILES ${RENDERER_INCLUDES})
source_group("Header Files\\OS" FILES ${OS_INCLUDES})
source_group("Header Files\\ObjectsAssets" FILES ${OBJECTSASSETS_INCLUDES})
source_group("Header Files\\UI" FILES ${UI_INCLUDES})
source_group("Header Files\\Shaders" FILES ${SHADERS_INCLUDES})
source_group("Header Files\\Util" FILES ${UTIL_INCLUDES})

source_group("Source Files\\Core" FILES ${CORE_SOURCES})
source_group("Source Files\\Renderer" FILES ${RENDERER_SOURCES})
source_group("Source Files\\OS" FILES ${OS_SOURCES})
source_group("Source Files\\ObjectsAssets" FILES ${OBJECTSASSETS_SOURCES})
source_group("Source Files\\UI" FILES ${UI_SOURCES})
source_group("Source Files\\Shaders" FILES ${SHADERS_SOURCES})
source_group("Source Files\\Util" FILES ${UTIL_SOURCES})

source_group("Library Files\\Boost" FILES ${BOOST_INCLUDES})
source_group("Library Files\\RapidXML" FILES ${XML_INCLUDES})

set(CMAKE_SUPPRESS_REGENERATION true)
set(VS_STARTUP_PROJECT "3D")