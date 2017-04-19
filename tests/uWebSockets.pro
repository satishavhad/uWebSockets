TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    ../src/Hub.cpp \
    ../src/WebSocket.cpp \
    ../src/HTTPSocket.cpp \
    ../src/Group.cpp \
    ../src/Extensions.cpp \
    ../uSockets/Berkeley.cpp \
    ../uSockets/Epoll.cpp

HEADERS += ../src/WebSocketProtocol.h \
    ../src/WebSocket.h \
    ../src/Hub.h \
    ../src/Group.h \
    ../src/HTTPSocket.h \
    ../src/uWS.h \
    ../src/Extensions.h \
    ../uSockets/Berkeley.h \
    ../uSockets/Epoll.h \
    ../src/uSockets.h

LIBS += -lssl -lcrypto -lz -lpthread -luv -lboost_system

QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3 -g

INCLUDEPATH += ../src
