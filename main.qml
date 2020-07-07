import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12

Window {
    visible: true
    flags: Qt.FramelessWindowHint | Qt.Window
    title: qsTr("STASYS")
    visibility: Qt.WindowFullScreen
    FontLoader { id: segoeUILight; source: "ui/fonts/segoe-ui-light.ttf" }
    FontLoader { id: segoUISemiBold; source: "ui/fonts/segoe-ui-semibold.ttf" }
    FontLoader { id: segoeUI; source: "ui/fonts/segoe-ui.ttf" }

    Rectangle {
        color: "#4f5859"
        width: parent.width
        height: 64

        Image {
            id: logo
            x: 20
            anchors.verticalCenter: parent.verticalCenter
            width: 30
            height: 30
            source: "ui/images/logo.svg"
            fillMode: Image.PreserveAspectFit
        }

        Text {
            text: "STASYS"
            color: "white"
            font.family: segoeUI.name
            font.pointSize: 30
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: logo.right
            anchors.leftMargin: 10
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
