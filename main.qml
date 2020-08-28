import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import QtCharts 2.3
import Qt.labs.qmlmodels 1.0
import QtMultimedia 5.12

import com.mrmmsmsa 1.0

Window {
    visible: true
    flags: Qt.FramelessWindowHint | Qt.Window
    title: qsTr("STASYS")
    visibility: Qt.WindowFullScreen
    color: "#f0f0f2"
    FontLoader { id: segoeUILight; source: "ui/fonts/segoe-ui-light.ttf" }
    FontLoader { id: segoeUISemiBold; source: "ui/fonts/segoe-ui-semibold.ttf" }
    FontLoader { id: segoeUI; source: "ui/fonts/segoe-ui.ttf" }

    SoundEffect {
        id: calibrationDoneSound
        source: "ui/sounds/done.wav"
    }

    QMLCppBridge {
        id: qmlCppBridge
        onCalibrationCompleted: {
            calibrationDoneSound.play();
            calibrateBtn.text = "CALIBRATE";
            if (success) {
                toast.show("Calibration finished successfully!");
            } else {
                toast.show("Calibration could not complete. Please try again");
            }
        }

        onUiRemovePreviousCalibCircle: {
            // TODO: Implement when adding fine calibration
        }

        onUiClearTrace: {
            targetTrace.resetTrace(resetGroupIfNecessary);
        }

        onUiUpdateView: {
            stabilityLbl.setStab(stab);
            descLbl.setDesc(desc);
            aimLbl.setAim(aim);

            shotLogList.model.append({score: score, stab: stab, desc: desc, aim: aim});
            shotGroupList.addShot(x, y);

            xtYtChart.updateXtYt(xt, yt, ts);
        }

        onUiAddToBeforeShotTrace: {
            targetTrace.addToBeforeShotTrace(x, y);
        }

        onUiDrawShotCircle: {
            targetTrace.drawShotCircle(x, y);
        }

        onUiAddToAfterShotTrace: {
            targetTrace.addToAfterShotTrace(x, y);
        }
    }

    ToastManager {
        id: toast
    }

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

            MouseArea {
                width: parent.width
                height: parent.height

                onClicked: {
                    calibrateBtn.text = "CALIBRATING";
                    qmlCppBridge.calibrationClicked();
                }
            }
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

            MouseArea {
                width: parent.width
                height: parent.height

                onClicked: {
                    shootBtn.text = "SHOOTING"
                    qmlCppBridge.shootClicked();
                }
            }
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

            MouseArea {
                width: parent.width
                height: parent.height

                onClicked: {
                    Qt.quit();
                }
            }
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
                        id: targetTrace
                        width: parent.width
                        height: parent.height
                        property var beforeShotTrace: []
                        property var afterShotTrace: []
                        property var shotPoints: []
                        property var factor: width / 170
                        property var radius: factor * 2.25

                        function transformPoint(x, y) {
                            return {x: x * factor + targetTrace.width / 2, y: targetTrace.height / 2 - y * factor};
                        }

                        function addToBeforeShotTrace(x, y) {
                            beforeShotTrace.push(transformPoint(x, y));
                            targetTrace.requestPaint();
                        }

                        function addToAfterShotTrace(x, y) {
                            afterShotTrace.push(transformPoint(x, y));
                            targetTrace.requestPaint();
                        }

                        function drawShotCircle(x, y) {
                            shotPoints.push(transformPoint(x, y));
                            targetTrace.requestPaint();
                        }

                        function resetTrace(resetGroupIfNecessary) {
                            beforeShotTrace = [];
                            afterShotTrace = [];

                            if (resetGroupIfNecessary && shotPoints.length == 10) {
                                shotPoints = [];
                            }

                            targetTrace.requestPaint();
                        }

                        onPaint: {
                            let ctx = getContext("2d");
                            ctx.reset();

                            if (beforeShotTrace.length > 0) {
                                ctx.strokeStyle = "#8ddf46";
                                ctx.lineWidth = 2;

                                ctx.beginPath();
                                ctx.moveTo(beforeShotTrace[0]["x"], beforeShotTrace[0]["y"]);
                                for (let i = 1; i < beforeShotTrace.length; i++) {
                                    ctx.lineTo(beforeShotTrace[i]["x"], beforeShotTrace[i]["y"]);
                                    // TODO: change this to bezier curve or some other interpolation
                                }
                                ctx.stroke();
                            }

                            if (afterShotTrace.length > 0) {
                                ctx.strokeStyle = "#df6f46";
                                ctx.lineWidth = 2;
                                ctx.beginPath();
                                ctx.moveTo(afterShotTrace[0]["x"], afterShotTrace[0]["y"]);
                                for (let i = 1; i < afterShotTrace.length; i++) {
                                    ctx.lineTo(afterShotTrace[i]["x"], afterShotTrace[i]["y"]);
                                    // TODO: change this to bezier curve or some other interpolation
                                }
                                ctx.stroke();
                            }

                            ctx.strokeStyle = "#ffffff";
                            ctx.lineWidth = 3;
                            ctx.fillStyle = "#04bfbf";
                            for (let i = 0; i < shotPoints.length; i++) {
                                ctx.beginPath();
                                ctx.ellipse(shotPoints[i]["x"] - radius, shotPoints[i]["y"] - radius, 2 * radius, 2 * radius);
                                ctx.stroke();
                                ctx.fill();
                            }
                        }
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

                    function setStab(stab) {
                        stab = stab.toFixed(0);
                        if (stab < 50) {
                            stabilityPBColor.color = "#DF6F45";
                        } else if (stab < 70) {
                            stabilityPBColor.color = "#DADF46";
                        } else {
                            stabilityPBColor.color = "#8CDF46";
                        }

                        stabilityPB.value = stab / 100;
                        stabilityVal.text = stab + "%";
                    }

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
                        value: 0

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
                                id: stabilityPBColor
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
                        id: stabilityVal
                        font.family: segoeUILight.name
                        font.pointSize: 60
                        anchors.top: stabilityTitle.bottom
                        anchors.topMargin: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "0%"
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

                    function setDesc(desc) {
                        desc = desc.toFixed(1);
                        descVal.text = desc + "s";
                    }

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
                        id: descVal
                        font.family: segoeUILight.name
                        font.pointSize: 60
                        anchors.top: descTitle.bottom
                        anchors.topMargin: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "0.0s"
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

                    function setAim(aim) {
                        aim = aim.toFixed(1);
                        aimVal.text = aim + "s";
                    }

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
                        id: aimVal
                        font.family: segoeUILight.name
                        font.pointSize: 60
                        anchors.top: aimTitle.bottom
                        anchors.topMargin: 0
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "0.0s"
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
                        model: ShotGroupListModel {}
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        spacing: 20
                        property var shotCount: 0;

                        function addShot(x, y) {
                            shotCount += 1;
                            if (shotCount != 1 && shotCount % 10 == 1) {
                                model.append({inPoints: [], outPoints: []});
                            }
                            currentIndex = count - 1
                            currentItem.addShot(x, y);
                        }

                        onCountChanged: {
                            currentIndex = count - 1
                        }

                        delegate: ItemDelegate {
                            width: shotGroupList.width - 10
                            height: width
                            anchors.horizontalCenter: parent.horizontalCenter
                            property var factor: (width / 2) / 29.75 // convert mm to px

                            function addShot(x, y) {
                                if (Math.sqrt(x * x + y * y) <= 29.75) {
                                    // within black circle
                                    inPoints.append({x: x * factor + width / 2, y: height / 2 - y * factor});
                                } else {
                                    var angle = Math.atan2(y, x);
                                    var newX = 29.75 * Math.cos(angle);
                                    var newY = 29.75 * Math.sin(angle);
                                    outPoints.append({x: newX * factor + width / 2, y: height / 2 - newY * factor});
                                }

                                zoomedShotCanvas.requestPaint();
                            }

                            Canvas {
                                id: zoomedShotCanvas
                                width: parent.width
                                height: parent.height

                                property var radius: parent.factor * 2.25;

                                onPaint: {
                                    var context = getContext("2d");
                                    context.strokeStyle = "#ffffff";
                                    context.lineWidth = 3;

                                    context.fillStyle = "#04bfbf";
                                    for (let i = 0; i < inPoints.count; i++)
                                        drawShot(context, inPoints.get(i));

                                    context.fillStyle = "#df468e";
                                    for (let i = 0; i < outPoints.count; i++)
                                        drawShot(context, outPoints.get(i));
                                }

                                function drawShot(context, point) {
                                    context.beginPath();
                                    context.ellipse(point["x"] - radius, point["y"] - radius, 2 * radius, 2 * radius);
                                    context.stroke();
                                    context.fill();
                                }

                                MouseArea {
                                    width: parent.width
                                    height: parent.height

                                    onClicked: {
                                        return;
                                    }
                                }
                            }

                            Image {
                                source: "ui/images/zoomedTarget.png"
                                width: parent.width
                                height: parent.height
                                mipmap: true
                                z: -1
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
                        model: ShotLogListModel {}
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        spacing: 20

                        onCountChanged: {
                            var newIndex = count - 1
                            currentIndex = newIndex
                        }

                        delegate: ItemDelegate {
                            width: shotLogList.width - 10
                            height: 75

                            Rectangle {
                                height: parent.height
                                width: parent.width
                                color: "#ffffff"

                                Text {
                                    id: logScoreLbl
                                    text: score.toFixed(1)
                                    color: "#04bfbf"
                                    font.family: segoeUILight.name
                                    font.pointSize: 30
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 20
                                }

                                ProgressBar {
                                    id: logStab
                                    value: stab / 100

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
                                            color: if (stab < 50) { "#DF6F45" } else if (stab < 70) { "#DADF46" } else { "#8CDF46" }
                                        }
                                    }
                                }

                                Text {
                                    id: logDescLbl
                                    text: desc.toFixed(1) + "s"
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
                                    text: aim.toFixed(1) + "s"
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
                    id: xtYtChart
                    title: ""
                    x: -12
                    y: -12
                    height: parent.height + 24
                    width: parent.width + 24
                    anchors.margins: 0
                    antialiasing: true

                    function updateXtYt(xt, yt, ts) {
                        this.removeAllSeries();
                        var xtSeries = this.createSeries(ChartView.SeriesTypeLine, "X", xAxis, yAxis);
                        var ytSeries = this.createSeries(ChartView.SeriesTypeLine, "Y", xAxis, yAxis);

                        for (let i = 0; i < ts.length; i++) {
                            xtSeries.append(ts[i], xt[i]);
                            ytSeries.append(ts[i], yt[i]);
                        }
                    }

                    ValueAxis {
                        id: xAxis
                        min: -0.5
                        max: 0.5
                    }

                    ValueAxis {
                        id: yAxis
                        min: -29.75
                        max: 29.75
                    }

                    LineSeries {
                        name: "X"
                        axisX: xAxis
                        axisY: yAxis
                    }

                    LineSeries {
                        name: "Y"
                        axisX: xAxis
                        axisY: yAxis
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
