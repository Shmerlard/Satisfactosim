import QtQuick

QtObject {
    // workspace
    readonly property color background:         "#1a1a1a"
    readonly property color gridLine:           "#333333"
    readonly property color overlayBg:          "#88000000"

    // surfaces
    readonly property color surface:            "#1e2838"
    readonly property color surfaceInput:       "#2a3a4a"
    readonly property color surfaceButton:      "#2a2a2a"
    readonly property color surfaceMenu:        "#2b2b2b"
    readonly property color surfaceNotification:"#cc2a1a1a"

    // node surfaces
    readonly property color surfaceFactory:     "#2d1e3d"
    readonly property color surfaceExtraction:  "#1a2e1a"

    // accents
    readonly property color accent:             "#e8a020"
    readonly property color accentFactory:      "#8e44ad"
    readonly property color accentExtraction:   "#27ae60"
    readonly property color selection:          "#4fc3f7"
    readonly property color selectionFill:      "#224fc3f7"
    readonly property color warning:            "#f0c040"
    readonly property color error:              "#e05050"
    readonly property color badge:              "#6a1bd8"

    // ports
    readonly property color portInput:          "#0d3349"
    readonly property color portOutput:         "#3d1a0d"

    // text
    readonly property color textPrimary:        "white"
    readonly property color textMuted:          "#aaa"
    readonly property color textOnAccent:       "#1a1a1a"
    readonly property color textNotification:   "#f0f0f0"

    // borders
    readonly property color border:             "#444"
    readonly property color borderLight:        "#555"
    readonly property color borderFocus:        "#666"

    // misc
    readonly property color iconBg:             "#808080"
}
