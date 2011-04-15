INCLUDE (CheckIncludeFiles)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

if (WIN32)
    message(STATUS "Using in source libraries")
    find_library(GD_LIBRARIES NAMES bgd PATHS ${CMAKE_CURRENT_SOURCE_DIR}/win32/lib)
    SET (LOCAL_GD 1)
    find_library(PNG_LIBRARIES NAMES libpng PATHS ${CMAKE_CURRENT_SOURCE_DIR}/win32/lib)
    set( CMAKE_REQUIRED_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/win32/include ${CMAKE_REQUIRED_INCLUDES})
    include_directories(${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/win32/include )
else()
    find_package(GD REQUIRED)
    find_package(PNG REQUIRED)
    find_package(GIF REQUIRED)
    include_directories(${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES})
endif()

###
# to minimize the amount of edits we have to make to the npiet
# source, we create a config.h file that plays nice with the
# autotools-isms in npiet.c
###
CHECK_INCLUDE_FILES (gd.h HAVE_GD_H)
CHECK_INCLUDE_FILES (png.h HAVE_PNG_H)
CHECK_INCLUDE_FILES (gif_lib.h HAVE_GIF_LIB_H)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)



set( CMAKE_BUILD_TYPE Debug)
set( DEBUG_BUILD_TYPE ON )

ENABLE_TESTING()

ADD_TEST(npiettest ${EXECUTABLE_OUTPUT_PATH}/npiettest Hello)

set(npiet_SRCS npiet.c npiet_utils.c)

# add_executable(npiet ${npiet_SRCS} )
# target_link_libraries( npiet ${GD_LIBRARIES} ${GIF_LIBRARIES} ${PNG_LIBRARIES})
add_library( npiet ${npiet_SRCS} )

target_link_libraries( npiet
                       ${GD_LIBRARIES}
                       ${GIF_LIBRARIES}
                       ${PNG_LIBRARIES} )

# Tests

set( npiettest_SRCS test/NPietTest.cpp )
qt4_automoc(${npiettest_SRCS})
ADD_EXECUTABLE(npiettest ${npiettest_SRCS} )
TARGET_LINK_LIBRARIES(npiettest
    ${QT_LIBRARIES}
    ${QT_QTTEST_LIBRARIES}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    npiet )

# copy libs to the output dirs
# not sure how msvc could be true and win32 not, but just in case..
if (MSVC AND WIN32) 
    message(STATUS "Copying dlls to the Debug and Release directories")
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/Debug)
    file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/Release)
    file(COPY win32/bin/ DESTINATION ${PROJECT_BINARY_DIR}/Debug FILES_MATCHING PATTERN "*.dll")
    file(COPY win32/bin/ DESTINATION ${PROJECT_BINARY_DIR}/Release FILES_MATCHING PATTERN "*.dll")
endif()