import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT

Item {
    anchors.fill: parent

    property real savedZoom: 1.0
    property real savedContentX: 0
    property real savedContentY: 0

    readonly property real zoomScale: workspaceLoader.item ? workspaceLoader.item.zoomScale : 1.0
    readonly property real contentX:  workspaceLoader.item ? workspaceLoader.item.contentX  : 0
    readonly property real contentY:  workspaceLoader.item ? workspaceLoader.item.contentY  : 0

    onSavedZoomChanged:     if (workspaceLoader.item) workspaceLoader.item.zoomScale = savedZoom
    onSavedContentXChanged: if (workspaceLoader.item) workspaceLoader.item.contentX  = savedContentX
    onSavedContentYChanged: if (workspaceLoader.item) workspaceLoader.item.contentY  = savedContentY

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Loader {
            id: workspaceLoader
            Layout.fillWidth: true
            Layout.fillHeight: true
            source: "Workspace.qml?v=" + Date.now()
        }
    }
}
