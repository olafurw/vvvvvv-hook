find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)

# Loader

add_executable(loader loader.cpp validate_dll.h validate_dll.cpp)
target_include_directories(loader PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(loader PRIVATE ${DETOURS_LIBRARY})

set_target_properties(loader
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Hook

add_library(hook SHARED main.cpp)
target_include_directories(hook PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(hook PRIVATE ${DETOURS_LIBRARY})
target_link_libraries(hook PRIVATE d3d9)
target_link_options(hook PRIVATE "/export:DetourFinishHelperProcess,@1,NONAME")

set_target_properties(hook
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
