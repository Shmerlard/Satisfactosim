// pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FACTORY_QT

Popup {
    id: root
    property real spawnX: 0
    property real spawnY: 0
    property var sourcePort: null   // set when opened by port drag

    function openForPort(portData, isInput) {
        sourcePort = {
            nodeIndex: portData.nodeIndex,
            portIndex: portData.portIndex,
            isInput:   isInput
        };
        // dragging from output → want recipes consuming this item (Inputs=1)
        // dragging from input  → want recipes producing this item (Outputs=2)
        var mode = isInput ? 2 : 1;
        SceneManager.recipes.setFilterString(portData.itemName);
        SceneManager.recipes.setFilterMode(mode);
        searchBox.currentMode = mode;
    }

    onClosed: {
        sourcePort = null;
        SceneManager.recipes.setFilterString("");
        SceneManager.recipes.setFilterMode(0);
        searchBox.currentMode = 0;
    }

    width: 650
    height: 340
    clip: true

    background: Rectangle {
        color: "#2b2b2b"
        border.color: "#555555"
        radius: 6
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 8

        // Left panel: special node types
        ColumnLayout {
            Layout.preferredWidth: 100
            Layout.fillHeight: true
            spacing: 6

            Text {
                text: "Special Nodes"
                color: "#aaaaaa"
                font.pixelSize: 11
            }

            TextField {
                id: nodeNameField
                Layout.fillWidth: true
                placeholderText: "Name (optional)"
                font.pixelSize: 12
            }

            Button {
                // width: 3
                Layout.fillWidth: true
                text: "Sub-Factory"
                onClicked: {
                    SceneManager.createSubFactory(nodeNameField.text, root.spawnX, root.spawnY);
                    nodeNameField.text = "";
                    root.close();
                }
            }

            Button {
                Layout.fillWidth: true
                text: "Input Edge"
                onClicked: {
                    SceneManager.createEdgeNode(true, nodeNameField.text, root.spawnX, root.spawnY);
                    nodeNameField.text = "";
                    root.close();
                }
            }

            Button {
                Layout.fillWidth: true
                text: "Output Edge"
                onClicked: {
                    SceneManager.createEdgeNode(false, nodeNameField.text, root.spawnX, root.spawnY);
                    nodeNameField.text = "";
                    root.close();
                }
            }

            Item { Layout.fillHeight: true }
        }

        // Divider
        Rectangle {
            Layout.preferredWidth: 1
            Layout.fillHeight: true
            color: "#444444"
        }

        // Right panel: recipe search + list
        ColumnLayout {
            id: recipeSelector
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 4

            PlaceMenuSearchBox { id: searchBox }

            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                ScrollBar.vertical.policy: ScrollBar.AsNeeded

                ColumnLayout {
                    width: parent.width
                    spacing: 2

                    Repeater {
                        model: SceneManager.recipes

                        delegate: PlaceMenuEntry {
                            implicitWidth: recipeSelector.width
                            recipe: model
                            spawnX: root.spawnX
                            spawnY: root.spawnY
                            spawnMenu: root
                            sourcePort: root.sourcePort
                        }
                    }
                }
            }
        }
    }
}
