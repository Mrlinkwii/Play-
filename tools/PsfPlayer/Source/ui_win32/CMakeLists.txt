cmake_minimum_required(VERSION 3.18)

set(CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/../../../../deps/Dependencies/cmake-modules
	${CMAKE_MODULE_PATH}
)
include(Header)

project(PsfPlayer)

if(NOT TARGET PsfCore)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../
		${CMAKE_CURRENT_BINARY_DIR}/PsfCore
	)
endif()
list(APPEND PSF_UI_WIN32_PROJECT_LIBS PsfCore)

if(NOT TARGET Framework_Win32)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../../../../deps/Framework/build_cmake/FrameworkWin32
		${CMAKE_CURRENT_BINARY_DIR}/Framework_Win32
	)
endif()
list(APPEND PSF_UI_WIN32_PROJECT_LIBS Framework_Win32)

if(NOT TARGET tdemu)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../../../../deps/Dependencies/tdemu-1.10
		${CMAKE_CURRENT_BINARY_DIR}/tdemu-1.10
	)
endif()
list(APPEND PSF_UI_WIN32_PROJECT_LIBS tdemu)

find_package(DirectX9 REQUIRED)
list(APPEND PSF_UI_WIN32_PROJECT_LIBS ${DirectX_D3D9_LIBRARY})
list(APPEND PSF_UI_WIN32_PROJECT_LIBS ${DirectX_D3DX9_LIBRARY})
include_directories(${DirectX_D3D9_INCLUDE_DIR} ${DirectX_D3DX9_INCLUDE_DIR})

set(AUDIO_LIBS)
find_package(OpenAL)
if(OPENAL_FOUND)
	if(NOT TARGET Framework_OpenAl)
		add_subdirectory(
			${CMAKE_CURRENT_SOURCE_DIR}/../../../../deps/Framework/build_cmake/FrameworkOpenAl
			${CMAKE_CURRENT_BINARY_DIR}/FrameworkOpenAl
		)
	endif()
	list(APPEND PSF_UI_WIN32_PROJECT_LIBS Framework_OpenAl)
	add_library(SH_OpenAL SHARED ../SH_OpenAL.cpp SH_OpenAL/DllMain.cpp)
	target_link_libraries(SH_OpenAL PRIVATE Framework_OpenAl Framework)
	list(APPEND AUDIO_LIBS SH_OpenAL)
endif()

find_package(XAudio)
if(XAudio_FOUND)
	add_library(SH_XAudio2 SHARED SH_XAudio2.cpp SH_XAudio2/DllMain.cpp)
	target_link_libraries(SH_XAudio2 PRIVATE ${XAudio_LIBRARY} Framework)
	target_include_directories(SH_XAudio2 PRIVATE ${XAudio_INCLUDE_DIR})
	list(APPEND AUDIO_LIBS SH_XAudio2)
endif()

add_library(SH_WaveOut SHARED SH_WaveOut.cpp SH_WaveOut/DllMain.cpp)
target_link_libraries(SH_WaveOut PRIVATE "winmm.lib" Framework)
list(APPEND AUDIO_LIBS SH_WaveOut)

if(TARGET_PLATFORM_WIN32_X64)
	set(MANIFEST PsfPlayer-x64.manifest)
else()
	set(MANIFEST PsfPlayer-x86.manifest)
endif()

if(DEBUGGER_INCLUDED)
	add_definitions("-DFUNCTIONSVIEW_STANDALONE")
	set(DEBUG_SRC
		DebugSupport/DebugExpressionEvaluator.cpp
		DebugSupport/DisAsm.cpp
		DebugSupport/FunctionsView.cpp
		DebugSupport/MemoryView.cpp
		DebugSupport/MemoryViewMIPS.cpp
		DebugSupport/RegViewGeneral.cpp
		DebugSupport/RegViewPage.cpp
		DebugSupport/WinUtils.cpp
		MiniDebugger.cpp
	)
endif()

set(WIN32_SRC

	DebugSupport/DirectXControl.cpp
	AboutWindow.cpp
	AcceleratorTable.cpp
	FileInformationPanel.cpp
	Main.cpp
	MainWindow.cpp
	PlaylistPanel.cpp
	SpuRegView.cpp
	SpuRegViewPanel.cpp
	StdAfx.cpp

	${MANIFEST}
)

add_executable(PsfPlayer WIN32 ${WIN32_SRC} ${DEBUG_SRC} Res.rc)
target_link_libraries(PsfPlayer PUBLIC d3d9 uxtheme ${PSF_UI_WIN32_PROJECT_LIBS} ${AUDIO_LIBS})
target_include_directories(PsfPlayer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/DebugSupport)

include(PrecompiledHeader)
add_precompiled_header(PsfPlayer StdAfx.h FORCEINCLUDE SOURCE_CXX StdAfx.cpp)

add_custom_command(TARGET PsfPlayer POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PROPERTY:tdemu,IMPORTED_LOCATION> "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
)
