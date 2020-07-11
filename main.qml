import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import QtCharts 2.3
import Qt.labs.qmlmodels 1.0

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
            id: leftContent
            anchors.margins: 20
            anchors.fill: parent
            color: "transparent"

            Rectangle {
                width: parent.width
                height: parent.height - 120
                anchors.top: leftContent.top
                anchors.left: leftContent.left
                color: "transparent"

                Rectangle {
                    width: parent.width < parent.height - 20 ? parent.width : parent.height - 20
                    height: width
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter

                    Image {
                        width: parent.width
                        height: parent.height
                        source: "ui/images/target.png"
                        mipmap: true
                        fillMode: Image.PreserveAspectFit
                    }

                    Canvas {
                        width: parent.width
                        height: parent.height
                        /* onPaint: {
                            var ctx = getContext("2d");
                            ctx.strokeStyle = Qt.rgba(1, 0, 0, 1);
                            ctx.lineWidth = 1;
                            ctx.beginPath();
                            ctx.moveTo(20, 0);//start point
                            ctx.bezierCurveTo(-10, 90, 210, 90, 180, 0);
                            ctx.stroke();
                        } */
                    }
                }
            }

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

    Rectangle {
        anchors.top: topBar.bottom
        anchors.right: parent.right

        width: parent.width / 2
        height: parent.height - topBar.height
        color: "transparent"

        Rectangle {
            id: rightContent
            anchors.margins: 20
            anchors.fill: parent
            color: "transparent"

            Rectangle {
                // zoomed target
                id: zoomedTargetRect
                width: parent.width / 3
                height: parent.height * 2 / 3
                anchors.left: parent.left
                anchors.top: parent.top
                color: "#ffffff"

                ScrollView {
                    width: parent.width
                    height: parent.height
                    anchors.top: parent.top
                    anchors.left: parent.left
                    clip: true

                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy: ScrollBar.AlwaysOn

                    ListView {
                        id: shotGroupList
                        verticalLayoutDirection: ListView.BottomToTop
                        model: 0
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        spacing: 20

                        delegate: ItemDelegate {
                            width: shotGroupList.width - 10
                            height: width
                            anchors.horizontalCenter: parent.horizontalCenter

                            Image {
                                source: "ui/images/zoomedTarget.png"
                                width: parent.width
                                height: width
                                mipmap: true
                            }
                        }
                    }
                }
            }

            Rectangle {
                // shot log
                id: shotLogRect
                width: parent.width * 2 / 3
                height: parent.height * 2 / 3
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.left: zoomedTargetRect.right
                anchors.leftMargin: 10
                color: "transparent"

                ScrollView {
                    width: parent.width
                    height: parent.height
                    anchors.top: parent.top
                    anchors.left: parent.left
                    clip: true

                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy: ScrollBar.AlwaysOn

                    ListView {
                        id: shotLogList
                        verticalLayoutDirection: ListView.BottomToTop
                        model: 10
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        spacing: 20

                        delegate: ItemDelegate {
                            width: shotLogList.width - 10
                            height: 75

                            Rectangle {
                                height: parent.height
                                width: parent.width
                                color: "#ffffff"

                                Text {
                                    id: logScoreLbl
                                    text: "7.0"
                                    color: "#04bfbf"
                                    font.family: segoeUILight.name
                                    font.pointSize: 30
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 20
                                }

                                ProgressBar {
                                    id: logStab
                                    value: 0.5

                                    anchors.right: logDescLbl.left
                                    anchors.rightMargin: 10
                                    anchors.left: logScoreLbl.right
                                    anchors.leftMargin: 10
                                    anchors.verticalCenter: parent.verticalCenter

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
                                            width: logStab.visualPosition * parent.width
                                            height: parent.height
                                            radius: 2
                                            // average color: "#DADF46"
                                            // good color: "#8CDF46"
                                            color: "#DF6F45"
                                        }
                                    }
                                }

                                Text {
                                    id: logDescLbl
                                    text: "3.0s"
                                    color: "#04bfbf"
                                    font.family: segoeUILight.name
                                    font.pointSize: 25
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: logDescIcon.left
                                    anchors.rightMargin: 10
                                }

                                Image {
                                    id: logDescIcon
                                    source: "ui/images/desc.svg"
                                    height: 30
                                    width: height
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: logAimLbl.left
                                    anchors.rightMargin: 10
                                    mipmap: true
                                }

                                Text {
                                    id: logAimLbl
                                    text: "1.2s"
                                    color: "#04bfbf"
                                    font.family: segoeUILight.name
                                    font.pointSize: 25
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: logAimIcon.left
                                    anchors.rightMargin: 10
                                }

                                Image {
                                    id: logAimIcon
                                    source: "ui/images/aim.svg"
                                    height: 30
                                    width: height
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: parent.right
                                    anchors.rightMargin: 20
                                    mipmap: true
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                // x-t/y-t chart
                id: chartRect
                width: parent.width
                height: parent.height / 3
                anchors.bottom: parent.bottom
                anchors.top: shotLogRect.bottom
                anchors.topMargin: 10
                color: "transparent"

                ChartView {
                    title: ""
                    x: -12
                    y: -12
                    height: parent.height + 24
                    width: parent.width + 24
                    anchors.margins: 0
                    antialiasing: true

                    ValueAxis {
                        id: xAxis
                        min: -0.5
                        max: 0.5
                    }

                    ValueAxis {
                        id: yAxis
                        min: -2.975
                        max: 2.975
                    }

                    LineSeries {
                        name: "X-T"
                        axisX: xAxis
                        axisY: yAxis
                        XYPoint { x: -0.5; y: 3.3 }
                        XYPoint { x: -0.4; y: 4.9 }
                        XYPoint { x: -0.3; y: 2.1 }
                        XYPoint { x: -0.2; y: 3.3 }
                        XYPoint { x: -0.1; y: 2.1 }
                        XYPoint { x: 0; y: 0 }
                        XYPoint { x: 0.1; y: 2.1 }
                        XYPoint { x: 0.2; y: 3.3 }
                        XYPoint { x: 0.3; y: 2.1 }
                        XYPoint { x: 0.4; y: 4.9 }
                        XYPoint { x: 0.5; y: 3.3 }
                    }

                    LineSeries {
                        name: "Y-T"
                        axisX: xAxis
                        axisY: yAxis
                        XYPoint { x: -0.5; y: 4.9 }
                        XYPoint { x: -0.4; y: 2.1 }
                        XYPoint { x: -0.3; y: 3.3 }
                        XYPoint { x: -0.2; y: 2.1 }
                        XYPoint { x: -0.1; y: 1.5 }
                        XYPoint { x: 0; y: 0 }
                        XYPoint { x: 0.1; y: 1.5 }
                        XYPoint { x: 0.2; y: 2.1 }
                        XYPoint { x: 0.3; y: 3.3 }
                        XYPoint { x: 0.4; y: 2.1 }
                        XYPoint { x: 0.5; y: 4.9 }
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
