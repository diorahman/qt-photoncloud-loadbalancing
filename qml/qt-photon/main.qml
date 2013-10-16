import QtQuick 1.1

Rectangle {
    width: 360
    height: 360

    Text {
        id: labelState
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }

    Connections{
        target: Driver
        onStateChanged: {
            console.log(logicState)
            labelState.text = logicState
        }
    }
}
