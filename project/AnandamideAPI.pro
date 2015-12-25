#-------------------------------------------------
#
# Project created by QtCreator 2014-11-17T17:37:55
#
#-------------------------------------------------

QT -= core gui
QT += opengl uitools widgets

DESTDIR = ../lib/
DLLDESTDIR = ../bin/

#CONFIG+=x64
#DEFINES+=x64

DOXYGEN_BIN = $$quote(..\\..\\..\\depend\\Doxygen\\bin\\doxygen.exe)
DOXYGEN_CFG = $$quote(..\\docs\\source\\AnandamideAPI.cfg)

defineReplace(compileDoc){
    exists($${DOXYGEN_BIN}){
        exists($${DOXYGEN_CFG}){
            return ($$quote(cmd /c $$escape_expand(\")$${DOXYGEN_BIN}$$escape_expand(\") $$1 $$escape_expand(\n\t)))
        }else{
            message(NOT EXIST DOXYGEN_CFG)
        }
    }else{
        message(NOT EXIST DOXYGEN_BIN)
    }
}

BIN_DIR = $$quote(..\\bin\\)
LIB_DIR = $$quote(..\\lib\\)
#TARGET = AnandamideAPI
TEMPLATE = lib

DEFINES += ANANDAMIDEAPI_LIBRARY

INCLUDEPATH += ../include
INCLUDEPATH += ../src/Anandamide
INCLUDEPATH += ../src/Common
INCLUDEPATH += ../src/Engine
INCLUDEPATH += ../src/Math
INCLUDEPATH += ../src/Renderer
INCLUDEPATH += ../src/Misc

SOURCES += \
    ../src/Misc/Camera2d.cpp \
    ../src/Misc/Grid.cpp \
    ../src/Anandamide/Anandamide.cpp \
    ../src/Anandamide/AnandamideAction.cpp \
    ../src/Anandamide/AnandamideEditor.cpp \
    ../src/Anandamide/AnandamideEvent.cpp \
    ../src/Anandamide/AnandamideInput.cpp \
    ../src/Anandamide/AnandamideLibrary.cpp \
    ../src/Anandamide/AnandamideLogic.cpp \
    ../src/Anandamide/AnandamideNeurone.cpp \
    ../src/Anandamide/AnandamideOutput.cpp \
    ../src/Anandamide/AnandamideParameter.cpp \
    ../src/Anandamide/AnandamideScript.cpp \
    ../src/Anandamide/AnandamideVariable.cpp \
    ../src/Common/Str.cpp \
    ../src/Common/Xml.cpp \
    ../src/Math/MathCore.cpp \
    ../src/Renderer/Renderer.cpp \
    ../src/Anandamide/AnandamideStdLib.cpp \
    ../src/Anandamide/AnandamideTypeInfo.cpp \
    ../src/Anandamide/AnandamideUi.cpp

HEADERS +=\
    ../src/Misc/Camera2d.h \
    ../src/Misc/Grid.h \
    ../src/Common/Xml.h \
    ../include/AnandamideAPI.h \
    ../include/Action.h \
    ../include/Anandamide.h \
    ../include/AnandamideAction.h \
    ../include/AnandamideEditor.h \
    ../include/AnandamideEvent.h \
    ../include/AnandamideInput.h \
    ../include/AnandamideLibAPI.h \
    ../include/AnandamideLibrary.h \
    ../include/AnandamideLogic.h \
    ../include/AnandamideNeurone.h \
    ../include/AnandamideOutput.h \
    ../include/AnandamideParameter.h \
    ../include/AnandamideScript.h \
    ../include/AnandamideStdLib.h \
    ../include/AnandamideVariable.h \
    ../include/Array.h \
    ../include/Map.h \
    ../include/MathCore.h \
    ../include/Str.h \
    ../include/Renderer.h \
    ../include/AnandamideTypeInfo.h \
    ../include/AnandamideDocumentation.h \
    ../include/Resource.h \
    ../include/AnandamideUi.h

QMAKE_CXXFLAGS += -std=c++1v

build_pass:CONFIG(debug, debug|release) {

    DEFINES += DEBUG

    build_pass:CONFIG(x64, x64|x32) {
        TARGET = AnandamideAPI_x64d
    } else:build_pass {
        TARGET = AnandamideAPI_x86d
    }
    LIBS += -lopengl32

    message($${TARGET})
} else:build_pass {
    #QMAKE_CXXFLAGS += /Ot /Oy- /GL /MP
    build_pass:CONFIG(x64, x64|x32) {
        TARGET = AnandamideAPI_x64
    } else:build_pass {
        TARGET = AnandamideAPI_x86
    }
    LIBS += -lopengl32

    message($${TARGET})
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    resources.qrc
