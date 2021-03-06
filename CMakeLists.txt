cmake_minimum_required(VERSION 2.6)

# Have Linux builds use the same _DEBUG / NDEBUG defines as Microsoft's C++ compiler.
if( UNIX )
	IF (CMAKE_BUILD_TYPE MATCHES "Debug" )
		ADD_DEFINITIONS(-D_DEBUG)
	ELSE()
		ADD_DEFINITIONS(-DNDEBUG)
	ENDIF()
ENDIF()

SET( PROJECT_NAME "ExocortexCrate" )

IF (UNIX AND NOT APPLE ) 
	ADD_DEFINITIONS(-DLINUX )
ENDIF()

IF( WIN32 )
	ADD_DEFINITIONS( -D_WIN64 -DWIN64 )
ENDIF()
 
set( Exocortex_INSTALL_BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/install" )

if( HDF5_ENABLE_THREADSAFE )
	add_definitions( -DH5_HAVE_THREADSAFE )
	if( WIN32 )
		SET( PROJECT_NAME "${PROJECT_NAME}_Windows7" )
		set( Exocortex_INSTALL_BASE_DIR ${Exocortex_INSTALL_BASE_DIR}/Windows7 )
	elseif( UNIX )
		IF (CMAKE_BUILD_TYPE MATCHES "Debug")
			SET( PROJECT_NAME "${PROJECT_NAME}_LinuxDebug" )
			set( Exocortex_INSTALL_BASE_DIR ${Exocortex_INSTALL_BASE_DIR}/LinuxDebug )	
		ELSE()
			SET( PROJECT_NAME "${PROJECT_NAME}_LinuxRelease" )
			set( Exocortex_INSTALL_BASE_DIR ${Exocortex_INSTALL_BASE_DIR}/LinuxRelease )	
		ENDIF()
	else()
		message(FATAL_ERROR "Should not get here")
	endif()  
else()
	SET( PROJECT_NAME "${PROJECT_NAME}_WindowsXP" )
	if( WIN32 )
		set( Exocortex_INSTALL_BASE_DIR ${Exocortex_INSTALL_BASE_DIR}/WindowsXP )
	else()
		message(FATAL_ERROR "Should not get here")
	endif()
endif()

if( MSVC9 )
	SET( PROJECT_NAME "${PROJECT_NAME}_2008" )
elseif( MSVC10 ) 
	SET( PROJECT_NAME "${PROJECT_NAME}_2010" )
elseif( MSVC11 ) 
	SET( PROJECT_NAME "${PROJECT_NAME}_2012" )
endif()


project( ${PROJECT_NAME} )

INCLUDE(./ExocortexCMakeShared.txt)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set( HDF5Folder "HDF5" )
set( ZlibFolder "HDF5" )
set( IlmBaseFolder "Alembic" )
set( AlembicFolder "Alembic" )

SET(DIR_SOFTIMAGEALEBMIC ${CMAKE_CURRENT_SOURCE_DIR})

get_filename_component( LIBRARY_ROOT "${CMAKE_CURRENT_SOURCE_DIR}" PATH )
set( LIBRARY_VERSION "20141211.01" )
set( LIBRARY_ROOT "${LIBRARY_ROOT}/Libraries.${LIBRARY_VERSION}" )
if( NOT EXISTS ${LIBRARY_ROOT} )
	message( "!!!" )
	message( "!!! Download the latest library from here: " )
	message( "!!! https://s3-us-west-2.amazonaws.com/exocortex-downloads/Libraries.${LIBRARY_VERSION}.7z")
	message( "!!!" )
	message( FATAL_ERROR "Can't find LIBRARY_ROOT: ${LIBRARY_ROOT}" )
endif()
 
# Configuration
SET( crate_MAJOR_VERSION "1" )
SET( crate_MINOR_VERSION "1" ) 
SET( crate_BUILD_VERSION "145")
SET( crate_VERSION "${crate_MAJOR_VERSION}.${crate_MINOR_VERSION}" )
SET( crate_FULLVERSION "${crate_MAJOR_VERSION}.${crate_MINOR_VERSION}.${crate_BUILD_VERSION}" )

add_definitions( "-Dcrate_MAJOR_VERSION=${crate_MAJOR_VERSION}" )
add_definitions( "-Dcrate_MINOR_VERSION=${crate_MINOR_VERSION}" )
add_definitions( "-Dcrate_BUILD_VERSION=${crate_BUILD_VERSION}" )

setup_os_name()
setup_cpu_name()

SET( ALEMBIC64 0 )
IF( WIN32 )
	IF( CMAKE_SIZEOF_VOID_P MATCHES "8" )
		SET( ALEMBIC64 1 )
	ENDIF( CMAKE_SIZEOF_VOID_P MATCHES "8" )
ENDIF( WIN32 )

IF( UNIX )
	IF( CMAKE_SIZEOF_VOID_P MATCHES "8" )
		SET( ALEMBIC64 1 )
	ENDIF( CMAKE_SIZEOF_VOID_P MATCHES "8" )
ENDIF( UNIX )

IF( UNIX )
	# Issues with fpic (requires position independent code option during linking to these libs).

	# C Flags
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC"  )
	SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g3"  )
	#SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -fPIC"  )

	# C++ Flags
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC"  )
	SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -g3"  )
	#SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC"  )

ENDIF()

# Boost.
set( BOOST_INCLUDEDIR "${LIBRARY_ROOT}/boost/include/boost-1_44" )
    
if ( UNIX )
	set ( BOOST_OSDIR "linux" )
	set( OS_NAME "linux" )
elseif( WIN32 )
	set ( BOOST_OSDIR "windows" )
	set( OS_NAME "windows" )
else()
	message(FATAL_ERROR "Should not get here") 
endif()

if( CMAKE_SIZEOF_VOID_P MATCHES "8" )
	set( CPU_NAME "x64" )
	set( BOOST_LIBRARYDIR "${LIBRARY_ROOT}/boost/lib/x64/${BOOST_OSDIR}" )
elseif( CMAKE_SIZEOF_VOID_P MATCHES "4" )
	set( CPU_NAME "x86" )
	set( BOOST_LIBRARYDIR "${LIBRARY_ROOT}/boost/lib/x86/${BOOST_OSDIR}" )
else()
	message(FATAL_ERROR "Should not get here")
endif()

#MESSAGE( "BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}" )
#MESSAGE( "BOOST_LIBRARYDIR: ${BOOST_LIBRARYDIR}" )
if( NOT EXISTS ${BOOST_INCLUDEDIR} )
	message(FATAL_ERROR "Can't find BOOST_INCLUDEDIR: ${BOOST_INCLUDEDIR}" )
endif()

if( NOT EXISTS ${BOOST_LIBRARYDIR} )
	message(FATAL_ERROR "Can't find BOOST_LIBRARYDIR: ${BOOST_LIBRARYDIR}" )
endif()

# Boost
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_DETAILED_FAILURE_MSG ON )
set(Boost_USE_STATIC_RUNTIME ON)
set(Boost_NO_SYSTEM_PATHS true )

if( MSVC11 OR MSVC10 )
	set(BOOST_COMPONENTS date_time program_options regex thread iostreams filesystem system unit_test_framework )
else()
	set(BOOST_COMPONENTS date_time program_options regex thread iostreams python filesystem system unit_test_framework )
endif()

find_package( Boost 1.44.0 COMPONENTS REQUIRED ${BOOST_COMPONENTS} )

include_directories( ${Boost_INCLUDE_DIRS} )
link_directories( ${Boost_LIBRARY_DIRS} )

IF( WIN32 )
	#We statically link to reduce dependancies (copied from Bullet CMakeLists.txt)
	FOREACH(flag_var CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_MINSIZEREL CMAKE_CXX_FLAGS_RELWITHDEBINFO)
		IF(${flag_var} MATCHES "/MD")
			STRING(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
		ENDIF(${flag_var} MATCHES "/MD")
		IF(${flag_var} MATCHES "/MDd")
			STRING(REGEX REPLACE "/MDd" "/MTd" ${flag_var} "${${flag_var}}")
		ENDIF(${flag_var} MATCHES "/MDd")
	ENDFOREACH(flag_var)
endif()

# For multithreaded builds		
# [mkonyk] Fix ~ these are only valid on Windows platforms.
IF( WIN32 )
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP" )
	SET( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP" )
	
	# For Alembic library
	SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DPLATFORM_WINDOWS" )
ENDIF( WIN32 )

SET( ILMBASE_ROOT_DIR "Shared/ilmbase-1.0.2" )
SET( ZLIB_ROOT_DIR "Shared/zlib-1.2.5" )
SET( HDF_ROOT_DIR "Shared/hdf5-1.8.9" )
SET( ALEMBIC_ROOT_DIR "Shared/alembic-1.5.0" )
SET( ALEMBIC_NO_TESTS TRUE )
SET( ALEMBIC_NO_BOOTSTRAP TRUE )
SET( ALEMBIC_NO_OPENGL TRUE )

add_definitions( -Dalembic_ver=${ALEMBIC_ROOT_DIR} )
add_definitions( -Dhdf_ver=${HDF_ROOT_DIR} )
add_definitions( -Dcrate_ver=crate-${crate_FULLVERSION} )

# ILM Math
include_directories( "${ILMBASE_ROOT_DIR}/Half" )
include_directories( "${ILMBASE_ROOT_DIR}/Iex" )
include_directories( "${ILMBASE_ROOT_DIR}/IlmThread" )
include_directories( "${ILMBASE_ROOT_DIR}/Imath" )

ADD_SUBDIRECTORY ( ${ILMBASE_ROOT_DIR} )
SET( ALEMBIC_ILMBASE_LIBS ilmbase )
 
# ZLib
SET( BUILD_SHARED_LIBS OFF )
SET( SKIP_INSTALL_ALL ON )
SET( SKIP_INSTALL_LIBRARIES ON )

# For multithreaded builds		
IF( WIN32 )
	SET( CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd" )
	SET( CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MTd" )
	SET( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT" )
	SET( CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /MT" )
ENDIF()



include_directories( ${ZLIB_ROOT_DIR} )
include_directories("${CMAKE_CURRENT_BINARY_DIR}/${ZLIB_ROOT_DIR}" )

ADD_SUBDIRECTORY ( ${ZLIB_ROOT_DIR} )
SET( ALEMBIC_ZLIB_LIBS zlib )

# HDF
SET( ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${ZLIB_ROOT_DIR}" )
SET( H5_HAVE_ZLIB_H ON )
SET( H5_ZLIB_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/${ZLIB_ROOT_DIR}/zlib.h" )
SET( ZLIB_LIBRARY zlib )
SET( H5_HAVE_FILTER_DEFLATE ON )
SET(HDF5_ENABLE_THREADSAFE ON CACHE BOOL "Thread safety")
SET(HDF5_ENABLE_SZIP_SUPPORT OFF CACHE BOOL "Disable sZip")
SET(HDF5_BUILD_HL_LIB ON CACHE BOOL "Build high level lib")

ADD_SUBDIRECTORY ( ${HDF_ROOT_DIR} )
include_directories( "${HDF_ROOT_DIR}/src" )
include_directories( "${HDF_ROOT_DIR}/hl/src" )
include_directories( "${HDF_ROOT_DIR}/c++/src" )
include_directories( "${HDF_ROOT_DIR}/hl/c++/src" )
include_directories( "${CMAKE_CURRENT_BINARY_DIR}/${HDF_ROOT_DIR}" )
SET( ALEMBIC_HDF5_LIBS hdf5 hdf5_hl )

# reset the output directories
UNSET(CMAKE_RUNTIME_OUTPUT_DIRECTORY CACHE)
UNSET(CMAKE_LIBRARY_OUTPUT_DIRECTORY CACHE)
UNSET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY CACHE)

# Alembic
add_definitions( -DNOMINMAX )	 # disable min/max macros from <windows.h>.
include_directories( ${ALEMBIC_ROOT_DIR} )
ADD_SUBDIRECTORY ( ${ALEMBIC_ROOT_DIR} )
SET( ALEMBIC_CORE_LIBS AlembicAbcMaterial AlembicAbcGeom AlembicAbc AlembicAbcCoreHDF5 AlembicAbcCoreOgawa AlembicAbcCoreAbstract AlembicAbcCoreFactory AlembicOgawa AlembicUtil )

SET( ALL_ALEMBIC_LIBS ${ALEMBIC_CORE_LIBS} ${ALEMBIC_HDF5_LIBS} ${ALEMBIC_ZLIB_LIBS}
	  ${ALEMBIC_ILMBASE_LIBS} ${ZLIB_LIBRARIES} ${Boost_LIBRARIES} )

ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Shared/CommonUtils" "${CMAKE_CURRENT_BINARY_DIR}/Shared/CommonUtils" )

include_directories( "${CMAKE_CURRENT_SOURCE_DIR}/Shared/CommonUtils" )


add_definitions( -D_WINSOCKAPI_ )
add_definitions( -D_WINSOCKAPI2_ )


# Python extension
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Python/CMakeLists.txt")
	if( NOT MSVC10 AND NOT MSVC11 ) 
		ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Python/2.6.7" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicPython/2.6.7" )
		if( WIN32 )
			ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Python/2.7.2" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicPython/2.7.2" )
		else()
			ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Python/2.5.2" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicPython/2.5.2" )
		endif()
	endif()
ENDIF()

# Arnold dso
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Arnold/CMakeLists.txt")
	if( NOT MSVC10 AND NOT MSVC11 ) 
		ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Arnold/3" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicArnold/3" )
		ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Arnold/4" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicArnold/4" )
	endif()
ENDIF()

# softimage plugin
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Softimage/CMakeLists.txt")
	if( NOT MSVC10 AND NOT MSVC11 ) 
		ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Softimage/2012" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicSoftimage" )
		ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Softimage/2014SP2" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicSoftimage2014SP2" )
	elseif( MSVC11 )
		ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Softimage/2015" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicSoftimage2015" )
	endif()
ENDIF()

# maya plugin
# guide to maya compiler versions: http://around-the-corner.typepad.com/adn/2012/06/maya-compiler-versions.html
IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Maya/CMakeLists.txt")
	if( NOT MSVC10 AND NOT MSVC11) 
		ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Maya/2011" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicMaya/2011" )
		ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Maya/2012" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicMaya/2012" )
		ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Maya/2013" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicMaya/2013" )
		#ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Maya/2013.5" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicMaya/2013.5" )
		ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Maya/2014" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicMaya/2014" )
		ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Maya/2015" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicMaya/2015" )
	endif()
ENDIF()

# houdini plugin
#IF(INCLUDE_ALEMBIC_HOUDINI)
#IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Houdini/CMakeLists.txt")
#	ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/Houdini" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembicHoudini" )
#ENDIF()
#ENDIF(INCLUDE_ALEMBIC_HOUDINI)

# 3dsmax plugin
if( WIN32 )
	IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/3DSMax/CMakeLists.txt")
		if( MSVC11 )
			ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/3DSMax/2015" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembic3DSMax/2015" )
		elseif( MSVC10 ) 
			ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/3DSMax/2014" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembic3DSMax/2014" )
			ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/3DSMax/2013" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembic3DSMax/2013" )
		else()
			ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/3DSMax/2012" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembic3DSMax/2012" )
			ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/3DSMax/2011" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembic3DSMax/2011" )
			ADD_SUBDIRECTORY ( "${CMAKE_CURRENT_SOURCE_DIR}/3DSMax/2010" "${CMAKE_CURRENT_BINARY_DIR}/ExocortexAlembic3DSMax/2010" )
		endif()
	ENDIF()
endif()
