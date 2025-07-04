cmake_minimum_required(VERSION 3.18)

set(CMAKE_MODULE_PATH
	${CMAKE_CURRENT_SOURCE_DIR}/../deps/Dependencies/cmake-modules
	${CMAKE_MODULE_PATH}
)
include(Header)

project(PlayCore)

set(PROJECT_LIBS)
set(DEFINITIONS_LIST)

include(PrecompiledHeader)

set(ENABLE_AMAZON_S3 ON CACHE BOOL "Enable loading disc from Amazon S3 servers")

if(DEBUGGER_INCLUDED)
	list(APPEND DEFINITIONS_LIST DEBUGGER_INCLUDED=1)
endif()
if(PROFILE)
	list(APPEND DEFINITIONS_LIST PROFILE=1)
endif()
list(APPEND DEFINITIONS_LIST _IOP_EMULATE_MODULES=1)

if(USE_AOT_CACHE)
	list(APPEND DEFINITIONS_LIST AOT_USE_CACHE=1)
endif()

if(BUILD_AOT_CACHE)
	list(APPEND DEFINITIONS_LIST AOT_BUILD_CACHE=1)
endif()

if(TARGET_PLATFORM_ANDROID OR TARGET_PLATFORM_IOS OR TARGET_PLATFORM_JS)
	list(APPEND DEFINITIONS_LIST LOGGING_ENABLED=0)
endif()

if(TARGET_PLATFORM_UNIX AND NOT TARGET_PLATFORM_UNIX_ARM AND NOT TARGET_PLATFORM_UNIX_AARCH64)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -mssse3")
endif()

# targets, packages and dependencies
if(NOT TARGET Framework)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../deps/Framework/build_cmake/Framework
		${CMAKE_CURRENT_BINARY_DIR}/Framework
		EXCLUDE_FROM_ALL
	)
endif()
list(APPEND PROJECT_LIBS Framework)

if(NOT TARGET Framework_Http)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../deps/Framework/build_cmake/FrameworkHttp
		${CMAKE_CURRENT_BINARY_DIR}/FrameworkHttp
	)
endif()
list(APPEND PROJECT_LIBS Framework_Http)

if(NOT TARGET app_shared)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/app_shared
		${CMAKE_CURRENT_BINARY_DIR}/app_shared
	)
endif()
list(APPEND PROJECT_LIBS app_shared)

if(ENABLE_AMAZON_S3)
	if(NOT TARGET Framework_Amazon)
		add_subdirectory(
			${CMAKE_CURRENT_SOURCE_DIR}/../deps/Framework/build_cmake/FrameworkAmazon
			${CMAKE_CURRENT_BINARY_DIR}/FrameworkAmazon
			EXCLUDE_FROM_ALL
		)
	endif()
	list(APPEND PROJECT_LIBS Framework_Amazon)
	set(AMAZON_S3_SRC
		s3stream/S3ObjectStream.cpp
	)
	list(APPEND DEFINITIONS_LIST HAS_AMAZON_S3=1)
endif()

if(NOT TARGET CodeGen)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../deps/CodeGen
		${CMAKE_CURRENT_BINARY_DIR}/CodeGen
	)
endif()
list(APPEND PROJECT_LIBS CodeGen)

find_package(BZip2)
if(NOT BZIP2_FOUND)
	MESSAGE("-- Using Provided BZip2 source")
	if(NOT TARGET BZip2::BZip2)
		add_subdirectory(
			${CMAKE_CURRENT_SOURCE_DIR}/../deps/Dependencies/build_cmake/bzip2-1.0.6
			${CMAKE_CURRENT_BINARY_DIR}/bzip2-1.0.6
			EXCLUDE_FROM_ALL
		)
	endif()
	list(APPEND PROJECT_LIBS BZip2::BZip2)
else()
	include_directories(${BZIP2_INCLUDE_DIR})
	list(APPEND PROJECT_LIBS ${BZIP2_LIBRARIES})
endif()

if(NOT TARGET libzstd_zlibwrapper_static)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../deps/Dependencies/build_cmake/zstd_zlibwrapper
		${CMAKE_CURRENT_BINARY_DIR}/zstd_zlibwrapper
	)
endif()
LIST(APPEND PROJECT_LIBS libzstd_zlibwrapper_static)

if(NOT TARGET xxHash::xxhash)
	option(BUILD_SHARED_LIBS "Build shared libs" OFF)
	set(XXHASH_BUILD_ENABLE_INLINE_API ON)
	set(XXHASH_BUILD_XXHSUM OFF)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../deps/Dependencies/xxHash/cmake_unofficial/
		${CMAKE_CURRENT_BINARY_DIR}/xxHash
		EXCLUDE_FROM_ALL
	)
endif()
LIST(APPEND PROJECT_LIBS xxHash::xxhash)

# If ICU is available, add its libraries because Framework might need its functions
find_package(ICUUC)
if(ICUUC_FOUND)
	list(APPEND PROJECT_LIBS ${ICUUC_LIBRARIES})
endif()

if(NOT TARGET libchdr)
	add_subdirectory(
		${CMAKE_CURRENT_SOURCE_DIR}/../deps/libchdr/
		${CMAKE_CURRENT_BINARY_DIR}/libchdr
		EXCLUDE_FROM_ALL
	)
endif()
list(APPEND PROJECT_LIBS chdr-static)

if(NOT (TARGET_PLATFORM_ANDROID OR TARGET_PLATFORM_IOS))
	set(THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads REQUIRED)
	list(APPEND PROJECT_LIBS Threads::Threads)
endif()

set(COMMON_SRC_FILES
	BasicBlock.cpp
	BasicBlock.h
	BiosDebugInfoProvider.h
	BlockLookupOneWay.h
	BlockLookupTwoWay.h
	ControllerInfo.cpp
	ControllerInfo.h
	COP_FPU.cpp
	COP_FPU.h
	COP_FPU_Reflection.cpp
	COP_SCU.cpp
	COP_SCU.h
	COP_SCU_Reflection.cpp
	discimages/ChdCdImageStream.cpp
	discimages/ChdCdImageStream.h
	discimages/ChdImageStream.cpp
	discimages/ChdImageStream.h
	discimages/ChdStreamSupport.cpp
	discimages/ChdStreamSupport.h
	discimages/CsoImageStream.cpp
	discimages/CsoImageStream.h
	discimages/CueSheet.cpp
	discimages/CueSheet.h
	discimages/IszImageStream.cpp
	discimages/IszImageStream.h
	discimages/MdsDiscImage.cpp
	discimages/MdsDiscImage.h
	DiskUtils.cpp
	DiskUtils.h
	ee/COP_VU.cpp
	ee/COP_VU.h
	ee/COP_VU_Reflection.cpp
	ee/DMAC.cpp
	ee/DMAC.h
	ee/Dmac_Channel.cpp
	ee/Dmac_Channel.h
	ee/EeBasicBlock.cpp
	ee/EeBasicBlock.h
	ee/Ee_IdleEvaluator.cpp
	ee/Ee_IdleEvaluator.h
	ee/Ee_LibMc2.cpp
	ee/Ee_LibMc2.h
	ee/Ee_SubSystem.cpp
	ee/Ee_SubSystem.h
	ee/EEAssembler.cpp
	ee/EEAssembler.h
	ee/EeExecutor.cpp
	ee/EeExecutor.h
	ee/FpAddTruncate.cpp
	ee/FpAddTruncate.h
	ee/FpMulTruncate.cpp
	ee/FpMulTruncate.h
	ee/GIF.cpp
	ee/GIF.h
	ee/INTC.cpp
	ee/INTC.h
	ee/IPU.cpp
	ee/IPU.h
	ee/IPU_DmVectorTable.cpp
	ee/IPU_DmVectorTable.h
	ee/IPU_MacroblockAddressIncrementTable.cpp
	ee/IPU_MacroblockAddressIncrementTable.h
	ee/IPU_MacroblockTypeBTable.cpp
	ee/IPU_MacroblockTypeBTable.h
	ee/IPU_MacroblockTypeITable.cpp
	ee/IPU_MacroblockTypeITable.h
	ee/IPU_MacroblockTypePTable.cpp
	ee/IPU_MacroblockTypePTable.h
	ee/IPU_MotionCodeTable.cpp
	ee/IPU_MotionCodeTable.h
	ee/MA_EE.cpp
	ee/MA_EE.h
	ee/MA_EE_Reflection.cpp
	ee/MA_VU.cpp
	ee/MA_VU.h
	ee/MA_VU_Lower.cpp
	ee/MA_VU_LowerReflection.cpp
	ee/MA_VU_Upper.cpp
	ee/MA_VU_UpperReflection.cpp
	ee/PS2OS.cpp
	ee/PS2OS.h
	ee/SIF.cpp
	ee/SIF.h
	ee/Timer.cpp
	ee/Timer.h
	ee/Vif.cpp
	ee/Vif.h
	ee/Vif1.cpp
	ee/Vif1.h
	ee/Vpu.cpp
	ee/Vpu.h
	ee/VuAnalysis.cpp
	ee/VuAnalysis.h
	ee/VuBasicBlock.cpp
	ee/VuBasicBlock.h
	ee/VuExecutor.cpp
	ee/VuExecutor.h
	ee/VUShared.cpp
	ee/VUShared.h
	ee/VUShared_Reflection.cpp
	ELF.h
	ElfDefs.h
	ElfFile.cpp
	ElfFile.h
	FpUtils.cpp
	FpUtils.h
	FrameDump.cpp
	FrameDump.h
	FrameLimiter.cpp
	FrameLimiter.h
	ScreenPositionListener.h
	InputConfig.cpp
	InputConfig.h
	GenericMipsExecutor.h
	gs/GsCachedArea.cpp
	gs/GsCachedArea.h
	gs/GsDebuggerInterface.h
	gs/GSH_Null.cpp
	gs/GSH_Null.h
	gs/GSHandler.cpp
	gs/GSHandler.h
	gs/GsPixelFormats.cpp
	gs/GsPixelFormats.h
	gs/GsSpriteRegion.h
	gs/GsTextureCache.h
	gs/GsTransferRange.h
	hdd/ApaDefs.h
	hdd/ApaReader.cpp
	hdd/ApaReader.h
	hdd/HddDefs.h
	hdd/PfsDefs.h
	hdd/PfsReader.cpp
	hdd/PfsReader.h
	input/InputBindingManager.cpp
	input/InputBindingManager.h
	input/InputProvider.h
	input/PH_GenericInput.cpp
	input/PH_GenericInput.h
	iop/ArgumentIterator.cpp
	iop/ArgumentIterator.h
	iop/ioman/DirectoryDevice.cpp
	iop/ioman/DirectoryDevice.h
	iop/ioman/McDumpDevice.cpp
	iop/ioman/McDumpDevice.h
	iop/ioman/HardDiskDevice.cpp
	iop/ioman/HardDiskDevice.h
	iop/ioman/HardDiskDumpDevice.cpp
	iop/ioman/HardDiskDumpDevice.h
	iop/ioman/OpticalMediaDevice.cpp
	iop/ioman/OpticalMediaDevice.h
	iop/ioman/OpticalMediaDirectoryIterator.cpp
	iop/ioman/OpticalMediaDirectoryIterator.h
	iop/ioman/PathDirectoryDevice.h
	iop/ioman/PathDirectoryIterator.cpp
	iop/ioman/PathDirectoryIterator.h
	iop/ioman/PreferenceDirectoryDevice.h
	iop/Ioman_Defs.h
	iop/Ioman_Device.h
	iop/Ioman_DirectoryIterator.h
	iop/Ioman_ScopedFile.cpp
	iop/Ioman_ScopedFile.h
	iop/Iop_Cdvdfsv.cpp
	iop/Iop_Cdvdfsv.h
	iop/Iop_Cdvdman.cpp
	iop/Iop_Cdvdman.h
	iop/Iop_Dev9.cpp
	iop/Iop_Dev9.h
	iop/Iop_Dmac.cpp
	iop/Iop_Dmac.h
	iop/Iop_DmacChannel.cpp
	iop/Iop_DmacChannel.h
	iop/Iop_Dmacman.cpp
	iop/Iop_Dmacman.h
	iop/Iop_Dynamic.cpp
	iop/Iop_Dynamic.h
	iop/Iop_FileIo.cpp
	iop/Iop_FileIo.h
	iop/Iop_FileIoHandler1000.cpp
	iop/Iop_FileIoHandler1000.h
	iop/Iop_FileIoHandler2100.cpp
	iop/Iop_FileIoHandler2100.h
	iop/Iop_FileIoHandler2200.cpp
	iop/Iop_FileIoHandler2200.h
	iop/Iop_Heaplib.cpp
	iop/Iop_Heaplib.h
	iop/Iop_Ilink.cpp
	iop/Iop_Ilink.h
	iop/Iop_Intc.cpp
	iop/Iop_Intc.h
	iop/Iop_Intrman.cpp
	iop/Iop_Intrman.h
	iop/Iop_Ioman.cpp
	iop/Iop_Ioman.h
	iop/Iop_IomanX.cpp
	iop/Iop_IomanX.h
	iop/Iop_LibSd.cpp
	iop/Iop_LibSd.h
	iop/Iop_Loadcore.cpp
	iop/Iop_Loadcore.h
	iop/Iop_McServ.cpp
	iop/Iop_McServ.h
	iop/Iop_Modload.cpp
	iop/Iop_Modload.h
	iop/Iop_Module.cpp
	iop/Iop_Module.h
	iop/Iop_MtapMan.cpp
	iop/Iop_MtapMan.h
	iop/namco_sys147/NamcoSys147NANDReader.cpp
	iop/namco_sys147/NamcoSys147NANDReader.h
	iop/namco_sys147/Iop_NamcoNANDDevice.cpp
	iop/namco_sys147/Iop_NamcoNANDDevice.h
	iop/namco_sys147/Iop_NamcoSys147.cpp
	iop/namco_sys147/Iop_NamcoSys147.h
	iop/namco_sys246/Iop_NamcoAcCdvd.cpp
	iop/namco_sys246/Iop_NamcoAcCdvd.h
	iop/namco_sys246/Iop_NamcoAcRam.cpp
	iop/namco_sys246/Iop_NamcoAcRam.h
	iop/namco_sys246/Iop_NamcoPadMan.cpp
	iop/namco_sys246/Iop_NamcoPadMan.h
	iop/namco_sys246/Iop_NamcoSys246.cpp
	iop/namco_sys246/Iop_NamcoSys246.h
	iop/Iop_PadMan.cpp
	iop/Iop_PadMan.h
	iop/Iop_PathUtils.cpp
	iop/Iop_PathUtils.h
	iop/Iop_PowerOff.cpp
	iop/Iop_PowerOff.h
	iop/Iop_RootCounters.cpp
	iop/Iop_RootCounters.h
	iop/Iop_Secrman.cpp
	iop/Iop_Secrman.h
	iop/Iop_SifCmd.cpp
	iop/Iop_SifCmd.h
	iop/Iop_SifDynamic.cpp
	iop/Iop_SifDynamic.h
	iop/Iop_SifMan.cpp
	iop/Iop_SifMan.h
	iop/Iop_SifManNull.cpp
	iop/Iop_SifManNull.h
	iop/Iop_SifManPs2.cpp
	iop/Iop_SifManPs2.h
	iop/Iop_Sio2.cpp
	iop/Iop_Sio2.h
	iop/Iop_Speed.cpp
	iop/Iop_Speed.h
	iop/Iop_Spu.cpp
	iop/Iop_Spu.h
	iop/Iop_Spu2.cpp
	iop/Iop_Spu2.h
	iop/Iop_Spu2_Core.cpp
	iop/Iop_Spu2_Core.h
	iop/Iop_SpuBase.cpp
	iop/Iop_SpuBase.h
	iop/Iop_Stdio.cpp
	iop/Iop_Stdio.h
	iop/Iop_SubSystem.cpp
	iop/Iop_SubSystem.h
	iop/Iop_Sysclib.cpp
	iop/Iop_Sysclib.h
	iop/Iop_Sysmem.cpp
	iop/Iop_Sysmem.h
	iop/Iop_Thbase.cpp
	iop/Iop_Thbase.h
	iop/Iop_Thevent.cpp
	iop/Iop_Thevent.h
	iop/Iop_Thfpool.cpp
	iop/Iop_Thfpool.h
	iop/Iop_Thmsgbx.cpp
	iop/Iop_Thmsgbx.h
	iop/Iop_Thsema.cpp
	iop/Iop_Thsema.h
	iop/Iop_Thvpool.cpp
	iop/Iop_Thvpool.h
	iop/Iop_Timrman.cpp
	iop/Iop_Timrman.h
	iop/Iop_Usbd.cpp
	iop/Iop_Usbd.h
	iop/Iop_Vblank.cpp
	iop/Iop_Vblank.h
	iop/IopBios.cpp
	iop/IopBios.h
	iop/UsbDefs.h
	iop/UsbDevice.h
	iop/UsbBuzzerDevice.cpp
	iop/UsbBuzzerDevice.h
	ISO9660/BlockProvider.h
	ISO9660/DirectoryRecord.cpp
	ISO9660/DirectoryRecord.h
	ISO9660/File.cpp
	ISO9660/File.h
	ISO9660/ISO9660.cpp
	ISO9660/ISO9660.h
	ISO9660/PathTable.cpp
	ISO9660/PathTable.h
	ISO9660/PathTableRecord.cpp
	ISO9660/PathTableRecord.h
	ISO9660/VolumeDescriptor.cpp
	ISO9660/VolumeDescriptor.h
	MA_MIPSIV.cpp
	MA_MIPSIV.h
	MA_MIPSIV_Reflection.cpp
	MA_MIPSIV_Templates.cpp
	MailBox.cpp
	MailBox.h
	MemoryMap.cpp
	MemoryMap.h
	MemoryUtils.cpp
	MemoryUtils.h
	MIPS.cpp
	MIPS.h
	MIPSAnalysis.cpp
	MIPSAnalysis.h
	MIPSArchitecture.cpp
	MIPSArchitecture.h
	MIPSAssembler.cpp
	MIPSAssembler.h
	MIPSCoprocessor.cpp
	MIPSCoprocessor.h
	MipsExecutor.h
	MipsFunctionPatternDb.cpp
	MipsFunctionPatternDb.h
	MIPSInstructionFactory.cpp
	MIPSInstructionFactory.h
	MipsJitter.cpp
	MipsJitter.h
	MIPSReflection.cpp
	MIPSReflection.h
	MIPSTags.cpp
	MIPSTags.h
	OpticalMedia.cpp
	OpticalMedia.h
	OsStructManager.h
	OsStructQueue.h
	OsVariableWrapper.h
	PadHandler.cpp
	PadHandler.h
	PadInterface.cpp
	PadInterface.h
	Pch.cpp
	Pch.h
	PH_Generic.cpp
	PH_Generic.h
	Profiler.cpp
	Profiler.h
	Ps2Const.h
	PS2VM.cpp
	PS2VM.h
	PS2VM_Preferences.h
	psx/PsxBios.cpp
	psx/PsxBios.h
	saves/Icon.cpp
	saves/Icon.h
	saves/MaxSaveImporter.cpp
	saves/MaxSaveImporter.h
	saves/McDumpReader.cpp
	saves/McDumpReader.h
	saves/McDumpSaveImporter.cpp
	saves/McDumpSaveImporter.h
	saves/MemoryCard.cpp
	saves/PsuSaveImporter.cpp
	saves/PsuSaveImporter.h
	saves/Save.cpp
	saves/Save.h
	saves/SaveExporter.cpp
	saves/SaveExporter.h
	saves/SaveImporterBase.cpp
	saves/SaveImporterBase.h
	saves/SaveImporter.cpp
	saves/SaveImporter.h
	saves/XpsSaveImporter.cpp
	saves/XpsSaveImporter.h
	ScopedVmPauser.cpp
	ScopedVmPauser.h
	ScreenShotUtils.cpp
	ScreenShotUtils.h
	SifDefs.h
	SifModule.h
	SifModuleAdapter.h
	states/MemoryStateFile.cpp
	states/MemoryStateFile.h
	states/RegisterState.cpp
	states/RegisterState.h
	states/RegisterStateUtils.h
	states/RegisterStateCollectionFile.cpp
	states/RegisterStateCollectionFile.h
	states/RegisterStateFile.cpp
	states/RegisterStateFile.h
	states/XmlStateFile.cpp
	states/XmlStateFile.h
	static_loop.h
	TimeUtils.h
	uint128.h
	VirtualPad.cpp
	VirtualPad.h
	${AMAZON_S3_SRC}
)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	add_compile_options("-fbracket-depth=512")
endif()

if(TARGET_PLATFORM_WIN32)
	set(PLATFORM_SPECIFIC_SRC_FILES VolumeStream.cpp)
	list(APPEND PROJECT_LIBS winmm)
endif()

if(TARGET_PLATFORM_MACOS OR TARGET_PLATFORM_UNIX OR TARGET_PLATFORM_JS)
	set(PLATFORM_SPECIFIC_SRC_FILES Posix_VolumeStream.cpp)
endif()

if(TARGET_PLATFORM_JS)
	set(PLATFORM_SPECIFIC_SRC_FILES ${PLATFORM_SPECIFIC_SRC_FILES} Js_DiscImageDeviceStream.cpp Js_DiscImageDeviceStream.h)
endif()

add_library(PlayCore STATIC ${COMMON_SRC_FILES} ${PLATFORM_SPECIFIC_SRC_FILES})
target_link_libraries(PlayCore ${PROJECT_LIBS})
target_include_directories(PlayCore
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/../deps/Framework/include
		${CMAKE_CURRENT_SOURCE_DIR}/../deps/CodeGen/include
)
target_compile_definitions(PlayCore PUBLIC ${DEFINITIONS_LIST})

if(TARGET_PLATFORM_WIN32)
	add_precompiled_header(PlayCore Pch.h FORCEINCLUDE SOURCE_CXX Pch.cpp)
endif()
