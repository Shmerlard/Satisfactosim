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
    readonly property string machineLimit: nodeData ? nodeData.machineLimit : ""
    // implicitWidth: nameText.implicitWidth + outputsCol.implicitWidth + 40
    // implicitHeight: Math.max(50, mainLayout.implicitHeight)
    implicitWidth: 150
    implicitHeight: 100

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#1a2e1a"
        border.color: selected ? "#4fc3f7" : "#27ae60"
        border.width: selected ? 2 : 1
        radius: 10
    }
    Item {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 0

        // --------------- CENTER ------------------
        Column {
            id: centerCol

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            Image {
                source: root.nodeData ? root.nodeData.machineIcon : ""
                width: 50
                height: 50
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                id: amountText
                text: root.machineCount + "/" + root.machineLimit
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
            }
            Text {
                id: nameText
                text: root.nodeData ? root.nodeData.machineName : ""
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
            }
        }

        // --------------- OUTPUTS ------------------
        Column {
            id: outputsCol

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            spacing: 5
            Repeater {
                model: root.nodeData ? root.nodeData.outputs : []
                delegate: Port {
                    portData: modelData
                    isInput: false
                    nodeRoot: root
                    contentContainer: root.contentContainer
                }
            }
        }
    }
}
