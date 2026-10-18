QT = core

CONFIG += c++17 cmdline

# Ruta absoluta a la base de datos (Qt ejecuta desde la carpeta de compilacion, no del codigo)
DEFINES += RUTA_BD=\\\"$$PWD/base_datos.txt\\\"

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        album.cpp \
        foto.cpp \
        main.cpp \
        mensaje.cpp \
        plataforma.cpp \
        usuario.cpp \
        utilidades.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    album.h \
    foto.h \
    mensaje.h \
    plataforma.h \
    usuario.h \
    utilidades.h

DISTFILES += \
    base_datos.txt
