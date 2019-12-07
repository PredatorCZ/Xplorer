cmake_minimum_required(VERSION 3.3)

project(Xplorer VERSION 1.0.1)

set(TARGETEX_LOCATION 3rd_party/PreCore/cmake)
include(${TARGETEX_LOCATION}/3dsmax.cmake)

build_target(
	TYPE SHARED
	SOURCES
		src/ColorMask.cpp
		src/ColorVar.cpp
		src/DllEntry.cpp
		src/Xplorer.def
		src/Xplorer.rc
	LINKS
		bmm core flt maxutil maxscrpt paramblk2 geom
	DEFINITIONS
		${MaxDefinitions}
	INCLUDES
		${MaxSDK}/include
		include
		3rd_party/PreCore/MAXEx
	LINK_DIRS
		${MaxSDKLibrariesPath}
	AUTHOR "Lukas Cone"
	DESCR "Xplorer 3DS Max Plugin"
	NAME "Xplorer"
	START_YEAR 2019
	PROPERTIES
		SUFFIX .dlu
		${MaxProperties}
)