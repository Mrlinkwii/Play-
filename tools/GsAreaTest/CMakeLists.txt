cmake_minimum_required(VERSION 3.18)

set(CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/../../deps/Dependencies/cmake-modules
	${CMAKE_MODULE_PATH}
)
include(Header)

project(GsAreaTest)

if (NOT TARGET PlayCore)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../../Source/
		${CMAKE_CURRENT_BINARY_DIR}/Source
	)
endif()

add_executable(GsAreaTest
	GsCachedAreaTest.cpp
	GsSpriteRegionTest.cpp
	GsTransferInvalidationTest.cpp
	Main.cpp

	GsCachedAreaTest.h
	GsSpriteRegionTest.h
	GsTransferInvalidationTest.h
	Test.h
)

target_link_libraries(GsAreaTest PlayCore)
add_test(NAME GsAreaTest
	COMMAND GsAreaTest
)