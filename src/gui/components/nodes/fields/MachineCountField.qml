pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import gui.theme

Item {
    id: root
    required property string count
    required property string limit

    implicitWidth: badge.width
    implicitHeight: badge.height
    Rectangle {
        id: badge
        width: amountText.width
        height: amountText.height
        anchors.horizontalCenter: parent.horizontalCenter
        radius: 5
        color: Theme.badge
        Text {
            id: amountText
            text: "C/L: " + root.count + " | " + root.limit
            anchors.horizontalCenter: parent.horizontalCenter
            color: Theme.textPrimary
        }
        HoverHandler {
            id: amountHover
        }
        ToolTip {
            visible: amountHover.hovered
            text: "C/L — Count / Limit\nCount: how many machines are running\nLimit: the machine cap you set"
            delay: 500
        }
    }
}
