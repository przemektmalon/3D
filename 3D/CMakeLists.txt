cmake_minimum_required(VERSION 3.3)

#set(CMAKE_C_COMPILER "clang-cl.exe")
#set(CMAKE_CXX_COMPILER "g++.exe")

set(LIBS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs")
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

project(3D)

#set( CMAKE_VERBOSE_MAKEFILE on )

set(BOOST_ROOT "${LIBS_DIR}/boost")

set(FREETYPE_INCLUDE_DIR "${LIBS_DIR}/freetype/include")
set(FREETYPE_LIBRARY "${LIBS_DIR}/freetype/lib/freetype281.lib")

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	set(BULLET_LIB_POSTFIX "_Debug")
else()
	set(BULLET_LIB_POSTFIX "")
endif()

set(BULLET_INCLUDE_DIR "${LIBS_DIR}/bullet/src")
set(BULLET_SOFTBODY_LIBRARY "${LIBS_DIR}/bullet/lib/${CMAKE_BUILD_TYPE}/BulletSoftBody${BULLET_LIB_POSTFIX}.lib")
set(BULLET_DYNAMICS_LIBRARY "${LIBS_DIR}/bullet/lib/${CMAKE_BUILD_TYPE}/BulletDynamics${BULLET_LIB_POSTFIX}.lib")
set(BULLET_COLLISION_LIBRARY "${LIBS_DIR}/bullet/lib/${CMAKE_BUILD_TYPE}/BulletCollision${BULLET_LIB_POSTFIX}.lib")
set(BULLET_MATH_LIBRARY "${LIBS_DIR}/bullet/lib/${CMAKE_BUILD_TYPE}/LinearMath${BULLET_LIB_POSTFIX}.lib")

set(GLEW_INCLUDE_DIR "${LIBS_DIR}/glew/include")
set(GLEW_LIBRARY "${LIBS_DIR}/glew/lib/Win32/glew32.lib")

set(GLM_INCLUDE_DIR "C:/Programming/libs/glm")

set(SOIL_INCLUDE_DIR "${LIBS_DIR}/soil/src")
set(SOIL_LIBRARY "${LIBS_DIR}/soil/lib/SOIL.lib")

set(ASSIMP_INCLUDE_DIR "${LIBS_DIR}/assimp/include")
set(ASSIMP_LIBRARY "${LIBS_DIR}/assimp/lib/assimp.lib")

set(XML_INCLUDE_DIR "${LIBS_DIR}/rapidxml")

find_package(OpenGL REQUIRED)
find_package(Bullet)
find_package(GLEW REQUIRED)

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
		${OPENGL_INCLUDE_DIR} ${BOOST_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR} ${BULLET_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${GLM_INCLUDE_DIR} ${SOIL_INCLUDE_DIR} ${ASSIMP_INCLUDE_DIR} ${XML_INCLUDE_DIR}
		${LIBS_DIR})

file(GLOB XML_INCLUDES ${XML_INCLUDE_DIR}/*.hpp)
file(GLOB BOOST_INCLUDES ${BOOST_INCLUDE_DIR}/*.hpp)

#set(NMAKE_COMPILE_FLAGS "-build=DEBUG")

set(CMAKE_CXX_STANDARD 11)
list(APPEND CMAKE_CXX_FLAGS "-fpermissive -w")

SET(GCC_COVERAGE_LINK_FLAGS    "--verbose")

include_directories(${ALL_INCLUDE_DIRS})

add_executable(3D ${MY_SOURCES} ${MY_INCLUDES} ${XML_INCLUDES} ${BOOST_INCLUDES})

set_target_properties(3D PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin/${CMAKE_BUILD_TYPE}")

#message("OpenGL Library ${OPENGL_LIBRARIES}")
#message("FreeType Library ${FREETYPE_LIBRARY}")
#message("Bullet Library ${BULLET_LIBRARIES}")
#message("Assimp Library ${ASSIMP_LIBRARY}")
#message("GLEW Library ${GLEW_LIBRARY}")
#message("SOIL Library ${SOIL_LIBRARY}")

target_link_libraries(3D ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARY} ${BULLET_LIBRARIES} ${ASSIMP_LIBRARY} ${GLEW_LIBRARY} ${SOIL_LIBRARY})

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

source_group("Library Files" FILES ${XML_INCLUDES})

set(CMAKE_SUPPRESS_REGENERATION true)
set(VS_STARTUP_PROJECT "3D")