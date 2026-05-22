pragma ComponentBehavior: Bound

import QtQuick
import gui.theme

Item {
    id: root

    property var nodeData
    property int modelIndex: -1
    property Item contentContainer: null
    property bool selected: false

    implicitWidth: 150
    implicitHeight: 150

    Rectangle {
        anchors.fill: parent
        color: Theme.surface
        border.color: root.selected ? Theme.selection : Theme.accent
        border.width: root.selected ? 2 : 1
        radius: 10
    }

    Text {
        anchors.centerIn: parent
        text: root.nodeData ? root.nodeData.name : "Splitter"
        color: Theme.textPrimary
    }

    Item {
        id: inputsCol
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: childrenRect.width

        Repeater {
            model: root.nodeData ? root.nodeData.inputs : []
            delegate: Port {
                required property var modelData
                required property int index
                portData: modelData
                nodeRoot: root
                isInput: true
                contentContainer: root.contentContainer
                y: 50 * (index + 1) - height / 2
            }
        }
    }

    Item {
        id: outputsCol
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: childrenRect.width

        Repeater {
            model: root.nodeData ? root.nodeData.outputs : []
            delegate: Port {
                required property var modelData
                required property int index
                anchors.right: parent ? parent.right : undefined
                portData: modelData
                nodeRoot: root
                isInput: false
                contentContainer: root.contentContainer
                y: 50 * (index + 1) - height / 2
            }
        }
    }
}
