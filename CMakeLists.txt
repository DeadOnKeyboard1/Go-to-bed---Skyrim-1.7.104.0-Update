cmake_minimum_required(VERSION 3.20)

project(gotobed VERSION 2.0.7.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(directxtk CONFIG QUIET)
find_package(CommonLibSSE CONFIG REQUIRED)

configure_file(cmake/plugin.h.in plugin.h)
configure_file(cmake/version.rc.in version.rc)

add_library(Detours STATIC
	extern/detours/detours.cpp
	extern/detours/modules.cpp
	extern/detours/disasm.cpp
	extern/detours/image.cpp
)
target_include_directories(Detours PUBLIC extern)

add_library("${PROJECT_NAME}" SHARED)

target_sources("${PROJECT_NAME}"
	PRIVATE
	src/Actor.cpp
	src/Actor.h
	src/AIProcess.cpp
	src/AIProcess.h
	src/DiagUtil.cpp
	src/DiagUtil.h
	src/EquipConditions.cpp
	src/EquipConditions.h
	src/EquipMask.cpp
	src/EquipMask.h
	src/EquipSequence.cpp
	src/EquipSequence.h
	src/Fixes.cpp
	src/Fixes.h
	src/init.cpp
	src/init.h
	src/JCApi.cpp
	src/JCApi.h
	src/JCTypes.h
	src/main.cpp
	src/MenuOpenHandler.cpp
	src/MenuOpenHandler.h
	src/Offsets.h
	src/Outfit.cpp
	src/Outfit.h
	src/pch.h
	src/Settings.cpp
	src/Settings.h
	src/SettingsPapyrus.cpp
	src/SettingsPapyrus.h
	src/UIFuncs.cpp
	src/UIFuncs.h
	src/UIUtil.cpp
	src/UIUtil.h
	"${PROJECT_BINARY_DIR}/plugin.h"
	"${PROJECT_BINARY_DIR}/version.rc"
)

target_precompile_headers("${PROJECT_NAME}"
	PRIVATE
	src/pch.h
)

target_compile_features("${PROJECT_NAME}"
	PRIVATE
	cxx_std_23
)

if(DEFINED ENV{USERPROFILE})
	file(TO_CMAKE_PATH "$ENV{USERPROFILE}" _USERPROFILE_PATH)
	file(TO_NATIVE_PATH "$ENV{USERPROFILE}" _USERPROFILE_NATIVE)
	set(PRIVACY_COMPILE_FLAGS
		/experimental:deterministic
		"/pathmap:${_USERPROFILE_PATH}=."
		"/pathmap:${_USERPROFILE_NATIVE}=."
	)
else()
	set(PRIVACY_COMPILE_FLAGS
		/experimental:deterministic
	)
endif()

target_compile_options(Detours
	PRIVATE
	${PRIVACY_COMPILE_FLAGS}
)

target_compile_options("${PROJECT_NAME}"
	PRIVATE
	/UENABLE_SKYRIM_VR
	/Zi
	${PRIVACY_COMPILE_FLAGS}
)

target_link_options("${PROJECT_NAME}"
	PRIVATE
	/DEBUG:FULL
	"/PDBALTPATH:%_PDB%"
	"/PDBSTRIPPED:${CMAKE_CURRENT_BINARY_DIR}/gotobed.stripped.pdb"
)

target_include_directories("${PROJECT_NAME}"
	PRIVATE
	"${PROJECT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/extern"
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_link_libraries("${PROJECT_NAME}"
	PRIVATE
	CommonLibSSE::CommonLibSSE
	Detours
)

install(TARGETS "${PROJECT_NAME}" RUNTIME DESTINATION "${SKYRIM_ROOT}/data/skse/plugins")