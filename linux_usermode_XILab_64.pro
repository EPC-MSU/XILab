TEMPLATE = app
CONFIG += 64bit
TARGET = XILab_64_user
QMAKE_CXXFLAGS += -m64 -std=c++11
DESTDIR = ./release_64
QT += core gui script svg xml network
CONFIG += ./release_64/obj
DEFINES += __LINUX__ QT_DLL QWT_DLL UNICODE QT_THREAD_SUPPORT QT_NO_DEBUG NDEBUG QT_CORE_LIB QT_GUI_LIB QT_SCRIPT_LIB QT_SVG_LIB
INCLUDEPATH += ./src \
    ./GeneratedFiles_64 \
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
MOC_DIR += ./GeneratedFiles_64
OBJECTS_DIR += ./release_64/obj
UI_DIR += ./GeneratedFiles_64
RCC_DIR += ./Resources
include(XILab.pri)
