cmake_minimum_required(VERSION 3.20)
project(vinjector 
    VERSION 1.0.0
    DESCRIPTION "A powerful process injector with ImGui-based user interface"
    LANGUAGES CXX RC
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT WIN32)
    message(FATAL_ERROR "vinjector can only be built on Windows")
endif()

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configuration types")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

option(USE_STATIC_RUNTIME "Link runtime statically" OFF)
option(ENABLE_DX9 "Build with DirectX 9 support" ON)
option(STRIP_BINARY "Strip symbols from binary" OFF)

if(MSVC)
    add_compile_options(
        /W4                  
        /WX-                 
        /MP                 
        /fp:precise            
    )
    
    # Runtime library
    if(USE_STATIC_RUNTIME)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    endif()
    
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    # GCC / Clang
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        -m64                  
        -mwindows             
    )
    
    # Strip symbols if requested
    if(STRIP_BINARY)
        add_compile_options(-s)
    endif()
endif()


set(SOURCES
    Main.cpp
    Inject/Inject.cpp
    ProcessCheck/ProcessCheck.cpp
)

# ImGui sources
set(IMGUI_SOURCES
    ImGui/imgui.cpp
    ImGui/imgui_draw.cpp
    ImGui/imgui_widgets.cpp
    ImGui/imgui_tables.cpp
    ImGui/imgui_demo.cpp
    ImGui/backend/imgui_impl_dx9.cpp
    ImGui/backend/imgui_impl_win32.cpp
)


set(RESOURCE_FILES
    resources.rc
)


set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/ImGui
    ${CMAKE_SOURCE_DIR}/ImGui/backend
)


find_package(DirectX REQUIRED)


add_executable(vinjector WIN32
    ${SOURCES}
    ${IMGUI_SOURCES}
    ${RESOURCE_FILES}
)


target_include_directories(vinjector PRIVATE ${INCLUDE_DIRS})


set_target_properties(vinjector PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    OUTPUT_NAME "vinjector"
    WIN32_EXECUTABLE ON
)


if(MSVC)
    set_target_properties(vinjector PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS"
    )
endif()

target_link_libraries(vinjector PRIVATE
    user32.lib
    gdi32.lib
    comdlg32.lib
    advapi32.lib
    ole32.lib
    comctl32.lib
    dwmapi.lib
    shlwapi.lib
    
    $<$<BOOL:${ENABLE_DX9}>:d3d9.lib>
)

if(ENABLE_DX9)
    target_compile_definitions(vinjector PRIVATE
        IMGUI_IMPL_DX9_ENABLE=1
    )
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_link_options(vinjector PRIVATE
        -static-libgcc
        -static-libstdc++
        -Wl,--subsystem,windows
    )
endif()

target_compile_definitions(vinjector PRIVATE
    WIN32
    _WINDOWS
    _CRT_SECURE_NO_WARNINGS
    NOMINMAX
)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(vinjector PRIVATE
        _DEBUG
    )
else()
    target_compile_definitions(vinjector PRIVATE
        NDEBUG
    )
    
    if(MSVC)
        target_compile_options(vinjector PRIVATE /O2)
    endif()
endif()

install(TARGETS vinjector
    RUNTIME DESTINATION bin
    COMPONENT executables
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-win64")
include(CPack)

message(STATUS "============================================")
message(STATUS "vinjector Build Configuration")
message(STATUS "============================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "DirectX 9 Support: ${ENABLE_DX9}")
message(STATUS "Static Runtime: ${USE_STATIC_RUNTIME}")
message(STATUS "Strip Symbols: ${STRIP_BINARY}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Architecture: x64")
message(STATUS "Output: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "============================================")
