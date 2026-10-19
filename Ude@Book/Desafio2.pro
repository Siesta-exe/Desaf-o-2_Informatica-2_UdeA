QT = core

CONFIG += c++17 cmdline

DEFINES += RUTA_DATOS=\\\"$$PWD\\\"

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
    albumes.txt \
    fotos.txt \
    mensajes.txt \
    relaciones.txt \
    usuarios.txt
