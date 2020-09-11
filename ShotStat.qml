import QtQuick 2.12
import QtQuick.Controls 2.15

Rectangle {
    width: (parent.width - 40) / 3
    height: parent.height
    color: "transparent"
    border.color: primaryColor
    radius: 5

    property var title: ""
    property var primaryColor: ""
    property var logoSource: ""
    property var stat: 0
    property var dp: 0
    property var suffix: ""

    Text {
        id: statTitle
        font.family: "Segoe UI"
        font.weight: Font.DemiBold
        font.pointSize: 20
        color: primaryColor
        text: title
        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 5
    }

    ProgressBar {
        id: stabilityPB
        value: stat / 100
        visible: logoSource == ""

        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.left: statTitle.right
        anchors.leftMargin: 10
        anchors.verticalCenter: statTitle.verticalCenter

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
                id: stabilityPBColor
                width: stabilityPB.visualPosition * parent.width
                height: parent.height
                radius: 2
                // bad color: "#DF6F45"
                // average color: "#DADF46"
                // good color: "#8CDF46"
                color: stat < 50 ? "#DF6F45" : stat < 70 ? "#DADF46" : "#8CDF46"
            }
        }
    }

    Image {
        visible: logoSource != ""
        source: logoSource
        height: 25
        width: 25
        mipmap: true
        fillMode: Image.PreserveAspectFit
        anchors.verticalCenter: statTitle.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 10
    }

    Text {
        id: statVal
        font.family: "Segoe UI"
        font.weight: Font.Light
        font.pointSize: 60
        anchors.top: statTitle.bottom
        anchors.topMargin: 0
        anchors.horizontalCenter: parent.horizontalCenter
        text: stat.toFixed(dp) + suffix
        color: primaryColor
    }
}
