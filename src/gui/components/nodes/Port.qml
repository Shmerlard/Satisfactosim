// pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import FACTORY_QT

Item {
    id: root

    property var portData
    property bool isInput: false
    property Item nodeRoot: null
    property Item contentContainer: null

    // FIX: limit lenth of th text, use fraction text instead
    implicitWidth: 50
    implicitHeight: 30
    z: 1

    // --- functions ---

    function connectionPoint() {
        var edgeX = isInput ? 0 : width;
        var centerY = mainLayout.y + mainLayout.height / 2;
        return mapToItem(contentContainer, edgeX, centerY);
    }

    function updateOffset() {
        if (!nodeRoot || !portData)
            return;
        var edgeX = isInput ? 0 : width;
        var centerY = mainLayout.y + mainLayout.height / 2;
        var pt = mapToItem(nodeRoot, edgeX, centerY);
        SceneManager.setPortOffset(portData.nodeIndex, portData.portIndex, pt);
    }

    // --- lifecycle ---

    Component.onCompleted: updateOffset()

    Component.onDestruction: {
        if (contentContainer) {
            var idx = contentContainer.portItems.indexOf(root);
            if (idx >= 0)
                contentContainer.portItems.splice(idx, 1);
        }
    }

    // --- offset tracking ---

    onXChanged: updateOffset()
    onYChanged: updateOffset()

    Connections {
        target: parent  // the Column
        function onYChanged() {
            root.updateOffset();
        }
    }

    // --- signals ---

    onContentContainerChanged: {
        if (contentContainer)
            contentContainer.portItems.push(root);
    }

    // --- visuals ---

    Rectangle {
        id: background
        anchors.fill: parent
        color: root.isInput ? Theme.portInput : Theme.portOutput
        ToolTip.visible: hoverHandler_i.hovered
        ToolTip.text: root.portData.itemName
        HoverHandler {
            id: hoverHandler_i
        }
    }

    Row {
        id: mainLayout
        layoutDirection: root.isInput ? Qt.LeftToRight : Qt.RightToLeft
        anchors.left: root.isInput ? parent.left : undefined
        anchors.right: root.isInput ? undefined : parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 4
        Image {
            source: root.portData ? root.portData.iconUrl : ""
            width: 25
            height: 25
            anchors.verticalCenter: parent.verticalCenter
            fillMode: Image.PreserveAspectFit
        }
        Text {
            text: root.portData ? root.portData.amount : ""
            color: Theme.textPrimary
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
