pragma ComponentBehavior: Bound
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FACTORY_QT
import "../controls/"

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
        SceneManager.recipes.setItemClassFilter(portData.itemClass);
        SceneManager.recipes.setFilterMode(mode);
        SceneManager.recipes.setSearchRecipeName(false);
        searchBox.currentMode = mode;
        searchBox.recipeNameEnabled = false;
    }

    onClosed: {
        sourcePort = null;
        SceneManager.recipes.setItemClassFilter("");
        SceneManager.recipes.setFilterString("");
        SceneManager.recipes.setFilterMode(0);
        SceneManager.recipes.setSearchRecipeName(true);
        searchBox.clear();
        searchBox.currentMode = 0;
        searchBox.recipeNameEnabled = true;
    }

    width: 650
    height: 340
    clip: true

    background: Rectangle {
        color: Theme.surfaceMenu
        border.color: Theme.borderLight
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
                color: Theme.textMuted
                font.pixelSize: 11
            }

            TextField {
                id: nodeNameField
                Layout.fillWidth: true
                placeholderText: "Name (optional)"
                font.pixelSize: 12
            }

            GButton {
                // width: 3
                Layout.fillWidth: true
                text: "Sub-Factory"
                onClicked: {
                    SceneManager.createSubFactory(nodeNameField.text, root.spawnX, root.spawnY);
                    nodeNameField.text = "";
                    root.close();
                }
            }

            GButton {
                Layout.fillWidth: true
                text: "Input Edge"
                onClicked: {
                    SceneManager.createEdgeNode(true, nodeNameField.text, root.spawnX, root.spawnY);
                    nodeNameField.text = "";
                    root.close();
                }
            }

            GButton {
                Layout.fillWidth: true
                text: "Output Edge"
                onClicked: {
                    SceneManager.createEdgeNode(false, nodeNameField.text, root.spawnX, root.spawnY);
                    nodeNameField.text = "";
                    root.close();
                }
            }

            GButton {
                Layout.fillWidth: true
                text: "Splitter"
                onClicked: {
                    SceneManager.createSplitterNode(root.spawnX, root.spawnY);
                    root.close();
                }
            }

            Item { Layout.fillHeight: true }
        }

        // Divider
        Rectangle {
            Layout.preferredWidth: 1
            Layout.fillHeight: true
            color: Theme.border
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
                            required property var model
                            required property var modelData
                            required property int index
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
