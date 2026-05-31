cmake_minimum_required(VERSION 3.28)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
	cmake_policy(SET CMP0141 NEW)
	set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(modeye LANGUAGES C CXX)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v2.0.8
    SOURCE_SUBDIR "cmake"
)
FetchContent_MakeAvailable(glad)
glad_add_library(glad_gl_core_46 REPRODUCIBLE LOADER API "gl:core=4.6")

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
)
set(GLM_BUILD_TESTS OFF)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(spdlog)

add_executable(modeye "src/Application.cpp" "src/Camera.cpp" "src/Buffer.cpp" "src/Mesh.cpp")

set_property(TARGET modeye PROPERTY CXX_STANDARD 17)
set_property(TARGET modeye PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET modeye PROPERTY CXX_EXTENSIONS OFF)

target_link_libraries(modeye PRIVATE glfw glad_gl_core_46 glm spdlog::spdlog)

set(MODEYE_UNIX_DEBUG_COMPILE_OPTIONS "-Wall" "-Wextra" "-Wpendantic" "-g")
set(MODEYE_UNIX_RELEASE_COMPILE_OPTIONS "-Wall" "-Wextra" "-Wpendantic" "-Werror" "-O3")
set(MODEYE_UNIX_COMPILE_OPTIONS 
    "$<$<CONFIG:DEBUG>:$<JOIN:${MODEYE_UNIX_DEBUG_COMPILE_OPTIONS},;>>"
    "$<$<CONFIG:RELEASE>:$<JOIN:${MODEYE_UNIX_RELEASE_COMPILE_OPTIONS},;>>"
)

set(MODEYE_MSVC_DEBUG_COMPILE_OPTIONS "/W4" "/Od")
set(MODEYE_MSVC_RELEASE_COMPILE_OPTIONS "/W4" "/WX" "/O2")
set(MODEYE_MSVC_COMPILE_OPTIONS 
    "$<$<CONFIG:DEBUG>:$<JOIN:${MODEYE_MSVC_DEBUG_COMPILE_OPTIONS},;>>"
    "$<$<CONFIG:RELEASE>:$<JOIN:${MODEYE_MSVC_RELEASE_COMPILE_OPTIONS},;>>"
)

set(MODEYE_COMPILE_OPTIONS 
    "$<$<CXX_COMPILER_ID:MSVC>:${MODEYE_MSVC_COMPILE_OPTIONS}>"
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:CLANG>>:${MODEYE_UNIX_COMPILE_OPTIONS}>"
)

target_compile_options(modeye PRIVATE "${MODEYE_COMPILE_OPTIONS}")

add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/shaders" "$<TARGET_FILE_DIR:modeye>/shaders"
    COMMENT "Copy shaders to the target directory"
)

add_dependencies(modeye copy_shaders)
