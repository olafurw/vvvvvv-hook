find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)

find_package(imgui CONFIG REQUIRED)

include(FindDirectX.cmake)

# Loader
add_executable(loader 
    loader/loader.cpp
    loader/validate_dll.h
    loader/validate_dll.cpp
    loader/get_library.h
    loader/get_library.cpp
)
target_include_directories(loader PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(loader PRIVATE ${DETOURS_LIBRARY})

set_target_properties(loader
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# Hook
add_library(hook SHARED 
    hook/hook.cpp
    hook/hook_create_device.h
    hook/hook_create_device.cpp
    hook/create_device.h
    hook/create_device.cpp
    hook/end_scene.h
    hook/end_scene.cpp
    hook/virtual_table.h
    hook/virtual_table.cpp
    hook/virtual_protect.h
    hook/virtual_protect.cpp

    hook/imgui_impl_dx9.h
    hook/imgui_impl_dx9.cpp
    hook/imgui_impl_win32.h
    hook/imgui_impl_win32.cpp
)
target_include_directories(hook PRIVATE ${DETOURS_INCLUDE_DIRS})
target_include_directories(hook PRIVATE ${DirectX_INCLUDE_DIRS})
target_link_libraries(hook PRIVATE ${DETOURS_LIBRARY})
target_link_libraries(hook PRIVATE ${DirectX_LIBRARIES})
target_link_libraries(hook PRIVATE imgui::imgui)
target_link_options(hook PRIVATE "/export:DetourFinishHelperProcess,@1,NONAME")

set_target_properties(hook
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
