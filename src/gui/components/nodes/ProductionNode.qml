pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts

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


    implicitWidth: 200
    implicitHeight: 150

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#1e2838"
        border.color: root.selected ? "#4fc3f7" : "#e8a020"
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
                height: 50
                color: "grey"
                radius: 10
                anchors.horizontalCenter: parent.horizontalCenter
                Image {
                    source: root.nodeData ? root.nodeData.machineIcon : ""
                    width: parent.width
                    height: parent.height
                }
            }
            Text {
                text: root.nodeData ? root.nodeData.name : ""
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: root.machineCount
                color: "white"
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                // text: root.nodeData ? root.nodeData.machineLimit : "DD"
                text: root.machineLimit
                color: "white"
            }
            Text {
                text: root.nodeData ? root.nodeData.machineName : ""
                color: "white"
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
}
