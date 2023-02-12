cmake_minimum_required(VERSION 3.2)
project(eigen)

option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)

#set(CMAKE_C_COMPILER /usr/bin/cc)
#set(CMAKE_CXX_COMPILER /usr/bin/g++)

set(Geant4_DIR /Users/kolodinevgenij/Geant4/geant4.11-install/lib/Geant4-11.0.3)
set(Qt5_DIR /Users/kolodinevgenij/qt-lib/qt-everywhere-src-5.15.8/qtbase/lib/cmake/Qt5)

if(WITH_GEANT4_UIVIS)
    find_package(Geant4 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 REQUIRED)
endif()

find_package(Qt5 COMPONENTS Widgets Core Gui REQUIRED)

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.h)


add_executable(eigen eigen.cpp ${sources} ${headers})
set_target_properties(eigen PROPERTIES COMPILE_DEFINITIONS BUILDER_STATIC_DEFINE)
target_link_libraries(eigen ${Geant4_LIBRARIES} Qt5::Widgets Qt5::Core Qt5::Gui) # not profiler version
#target_link_libraries(eigen ${Geant4_LIBRARIES} Qt5::Widgets Qt5::Core Qt5::Gui easy_profiler) # profiler version

set(eigen_FILES model.gdml run.mac vis.mac)

foreach(_file ${eigen_FILES})
    configure_file(
        ${PROJECT_SOURCE_DIR}/${_file}
        ${PROJECT_BINARY_DIR}/${_file}
        COPYONLY
    )
endforeach()

install(TARGETS eigen DESTINATION bin)

