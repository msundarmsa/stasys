QT += quick widgets multimedia svg

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += /usr/local/Cellar/opencv/4.5.1_2/include/opencv4/
LIBS += /usr/local/Cellar/opencv/4.5.1_2/lib/libopencv_core.dylib
LIBS += /usr/local/Cellar/opencv/4.5.1_2/lib/libopencv_highgui.dylib
LIBS += /usr/local/Cellar/opencv/4.5.1_2/lib/libopencv_imgcodecs.dylib
LIBS += /usr/local/Cellar/opencv/4.5.1_2/lib/libopencv_imgproc.dylib
LIBS += /usr/local/Cellar/opencv/4.5.1_2/lib/libopencv_features2d.dylib
LIBS += /usr/local/Cellar/opencv/4.5.1_2/lib/libopencv_calib3d.dylib
LIBS += /usr/local/Cellar/opencv/4.5.1_2/lib/libopencv_videoio.dylib

INCLUDEPATH += /usr/local/Cellar/sfml/2.5.1_1/include/
LIBS += /usr/local/Cellar/sfml/2.5.1_1/lib/libsfml-audio.dylib
LIBS += /usr/local/Cellar/sfml/2.5.1_1/lib/libsfml-system.dylib

INCLUDEPATH += /usr/local/Cellar/portaudio/HEAD-b14e866/include/
LIBS += /usr/local/Cellar/portaudio/HEAD-b14e866/lib/libportaudio.dylib
LIBS += /usr/local/Cellar/portaudio/HEAD-b14e866/lib/libportaudiocpp.dylib

SOURCES += \
        RecordThread.cpp \
        ShootThread.cpp \
        Shot.cpp \
        ShotTrace.cpp \
        SoundPressureSensor.cpp \
        SystemClock.cpp \
        cvcamera.cpp \
        main.cpp \
        qmlcppbridge.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    CalibrationThread.h \
    MicThread.h \
    RecordThread.h \
    ShootThread.h \
    Shot.h \
    ShotTrace.h \
    SystemClock.h \
    Utils.h \
    cvcamera.h \
    qmlcppbridge.h \
    spline.h

DISTFILES += \
    NavBarButton.qml \
    ShotStat.qml \
    Toast.qml \
    ToastManager.qml
