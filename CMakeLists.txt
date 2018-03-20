cmake_minimum_required(VERSION 3.9)
project(ceph_s3_tests)

set(CMAKE_CXX_STANDARD 17)

#######
## GOOGLE TEST SETUP
#######
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )
execute_process(COMMAND "${CMAKE_COMMAND}" --build .
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src"
        "${CMAKE_BINARY_DIR}/googletest-build")

# The gtest/gmock targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if(CMAKE_VERSION VERSION_LESS 2.8.11)
    include_directories("${gtest_SOURCE_DIR}/include"
            "${gmock_SOURCE_DIR}/include")
endif()

# Now simply link your own targets against gtest, gmock,
# etc. as appropriate
include_directories(cmake-build-debug/googletest-src/googlemock/include)
include_directories(cmake-build-debug/googletest-src/googletest/include)

########
## AWS SDK SETUP
########
# Locate the AWS SDK for C++ package.
# Requires that you build with:
#   -Daws-sdk-cpp_DIR=/path/to/sdk_build
# or export/set:
#   CMAKE_PREFIX_PATH=/path/to/sdk_build
set(AWS_MODULES core s3)
set(AWS_TARGETS)
foreach(MODULE ${AWS_MODULES})
    find_package(aws-cpp-sdk-${MODULE} REQUIRED CONFIG
            PATHS "${CMAKE_BINARY_DIR}/aws-sdk-cpp/lib/cmake/aws-cpp-sdk-${MODULE}")
    list(APPEND AWS_TARGETS aws-cpp-sdk-${MODULE})
endforeach()

# Link to the SDK shared libraries.
add_definitions(-DUSE_IMPORT_EXPORT)

add_executable(ceph_s3_tests main.cpp)

target_link_libraries(ceph_s3_tests gtest gtest_main aws-cpp-sdk-s3)