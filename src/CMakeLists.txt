find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)

include(FindDirectX.cmake)

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
target_include_directories(hook PRIVATE ${DirectX_INCLUDE_DIRS})
target_link_libraries(hook PRIVATE ${DETOURS_LIBRARY})
target_link_libraries(hook PRIVATE ${DirectX_LIBRARIES})
target_link_options(hook PRIVATE "/export:DetourFinishHelperProcess,@1,NONAME")

set_target_properties(hook
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# "Game"

add_executable(triangle WIN32 triangle.cpp)
target_include_directories(triangle PRIVATE ${DirectX_INCLUDE_DIRS})
target_link_libraries(triangle PRIVATE ${DirectX_LIBRARIES})

set_target_properties(triangle
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)