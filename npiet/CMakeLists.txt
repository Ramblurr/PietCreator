INCLUDE (CheckIncludeFiles)

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules )

find_package(GD REQUIRED)
find_package(PNG REQUIRED)
find_package(GIF REQUIRED)

###
# to minimize the amount of edits we have to make to the npiet
# source, we create a config.h file that plays nice with the
# autotools-isms in npiet.c
###

CHECK_INCLUDE_FILES (gd.h HAVE_GD_H)
CHECK_INCLUDE_FILES (png.h HAVE_PNG_H)
CHECK_INCLUDE_FILES (gif_lib.h HAVE_GIF_LIB_H)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES} )

set( CMAKE_BUILD_TYPE Debug)
set( DEBUG_BUILD_TYPE ON )

ENABLE_TESTING()

ADD_TEST(npiettest ${EXECUTABLE_OUTPUT_PATH}/npiettest Hello)

set(npiet_SRCS npiet.c )

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

