// pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Layouts
// import Bridge
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

    function connectionPoint() {
        return mapToItem(contentContainer, isInput ? 0 : width, mainLayout.y + mainLayout.height / 2);
    }

    function updateOffset() {
        if (!nodeRoot) return;
        var pt = isInput ? mapToItem(nodeRoot, 0, mainLayout.y + mainLayout.height / 2)
                         : mapToItem(nodeRoot, width, mainLayout.y + mainLayout.height / 2);
        SceneManager.setPortOffset(portData.nodeIndex, portData.portIndex, pt);
    }

    Component.onCompleted: updateOffset()
    onContentContainerChanged: {
        if (contentContainer)
            contentContainer.portItems.push(root);
    }
    Component.onDestruction: {
        if (contentContainer) {
            var idx = contentContainer.portItems.indexOf(root);
            if (idx >= 0) contentContainer.portItems.splice(idx, 1);
        }
    }
    onYChanged: updateOffset()

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
