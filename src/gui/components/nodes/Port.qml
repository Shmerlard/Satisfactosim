import QtQuick
import QtQuick.Layouts

Item {
    id: root
    property var portData
    property bool isInput: false
    property Item nodeRoot: null
    property Item contentContainer: null
    implicitWidth: mainLayout.width + 5
    implicitHeight: mainLayout.height + 5
    z: 1
    function updateOffset() {
        if (!nodeRoot)
            return;
        var pt = isInput ? mapToItem(nodeRoot, 0, mainLayout.y + mainLayout.height / 2) : mapToItem(nodeRoot, width, mainLayout.y + mainLayout.height / 2);
        sceneManager.setPortOffset(portData.nodeIndex, portData.portIndex, pt);
    }
    Component.onCompleted: updateOffset()
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
            text: root.isInput ? "INPUT" : "OUTPUT"
            color: "white"
        }
    }
    MouseArea {
        id: mouseArea

        anchors.fill: parent
        hoverEnabled: true
        preventStealing: true
        acceptedButtons: Qt.LeftButton

        propagateComposedEvents: true
        onPressed: mouse => {
            var pt = mapToItem(contentContainer, isInput ? 0 : width, mainLayout.y + mainLayout.height / 2);
            contentContainer.startPortDrag(portData.nodeIndex, portData.portIndex, pt.x, pt.y);
        }

        onPositionChanged: mouse => {
            if (!contentContainer.pendingConn)
                return;
            var pt = mapToItem(contentContainer, mouse.x, mouse.y);
            contentContainer.pendingMouseX = pt.x;
            contentContainer.pendingMouseY = pt.y;
        }

        onReleased: mouse => {
            var pt = mapToItem(contentContainer, mouse.x, mouse.y);
            contentContainer.endPortDrag(pt.x, pt.y);
        }
        // onPressed: mouse => {
        //     console.log("port pressed", portData.nodeIndex, portData.portIndex);
        //     var pt = mapToItem(contentContainer, isInput ? 0 : width, mainLayout.y + mainLayout.height / 2);
        //     contentContainer.pendingConn = {
        //         srcNodeIdx: portData.nodeIndex,
        //         srcPortIdx: portData.portIndex,
        //         startX: pt.x,
        //         startY: pt.y
        //     };
        //     contentContainer.pendingMouseX = pt.x;
        //     contentContainer.pendingMouseY = pt.y;
        // }
        //
        // onPositionChanged: mouse => {
        //     // console.log("position changed", contentContainer.pendingConn)
        //     if (!contentContainer.pendingConn)
        //         return;
        //     var pt = mapToItem(contentContainer, mouse.x, mouse.y);
        //     contentContainer.pendingMouseX = pt.x;
        //     contentContainer.pendingMouseY = pt.y;
        // }
        //
        // onReleased: mouse => {
        //     console.log("released, pendingConn:", JSON.stringify(contentContainer.pendingConn), "pendingTarget:", JSON.stringify(contentContainer.pendingTarget));
        //     if (contentContainer.pendingConn && contentContainer.pendingTarget) {
        //         var src = contentContainer.pendingConn;
        //         var dst = contentContainer.pendingTarget;
        //         sceneManager.connectNodes(src.srcNodeIdx, src.srcPortIdx, dst.nodeIndex, dst.portIndex);
        //     }
        //     contentContainer.pendingConn = null;
        //     contentContainer.pendingTarget = null;
        // }
        //
        // HoverHandler {
        //     onHoveredChanged: {
        //         if (!contentContainer || !contentContainer.pendingConn)
        //             return;
        //         contentContainer.pendingTarget = hovered ? portData : null;
        //     }
        // }
        // onContainsMouseChanged: {
        //     onContainsMouseChanged: {
        //         console.log("containsMouse:", containsMouse, "node:", portData.nodeIndex, "port:", portData.portIndex);
        //         if (!contentContainer || !contentContainer.pendingConn)
        //             return;
        //         contentContainer.pendingTarget = containsMouse ? portData : null;
        //     }
        //     if (!contentContainer || !contentContainer.pendingConn)
        //         return;
        //     contentContainer.pendingTarget = containsMouse ? portData : null;
        // }
        // onPressed: mouse => {
        //     if (mouse.button === Qt.LeftButton) {
        //         console.log(root.portData);
        //     }
        // }
    }
}
