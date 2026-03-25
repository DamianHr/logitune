import QtQuick

// Mouse device render — placeholder dark gradient shape with
// 7 invisible clickable button zones overlaid.
Item {
    id: root

    implicitWidth:  220
    implicitHeight: 300

    signal buttonClicked(int buttonId)

    // ── Mouse body ─────────────────────────────────────────────────────────
    Rectangle {
        id: mouseBody
        anchors.centerIn: parent
        width:  parent.implicitWidth
        height: parent.implicitHeight
        radius: 28

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#2C2C2E" }
            GradientStop { position: 1.0; color: "#1A1A1A" }
        }

        // Left/Right button divider line
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            y: 24
            width: 1
            height: 110
            color: "#3A3A3C"
            opacity: 0.7
        }

        // Scroll wheel
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            y: 50
            width: 18
            height: 56
            radius: 9
            color: "#3A3A3C"

            // Wheel grip lines
            Repeater {
                model: 4
                delegate: Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    y: 8 + index * 11
                    width: 12; height: 2
                    radius: 1
                    color: "#555555"
                }
            }
        }

        // Left side thumb zone bump
        Rectangle {
            x: -6
            y: parent.height * 0.40
            width: 16
            height: 60
            radius: 8
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#323234" }
                GradientStop { position: 1.0; color: "#222224" }
            }
        }

        // Top button (behind scroll)
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.horizontalCenterOffset: 24
            y: 30
            width: 20; height: 10
            radius: 5
            color: "#3A3A3C"
        }
    }

    // ── Button zone overlays (invisible hit areas) ─────────────────────────
    // Positions are approximate relative to the 220×300 mouse body.

    // Button 0 — Left click (top-left half)
    MouseArea {
        x: 10; y: 20
        width: 95; height: 120
        cursorShape: Qt.PointingHandCursor
        onClicked: root.buttonClicked(0)
        // Debug: uncomment to visualise zones
        // Rectangle { anchors.fill: parent; color: "red"; opacity: 0.2 }
    }

    // Button 1 — Right click (top-right half)
    MouseArea {
        x: 115; y: 20
        width: 95; height: 120
        cursorShape: Qt.PointingHandCursor
        onClicked: root.buttonClicked(1)
    }

    // Button 2 — Middle / scroll wheel click (center strip)
    MouseArea {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.horizontalCenterOffset: 0
        y: 50; width: 30; height: 68
        cursorShape: Qt.PointingHandCursor
        onClicked: root.buttonClicked(2)
    }

    // Button 3 — Back (thumb rear, left side lower)
    MouseArea {
        x: 0; y: 185
        width: 40; height: 40
        cursorShape: Qt.PointingHandCursor
        onClicked: root.buttonClicked(3)
    }

    // Button 4 — Forward (thumb front, left side upper)
    MouseArea {
        x: 0; y: 140
        width: 40; height: 40
        cursorShape: Qt.PointingHandCursor
        onClicked: root.buttonClicked(4)
    }

    // Button 5 — Thumb / gesture (left side middle)
    MouseArea {
        x: 0; y: 160
        width: 40; height: 24
        cursorShape: Qt.PointingHandCursor
        onClicked: root.buttonClicked(5)
    }

    // Button 6 — Top (right side behind scroll)
    MouseArea {
        x: 162; y: 24
        width: 36; height: 22
        cursorShape: Qt.PointingHandCursor
        onClicked: root.buttonClicked(6)
    }
}
