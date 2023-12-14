TEMPLATE = app
CONFIG += 32bit
TARGET = XILab_32_user
QMAKE_CXXFLAGS += -m32 -std=c++11
DESTDIR = ./release_32
QT += core gui script svg xml network
CONFIG += ./release_32/obj
DEFINES += __LINUX__ QT_DLL QWT_DLL UNICODE QT_THREAD_SUPPORT QT_NO_DEBUG NDEBUG QT_CORE_LIB QT_GUI_LIB QT_SCRIPT_LIB QT_SVG_LIB
INCLUDEPATH += ./src \
    ./GeneratedFiles_32 \
    /usr/local/qwt-%qwtver/include/ \
    ../libximc/libximc/include/ \
    ./
QMAKE_LFLAGS += '-Wl,-rpath,\'\$$ORIGIN\'' 
LIBS += \
    -L"./usr/lib/" \
    -L"/usr/local/qwt-%qwtver/lib/" \
    -lqwt \
    -lbindy \
    -lximc
DEPENDPATH += .
MOC_DIR += ./GeneratedFiles_32
OBJECTS_DIR += ./release_32/obj
UI_DIR += ./GeneratedFiles_32
RCC_DIR += ./Resources
include(XILab.pri)
