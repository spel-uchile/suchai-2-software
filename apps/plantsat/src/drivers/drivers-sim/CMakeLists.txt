set(SOURCE_FILES
        interface.c
        cdh.c
)

add_library(drivers-sim ${SOURCE_FILES})
target_include_directories(drivers-sim PUBLIC include)
target_link_libraries(drivers-sim PRIVATE -lzmq)