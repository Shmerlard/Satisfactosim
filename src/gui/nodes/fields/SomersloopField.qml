pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import gui.theme

Item {

    id: root
    required property int somersloopCount
    required property int somersloopSlot

    implicitWidth: row.width
    implicitHeight: row.height

    Row {
        id: row
        anchors.horizontalCenter: parent.horizontalCenter
        Image {
            source: "image://assets/misc/Somersloop.webp"
            anchors.verticalCenter: parent.verticalCenter
            width: 10
            height: 10
        }

        Text {
            visible: root.somersloopSlot > 0
            text: "sl: " + root.somersloopCount + " / " + root.somersloopSlot
            color: Theme.textPrimary
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
