cmake_minimum_required (VERSION 3.8)

# A interface to reuse common properties.
# You can add more external include paths of other projects that are needed for your project.
add_library ("${PROJECT_NAME}_interface" INTERFACE)
target_link_libraries ("${PROJECT_NAME}_interface" INTERFACE seqan3::seqan3)
target_include_directories ("${PROJECT_NAME}_interface" INTERFACE ../include)
target_compile_options ("${PROJECT_NAME}_interface" INTERFACE "-pedantic" "-Wall" "-Wextra")

# An object library (without main) to be used in multiple targets.
add_library ("${PROJECT_NAME}_lib" STATIC nfa_pointer.cpp graphMaker.cpp korotkov_nfa.cpp utils.cpp index.cpp query.cpp benchmark_main.cpp)
target_link_libraries ("${PROJECT_NAME}_lib" PUBLIC "${PROJECT_NAME}_interface")

add_executable ("${PROJECT_NAME}" main.cpp)
target_link_libraries ("${PROJECT_NAME}" PRIVATE "${PROJECT_NAME}_lib")

#set(BENCH benchmark_main.cpp nfa_pointer.cpp)
#add_executable(benchmark ${PROJECT_SOURCE_DIR}/src/${BENCH})