import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtGraphicalEffects 1.0
import QtQuick.Controls.Styles 1.4
import QtCharts 2.3
import Qt.labs.qmlmodels 1.0
import QtMultimedia 5.12
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.12

import com.mrmmsmsa 1.0

Window {
    id: window
    visible: true
    flags: Qt.FramelessWindowHint | Qt.Window
    title: qsTr("STASYS v1.0.0a")
    visibility: Qt.WindowFullScreen
    color: primaryColor
    FontLoader { source: "ui/fonts/segoe-ui-light.ttf" }
    FontLoader { source: "ui/fonts/segoe-ui-semibold.ttf" }
    FontLoader { source: "ui/fonts/segoe-ui.ttf" }

    property var primaryColor: "#464646"
    property var secondaryColor: "#D7EC58"
    property var tertiaryColor: "#FF1493"
    property var quaternaryColor: "#000000"
    property var accentColor1: "#10e2e6"
    property var accentColor2: "#DF2935"

    property var circles: [
        { radius:311/680, fill:"", border:"#D7EC58" },
        { radius:279/680, fill:"", border:"#D7EC58" },
        { radius:247/680, fill:"", border:"#D7EC58" },
        { radius:43/136, fill:"", border:"#D7EC58" },
        { radius:183/680, fill:"", border:"#D7EC58" },
        { radius:151/680, fill:"", border:"#D7EC58" },
        { radius:7/40, fill:"#D7EC58", border:"" },
        { radius:87/680, fill:"#D7EC59", border:"#464646" },
        { radius:11/136, fill:"#D7EC60", border:"#464646" },
        { radius:23/680, fill:"#D7EC61", border:"#464646" },
        { radius:1/68, fill:"#D7EC62", border:"#464646" }
    ]

    property var circleNumbers: [
        { text: "1", x:   1/2  , y: 127/136, color: "#D7EC58" },
        { text: "1", x:   1/2  , y:   9/136, color: "#D7EC58" },
        { text: "1", x: 127/136, y:   1/2  , color: "#D7EC58" },
        { text: "1", x:   9/136, y:   1/2  , color: "#D7EC58" },
        { text: "2", x:   1/2  , y: 603/680, color: "#D7EC58" },
        { text: "2", x:   1/2  , y:  77/680, color: "#D7EC58" },
        { text: "2", x: 603/680, y:   1/2  , color: "#D7EC58" },
        { text: "2", x:  77/680, y:   1/2  , color: "#D7EC58" },
        { text: "3", x:   1/2  , y: 571/680, color: "#D7EC58" },
        { text: "3", x:   1/2  , y: 109/680, color: "#D7EC58" },
        { text: "3", x: 571/680, y:   1/2  , color: "#D7EC58" },
        { text: "3", x: 109/680, y:   1/2  , color: "#D7EC58" },
        { text: "4", x:   1/2  , y: 539/680, color: "#D7EC58" },
        { text: "4", x:   1/2  , y: 141/680, color: "#D7EC58" },
        { text: "4", x: 539/680, y:   1/2  , color: "#D7EC58" },
        { text: "4", x: 141/680, y:   1/2  , color: "#D7EC58" },
        { text: "5", x:   1/2  , y: 507/680, color: "#D7EC58" },
        { text: "5", x:   1/2  , y: 173/680, color: "#D7EC58" },
        { text: "5", x: 507/680, y:   1/2  , color: "#D7EC58" },
        { text: "5", x: 173/680, y:   1/2  , color: "#D7EC58" },
        { text: "6", x:   1/2  , y:  95/136, color: "#D7EC58" },
        { text: "6", x:   1/2  , y:  41/136, color: "#D7EC58" },
        { text: "6", x:  95/136, y:   1/2  , color: "#D7EC58" },
        { text: "6", x:  41/136, y:   1/2  , color: "#D7EC58" },
        { text: "7", x:   1/2  , y: 443/680, color: "#464646" },
        { text: "7", x:   1/2  , y: 237/680, color: "#464646" },
        { text: "7", x: 443/680, y:   1/2  , color: "#464646" },
        { text: "7", x: 237/680, y:   1/2  , color: "#464646" },
        { text: "8", x:   1/2  , y: 411/680, color: "#464646" },
        { text: "8", x:   1/2  , y: 269/680, color: "#464646" },
        { text: "8", x: 411/680, y:   1/2  , color: "#464646" },
        { text: "8", x: 269/680, y:   1/2  , color: "#464646" }
    ]

    Component.onCompleted: {
        /*cameraMicError.open();
        targetTrace.resetTrace(true);
        targetTrace.drawShotCircle(0,0);
        shotGroupList.addShot(0,0);
        targetTrace.resetTrace(true);
        targetTrace.drawShotCircle(10,35);
        shotGroupList.addShot(10,35);
        targetTrace.resetTrace(true);
        targetTrace.drawShotCircle(-1,2.5);
        shotGroupList.addShot(-1,2.5);
        targetTrace.resetTrace(true);
        targetTrace.drawShotCircle(-10,35);
        shotGroupList.addShot(-10,35);
        targetTrace.resetTrace(true);
        targetTrace.drawShotCircle(-45,0);
        shotGroupList.addShot(-45,0);
        targetTrace.resetTrace(true);
        targetTrace.drawShotCircle(45,0);
        shotGroupList.addShot(45,0);
        targetTrace.resetTrace(true);
        targetTrace.drawShotCircle(10,-35);
        shotGroupList.addShot(10,-35);
        targetTrace.resetTrace(true);
        targetTrace.drawShotCircle(11,-35);
        shotGroupList.addShot(11,-35);
        targetTrace.resetTrace(true);
        targetTrace.drawShotCircle(12,-35);
        shotGroupList.addShot(12,-35);
        targetTrace.resetTrace(true);
        targetTrace.drawShotCircle(13,-35);
        shotGroupList.addShot(13,-35);
        targetTrace.resetTrace(true);
        targetTrace.drawShotCircle(14,-35);
        shotGroupList.addShot(14,-35);

        qmlCppBridge.uiUpdateView(1, 10.0, 75, 3.6, 5.2, "top", [], [], []);
        qmlCppBridge.uiUpdateView(2, 9.5, 82.5, 20, 60, "top_right", [10, 20, 30], [3, 2, 1], [-1, 0, 1]);
        qmlCppBridge.uiUpdateView(3, 10.0, 75, 3.6, 5.2, "right", [], [], []);
        qmlCppBridge.uiUpdateView(4, 9.5, 82.5, 2, 6, "bottom_right", [10, 20, 30], [3, 2, 1], [-1, 0, 1]);
        qmlCppBridge.uiUpdateView(5, 10.0, 75, 3.6, 5.2, "bottom", [], [], []);
        qmlCppBridge.uiUpdateView(6, 9.5, 82.5, 2, 6, "bottom_left", [10, 20, 30], [3, 2, 1], [-1, 0, 1]);
        qmlCppBridge.uiUpdateView(7, 10.0, 75, 3.6, 5.2, "left", [], [], []);
        qmlCppBridge.uiUpdateView(8, 9.5, 82.5, 2, 6, "top_left", [10, 20, 30], [3, 2, 1], [-1, 0, 1]);
        qmlCppBridge.uiUpdateView(9, 10.0, 75, 3.6, 5.2, "top", [], [], []);
        qmlCppBridge.uiUpdateView(10, 9.5, 82.5, 2, 6, "top", [10, 20, 30], [3, 2, 1], [-1, 0, 1]);*/
    }

    SoundEffect {
        id: calibrationDoneSound
        source: "ui/sounds/done.wav"
    }

    Popup {
        id: cameraMicError
        x: (window.width - width) / 2
        y: (window.height - height) / 2
        height: errorContent.height + 40
        width: errorContent.width + 40
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Rectangle {
            anchors.fill: parent
            color: primaryColor
            border.color: secondaryColor
            radius: 10
        }

        ColumnLayout {
            id: errorContent
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20

            Image {
                id: webcamImg
                Layout.preferredHeight: 50
                Layout.preferredWidth: 50
                mipmap: true
                Layout.alignment: Qt.AlignCenter
                source: "ui/images/webcam.svg"
            }

            Text {
                id: errorText
                Layout.alignment: Qt.AlignCenter
                font.family: "Segoe UI"
                font.pointSize: 20
                horizontalAlignment: Text.AlignHCenter
                text: "STASYS Camera was not detected.\nPlease plug the camera in and try again."
                color: secondaryColor
            }

            NavBarButton {
                defaultText: "OK"
                activeText: "OK"
                onClickedHandler: cameraMicError.close
                activeColor: secondaryColor
                activeTextColor: primaryColor
                Layout.alignment: Qt.AlignCenter
            }
        }
    }

    Popup {
        id: settingsDialog
        x: (window.width - width) / 2
        y: (window.height - height) / 2
        height: 500
        width: 500
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        property var micOptions: []
        property var defaultMic: ""
        property var cameraOptions: []
        property var defaultCamera: 0

        onClosed: {
            micChart.reset();
            qmlCppBridge.settingsClosed();
            settingsBtn.active = false;
        }

        ColumnLayout {
            width: parent.width
            height: parent.height
            RowLayout {
                id: micSelectRect
                width: parent.width
                Text {
                    id: micText
                    text: "Microphone: "
                }

                ComboBox {
                    id: micSelect
                    Layout.fillWidth: true
                    model: settingsDialog.micOptions
                    onCurrentValueChanged: {
                        qmlCppBridge.micChanged(currentValue);
                    }
                }
            }

            ChartView {
                id: micChart
                title: ""
                Layout.preferredWidth: parent.width
                Layout.preferredHeight: 300
                antialiasing: true
                property var dBs: []
                property var dBSeries: null
                property var thresholdSeries: null

                function reset() {
                    dBs = [];
                }

                function updateMicChart(dB) {
                    if (dBSeries != null) {
                        this.removeSeries(dBSeries);
                    }

                    if (dBs.length < 60) {
                        dBs.push(dB);
                    } else {
                        dBs.push(dB);
                        dBs.shift();
                    }

                    dBSeries = this.createSeries(ChartView.SeriesTypeLine, "dB", micChartxAxis, micChartyAxis);

                    for (let i = 0; i < dBs.length; i++) {
                        dBSeries.append(i, dBs[i]);
                    }
                }

                function setThreshold(threshold) {
                    if (thresholdSeries != null) {
                        this.removeSeries(thresholdSeries);
                    }

                    thresholdSeries = this.createSeries(ChartView.SeriesTypeLine, "threshold", micChartxAxis, micChartyAxis);
                    for (let i = 0; i < micChartxAxis.max; i++) {
                        thresholdSeries.append(i, threshold);
                    }
                }

                ValueAxis {
                    id: micChartxAxis
                    min: 0
                    max: 60
                }

                ValueAxis {
                    id: micChartyAxis
                    min: 0
                    max: 100
                }
            }

            RowLayout {
                id: thresholdRect
                width: parent.width
                Text {
                    id: thresholdText
                    text: "Threshold: " + thresholdSlider.value.toFixed(0);
                }

                Slider {
                    id: thresholdSlider
                    Layout.fillWidth: true
                    from: 0
                    to: 100
                    stepSize: 1

                    onValueChanged: {
                        micChart.setThreshold(value);
                        qmlCppBridge.micThresholdChanged(value);
                    }
                }
            }

            RowLayout {
                id: cameraSelectRect
                width: parent.width
                Text {
                    id: cameraText
                    text: "Camera: "
                }

                ComboBox {
                    id: cameraSelect
                    Layout.fillWidth: true
                    model: settingsDialog.cameraOptions
                    currentIndex: settingsDialog.defaultCamera

                    onCurrentIndexChanged: {
                        qmlCppBridge.cameraChanged(currentIndex);
                    }
                }
            }

            RowLayout {
                width: parent.width
                Text {
                    text: "Up/Down Detection: "
                }

                CheckBox {
                    id: upDownDetectionCheckBox
                    checked: true

                    onCheckedChanged: {
                        qmlCppBridge.upDownDetectionChanged(checked);
                    }
                }
            }
        }
    }

    QMLCppBridge {
        id: qmlCppBridge

        onUiCalibrationStarted: {
            calibrateBtn.active = true;
        }

        onUiCalibrationEnded: {
            calibrationDoneSound.play();
            calibrateBtn.active = false;
            if (success) {
                toast.show("Calibration finished successfully!", 3000);
            } else {
                toast.show("Calibration could not complete. Please try again", 3000);
            }
        }

        onUiShootingStarted: {
            shootBtn.active = true;
        }

        onUiShootingEnded: {
            shootBtn.active = false;
            targetTrace.resetTrace(false);
            targetTrace.requestPaint();
        }

        onUiShowToast: {
            toast.show(message, 3000);
        }

        onUiRemovePreviousCalibCircle: {
            targetTrace.clearCalibration();
        }

        onUiClearTrace: {
            targetTrace.resetTrace(resetGroupIfNecessary);
        }

        onUiUpdateView: {
            stabilityLbl.stat = stab;
            descLbl.stat = desc;
            aimLbl.stat = aim;

            shotLogList.model.insert(0, {sn: sn, score: score, stab: stab, desc: desc, aim: aim, angle: angle});

            xtYtChart.updateXtYt(xt, yt, ts);
        }

        onUiAddToBeforeShotTrace: {
            targetTrace.addToBeforeShotTrace(x, y);
        }

        onUiDrawShotCircle: {
            targetTrace.drawShotCircle(x, y);
            shotGroupList.addShot(x, y);
        }

        onUiAddToAfterShotTrace: {
            targetTrace.addToAfterShotTrace(x, y);
        }

        onUiSettingsOpened: {
            settingsDialog.micOptions = micOptions;
            settingsDialog.defaultMic = defaultMic;
            thresholdSlider.value = TRIGGER_DB;
            micChart.setThreshold(TRIGGER_DB);
            settingsDialog.cameraOptions = cameraOptions;
            settingsDialog.defaultCamera = defaultCamera;
            upDownDetectionCheckBox.checked = upDownDetection;
        }

        onUiUpdateSamples: {
            micChart.updateMicChart(dB);
        }

        onUiCameraMicError: {
            cameraMicError.open();
        }
    }

    ToastManager {
        id: toast
    }

    Rectangle {
        id: topBar
        color: primaryColor
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
            font.family: "Segoe UI"
            font.pointSize: 25
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: logo.right
            anchors.leftMargin: 10
        }

        NavBarButton {
            id: settingsBtn
            logoSource: "ui/images/settings.svg"
            pressedLogoSource: "ui/images/settings_pressed.svg"
            clickedLogoSource: "ui/images/settings_clicked.svg"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: divider0.right
            anchors.rightMargin: 20
            activeColor: secondaryColor
            onClickedHandler: function (){
                qmlCppBridge.settingsOpened();
                settingsDialog.open();
                active = true;
            }
        }

        Rectangle {
            id: divider0
            color: "white"
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: calibrateBtn.left
            anchors.rightMargin: 20
            width: 2
            height: parent.height - 30
        }

        NavBarButton {
            id: calibrateBtn
            defaultText: "CALIBRATE"
            activeText: "CALIBRATING"
            onClickedHandler: qmlCppBridge.calibrationClicked
            activeColor: secondaryColor
            activeTextColor: primaryColor
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

        NavBarButton {
            id: shootBtn
            defaultText: "SHOOT"
            activeText: "SHOOTING"
            onClickedHandler: qmlCppBridge.shootClicked
            activeColor: secondaryColor
            activeTextColor: primaryColor
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

            MouseArea {
                width: parent.width
                height: parent.height

                onClicked: {
                    qmlCppBridge.closingApplication();
                    Qt.quit();
                }

                onPressed: {
                    parent.source = "ui/images/close_clicked.svg";
                }

                onReleased: {
                    parent.source = "ui/images/close.svg";
                }
            }
        }
    }

    Rectangle {
        anchors.top: topBar.bottom
        anchors.left: parent.left

        width: height - 140
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
                    clip: true
                    color: "transparent"
                    border.color: secondaryColor
                    radius: 10

                    Canvas {
                        anchors.fill: parent

                        onPaint: {
                            let ctx = getContext("2d");
                            for (const circle of circles) {
                                ctx.fillStyle = circle.fill;
                                ctx.strokeStyle = circle.border;

                                const radius = circle.radius * width;
                                const x = width / 2 - radius;
                                const y = height / 2 - radius;

                                ctx.beginPath();
                                ctx.ellipse(x, y, radius * 2, radius * 2);
                                if (circle.fill != "") {
                                    ctx.fill();
                                }

                                if (circle.border != "") {
                                    ctx.stroke();
                                }
                            }

                            ctx.textAlign = "center";
                            ctx.textBaseline = "middle";
                            ctx.font = "normal normal 0 10px 'Segoe UI'";
                            for (const circleNum of circleNumbers) {
                                ctx.strokeStyle = circleNum.color;
                                const x = circleNum.x * width;
                                const y = circleNum.y * height;
                                ctx.beginPath();
                                ctx.strokeText(circleNum.text, x, y);
                            }
                        }
                    }

                    Canvas {
                        id: calibratingCircle
                        visible: false
                        width: targetTrace.radius * 2 + 6
                        height: targetTrace.radius * 2 + 6
                        x: 0
                        y: 0
                        z: 100
                        Drag.active: dragArea.drag.active

                        onXChanged: {
                            targetTrace.requestPaint();
                        }

                        onYChanged: {
                            targetTrace.requestPaint();
                        }

                        onPaint: {
                            let ctx = getContext("2d");
                            ctx.strokeStyle = "#ef476f";
                            ctx.lineWidth = 2;
                            ctx.setLineDash([1, 1]);
                            ctx.ellipse(0, 0, width, width);
                            ctx.stroke();
                        }

                        MouseArea {
                            id: dragArea
                            anchors.fill: parent
                            drag.target: parent

                            onReleased: {
                                let deltaX = parent.x - targetTrace.calibrationPoint.x;
                                let deltaY = targetTrace.calibrationPoint.y - parent.y;
                                qmlCppBridge.adjustCalibration(deltaX / targetTrace.factor, deltaY / targetTrace.factor);
                                targetTrace.requestPaint();
                            }
                        }
                    }

                    Canvas {
                        id: targetTrace
                        width: parent.width
                        height: parent.height
                        z: 2
                        property var beforeShotTrace: []
                        property var shotCircles: []
                        property var afterShotTrace: []
                        property var factor: width / 170
                        property var radius: factor * 2.25
                        property var calibrationPoint: {"x": -1, "y": -1}

                        function clearCalibration() {
                            calibratingCircle.visible = false;
                            calibrationPoint = {"x": -1, "y": -1};
                            requestPaint();
                        }

                        function calibrateShot(shot) {
                            calibrationPoint = {"x": shot.x - 2, "y": shot.y - 2};
                            calibratingCircle.x = shot.x - 2;
                            calibratingCircle.y = shot.y - 2;
                            calibratingCircle.visible = true;
                            requestPaint();
                        }

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
                            let point = transformPoint(x, y);

                            shotCircles.push(Qt.createQmlObject("import QtQuick 2.0;
                                Rectangle {
                                    id: draggableCircle
                                    width: (targetTrace.radius + border.width) * 2
                                    height: (targetTrace.radius + border.width) * 2
                                    color: tertiaryColor
                                    border.width: 1
                                    border.color: '#ffffff'
                                    x: " + point.x + " - width / 2
                                    y: " + point.y + " - height / 2
                                    z: 3
                                    radius: targetTrace.radius + border.width

                                    MouseArea {
                                        anchors.fill: parent

                                        onPressed: {
                                            if (" + shotCircles.length + " + 1 == targetTrace.shotCircles.length) {
                                                // only if shot clicked is the most recent shot
                                                qmlCppBridge.stopRecording();
                                                targetTrace.calibrateShot(parent);
                                            }
                                        }
                                    }
                                }", parent));
                        }

                        function resetTrace(resetGroupIfNecessary) {
                            beforeShotTrace = [];
                            afterShotTrace = [];

                            if (resetGroupIfNecessary && shotCircles.length == 10) {
                                shotCircles.forEach(shotCircle => shotCircle.destroy());
                                shotCircles = [];

                                shotGroupList.showPrevGroup();
                            }

                            if (resetGroupIfNecessary) {
                                if (targetTrace.shotCircles.length > 0) {
                                    targetTrace.shotCircles[shotCircles.length - 1].color = quaternaryColor;
                                    targetTrace.shotCircles[shotCircles.length - 1].z = 1;
                                }
                            }

                            targetTrace.requestPaint();
                        }

                        onPaint: {
                            let ctx = getContext("2d");
                            ctx.reset();
                            ctx.lineWidth = 2;

                            if (calibrationPoint.x != -1 && calibrationPoint.y != -1) {
                                ctx.strokeStyle = tertiaryColor;
                                ctx.beginPath();
                                let pointStartX = calibrationPoint.x + 2.5 + radius;
                                let pointStartY = calibrationPoint.y + 2.5 + radius;
                                let pointEndX = calibratingCircle.x + calibratingCircle.width / 2;
                                let pointEndY = calibratingCircle.y + calibratingCircle.width / 2;
                                ctx.moveTo(pointStartX, pointStartY);
                                ctx.lineTo(pointEndX, pointEndY);
                                ctx.stroke();
                            }

                            if (beforeShotTrace.length > 0) {
                                ctx.strokeStyle = accentColor1;

                                ctx.beginPath();
                                ctx.moveTo(beforeShotTrace[0]["x"], beforeShotTrace[0]["y"]);
                                for (let i = 1; i < beforeShotTrace.length; i++) {
                                    ctx.lineTo(beforeShotTrace[i]["x"], beforeShotTrace[i]["y"]);
                                    // TODO: change this to bezier curve or some other interpolation
                                }
                                ctx.stroke();
                            }

                            if (afterShotTrace.length > 0) {
                                ctx.strokeStyle = accentColor2;
                                ctx.beginPath();
                                ctx.moveTo(afterShotTrace[0]["x"], afterShotTrace[0]["y"]);
                                for (let i = 1; i < afterShotTrace.length; i++) {
                                    ctx.lineTo(afterShotTrace[i]["x"], afterShotTrace[i]["y"]);
                                    // TODO: change this to bezier curve or some other interpolation
                                }
                                ctx.stroke();
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: targetTrace.width
                height: 120
                anchors.bottom: parent.bottom
                color: "transparent"

                ShotStat {
                    id: stabilityLbl
                    title: "STABILITY"
                    primaryColor: secondaryColor
                    dp: 0
                    suffix: "%"
                    anchors.left: parent.left
                }

                ShotStat {
                    id: descLbl
                    title: "DESC"
                    primaryColor: secondaryColor
                    logoSource: "ui/images/desc.svg"
                    dp: 1
                    suffix: "s"
                    anchors.left: stabilityLbl.right
                    anchors.leftMargin: 20
                }

                ShotStat {
                    id: aimLbl
                    title: "AIM"
                    primaryColor: secondaryColor
                    logoSource: "ui/images/aim.svg"
                    dp: 1
                    suffix: "s"
                    anchors.left: descLbl.right
                    anchors.leftMargin: 20
                }
            }
        }
    }

    Rectangle {
        anchors.top: topBar.bottom
        anchors.right: parent.right

        width: parent.width - leftContent.parent.width + 20
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
                width: parent.width / 3 - 40
                height: parent.height
                anchors.left: parent.left
                anchors.top: parent.top
                color: "transparent"
                border.color: secondaryColor
                radius: 10

                ScrollView {
                    width: parent.width
                    height: parent.height
                    anchors.top: parent.top
                    anchors.left: parent.left
                    clip: true

                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded

                    ListView {
                        id: shotGroupList
                        verticalLayoutDirection: ListView.TopToBottom
                        model: ListModel {}
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        spacing: 20
                        property var shotCount: 0;
                        property var shotBuffer: [];

                        function showPrevGroup() {
                            model.insert(0, { points: [] });
                            currentIndex = 0;
                            for (const shot of shotBuffer) {
                                currentItem.addShot(shot.x, shot.y);
                            }

                            shotBuffer = [];
                        }

                        function addShot(x, y) {
                            shotBuffer.push({x: x, y: y});
                        }

                        delegate: ItemDelegate {
                            width: shotGroupList.width - 10
                            height: width
                            anchors.horizontalCenter: parent.horizontalCenter
                            visible: true
                            property var factor: (width / 2) / 29.75 // convert mm to px

                            function transformPoint(x, y) {
                                return { x: x * factor + width / 2, y: height / 2 - y * factor };
                            }

                            function addShot(x, y) {
                                let angle = Math.atan2(y, x);
                                if (Math.sqrt(x * x + y * y) <= 29.75) {
                                    // within black circle
                                    let pos = transformPoint(x, y);
                                    points.append({insideCircle: true, x: pos.x, y: pos.y, angle: angle});
                                } else {
                                    let newX = 29 * Math.cos(angle);
                                    let newY = 29 * Math.sin(angle);
                                    let pos = transformPoint(newX, newY);
                                    points.append({insideCircle: false, x: pos.x, y: pos.y, angle: angle});
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
                                    context.reset();
                                    context.strokeStyle = "#ffffff";
                                    context.lineWidth = 3;

                                    for (let i = 0; i < points.count; i++) {
                                        context.fillStyle = quaternaryColor;
                                        if (points.get(i).insideCircle) {
                                            drawShot(context, points.get(i));
                                        } else {
                                            drawTriangle(context, points.get(i));
                                        }
                                    }
                                }

                                function drawShot(context, point) {
                                    context.beginPath();
                                    context.ellipse(point["x"] - radius, point["y"] - radius, 2 * radius, 2 * radius);
                                    context.stroke();
                                    context.fill();
                                }

                                function drawTriangle(context, point) {
                                    let deltaAngle = Math.PI / 64;
                                    let point1X = 27 * Math.cos(point.angle + deltaAngle);
                                    let point1Y = 27 * Math.sin(point.angle + deltaAngle);
                                    let point1 = transformPoint(point1X, point1Y);

                                    let point2X = 27 * Math.cos(point.angle - deltaAngle);
                                    let point2Y = 27 * Math.sin(point.angle - deltaAngle);
                                    let point2 = transformPoint(point2X, point2Y);

                                    context.beginPath();
                                    context.moveTo(point["x"], point["y"]);
                                    context.lineTo(point1.x, point1.y);
                                    context.lineTo(point2.x, point2.y);
                                    context.lineTo(point.x, point.y);
                                    context.stroke();
                                    context.fill();
                                }
                            }

                            Canvas {
                                anchors.fill: parent
                                z: -1

                                onPaint: {
                                    const ctx = getContext("2d");
                                    const startCircleIndex = 6;
                                    const startCircleNumIndex = 24;

                                    const factor = width / (2 * circles[startCircleIndex].radius);
                                    for (let i = startCircleIndex; i < circles.length; i++) {
                                        const circle = circles[i];
                                        ctx.fillStyle = circle.fill;
                                        ctx.strokeStyle = circle.border;

                                        const radius = circle.radius * factor;
                                        const x = width / 2 - radius;
                                        const y = height / 2 - radius;

                                        ctx.beginPath();
                                        ctx.ellipse(x, y, radius * 2, radius * 2);
                                        if (circle.fill != "") {
                                            ctx.fill();
                                        }

                                        if (circle.border != "") {
                                            ctx.stroke();
                                        }
                                    }

                                    ctx.textAlign = "center";
                                    ctx.textBaseline = "middle";
                                    ctx.font = "normal normal 0 10px 'Segoe UI'";
                                    for (let i = startCircleNumIndex; i < circleNumbers.length; i++) {
                                        const circleNum = circleNumbers[i];
                                        ctx.strokeStyle = circleNum.color;
                                        const deltaX = (circleNum.x - 1/2) * factor;
                                        const deltaY = (1/2 - circleNum.y) * factor;
                                        const x = width / 2 + deltaX;
                                        const y = height / 2 + deltaY;

                                        ctx.beginPath();
                                        ctx.strokeText(circleNum.text, x, y);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                // shot log
                id: shotLogRect
                width: parent.width * 2 / 3
                height: parent.height * 5 / 9
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.left: zoomedTargetRect.right
                anchors.leftMargin: 20
                color: "transparent"
                border.color: secondaryColor
                radius: 10

                ScrollView {
                    width: parent.width
                    height: parent.height
                    anchors.top: parent.top
                    anchors.left: parent.left
                    clip: true

                    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                    ScrollBar.vertical.policy: ScrollBar.AsNeeded

                    ListView {
                        id: shotLogList
                        verticalLayoutDirection: ListView.TopToBottom
                        model: ListModel {}
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.bottom: parent.bottom
                        spacing: 10

                        delegate: ItemDelegate {
                            width: shotLogList.width - 10
                            height: shotLogList.height / 10 - shotLogList.spacing

                            Rectangle {
                                height: parent.height
                                width: parent.width
                                color: "transparent"

                                Rectangle {
                                    id: logNumCircle
                                    width: parent.height / 2
                                    height: parent.height / 2
                                    radius: parent.height / 4
                                    color: secondaryColor
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 20

                                    Text {
                                        width: parent.width
                                        height: parent.height
                                        color: primaryColor
                                        fontSizeMode: Text.Fit
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                        text: sn
                                    }
                                }

                                Text {
                                    id: logScoreLbl
                                    width: 40
                                    horizontalAlignment: Text.AlignHCenter
                                    text: score.toFixed(1)
                                    color: secondaryColor
                                    font.pointSize: 25
                                    font.family: "Segoe UI"
                                    font.weight: Font.Light
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: logNumCircle.right
                                    anchors.leftMargin: 20
                                }

                                Image {
                                    id: logAngle
                                    source: "ui/images/arrow_" + angle + ".svg"
                                    height: parent.height
                                    width: height
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: logScoreLbl.right
                                    anchors.leftMargin: 20
                                    mipmap: true
                                }

                                ProgressBar {
                                    id: logStab
                                    value: stab / 100

                                    anchors.right: logDescLbl.left
                                    anchors.rightMargin: 30
                                    anchors.left: logAngle.right
                                    anchors.leftMargin: 30
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
                                    width: 40
                                    horizontalAlignment: Text.AlignHCenter
                                    text: desc.toFixed(1) + "s"
                                    color: secondaryColor
                                    font.family: "Segoe UI"
                                    font.weight: Font.Light
                                    font.pointSize: 20
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: logDescIcon.left
                                    anchors.rightMargin: 10
                                }

                                Image {
                                    id: logDescIcon
                                    source: "ui/images/desc.svg"
                                    height: parent.height / 2
                                    width: height
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: logAimLbl.left
                                    anchors.rightMargin: 20
                                    mipmap: true
                                }

                                Text {
                                    id: logAimLbl
                                    width: 40
                                    horizontalAlignment: Text.AlignHCenter
                                    text: aim.toFixed(1) + "s"
                                    color: secondaryColor
                                    font.family: "Segoe UI"
                                    font.weight: Font.Light
                                    font.pointSize: 20
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: logAimIcon.left
                                    anchors.rightMargin: 10
                                }

                                Image {
                                    id: logAimIcon
                                    source: "ui/images/aim.svg"
                                    height: parent.height / 2
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
                width: shotLogRect.width
                height: parent.height * 4 / 9
                anchors.bottom: parent.bottom
                anchors.top: shotLogRect.bottom
                anchors.topMargin: 20
                anchors.right: parent.right
                color: "transparent"
                border.color: secondaryColor
                radius: 10

                Rectangle {
                    x: parent.width - 40
                    y: (parent.height - height) / 2 - 20
                    height: textX.height + 5 + textY.height
                    color: "transparent"

                    Rectangle {
                        id: xColor
                        height: textX.height - 7.5
                        width: height
                        color: accentColor1
                        anchors.left: parent.left
                        anchors.verticalCenter: textX.verticalCenter
                    }

                    Text {
                        id: textX
                        text: "X"
                        color: secondaryColor
                        anchors.top: parent.top
                        anchors.left: xColor.right
                        anchors.leftMargin: 5
                    }

                    Rectangle {
                        id: yColor
                        height: textX.height - 7.5
                        width: height
                        color: accentColor2
                        anchors.left: parent.left
                        anchors.verticalCenter: textY.verticalCenter
                    }

                    Text {
                        id: textY
                        text: "Y"
                        color: secondaryColor
                        anchors.top: textX.bottom
                        anchors.topMargin: 5
                        anchors.left: yColor.right
                        anchors.leftMargin: 5
                    }
                }

                ChartView {
                    id: xtYtChart
                    title: ""
                    x: -12
                    y: -12
                    height: parent.height + 24
                    width: parent.width + 6
                    anchors.margins: 0
                    antialiasing: true
                    backgroundColor: "transparent"
                    titleColor: secondaryColor
                    legend.visible: false
                    legend.labelColor: secondaryColor
                    legend.alignment: "AlignRight"

                    function updateXtYt(xt, yt, ts) {
                        this.removeAllSeries();
                        var xtSeries = this.createSeries(ChartView.SeriesTypeLine, "X", xAxis, yAxis);
                        var ytSeries = this.createSeries(ChartView.SeriesTypeLine, "Y", xAxis, yAxis);

                        xtSeries.color = accentColor1;
                        ytSeries.color = accentColor2;

                        for (let i = 0; i < ts.length; i++) {
                            xtSeries.append(ts[i], xt[i]);
                            ytSeries.append(ts[i], yt[i]);
                        }
                    }

                    ValueAxis {
                        id: xAxis
                        min: -0.5
                        max: 0.5
                        titleText: "<font color='" + secondaryColor + "'>time (s)</font>"
                        color: secondaryColor
                        labelsColor: secondaryColor
                        gridLineColor: secondaryColor
                    }

                    ValueAxis {
                        id: yAxis
                        min: -40
                        max: 40
                        titleText: "<font color='" + secondaryColor + "'>displacement (mm)</font>"
                        color: secondaryColor
                        labelsColor: secondaryColor
                        gridLineColor: secondaryColor
                    }

                    LineSeries {
                        name: "X"
                        axisX: xAxis
                        axisY: yAxis
                        color: accentColor1
                    }

                    LineSeries {
                        name: "Y"
                        axisX: xAxis
                        axisY: yAxis
                        color: accentColor2
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
