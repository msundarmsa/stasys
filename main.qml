import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4

Window {
    visible: true
    flags: Qt.FramelessWindowHint | Qt.Window
    title: qsTr("STASYS")
    visibility: Qt.WindowFullScreen
    color: "#f0f0f2"
    FontLoader { id: segoeUILight; source: "ui/fonts/segoe-ui-light.ttf" }
    FontLoader { id: segoeUISemiBold; source: "ui/fonts/segoe-ui-semibold.ttf" }
    FontLoader { id: segoeUI; source: "ui/fonts/segoe-ui.ttf" }

    Rectangle {
        id: topBar
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
            mipmap: true
        }

        Text {
            text: "STASYS"
            color: "white"
            font.family: segoeUI.name
            font.pointSize: 25
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: logo.right
            anchors.leftMargin: 10
        }

        Text {
            id: calibrateBtn
            text: "CALIBRATE"
            color: "white"
            font.family: segoeUILight.name
            font.pointSize: 20
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: divider1.left
            anchors.rightMargin: 20
        }

        Rectangle {
            id: divider1
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: shootBtn.left
            anchors.rightMargin: 20
            width: 2
            height: parent.height - 30
        }

        Text {
            id: shootBtn
            text: "SHOOT"
            color: "white"
            font.family: segoeUILight.name
            font.pointSize: 20
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: divider2.left
            anchors.rightMargin: 20
        }

        Rectangle {
            id: divider2
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: closeBtn.left
            anchors.rightMargin: 20
            width: 2
            height: parent.height - 30
        }

        Image {
            id: closeBtn
            width: 20
            height: 20
            fillMode: Image.PreserveAspectFit
            source: "ui/images/close.svg"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20
        }
    }

    Rectangle {
        anchors.top: topBar.bottom
        anchors.left: parent.left

        width: parent.width / 2
        height: parent.height - topBar.height
        color: "transparent"

        Rectangle {
            anchors.margins: 20
            anchors.fill: parent
            color: "transparent"

            Rectangle {
                width: parent.width
                height: 120
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                color: "transparent"

                Rectangle {
                    id: stabilityLbl
                    width: (parent.width - 40) / 3
                    height: parent.height
                    anchors.left: parent.left
                    color: "#ffffff"
                    border.color: "#0000001E"

                    layer.enabled: true
                    layer.effect: DropShadow {
                        horizontalOffset: 2
                        verticalOffset: 2
                        color: "#19000000"
                    }

                    Text {
                        id: stabilityTitle
                        font.family: segoeUISemiBold.name
                        font.pointSize: 20
                        color: "#4f5859"
                        text: "STABILITY"
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.top: parent.top
                        anchors.topMargin: 5
                    }

                    ProgressBar {
                        id: stabilityPB
                        value: 0.5

                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        anchors.left: stabilityTitle.right
                        anchors.leftMargin: 10
                        anchors.verticalCenter: stabilityTitle.verticalCenter

                        background: Rectangle {
                            implicitWidth: 200
                            implicitHeight: 6
                            color: "#e6e6e6"
                            radius: 3
                        }

                        contentItem: Item {
                            implicitWidth: 200
                            implicitHeight: 4

                            Rectangle {
                                width: stabilityPB.visualPosition * parent.width
                                height: parent.height
                                radius: 2
                                // average color: "#DADF46"
                                // good color: "#8CDF46"
                                color: "#DF6F45"
                            }
                        }
                    }

                    Text {
                        font.family: segoeUILight.name
                        font.pointSize: 60
                        anchors.top: stabilityTitle.bottom
                        anchors.topMargin: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "69%"
                        color: "#04bfbf"
                    }
                }

                Rectangle {
                    id: descLbl
                    width: (parent.width - 40) / 3
                    height: parent.height
                    anchors.left: stabilityLbl.right
                    anchors.leftMargin: 20
                    color: "#ffffff"

                    layer.enabled: true
                    layer.effect: DropShadow {
                        horizontalOffset: 2
                        verticalOffset: 2
                        color: "#19000000"
                    }

                    Text {
                        id: descTitle
                        font.family: segoeUISemiBold.name
                        font.pointSize: 20
                        color: "#4f5859"
                        text: "DESC TIME"
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.top: parent.top
                        anchors.topMargin: 5
                    }

                    Image {
                        source: "ui/images/desc.svg"
                        height: 25
                        width: 25
                        fillMode: Image.PreserveAspectFit
                        anchors.verticalCenter: descTitle.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                    }

                    Text {
                        font.family: segoeUILight.name
                        font.pointSize: 60
                        anchors.top: descTitle.bottom
                        anchors.topMargin: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "3.0s"
                        color: "#04bfbf"
                    }
                }

                Rectangle {
                    id: aimLbl
                    width: (parent.width - 40) / 3
                    height: parent.height
                    anchors.left: descLbl.right
                    anchors.leftMargin: 20
                    color: "#ffffff"

                    layer.enabled: true
                    layer.effect: DropShadow {
                        horizontalOffset: 2
                        verticalOffset: 2
                        color: "#19000000"
                    }

                    Text {
                        id: aimTitle
                        font.family: segoeUISemiBold.name
                        font.pointSize: 20
                        color: "#4f5859"
                        text: "AIM TIME"
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        anchors.top: parent.top
                        anchors.topMargin: 5
                    }

                    Image {
                        source: "ui/images/aim.svg"
                        height: 25
                        width: 25
                        mipmap: true
                        fillMode: Image.PreserveAspectFit
                        anchors.verticalCenter: aimTitle.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                    }

                    Text {
                        font.family: segoeUILight.name
                        font.pointSize: 60
                        anchors.top: aimTitle.bottom
                        anchors.topMargin: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "1.5s"
                        color: "#04bfbf"
                    }
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
