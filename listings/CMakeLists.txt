file(GLOB SOURCES "*.cpp")

find_package(TBB REQUIRED)

# Remove listing_10_3.cpp from the SOURCES list
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/listing_4_15.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/listing_4_16.cpp")
list(REMOVE_ITEM SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/listing_10_3.cpp")

foreach(SOURCE ${SOURCES})
    get_filename_component(EXECUTABLE_NAME ${SOURCE} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${SOURCE})
    target_link_libraries(${EXECUTABLE_NAME} TBB::tbb)
endforeach()
