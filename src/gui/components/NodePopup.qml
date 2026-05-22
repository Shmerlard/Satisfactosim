import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT
import gui.theme
import "./controls/"

Popup {
    id: root

    property var nodeData: null

    // FIX: only show somersloop for available

    readonly property int somersloopCount: {
        if (!nodeData)
            return 0;
        if (nodeData.nodeType !== 3 && nodeData.nodeType !== 4)
            return 0;
        return nodeData.somersloopCount;
    }
    readonly property int somersloopSlotSize: {
        if (!nodeData)
            return 0;
        if (nodeData.nodeType !== 3 && nodeData.nodeType !== 4)
            return 0;
        return nodeData.somersloopSlotSize;
    }
    readonly property bool somersloopAvailable: {
        if (somersloopSlotSize > 0)
            return true;
        return false;
    }

    width: 220
    padding: 14
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    onOpened: {
        limitField.text = nodeData && (nodeData.nodeType === 3 || nodeData.nodeType === 4) ? String(nodeData.machineLimit) : "";
        overclockField.text = nodeData && (nodeData.nodeType === 3 || nodeData.nodeType === 4) ? String(Math.round(nodeData.overclock * 100)) : "";
        renameField.text = nodeData ? nodeData.name : "";
    }

    background: Rectangle {
        color: Theme.surface
        border.color: Theme.accent
        border.width: 1
        radius: 8
    }

    ColumnLayout {
        width: parent.width
        spacing: 10

        // --- header ---
        Text {
            text: nodeData ? nodeData.name : ""
            color: Theme.textPrimary
            font.bold: true
            font.pixelSize: 14
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Theme.border
        }

        // --- rename ---
        Text {
            text: "Rename"
            color: Theme.textMuted
            font.pixelSize: 11
        }
        TextField {
            id: renameField
            Layout.fillWidth: true
            placeholderText: nodeData ? nodeData.name : ""
            background: Rectangle {
                color: Theme.surfaceInput
                radius: 4
            }
            color: Theme.textPrimary
            onTextChanged: {
                nodeData.name = text;
            }
        }

        // --- machine limit (production + extraction) ---
        ColumnLayout {
            visible: nodeData && (nodeData.nodeType === 3 || nodeData.nodeType === 4)
            Layout.fillWidth: true
            spacing: 4

            Text {
                text: "Machine Limit"
                color: Theme.textMuted
                font.pixelSize: 11
            }
            Item {
                Layout.fillWidth: true
                height: 30
                Rectangle {
                    id: dec1
                    anchors.left: parent.left
                    width: limitField.height
                    height: limitField.height
                    radius: 4
                    color: Theme.surfaceButton
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
                        color: Theme.textPrimary
                    }
                }
                TextField {
                    id: limitField
                    anchors.left: dec1.right
                    anchors.horizontalCenter: parent.horizontalCenter
                    placeholderText: "no limit"
                    inputMethodHints: Qt.ImhFormattedNumbersOnly
                    background: Rectangle {
                        color: Theme.surfaceInput
                        radius: 4
                    }
                    color: Theme.textPrimary
                    onEditingFinished: {
                        let val = parseFloat(text);
                        if (!isNaN(val))
                            SceneManager.setMachineLimit(root.nodeData, val);
                    }
                }
                Rectangle {
                    width: limitField.height
                    height: limitField.height
                    color: Theme.warning
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
                        color: Theme.textOnAccent
                    }
                }
            }
        }

        // --- overclock (production + extraction) ---
        ColumnLayout {
            visible: nodeData && (nodeData.nodeType === 3 || nodeData.nodeType === 4)
            Layout.fillWidth: true
            spacing: 4

            Text {
                text: "Overclock (%)"
                color: Theme.textMuted
                font.pixelSize: 11
            }
            TextField {
                id: overclockField
                Layout.fillWidth: true
                placeholderText: "100"
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                background: Rectangle {
                    color: Theme.surfaceInput
                    radius: 4
                }
                color: Theme.textPrimary
                onEditingFinished: {
                    let val = parseFloat(text);
                    if (!isNaN(val))
                        SceneManager.setOverclock(root.nodeData, val / 100.0);
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
                color: Theme.textMuted
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
                        onClicked: if (nodeData)
                            SceneManager.setPurity(nodeData, modelData.value)

                        background: Rectangle {
                            color: parent.checked ? Theme.accent : Theme.surfaceInput
                            border.color: parent.checked ? Theme.accent : Theme.borderLight
                            border.width: 1
                            radius: 4
                        }
                        contentItem: Text {
                            text: parent.text
                            color: parent.checked ? Theme.textOnAccent : Theme.textMuted
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 11
                            font.bold: parent.checked
                        }
                    }
                }
            }

            // --- tier (only if more than one tier exists) ---
            Text {
                visible: nodeData && nodeData.tierCount > 1
                text: "Mark"
                color: Theme.textMuted
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
                        onClicked: if (nodeData)
                            SceneManager.setTier(nodeData, index)

                        background: Rectangle {
                            color: parent.checked ? Theme.accent : Theme.surfaceInput
                            border.color: parent.checked ? Theme.accent : Theme.borderLight
                            border.width: 1
                            radius: 4
                        }
                        contentItem: Text {
                            text: parent.text
                            color: parent.checked ? Theme.textOnAccent : Theme.textMuted
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.pixelSize: 11
                            font.bold: parent.checked
                        }
                    }
                }
            }
        }

        ColumnLayout {
            visible: root.somersloopAvailable
            Layout.fillWidth: true
            spacing: 4
            Text {
                text: "Somersloop"
                color: Theme.textMuted
                font.pixelSize: 11
            }
            Item {
                Layout.fillWidth: true
                height: 30
                GButton {
                    text: "-1"
                    onClicked: SceneManager.setSomersloopCount(root.nodeData, root.somersloopCount - 1)
                }
                // Rectangle {
                //     id: somersloopDec1
                //     anchors.left: parent.left
                //     width: somersloopField.height
                //     height: somersloopField.height
                //     radius: 4
                //     color: Theme.surfaceButton
                //     TapHandler {
                //         onTapped: SceneManager.setSomersloopCount(root.nodeData, root.somersloopCount - 1)
                //     }
                //     Text {
                //         anchors.fill: parent
                //         horizontalAlignment: Text.AlignHCenter
                //         verticalAlignment: Text.AlignVCenter
                //         text: "-1"
                //         color: Theme.textPrimary
                //     }
                // }
                Text {
                    id: somersloopField
                    // anchors.left: somersloopDec1.right
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: "sl: " + root.somersloopCount + "/" + root.somersloopSlotSize
                    color: Theme.textPrimary
                }
                GButton {
                    text: "+1"
                    onClicked: SceneManager.setSomersloopCount(root.nodeData, root.somersloopCount + 1)
                    anchors.right: parent.right
                }
                // Rectangle {
                //     width: somersloopField.height
                //     height: somersloopField.height
                //     color: Theme.warning
                //     anchors.right: parent.right
                //     radius: 4
                //     TapHandler {
                //         onTapped: SceneManager.setSomersloopCount(root.nodeData, root.somersloopCount + 1)
                //     }
                //     Text {
                //         anchors.fill: parent
                //         horizontalAlignment: Text.AlignHCenter
                //         verticalAlignment: Text.AlignVCenter
                //         text: "+1"
                //         color: Theme.textOnAccent
                //     }
                // }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Theme.border
        }

        // --- enter factory (factory only) ---
        GButton {
            visible: nodeData && nodeData.nodeType === 2
            text: "Enter Factory"
            icon: "image://assets/misc/log-in.png"
            Layout.fillWidth: true
            onClicked: {
                SceneManager.enterFactory(nodeData);
                root.close();
            }
        }

        // --- delete ---
        GButton {
            text: "Delete"
            icon: "image://assets/misc/trash.png"
            onClicked: {
                SceneManager.deleteNode(nodeData);
                root.close();
            }
        }
    }
}
