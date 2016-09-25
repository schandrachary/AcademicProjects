include_directories(${HandGesture_SOURCE_DIR}/include ${PSYNC_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} main.cpp HandGesture.cpp)
target_link_libraries(${PROJECT_NAME} ${PSYNC_LIBS})
