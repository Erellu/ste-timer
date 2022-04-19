project(ste-timer-example-1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ste-timer-example-1 main.cpp)

# Don't do that, that's for the sake of example.
target_link_directories(ste-timer-example-1 PRIVATE "../../include/")
