import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT

Popup {
    id: root

    property var nodeData: null

    readonly property int somersloopCount: nodeData ? nodeData.somersloopCount : 0
    readonly property int somersloopSlotSize: nodeData ? nodeData.somersloopSlotSize : 0

    width: 220
    padding: 14
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    onOpened: {
        // renameField.clear() // FIX: doesnt clear
        limitField.text = nodeData && (nodeData.nodeType === 3 || nodeData.nodeType === 4) ? String(nodeData.machineLimit) : "";
        renameField.text = nodeData ? nodeData.name : "";
    }

    background: Rectangle {
        color: "#1e2838"
        border.color: "#e8a020"
        border.width: 1
        radius: 8
    }

    ColumnLayout {
        width: parent.width
        spacing: 10

        // --- header ---
        Text {
            text: nodeData ? nodeData.name : ""
            color: "white"
            font.bold: true
            font.pixelSize: 14
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#444"
        }

        // --- rename ---
        Text {
            text: "Rename"
            color: "#aaa"
            font.pixelSize: 11
        }
        TextField {
            id: renameField
            Layout.fillWidth: true
            placeholderText: nodeData ? nodeData.name : ""
            background: Rectangle {
                color: "#2a3a4a"
                radius: 4
            }
            color: "white"
            // onTextChanged: console.log(text)
            onTextChanged: {
                nodeData.name = text;
                // nodeData.setName(text);
            }
            // onAccepted: { /* TODO */ root.close() }
        }

        // --- machine limit (production + extraction) ---
        ColumnLayout {
            visible: nodeData && (nodeData.nodeType === 3 || nodeData.nodeType === 4)
            Layout.fillWidth: true
            spacing: 4

            Text {
                text: "Machine Limit"
                color: "#aaa"
                // color: "yellow"
                font.pixelSize: 11
            }
            Item {
                // width: 20
                Layout.fillWidth: true
                height: 30
                Rectangle {
                    id: dec1
                    anchors.left: parent.left
                    width: limitField.height
                    height: limitField.height
                    radius: 4
                    TapHandler {
                        onTapped: {
                            SceneManager.incMachineLimit(root.nodeData, -1);
                        }
                    }
                    Text {
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: "-1"
                    }
                }
                TextField {
                    id: limitField
                    anchors.left: dec1.right
                    anchors.horizontalCenter: parent.horizontalCenter
                    placeholderText: "no limit"
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    background: Rectangle {
                        color: "#2a3a4a"
                        radius: 4
                    }
                    color: "white"
                    onEditingFinished: {
                        let val = parseFloat(text);
                        if (!isNaN(val))
                            SceneManager.setMachineLimit(root.nodeData, val);
                    }
                }
                Rectangle {
                    width: limitField.height
                    height: limitField.height
                    color: "yellow"
                    anchors.right: parent.right
                    radius: 4
                    TapHandler {
                        onTapped: {
                            SceneManager.incMachineLimit(nodeData, 1);
                        }
                    }
                    Text {
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: "+1"
                    }
                }
            }
        }

        // --- purity + tier (extraction only) ---
        ColumnLayout {
            visible: nodeData && nodeData.nodeType === 3
            Layout.fillWidth: true
            spacing: 4

            Text {
                text: "Purity"
                color: "#aaa"
                font.pixelSize: 11
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: 4

                Repeater {
                    model: [
                        {
                            label: "Impure",
                            value: 0
                        },
                        {
                            label: "Normal",
                            value: 1
                        },
                        {
                            label: "Pure",
                            value: 2
                        }
                    ]
                    delegate: Button {
                        required property var modelData
                        Layout.fillWidth: true
                        text: modelData.label
                        checkable: true
                        checked: nodeData && nodeData.purity === modelData.value
                        ButtonGroup.group: purityGroup
                        onClicked: if (nodeData)
                            nodeData.purity = modelData.value

                        background: Rectangle {
                            color: parent.checked ? "#e8a020" : "#2a3a4a"
                            border.color: parent.checked ? "#e8a020" : "#555"
                            border.width: 1
                            radius: 4
                        }
                        contentItem: Text {
                            text: parent.text
                            color: parent.checked ? "#1a1a1a" : "#aaaaaa"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 11
                            font.bold: parent.checked
                        }
                    }
                }
                ButtonGroup {
                    id: purityGroup
                }
            }

            // --- tier (only if more than one tier exists) ---
            Text {
                visible: nodeData && nodeData.tierCount > 1
                text: "Mark"
                color: "#aaa"
                font.pixelSize: 11
            }
            RowLayout {
                visible: nodeData && nodeData.tierCount > 1
                Layout.fillWidth: true
                spacing: 4

                Repeater {
                    model: nodeData ? nodeData.tierCount : 0
                    delegate: Button {
                        required property int index
                        Layout.fillWidth: true
                        text: "Mk." + (index + 1)
                        checkable: true
                        checked: nodeData && nodeData.tier === index
                        ButtonGroup.group: tierGroup
                        onClicked: if (nodeData)
                            nodeData.tier = index

                        background: Rectangle {
                            color: parent.checked ? "#e8a020" : "#2a3a4a"
                            border.color: parent.checked ? "#e8a020" : "#555"
                            border.width: 1
                            radius: 4
                        }
                        contentItem: Text {
                            text: parent.text
                            color: parent.checked ? "#1a1a1a" : "#aaaaaa"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 11
                            font.bold: parent.checked
                        }
                    }
                }
                ButtonGroup {
                    id: tierGroup
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 4
            Text {
                text: "Somersloop"
                color: "#aaa"
                font.pixelSize: 11
            }
            Item {
                Layout.fillWidth: true
                height: 30
                Rectangle {
                    id: somersloopDec1
                    anchors.left: parent.left
                    width: somersloopField.height
                    height: somersloopField.height
                    radius: 4
                    TapHandler {
                        onTapped: SceneManager.setSomersloopCount(root.nodeData, root.somersloopCount - 1)
                    }
                    Text {
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: "-1"
                    }
                }
                Text {
                    id: somersloopField
                    anchors.left: somersloopDec1.right
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "sl: " + root.somersloopCount + "/" + root.somersloopSlotSize
                }
                // TextField {
                //     id: somersloopField
                //     anchors.left: somersloopDec1.right
                //     anchors.horizontalCenter: parent.horizontalCenter
                //     placeholderText: "no limit"
                //     inputMethodHints: Qt.ImhFormattedNumbersOnly
                //     background: Rectangle {
                //         color: "#2a3a4a"
                //         radius: 4
                //     }
                //     color: "white"
                //     // onEditingFinished: {
                //     //     let val = parseFloat(text);
                //     //     if (!isNaN(val))
                //     //         SceneManager.setMachineLimit(root.nodeData, val);
                //     // }
                // }
                Rectangle {
                    width: somersloopField.height
                    height: somersloopField.height
                    color: "yellow"
                    anchors.right: parent.right
                    radius: 4
                    TapHandler {
                        onTapped: SceneManager.setSomersloopCount(root.nodeData, root.somersloopCount + 1)
                    }
                    Text {
                        anchors.fill: parent
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        text: "+1"
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: "#444"
        }

        // --- enter factory (factory only) ---
        Button {
            visible: nodeData && nodeData.nodeType === 2
            text: "Enter Factory"
            Layout.fillWidth: true
            // onClicked: { /* TODO */ root.close() }
        }

        // --- delete ---
        Button {
            text: "Delete"
            Layout.fillWidth: true
            onClicked: {
                SceneManager.deleteNode(nodeData);
                root.close();
            }
            // onClicked: { /* TODO */ root.close() }
        }
    }
}
