cmake_minimum_required (VERSION 2.7)
project (lodepng)

add_library (${PROJECT_NAME} lodepng.cpp)

install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib)
install(FILES lodepng.h DESTINATION include)

