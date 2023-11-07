set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_VERBOSE_MAKEFILE 1)
set(EXEC_NAME debug_view)

# In this CMakeLists.txt file, components are all built separately and linked together
# If you prefer to compile source files + dependencies (OpenGL loaders, imgui, VtkViewer, etc.)
# together, please see CMakeLists-alt.txt
file(GLOB_RECURSE header_file "*.h")
file(GLOB source_file "main.cpp" "*.cpp")
add_executable(${EXEC_NAME} ${header_file} ${source_file})

target_link_libraries(${EXEC_NAME} OpenGL::GL imgui_vtk_viewer)

if (VTK_VERSION VERSION_LESS "9.0.0")
include(${VTK_USE_FILE})
else()
# vtk_module_autoinit is needed
vtk_module_autoinit(
TARGETS ${EXEC_NAME}
MODULES ${VTK_LIBRARIES}
)
endif()

set(Bin_Root "${CMAKE_SOURCE_DIR}/Bin")
set(Lib_Root "${CMAKE_SOURCE_DIR}/Lib")
set_target_properties(${EXEC_NAME}
PROPERTIES
RUNTIME_OUTPUT_DIRECTORY ${Bin_Root}
ARCHIVE_OUTPUT_DIRECTORY ${Lib_Root}
VS_DEBUGGER_WORKING_DIRECTORY ${Bin_Root}/$<CONFIG>
)