cache()

# Add more folders to ship with the application, here
folder_01.source = qml/qt-photon
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

macx{

# put SDK directory inside the same dir with source directory
# e.g.
# $ ls
# $ qt-photon sdk
# where
# $ ls sdk
# $ photon
# and
# $ ls sdk/photon
# $ Photon-MacOSX_v3-2-2-0_Cloud_SDK

  SDK = $$PWD/../sdk/photon/Photon-MacOSX_v3-2-2-0_Cloud_SDK

  INCLUDEPATH += $$SDK
  LIBS += -L$$SDK/Common-cpp/lib
  LIBS += -L$$SDK/Photon-cpp/lib
  LIBS += -L$$SDK/LoadBalancing-cpp/lib
  LIBS += -lLoadBalancing-cpp_release_macosx -lPhoton-cpp_release_macosx -lCommon-cpp_release_macosx
# debug version
#LIBS += -lLoadBalancing-cpp_debug_macosx -lPhoton-cpp_debug_macosx -lCommon-cpp_debug_macosx
  LIBS += -framework Cocoa
}

# CONFIG -= app_bundle

SOURCES += main.cpp \
    NetworkLogic.cpp \
    driver.cpp

# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    OutputListener.h \
    NetworkLogic.h \
    driver.h \
    enums.h
