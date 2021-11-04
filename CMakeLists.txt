cmake_minimum_required( VERSION 3.3 )

project( Ciri )

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package( OpenGL REQUIRED )
include_directories( ${OPENGL_INCLUDE_DIRS} )

# Address Sanitizer
string (APPEND CMAKE_CXX_FLAGS_DEBUG "-fno-omit-frame-pointer -fsanitize=address")
string (APPEND CMAKE_LINKER_FLAGS_DEBUG "-fno-omit-frame-pointer -fsanitize=address")

# C++ Standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

file(GLOB_RECURSE source CONFIGURE_DEPENDS "src/*.hpp" "src/*.cpp")

add_executable( Ciri ${source})
target_link_libraries( Ciri ${OPENGL_LIBRARIES})