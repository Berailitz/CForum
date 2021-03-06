QT += quick websockets concurrent
CONFIG += c++17
QMAKE_CXXFLAGS += /std:c++17 /utf-8

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    board.cpp \
    cforum.cpp \
    user.cpp \
    cfcommon.cpp \
    admin.cpp \
    comment.cpp \
    guest.cpp \
    moderator.cpp \
    normal_user.cpp \
    post.cpp \
    error_handler.cpp \
    client_state.cpp \
    message.cpp \
    request_message.cpp \
    response_message.cpp \
    server_main.cpp \
    forum_server.cpp \
    client_descriptor.cpp

RESOURCES += server_qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    board.h \
    cforum.h \
    user.h \
    cfcommon.h \
    admin.h \
    comment.h \
    guest.h \
    moderator.h \
    normal_user.h \
    post.h \
    error_handler.h \
    client_state.h \
    message.h \
    request_message.h \
    response_message.h \
    forum_server.h \
    client_descriptor.h
