cmake_minimum_required(VERSION 3.18)

set(CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/../../deps/Dependencies/cmake-modules
	${CMAKE_MODULE_PATH}
)
include(Header)

project(AutoTest)
if (NOT TARGET PlayCore)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../../Source/
		${CMAKE_CURRENT_BINARY_DIR}/Source
	)
endif()

if(TARGET_PLATFORM_WIN32)
	if(NOT TARGET gsh_opengl_win32)
		add_subdirectory(
			${CMAKE_CURRENT_SOURCE_DIR}/../../Source/gs/GSH_OpenGLWin32
			${CMAKE_CURRENT_BINARY_DIR}/gs/GSH_OpenGLWin32
		)
	endif()
	list(APPEND AUTOTEST_PROJECT_LIBS gsh_opengl_win32)

	if(NOT TARGET gsh_d3d9)
		add_subdirectory(
			${CMAKE_CURRENT_SOURCE_DIR}/../../Source/gs/GSH_Direct3D9
			${CMAKE_CURRENT_BINARY_DIR}/gs/GSH_Direct3D9
		)
	endif()
	list(APPEND AUTOTEST_PROJECT_LIBS gsh_d3d9)
endif()

add_executable(autotest
	JUnitTestReportWriter.cpp
	Main.cpp
)
target_link_libraries(autotest PlayCore ${AUTOTEST_PROJECT_LIBS})
