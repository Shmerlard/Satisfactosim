// pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
import FACTORY_QT

Item {
    id: root

    property var portData
    property bool isInput: false
    property Item nodeRoot: null
    property Item contentContainer: null

    implicitWidth: mainLayout.width + 5
    implicitHeight: mainLayout.height + 5
    z: 1

    // --- functions ---

    function connectionPoint() {
        var edgeX = isInput ? 0 : width;
        var centerY = mainLayout.y + mainLayout.height / 2;
        return mapToItem(contentContainer, edgeX, centerY);
    }

    function updateOffset() {
        if (!nodeRoot)
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
        function onYChanged() { root.updateOffset() }
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
        color: root.isInput ? "#0d3349" : "#3d1a0d"
    }

    RowLayout {
        id: mainLayout
        layoutDirection: root.isInput ? Qt.LeftToRight : Qt.RightToLeft

        Image {
            source: root.portData.iconUrl
            Layout.preferredWidth: 15
            Layout.preferredHeight: 15
            fillMode: Image.PreserveAspectFit
        }
        Text {
            text: root.portData.amount
            color: "white"
        }
    }
}
