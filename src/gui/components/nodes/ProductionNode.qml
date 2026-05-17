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

    readonly property string machineCount: nodeData ? nodeData.machineCount.toFixed(2) : "NAME"

    // implicitWidth: inputsCol.implicitWidth + centerCol.implicitWidth + outputsCol.implicitWidth + 40
    implicitWidth: 200
    implicitHeight: 150
    // implicitHeight: Math.max(50, mainLayout.implicitHeight)

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#1e2838"
        // color: "yellow"
        border.color: root.selected ? "#4fc3f7" : "#e8a020"
        border.width: root.selected ? 2 : 1
        radius: 10
    }
    Item {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 0

        Column {
            id: inputsCol
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            spacing: 5
            Repeater {
                model: root.nodeData ? root.nodeData.inputs : []
                delegate: Port {
                    required property var modelData
                    portData: modelData
                    nodeRoot: root
                    isInput: true
                    contentContainer: root.contentContainer
                }
            }
        }
        Column {
            id: centerCol
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            Rectangle {
                width: 50
                height: 50
                color: "grey"
                radius: 10
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
                // text: root.nodeData ? root.nodeData.machineCount : "DD"
                text: root.machineCount
                color: "white"
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: root.nodeData ? root.nodeData.machineLimit : "DD"
                color: "white"
            }
            Text {
                text: root.nodeData ? root.nodeData.machineName : ""
                color: "white"
            }
        }


        Column {
            id: outputsCol
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: 5

            Repeater {
                model: root.nodeData ? root.nodeData.outputs : []
                delegate: Port {
                    required property var modelData
                    anchors.right: parent.right
                    portData: modelData
                    nodeRoot: root
                    isInput: false
                    contentContainer: root.contentContainer
                }
            }
        }
    }
}
