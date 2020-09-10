import QtQuick 2.12

Rectangle {
    width: (logoSource != "" ? logo.width : btnText.width) + 20
    height: (logoSource != "" ? logo.height : btnText.height) + 10
    radius: 5
    color: active ? "#ef476f" : "transparent"

    property var active: false
    property var defaultText: ""
    property var activeText: ""
    property var onClickedHandler: null
    property var logoSource: ""
    property var pressedLogoSource: ""

    Image {
        id: logo
        anchors.centerIn: parent
        width: logoSource != null ? 20 : 0
        height: logoSource != null ? 20 : 0
        fillMode: Image.PreserveAspectFit
        source: logoSource
        mipmap: true

        MouseArea {
            width: parent.width
            height: parent.height

            onClicked: {
                if (onClickedHandler != null) {
                    onClickedHandler();
                }
            }

            onPressed: {
                parent.source = pressedLogoSource;
            }

            onReleased: {
                parent.source = logoSource;
            }
        }
    }

    Text {
        id: btnText
        anchors.centerIn: parent
        text: active ? activeText : defaultText
        color: "#ffffff"
        font.family: "Segoe UI"
        font.weight: Font.DemiBold
        font.pointSize: 20

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (onClickedHandler != null) {
                    onClickedHandler();
                }
            }

            onPressed: {
                parent.color = "#bebebe"
            }
            onReleased: {
                parent.color = "#ffffff"
            }
        }
    }
}
