cmake_minimum_required(VERSION 3.3)
project(Ciri)

# OpenGL
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
include_directories(src ${OPENGL_INCLUDE_DIRS})

# Address Sanitizer
if (UNIX)
	set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
	set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

# Define DEBUG and RELEASE flags
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCIRI_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DCIRI_RELEASE")

# Compile Commands
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)

# C++ Standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)

# ImGui
set(imgui-directory dependencies/imgui)
set(imgui-source ${imgui-directory}/imconfig.h
	${imgui-directory}/imgui.h
	${imgui-directory}/imgui.cpp
	${imgui-directory}/imgui_draw.cpp
	${imgui-directory}/imgui_internal.h
	${imgui-directory}/imgui_widgets.cpp
	${imgui-directory}/imstb_rectpack.h
	${imgui-directory}/imstb_textedit.h
	${imgui-directory}/imstb_truetype.h
    ${imgui-directory}/imgui_tables.cpp
	${imgui-directory}/imgui_demo.cpp
	${imgui-directory}/backends/imgui_impl_glfw.cpp
	${imgui-directory}/backends/imgui_impl_opengl3.cpp
)

# fork_awesome icons
set(fork_awesome-include dependencies/fork_awesome/include)

# pch
set(ciri-pch src/PCH.h)

add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glad)
add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/spdlog)
add_subdirectory(dependencies/stb_image)
add_subdirectory(dependencies/tinyobjloader)

file(GLOB_RECURSE source CONFIGURE_DEPENDS "src/*.h" "src/*.hpp" "src/*.cpp")

add_executable(Ciri ${source} ${imgui-source})
target_link_libraries(Ciri ${OPENGL_LIBRARIES} glfw glad glm spdlog stb_image tinyobjloader)
target_include_directories(Ciri PRIVATE ${imgui-directory} ${stb_image-include} ${fork_awesome-include})
target_precompile_headers(Ciri PRIVATE ${ciri-pch})

# Visual Studio Startup Project
if(WIN32)
	set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Ciri)
endif()

# Symlink Resources
add_custom_command(TARGET Ciri PRE_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:Ciri>/resources)