cmake_minimum_required(VERSION 2.8)
project (thread1)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include("${wxWidgets_USE_FILE}")
add_executable(${PROJECT_NAME} thread1.cpp)
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
# Set the location for linstallation
# Use "sudo make install" to apply
install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_SOURCE_DIR})
