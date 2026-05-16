import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT

Popup {
    id: root

    property var nodeData: null

    width: 220
    padding: 14
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

    onOpened: {
        renameField.clear() // FIX: doesnt clear
        limitField.text = nodeData && (nodeData.nodeType === 3 || nodeData.nodeType === 4)
            ? String(nodeData.machineLimit) : ""
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

        Rectangle { Layout.fillWidth: true; height: 1; color: "#444" }

        // --- rename ---
        Text { text: "Rename"; color: "#aaa"; font.pixelSize: 11 }
        TextField {
            id: renameField
            Layout.fillWidth: true
            placeholderText: nodeData ? nodeData.name : ""
            background: Rectangle { color: "#2a3a4a"; radius: 4 }
            color: "white"
            // onTextChanged: console.log(text)
            onTextChanged: {
                nodeData.name = text
                // nodeData.setName(text);
            }
            // onAccepted: { /* TODO */ root.close() }
        }

        // --- machine limit (production + extraction) ---
        ColumnLayout {
            visible: nodeData && (nodeData.nodeType === 3 || nodeData.nodeType === 4)
            Layout.fillWidth: true
            spacing: 4

            Text { text: "Machine Limit"; color: "#aaa"; font.pixelSize: 11 }
            TextField {
                id: limitField
                Layout.fillWidth: true
                placeholderText: "no limit"
                inputMethodHints: Qt.ImhFormattedNumbersOnly
                background: Rectangle { color: "#2a3a4a"; radius: 4 }
                color: "white"
                onEditingFinished: {
                    let val = parseFloat(text)
                    if (!isNaN(val))
                        SceneManager.setMachineLimit(nodeData, val)
                }
            }
        }

        // --- purity + tier (extraction only) ---
        ColumnLayout {
            visible: nodeData && nodeData.nodeType === 3
            Layout.fillWidth: true
            spacing: 4

            Text { text: "Purity"; color: "#aaa"; font.pixelSize: 11 }
            // TODO: purity selector (Impure / Normal / Pure)

            Text { text: "Tier"; color: "#aaa"; font.pixelSize: 11 }
            TextField {
                id: tierField
                Layout.fillWidth: true
                placeholderText: "tier..."
                inputMethodHints: Qt.ImhDigitsOnly
                background: Rectangle { color: "#2a3a4a"; radius: 4 }
                color: "white"
                // onAccepted: { /* TODO */ root.close() }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: "#444" }

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
                SceneManager.deleteNode(nodeData)
                root.close()
            }
            // onClicked: { /* TODO */ root.close() }
        }
    }
}
