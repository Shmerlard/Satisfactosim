
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT

Item {
    anchors.fill: parent

    // readonly property real mouseContentX: workspace.mouseContentX
    // readonly property real mouseContentY: workspace.mouseContentY

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 200
            Layout.fillHeight: true
            color: "#2b2b2b"
            // color: "blue"

                Text {
                    // text: GameLibrary.recipesList[10].recipeName
                    color: "white"
                }
        }

        // Workspace {
        //     id: workspace
        //     anchors.fill: parent
        //     Layout.fillWidth: true
        //     Layout.fillHeight: true
        //     sceneSize: 5000
        // }
    }
}
