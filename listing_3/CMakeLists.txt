file(GLOB SOURCES "*.cpp")

find_package(TBB REQUIRED)

foreach(SOURCE ${SOURCES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE})
    target_link_libraries(${EXECUTABLE_NAME} TBB::tbb)
endforeach()
