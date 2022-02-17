#
# General Settings
#
cmake_minimum_required (VERSION 3.6.0 FATAL_ERROR)

project (OscarIso8583)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/lib)
set(PROJECT_TARGET_DIR ${CMAKE_SOURCE_DIR}/build/target)

set(TARGET_DIR ${CMAKE_SOURCE_DIR}/build/target)
set(TARGET_EXECUTABLE_NAME ${PROJECT_NAME})
set(TARGET_LIBRARY_NAME ${PROJECT_NAME})

#
# Current source file(s) to compile
#
file(GLOB HEADER_FILES ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB LIBSRC_FILES ${PROJECT_SOURCE_DIR}/libsource/*.c)
file(GLOB MAIN_FILE ${PROJECT_SOURCE_DIR}/source/main.cpp)
message(${LIBSRC_FILES})


#
# Project Internal Reference(s)
#
include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${TARGET_DIR}/lib)
#include_directories("${PROJECT_SOURCE_DIR}/dl_c_iso8583_v0_0_3")

#
# Project Compilation Flags
#
set(PROJECT_CXXFLAGS "-DTO_EBCDIC")
# add_compile_options("-DTO_EBCDIC")
set(CMAKE_CXX_FLAGS "-g ${PROJECT_CXXFLAGS}")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${PROJECT_CXXFLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-Wno-deprecated -Wno-all")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${PROJECT_CXXFLAGS}")

                       
#
# All libraries required to link executables
#
set(DEPENDENCY_LIBRARIES)

#
# Special commands
#

function(install_target TARGET_NAME FILES TARGET_DIR)

	if(NOT TARGET ${TARGET_NAME})
		add_custom_target(${TARGET_NAME} ALL)
	endif()

	add_custom_command(TARGET ${TARGET_NAME} PRE_BUILD
					 COMMAND mkdir -p ${TARGET_DIR}
					 )

	foreach(file ${FILES})
		get_filename_component(FILE_NAME ${file} NAME)
		add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
						 COMMAND cp -rpfHd ${file} ${TARGET_DIR}/${FILE_NAME}
						 COMMENT "Copying ${FILE_NAME} to ${TARGET_DIR} ...")
	endforeach()
endfunction()

#
# Build Project
#

set(DEPENDENCY_LIBRARIES_LIB ${DEPENDENCY_LIBRARIES})
list(REMOVE_ITEM DEPENDENCY_LIBRARIES_LIB ${PROJECT_NAME})

if(MAIN_FILE)
    add_executable(${PROJECT_NAME}-bin ${MAIN_FILE})
    set_target_properties(${PROJECT_NAME}-bin PROPERTIES OUTPUT_NAME ${TARGET_EXECUTABLE_NAME})

    if(LIBSRC_FILES)
        list(REMOVE_ITEM LIBSRC_FILES ${MAIN_FILE})
        add_dependencies(${PROJECT_NAME}-bin ${PROJECT_NAME}-lib)
        set(DEPENDENCY_LIBRARIES ${DEPENDENCY_LIBRARIES} ${PROJECT_NAME})
    endif(LIBSRC_FILES)

    target_link_libraries(${PROJECT_NAME}-bin ${DEPENDENCY_LIBRARIES})
endif(MAIN_FILE)

if(LIBSRC_FILES)

    add_library(${PROJECT_NAME}-obj OBJECT ${LIBSRC_FILES})
    #message(${LIBSRC_FILES})
    # shared and static libraries built from the same object files
    add_library(${PROJECT_NAME}-lib SHARED $<TARGET_OBJECTS:${PROJECT_NAME}-obj>)

    target_link_libraries(${PROJECT_NAME}-lib PUBLIC ${DEPENDENCY_LIBRARIES_LIB})
    set_target_properties(${PROJECT_NAME}-lib PROPERTIES OUTPUT_NAME ${TARGET_LIBRARY_NAME})

endif(LIBSRC_FILES)

#
# install_project()
#

if(MAIN_FILE)
    if( NOT TARGET_EXECUTABLES )
        set(TARGET_EXECUTABLES ${EXECUTABLE_OUTPUT_PATH}/${TARGET_EXECUTABLE_NAME})
        message(STATUS "Default executable ${TARGET_EXECUTABLES} will be in the target list")
        install_target(${PROJECT_NAME}-bin "${TARGET_EXECUTABLES}" ${PROJECT_TARGET_DIR}/bin)
    endif()
endif(MAIN_FILE)

if(LIBSRC_FILES)
    if( NOT TARGET_LIBRARIES )
        set(TARGET_LIBRARIES ${LIBRARY_OUTPUT_PATH}/lib${TARGET_LIBRARY_NAME}.so)
        message(STATUS "Default library ${TARGET_LIBRARIES} will be in the target list")
    endif()
    install_target(${PROJECT_NAME}-lib "${TARGET_LIBRARIES}" ${PROJECT_TARGET_DIR}/lib)
endif(LIBSRC_FILES)

if(HEADER_FILES)
    if( NOT TARGET_HEADERS )
        set(TARGET_HEADERS ${HEADER_FILES})
        message(STATUS "All header files will be in the target list")
        install_target(${PROJECT_NAME}-header "${TARGET_HEADERS}" ${PROJECT_TARGET_DIR}/include)
    endif()
endif(HEADER_FILES)