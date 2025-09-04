cmake_minimum_required(VERSION 3.20)
project(Map C)
set(CMAKE_C_STANDARD 99)

# Where you installed raylib with "cmake --install"
list(APPEND CMAKE_PREFIX_PATH "C:/raylib")
find_package(raylib CONFIG REQUIRED)

add_executable(play preview.c)

# ---- Visual Leak Detector (VLD) wiring (MSVC only) ----
#if (MSVC)
#  set(VLD_ROOT "C:/Program Files (x86)/Visual Leak Detector")

  #if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    #set(_VLD_LIB "${VLD_ROOT}/lib/Win64/vld.lib")
  #else()
    #set(_VLD_LIB "${VLD_ROOT}/lib/Win32/vld.lib")
#  endif()

  #add_library(VLD::VLD UNKNOWN IMPORTED)
  #set_target_properties(VLD::VLD PROPERTIES
    #IMPORTED_LOCATION             "${_VLD_LIB}"
    #INTERFACE_INCLUDE_DIRECTORIES "${VLD_ROOT}/include"
#  )

  # Link VLD for Debug (and RelWithDebInfo, optional)
  #target_link_libraries(play PRIVATE $<$<CONFIG:Debug>:VLD::VLD>)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

foreach(tgt IN ITEMS play)
  set_target_properties(${tgt} PROPERTIES LINK_FLAGS "/STACK:16777216")
  target_link_libraries(${tgt} PRIVATE raylib opengl32 gdi32 winmm user32 shell32)
  target_compile_definitions(${tgt} PRIVATE _CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)
  target_include_directories(${tgt} PRIVATE "C:/raylib/include")
  # Set stack size to 16 MB (default is ~1 MB)
endforeach()
