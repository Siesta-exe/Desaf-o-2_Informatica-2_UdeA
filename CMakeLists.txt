cmake_minimum_required(VERSION 3.16)

project(Proyecto_desafio2 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(Proyecto_desafio2
  main.cpp
  plataforma.h plataforma.cpp
  usuario.h usuario.cpp
  album.h album.cpp
  foto.h foto.cpp
  mensaje.h mensaje.cpp
)
target_link_libraries(Proyecto_desafio2 PRIVATE Qt${QT_VERSION_MAJOR}::Core)

include(GNUInstallDirs)
install(TARGETS Proyecto_desafio2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
