import QtQuick
import QtQuick.Layouts

Item {
    id: root

    property var nodeData
    property int modelIndex: -1
    property Item contentContainer: null
    property bool selected: false
    property bool hovered: false

    implicitWidth: inputsCol.implicitWidth + centerCol.implicitWidth + outputsCol.implicitWidth + 40
    implicitHeight: Math.max(50, mainLayout.implicitHeight)

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#1e2838"
        border.color: selected ? "#4fc3f7" : "#e8a020"
        border.width: selected ? 2 : 1
        radius: 10
    }
    RowLayout {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 0

        ColumnLayout {
            id: inputsCol
            Repeater {
                model: root.nodeData ? root.nodeData.inputs : []
                delegate: Port {
                    portData: modelData
                    nodeRoot: root
                    isInput: true
                    contentContainer: root.contentContainer
                }
            }
        }
        Item { Layout.fillWidth: true }
        ColumnLayout {
            id: centerCol
            Item {}
            Rectangle {
                    Layout.preferredHeight: 50
                    Layout.preferredWidth: 50
                    color: "grey"
                    radius: 10
                    Layout.alignment: Qt.AlignHCenter
                Image {
                    source: root.nodeData ? root.nodeData.machineIcon : ""
                    width: parent.width
                    height: parent.height
                }
            }
            Text {
                text: root.nodeData ? root.nodeData.name : ""
                Layout.alignment: Qt.AlignHCenter
                color: "white"
            }
            Text {
                Layout.alignment: Qt.AlignHCenter
                text: root.nodeData ? root.nodeData.machineCount : ""
                color: "white"
            }
            Text {
                Layout.alignment: Qt.AlignHCenter
                text: root.nodeData ? root.nodeData.machineName : ""
                color: "white"
            }
        }
        Item { Layout.fillWidth: true }
        ColumnLayout {
            id: outputsCol
            Repeater {
                model: root.nodeData ? root.nodeData.outputs : []
                delegate: Port {
                    portData: modelData
                    nodeRoot: root
                    isInput: false
                    contentContainer: root.contentContainer
                }
            }
        }
    }
}
