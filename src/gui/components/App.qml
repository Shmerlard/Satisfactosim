
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import FACTORY_QT

Item {
    anchors.fill: parent

    property alias zoomScale: workspace.zoomScale
    property alias contentX:  workspace.contentX
    property alias contentY:  workspace.contentY

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

        Workspace {
            id: workspace
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
