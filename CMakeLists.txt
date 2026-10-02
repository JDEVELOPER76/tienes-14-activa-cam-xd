cmake_minimum_required(VERSION 3.16)

project(CamaraApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenCV REQUIRED)

add_executable(camara
    main.cpp

    camara/Camara.cpp
    camara/Camara.h

    gui/VentanaCamara.cpp
    gui/VentanaCamara.h

    gui/VentanaConfig.cpp
    gui/VentanaConfig.h

    config/Configuracion.cpp
    config/Configuracion.h
)

target_include_directories(camara PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(camara
    Qt6::Widgets
    ${OpenCV_LIBS}
)
