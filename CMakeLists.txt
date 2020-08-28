cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(stasys-cli VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(OpenCV REQUIRED)
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

add_executable(stasys-cli stasys-cli.cpp CalibrationThread.h RecordThread.cpp
	RecordThread.h ShootThread.cpp ShootThread.h Shot.cpp
	Shot.h ShotTrace.cpp ShotTrace.h SoundPressureSensor.cpp SystemClock.cpp
	SystemClock.h Utils.h)

target_link_libraries(stasys-cli ${OpenCV_LIBS} sfml-graphics sfml-audio)
