project(ctsp_reader)

add_executable(${PROJECT_NAME}
src/main.cpp 
)

target_link_libraries(
    ${PROJECT_NAME}
    sub::ctsp_io
)  
