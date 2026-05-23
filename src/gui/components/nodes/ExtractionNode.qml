pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import gui.theme

Item {
    id: root

    property var nodeData
    property int modelIndex: -1
    property Item contentContainer: null
    property bool selected: false
    property bool hovered: false

    readonly property string machineCount: nodeData ? nodeData.machineCount.toFixed(2) : "NAME"
    readonly property string machineLimit: nodeData ? nodeData.machineLimit : ""
    readonly property string nodeName: nodeData ? nodeData.name : ""
    readonly property string overclock: nodeData ? nodeData.overclock : ""

    implicitWidth: 150
    implicitHeight: 150

    Rectangle {
        id: background
        anchors.fill: parent
        color: Theme.surfaceExtraction
        border.color: root.selected ? Theme.selection : Theme.accentExtraction
        border.width: root.selected ? 2 : 1
        radius: 10
        HoverHandler {
            id: backgroundHover
        }
        ToolTip {
            visible: backgroundHover.hovered
            text: "DSDDSDS"
            delay: 500
        }
    }
    Item {
        id: mainLayout
        anchors.fill: parent
        anchors.margins: 0

        // --------------- CENTER ------------------
        Column {
            id: centerCol

            x: (outputsCol.x - width) / 2
            anchors.verticalCenter: parent.verticalCenter
            spacing: 5

            Text {
                id: nodeName
                text: root.nodeName
                anchors.horizontalCenter: parent.horizontalCenter
                color: Theme.textPrimary
            }
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
            // ---------- OVERCLOCK ------------
            Rectangle {
                anchors.horizontalCenter: parent.horizontalCenter
                width: overclockText.width
                height: overclockText.height
                radius: 5
                color: Theme.badge
                Text {
                    id: overclockText
                    text: root.overclock
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: Theme.textPrimary
                }
                HoverHandler {
                    id: overclockHover
                }
                ToolTip {
                    visible: overclockHover.hovered
                    delay: 500
                }
            }

            // ---------- AMOUNT/LIMIT ------------
            Rectangle {
                width: amountText.width
                height: amountText.height
                anchors.horizontalCenter: parent.horizontalCenter
                radius: 5
                color: Theme.badge
                Text {
                    id: amountText
                    text: "C/L: " + root.machineCount + "/" + root.machineLimit
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
