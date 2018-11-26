import QtQuick 2.9
import QtQuick.Controls 2.2

ApplicationWindow {
    visible: true
    flags: Qt.Window | Qt.WindowFullscreenButtonHint
    visibility: "Maximized"
    width: 1280
    height: 720

    SwipeView {
        id: swipeView
        anchors.fill: parent

        HomePage {
        }

        ForumView {
        }

        BoardView {
        }

        ThreadView {
        }
    }
}
