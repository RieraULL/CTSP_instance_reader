# Set the project name
project (ctsp_io)


file(GLOB SOURCES 
    "src/TSPLIB_instance.cpp"
    "src/PTSP_instance.cpp"
    "src/CTSP_instance.cpp"
)


# Add a library with the above sources
add_library(${PROJECT_NAME} 
    ${SOURCES})


    
add_library(sub::ctsp_io ALIAS ${PROJECT_NAME})

target_include_directories( ${PROJECT_NAME}
    PUBLIC ${PROJECT_SOURCE_DIR}/include
)


