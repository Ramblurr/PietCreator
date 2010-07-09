project(pietcreator)
cmake_minimum_required(VERSION 2.6)
find_package(Qt4 REQUIRED)

include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR} .)

add_subdirectory( npiet )

set( pietcreator_SRCS 
    main.cpp
    MainWindow.cpp
    ImageModel.cpp
    PixelDelegate.cpp
    KColorCells.cpp
    KColorMimeData.cpp
    KColorPatch.cpp
    ViewMonitor.cpp )

qt4_automoc(${pietcreator_SRCS})

qt4_wrap_ui( pietcreator_SRCS MainWindow.ui )

add_executable(pietcreator ${pietcreator_SRCS})

target_link_libraries( pietcreator
                       ${QT_QTCORE_LIBRARY}
                       ${QT_QTGUI_LIBRARY}
                       ${GD_LIBRARIES}
                       ${GIF_LIBRARIES}
                       ${PNG_LIBRARIES}
                       npiet  )
