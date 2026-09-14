cmake_minimum_required(VERSION 3.20)

project(gotobed VERSION 2.1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
	src/init.cpp
	src/init.h
	src/main.cpp
	src/MenuOpenHandler.cpp
	src/MenuOpenHandler.h
	src/Offsets.h
	src/pch.h
	src/Settings.cpp
	src/Settings.h
	src/SettingsPapyrus.cpp
	src/SettingsPapyrus.h
	src/TESObjectREFR.cpp
	src/TESObjectREFR.h
	"${PROJECT_BINARY_DIR}/plugin.h"
	"${PROJECT_BINARY_DIR}/version.rc"
)

target_precompile_headers("${PROJECT_NAME}"
	PRIVATE
	src/pch.h
)

target_compile_features("${PROJECT_NAME}"
	PRIVATE
	cxx_std_20
)

target_compile_options("${PROJECT_NAME}"
	PRIVATE
	/UENABLE_SKYRIM_VR
	/Zi
)

target_link_options("${PROJECT_NAME}"
	PRIVATE
	/DEBUG:FULL
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