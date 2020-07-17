if (DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message("VCPKG ROOT IS: $ENV{VCPKG_ROOT}")
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
            CACHE STRING "")
endif ()

message(STATUS "CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")

if (DEFINED VCPKG_TARGET_TRIPLET)
    message("VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
else ()
    message("VCPKG_TARGET_TRIPLET NOT FOUND.")
endif ()