import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform as Platform
import FACTORY_QT

ApplicationWindow {
    id: appWindow
    width: 1200
    height: 800
    visible: true

    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem {
                text: "Save..."
                onClicked: saveDialog.open()
            }
            MenuItem {
                text: "Open..."
                onClicked: openDialog.open()
            }
            MenuSeparator {}
            MenuItem {
                text: "Exit"
                onClicked: Qt.quit()
            }
        }
        Menu {
            title: "Factory"
            MenuItem {
                text: "Go to Root"
                onClicked: SceneManager.enterRootFactory()
            }
            MenuItem {
                text: "Go Up"
                enabled: !SceneManager.isRootFactory
                onClicked: SceneManager.enterParentFactory()
            }
        }
        Menu {
            title: "View"
            MenuItem {
                text: "Reload UI (F5)"
                onClicked: reloadTimer.start()
            }
        }
    }

    property real savedZoom: 1.0
    property real savedContentX: 0
    property real savedContentY: 0

    Loader {
        id: uiLoader
        anchors.fill: parent
        source: "gui/App.qml"
        // qmllint disable missing-property
        readonly property real zoomScale: item ? item.zoomScale : 1.0
        readonly property real contentX: item ? item.contentX : 0
        readonly property real contentY: item ? item.contentY : 0
        onLoaded: {
            item.savedZoom = appWindow.savedZoom;
            item.savedContentX = appWindow.savedContentX;
            item.savedContentY = appWindow.savedContentY;
        }
        // qmllint enable missing-property
    }
    Shortcut {
        sequence: "F5"
        onActivated: {
            if (uiLoader.item) {
                appWindow.savedZoom = uiLoader.zoomScale;
                appWindow.savedContentX = uiLoader.contentX;
                appWindow.savedContentY = uiLoader.contentY;
            }
            uiLoader.source = "";
            reloadTimer.restart();
        }
    }

    Timer {
        id: reloadTimer
        interval: 50
        repeat: false
        onTriggered: {
            reloader.clearCache();
            uiLoader.source = "gui/App.qml?v=" + Date.now();
        }
    }

    Platform.FileDialog {
        id: saveDialog
        title: "Save Factory"
        fileMode: Platform.FileDialog.SaveFile
        nameFilters: ["Factory files (*.json)", "All files (*)"]
        defaultSuffix: "json"
        onAccepted: SceneManager.save(file.toString())
    }

    Platform.FileDialog {
        id: openDialog
        title: "Open Factory"
        fileMode: Platform.FileDialog.OpenFile
        nameFilters: ["Factory files (*.json)", "All files (*)"]
        onAccepted: SceneManager.load(file.toString())
    }
}
