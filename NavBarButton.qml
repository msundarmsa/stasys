import QtQuick 2.12

Rectangle {
    width: (logoSource != "" ? logo.width : btnText.width) + 20
    height: (logoSource != "" ? logo.height : btnText.height) + 10
    radius: 5
    color: active ? activeColor : "transparent"

    property var active: false
    property var defaultText: ""
    property var activeText: ""
    property var onClickedHandler: null
    property var logoSource: ""
    property var pressedLogoSource: ""
    property var clickedLogoSource: ""
    property var activeColor: ""
    property var activeTextColor: ""

    Image {
        id: logo
        anchors.centerIn: parent
        width: logoSource != null ? 20 : 0
        height: logoSource != null ? 20 : 0
        fillMode: Image.PreserveAspectFit
        source: pressed ? pressedLogoSource : active ? clickedLogoSource : logoSource
        mipmap: true

        property var pressed: false

        MouseArea {
            width: parent.width
            height: parent.height

            onClicked: {
                if (onClickedHandler != null) {
                    onClickedHandler();
                }
            }

            onPressed: {
                parent.pressed = true;
            }

            onReleased: {
                parent.pressed = false;
            }
        }
    }

    Text {
        id: btnText
        anchors.centerIn: parent
        text: active ? activeText : defaultText
        color: pressed ? activeColor : active ? activeTextColor : "#ffffff"
        font.family: "Segoe UI"
        font.weight: Font.DemiBold
        font.pointSize: 20

        property var pressed: false

        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (onClickedHandler != null) {
                    onClickedHandler();
                }
            }
            onPressed: {
                parent.pressed = true;
            }
            onReleased: {
                parent.pressed = false;
            }
        }
    }
}
