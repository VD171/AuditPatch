cmake_minimum_required(VERSION 3.22.1)
project(auditpatch)

set(CXX_FLAGS "${CXX_FLAGS} -fno-exceptions -fno-rtti -fvisibility=hidden -fvisibility-inlines-hidden")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_FLAGS}")

find_package(cxx REQUIRED CONFIG)
link_libraries(cxx::cxx)

set(Plugin.Android.BionicLinkerUtil "Enable android bionic linker util" ON)
add_subdirectory(third_party/Dobby)

add_library(${MODULE_NAME} SHARED hook.cpp)
target_link_libraries(${MODULE_NAME} PRIVATE log dobby_static)

add_custom_target(
    ptrace_injector ALL
    COMMAND ${ANDROID_NDK}/ndk-build -C ${CMAKE_CURRENT_SOURCE_DIR}/third_party/Android-Ptrace-Injector
)

add_dependencies(${MODULE_NAME} ptrace_injector)
