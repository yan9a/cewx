cmake_minimum_required(VERSION 2.8)
project (th-gui)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include("${wxWidgets_USE_FILE}")
add_executable(${PROJECT_NAME} th-gui.cpp)
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
# Set the location for linstallation
# Use "sudo make install" to apply
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_SOURCE_DIR})
