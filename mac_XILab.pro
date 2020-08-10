TEMPLATE = app
TARGET = XILab
CONFIG += app_bundle
CONFIG += x86_64
DESTDIR = ./release
DEFINES += __APPLE__ QT_DLL QWT_DLL UNICODE QT_THREAD_SUPPORT QT_NO_DEBUG NDEBUG QT_CORE_LIB QT_GUI_LIB QT_SCRIPT_LIB QT_SVG_LIB
ICON = Resources/images/icons/motor-usb14.icns
QT += core gui script svg xml network
INCLUDEPATH += ./GeneratedFiles \
    ./src \
    /usr/local/qwt-%qwtver/lib/qwt.framework/Headers \
    ./macosx
QMAKE_MAC_SDK = /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
QMAKE_CXXFLAGS += -std=c++11
QMAKE_LFLAGS += -Wl,-rpath,@executable_path/../Frameworks
LIBS += -F./macosx/ -framework libximc \
"./macosx/libximc.framework/Versions/*/Frameworks/libbindy.dylib" \
-F/usr/local/qwt-%qwtver/lib -framework qwt
MOC_DIR += ./GeneratedFiles
OBJECTS_DIR += ./release/obj
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles

include(XILab.pri)
