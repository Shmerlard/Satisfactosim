pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import gui.theme

Item {
    id: root

    property var nodeData
    property int modelIndex: -1
    property Item contentContainer: null
    property bool selected: false
    property bool hovered: false

    implicitWidth: 150
    implicitHeight: 100

    Rectangle {
        id: background
        anchors.fill: parent
        color: Theme.surfaceFactory
        border.color: root.selected ? Theme.selection : Theme.accentFactory
        border.width: root.selected ? 2 : 1
        radius: 10
    }
    Item {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 0
        // --------------- INPUT ------------------
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
                    isInput: true
                    contentContainer: root.contentContainer
                    nodeRoot: root
                    y: 25 * (index + 1) - height / 2
                }
            }
        }

        // --------------- CENTER ------------------
        Column {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            Image {
                source: "image://assets/misc/industry.png"
                width: 50
                height: 50
                Layout.alignment: Qt.AlignHCenter
            }
            Text {
                text: root.nodeData ? root.nodeData.name : ""
                color: Theme.textPrimary
            }
        }

        // --------------- OUTPUTS ------------------
        Item {
            id: outputsCol
            anchors.right: parent ? parent.right : undefined
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
                    isInput: false
                    contentContainer: root.contentContainer
                    nodeRoot: root
                    y: 25 * (index + 1) - height / 2
                }
            }
        }
    }
}
