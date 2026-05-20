pragma ComponentBehavior: Bound

import QtQuick

Item {
    id: root

    property var nodeData
    property int modelIndex: -1
    property Item contentContainer: null
    property bool selected: false

    implicitWidth: 120
    implicitHeight: 100

    Rectangle {
        anchors.fill: parent
        color: "#1e2838"
        border.color: root.selected ? "#4fc3f7" : "#e8a020"
        border.width: root.selected ? 2 : 1
        radius: 10
    }

    Text {
        anchors.centerIn: parent
        text: root.nodeData ? root.nodeData.name : "Splitter"
        color: "white"
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
                anchors.right: parent.right
                portData: modelData
                nodeRoot: root
                isInput: false
                contentContainer: root.contentContainer
                y: 50 * (index + 1) - height / 2
            }
        }
    }
}
