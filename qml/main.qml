import QtQuick 2.9
import QtQuick.Controls 2.2

import "ForumViews"

ApplicationWindow {
    visible: true
    flags: Qt.Window | Qt.WindowFullscreenButtonHint
    visibility: "Maximized"
    width: 1080
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

        Connections {
            target: forumController
            onForumOpened: swipeView.setCurrentIndex(1)
            onBoardOpened: swipeView.setCurrentIndex(2)
            onThreadOpened: swipeView.setCurrentIndex(3)
        }
    }
}
