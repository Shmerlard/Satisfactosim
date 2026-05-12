import QtQuick

Item {
    id: root

    property real mouseContentX: 0
    property real mouseContentY: 0

    anchors.bottom: parent.bottom
    anchors.left: parent.left
    anchors.margins: 6
    width: statusText.implicitWidth + 12
    height: statusText.implicitHeight + 6
    Rectangle {
        anchors.fill: parent

        color: "#88000000"
        radius: 4

        Text {
            id: statusText
            anchors.centerIn: parent
            text: "x: " + root.mouseContentX + "  y: " + root.mouseContentY
            color: "white"
            font.pixelSize: 11
            font.family: "monospace"
        }
    }
}
