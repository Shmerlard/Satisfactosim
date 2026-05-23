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

    // readonly property string machineCount: nodeData ? nodeData.machineCount.toFixed(2) : "NAME"
    readonly property string machineCount: nodeData ? nodeData.machineCountStr : "ddd"
    readonly property string machineLimit: nodeData ? nodeData.machineLimit.toFixed(2) : ""
    readonly property int somersloopCount: nodeData ? nodeData.somersloopCount : 0
    readonly property int somersloopSlotSize: nodeData ? nodeData.somersloopSlotSize : 0
    readonly property string nodeName: nodeData ? nodeData.name : ""

    implicitWidth: 200
    implicitHeight: 150

    Rectangle {
        id: background
        anchors.fill: parent
        color: Theme.surface
        border.color: root.selected ? Theme.selection : Theme.accent
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
                    nodeRoot: root
                    isInput: true
                    contentContainer: root.contentContainer
                    y: 50 * (index + 1) - height / 2
                }
            }
        }

        // --------------- CENTER ------------------
        Column {
            id: centerCol
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            Rectangle {
                width: 50
                height: 70
                color: Theme.iconBg
                radius: 10
                anchors.horizontalCenter: parent.horizontalCenter
                Image {
                    source: root.nodeData ? root.nodeData.machineIcon : ""
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: parent.width
                }
                Text {
                    text: root.nodeData ? root.nodeData.machineName : ""
                    // color: Theme.textPrimary
                    color: "black"
                    font.pixelSize: 10
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                }
            }
            Text {
                text: root.nodeData ? root.nodeData.name : ""
                color: Theme.textPrimary
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                text: root.machineCount
                color: Theme.textPrimary
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                text: root.machineLimit
                color: Theme.textPrimary
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                visible: root.somersloopSlotSize > 0
                text: "sl: " + root.somersloopCount + "/" + root.somersloopSlotSize
                color: Theme.textPrimary
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }

        // --------------- OUTPUTS ------------------
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
}
