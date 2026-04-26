cmake_minimum_required(VERSION 3.20)
project(Donogan C)

set(CMAKE_C_STANDARD 99)

# Use static MSVC runtime: /MT for Release, /MTd for Debug.
# Must be before add_executable().
if (MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

list(APPEND CMAKE_PREFIX_PATH "C:/raylib")
find_package(raylib CONFIG REQUIRED)

set(SRC preview.c)

if (WIN32)
  set(APP_RC "${CMAKE_CURRENT_SOURCE_DIR}/res/app.rc")
  list(APPEND SRC "${APP_RC}")
endif()

add_executable(play ${SRC})

if (MSVC)
  set_property(TARGET play PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set_target_properties(play PROPERTIES
  LINK_FLAGS "/STACK:16777216"
)

target_link_libraries(play PRIVATE
  raylib
  opengl32
  gdi32
  winmm
  user32
  shell32
)

target_compile_definitions(play PRIVATE
  _CRT_SECURE_NO_WARNINGS
  NOMINMAX
  WIN32_LEAN_AND_MEAN
)

target_include_directories(play PRIVATE
  "C:/raylib/include"
)

add_custom_command(TARGET play POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
          "${CMAKE_CURRENT_SOURCE_DIR}/res"
          "$<TARGET_FILE_DIR:play>/res"
)