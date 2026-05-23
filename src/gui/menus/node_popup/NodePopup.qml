pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT
import gui.theme
import gui.controls

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
        // limitField.text = nodeData && (nodeData.nodeType === 3 || nodeData.nodeType === 4) ? String(nodeData.machineLimit) : "";
        // overclockField.text = nodeData && (nodeData.nodeType === 3 || nodeData.nodeType === 4) ? String(Math.round(nodeData.overclock * 100)) : "";
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

        // --- rename ---
        Text {
            text: "Rename"
            color: Theme.textMuted
            font.pixelSize: 11
        }
        TextField {
            id: renameField
            Layout.fillWidth: true
            placeholderText: root.nodeData ? root.nodeData.name : ""
            background: Rectangle {
                color: Theme.surfaceInput
                radius: 4
            }
            color: Theme.textPrimary
            onTextChanged: {
                root.nodeData.name = text;
            }
        }

        // --- machine limit (production + extraction) ---
        Loader {
            id: limitLoader
            Layout.fillWidth: true
            active: root.nodeData && root.nodeData.isMachine

            sourceComponent: LimitSection {
                nodeData: root.nodeData
                width: limitLoader.width
            }
        }

        // --- overclock ----
        Loader {
            id: overclockLoader
            Layout.fillWidth: true
            active: root.nodeData && root.nodeData.isMachine
            sourceComponent: OverclockSection {
                nodeData: root.nodeData
                width: overclockLoader.width
            }
        }

        // --- purity ---
        Loader {
            id: purityLoader
            Layout.fillWidth: true
            active: root.nodeData && root.nodeData.nodeType === 3
            sourceComponent: PuritySection {
                nodeData: root.nodeData
                width: purityLoader.width
            }
        }
        // --- tiers ---
        Loader {
            id: tiersLoader
            Layout.fillWidth: true
            active: root.nodeData && root.nodeData.nodeType === 3 && root.nodeData.tierCount > 1
            sourceComponent: TierSection {
                nodeData: root.nodeData
                width: tiersLoader.width
            }
        }
        // --- splitter weights ---
        Loader {
            id: weightsLoader
            Layout.fillWidth: true
            active: root.nodeData && root.nodeData.nodeType === 5
            sourceComponent: WeightsSection {
                nodeData: root.nodeData
                width: weightsLoader.width
            }
        }

        Rectangle {
            Layout.fillWidth: true
            height: 1
            color: Theme.border
        }

        // --- enter factory (factory only) ---
        GButton {
            visible: root.nodeData && root.nodeData.nodeType === 2
            text: "Enter Factory"
            icon: "image://assets/misc/log-in.png"
            Layout.fillWidth: true
            onClicked: {
                SceneManager.enterFactory(root.nodeData);
                root.close();
            }
        }

        // --- delete ---
        GButton {
            text: "Delete"
            icon: "image://assets/misc/trash.png"
            onClicked: {
                SceneManager.deleteNode(root.nodeData);
                root.close();
            }
        }
    }
}
