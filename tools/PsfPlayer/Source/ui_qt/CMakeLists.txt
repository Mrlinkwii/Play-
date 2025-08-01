cmake_minimum_required(VERSION 3.18)

set(CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/../../../../deps/Dependencies/cmake-modules
	${CMAKE_MODULE_PATH}
)
include(Header)

project(PsfPlayer)
set(PSFPLAYER_PROJECT_LIBS)

if(NOT TARGET PsfCore)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../
		${CMAKE_CURRENT_BINARY_DIR}/PsfCore
	)
endif()
list(APPEND PSFPLAYER_PROJECT_LIBS PsfCore)

set(QT_SOURCES
	main.cpp
	mainwindow.cpp
	mainwindow.h
	playlistmodel.cpp
	playlistmodel.h
	../../../../Source/ui_qt/QStringUtils.cpp
	../../../../Source/ui_qt/QStringUtils.h
)

set(QT_MOC_HEADERS
	mainwindow.h
	playlistmodel.h
)

set(QT_UIS
	Qt_ui/mainwindow.ui
)

set(QT_RESOURCES
)

if(DEBUGGER_INCLUDED)
	if(NOT TARGET PlayDebugSupport)
		add_subdirectory(
			${CMAKE_CURRENT_SOURCE_DIR}/../../../../Source/ui_qt/DebugSupport
			${CMAKE_CURRENT_BINARY_DIR}/PlayDebugSupport
		)
	endif()
	list(APPEND QT_SOURCES
		debuggerwindow.cpp
		debuggerwindow.h
	)
	list(APPEND QT_MOC_HEADERS
		debuggerwindow.h
	)
	list(APPEND QT_UIS
		Qt_ui/debuggerwindow.ui
		Qt_ui/debugmenu.ui
	)
	list(APPEND PSFPLAYER_PROJECT_LIBS PlayDebugSupport)
endif()

if(TARGET_PLATFORM_MACOS OR TARGET_PLATFORM_UNIX)
	if(NOT TARGET sh_openal)
		add_subdirectory(
			${CMAKE_CURRENT_SOURCE_DIR}/SH_OpenAL
			${CMAKE_CURRENT_BINARY_DIR}/SH_OpenAL
		)
	endif()
	list(APPEND PSFPLAYER_PROJECT_LIBS sh_openal)
endif()

set(QT_COMPONENTS Widgets Core)
find_package(Qt6 COMPONENTS ${QT_COMPONENTS})
if(NOT Qt6_FOUND)
	find_package(Qt5 REQUIRED COMPONENTS ${QT_COMPONENTS})
endif()
list(APPEND PSFPLAYER_PROJECT_LIBS Qt::Widgets Qt::Core)

QT_ADD_RESOURCES(QT_RES_SOURCES ${QT_RESOURCES})
QT_WRAP_UI(QT_UI_HEADERS ${QT_UIS})
QT_WRAP_CPP(QT_MOC_SRCS ${QT_MOC_HEADERS})

if(TARGET_PLATFORM_WIN32)
	set(QT_SOURCES
		${QT_SOURCES}
		../ui_win32/SH_WaveOut.cpp
		../ui_win32/SH_WaveOut.h
	)
	list(APPEND PSFPLAYER_PROJECT_LIBS winmm)
	add_executable(PsfPlayer WIN32 ${QT_SOURCES} ${QT_MOC_SRCS} ${QT_RES_SOURCES} ${QT_UI_HEADERS})
else()
	add_executable(PsfPlayer ${QT_SOURCES} ${QT_MOC_SRCS} ${QT_RES_SOURCES} ${QT_UI_HEADERS})
endif()

target_link_libraries(PsfPlayer ${PSFPLAYER_PROJECT_LIBS})
target_include_directories(PsfPlayer PUBLIC
	./
	${CMAKE_CURRENT_BINARY_DIR}
)

if(TARGET_PLATFORM_WIN32)
	find_program(WINDEPLOYQT_EXE windeployqt HINTS "${QT_BINARY_DIRECTORY}")
	add_custom_command(TARGET PsfPlayer POST_BUILD
		COMMAND "${WINDEPLOYQT_EXE}" $<IF:$<CONFIG:Debug>,--debug,--release>
									 --no-translations
									 --no-compiler-runtime
									 "$<TARGET_FILE:PsfPlayer>"
	)
endif()
