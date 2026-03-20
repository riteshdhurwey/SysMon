cmake_minimum_required(VERSION 3.16)
project(SysMon)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# ── Main binary ───────────────────────────────────────────────────────
file(GLOB SRC_FILES src/*.cpp)
add_executable(sysmon ${SRC_FILES})
target_include_directories(sysmon PRIVATE include)
target_link_libraries(sysmon ncursesw tinfo pthread)

# ── Tests ─────────────────────────────────────────────────────────────
set(GTEST_INCLUDE_DIR /usr/include)
set(GTEST_LIBRARY /usr/lib/libgtest.so)
set(GTEST_MAIN_LIBRARY /usr/lib/libgtest_main.so)
find_package(GTest REQUIRED)

# All src files except main.cpp
file(GLOB TEST_SRC src/*.cpp)
list(FILTER TEST_SRC EXCLUDE REGEX ".*main\\.cpp$")

add_executable(test_all tests/test_all.cpp ${TEST_SRC})
target_include_directories(test_all PRIVATE include)
target_link_libraries(test_all GTest::GTest GTest::Main ncursesw tinfo pthread)

# ── Install ───────────────────────────────────────────────────────────
install(TARGETS sysmon DESTINATION /usr/local/bin)