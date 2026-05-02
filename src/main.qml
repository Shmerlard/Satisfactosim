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

    // Platform.FileDialog {
    //     id: saveDialog
    //     title: "Save Factory"
    //     fileMode: Platform.FileDialog.SaveFile
    //     nameFilters: ["Factory files (*.json)", "All files (*)"]
    //     onAccepted: SessionManager.save(file)
    // }
    //
    // Platform.FileDialog {
    //     id: openDialog
    //     title: "Open Factory"
    //     fileMode: Platform.FileDialog.OpenFile
    //     nameFilters: ["Factory files (*.json)", "All files (*)"]
    //     onAccepted: SessionManager.load(file)
    // }
    //
    // menuBar: MenuBar {
    //     Menu {
    //         title: "File"
    //         MenuItem {
    //             text: "Save..."
    //             onClicked: saveDialog.open()
    //         }
    //         MenuItem {
    //             text: "Open..."
    //             onClicked: openDialog.open()
    //         }
    //         MenuSeparator {}
    //         MenuItem {
    //             text: "Exit"
    //             onClicked: Qt.quit()
    //         }
    //     }
    //     Menu {
    //         title: "Factory"
    //         MenuItem {
    //             text: "Go to Root"
    //             onClicked: SessionManager.goToRoot()
    //         }
    //         MenuItem {
    //             text: "Go Up"
    //             enabled: SessionManager.activeFactory !== SessionManager.rootFactory
    //             onClicked: SessionManager.goParentFactory()
    //         }
    //     }
    //     Menu {
    //         title: "View"
    //         MenuItem {
    //             text: "Reload UI (F5)"
    //             onClicked: reloadTimer.start()
    //         }
    //         MenuItem {
    //             text: "Reset Zoom"
    //             // Assuming you expose a way to reach the workspace
    //         }
    //     }
    // }
    // footer: Rectangle {
    //     height: 24
    //     color: "#2b2b2b"
    //
    //     Text {
    //         anchors.right: parent.right
    //         anchors.verticalCenter: parent.verticalCenter
    //         anchors.rightMargin: 8
    //         color: "#cccccc"
    //         font.pixelSize: 12
    //         text: uiLoader.item
    //               ? ("%1, %2").arg(uiLoader.item.mouseContentX).arg(uiLoader.item.mouseContentY)
    //               : ""
    //     }
    // }

    Loader {
        id: uiLoader
        anchors.fill: parent
        source: "gui/components/App.qml"
    }
    Shortcut {
        sequence: "F5"
        onActivated: {
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
            uiLoader.source = "gui/components/App.qml";
        }
    }
}
