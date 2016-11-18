project(myAwesomeSolution)
cmake_minimum_required(VERSION 2.8.12)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
file(GLOB myAwesomeProgram_src
	"src/*.h"
	"src/*.cpp"
)
add_executable(myAwesomeProgram ${myAwesomeProgram_src})
target_link_libraries(myAwesomeProgram ${CONAN_LIBS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT "myAwesomeProgram")