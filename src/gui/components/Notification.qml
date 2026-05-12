import QtQuick
import FACTORY_QT

Item {
    id: root

    anchors.top: parent.top
    anchors.right: parent.right
    anchors.margins: 12

    width: label.implicitWidth + 24
    height: label.implicitHeight + 16

    visible: opacity > 0
    opacity: 0

    Connections {
        target: SessionManager
        function onOperationFailed(reason) { root.show(reason) }
    }

    function show(message) {
        label.text = message;
        opacity = 1;
        hideTimer.restart();
    }

    Timer {
        id: hideTimer
        interval: 3000
        onTriggered: fadeOut.start()
    }

    NumberAnimation {
        id: fadeOut
        target: root
        property: "opacity"
        to: 0
        duration: 400
        easing.type: Easing.InQuad
    }

    Rectangle {
        anchors.fill: parent
        color: "#cc2a1a1a"
        border.color: "#e05050"
        border.width: 1
        radius: 6

        Text {
            id: label
            anchors.centerIn: parent
            color: "#f0f0f0"
            font.pixelSize: 13
            font.family: "monospace"
        }
    }
}
