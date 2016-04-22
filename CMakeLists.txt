cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)

find_package(Boost COMPONENTS system thread date_time regex chrono REQUIRED)

include_directories(
	${Boost_INCLUDE_DIRS}
	${PROJECT_SOURCE_DIR}
)

add_executable(master master.cpp)
add_executable(slave slave.cpp)
target_link_libraries(master ${Boost_LIBRARIES})
target_link_libraries(slave ${Boost_LIBRARIES})
