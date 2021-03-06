import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "ForumItems"
import "ForumDialogs"

Page {
    Layout.fillWidth: true
    Layout.preferredWidth: 1920
    Layout.preferredHeight: 1080
    Flickable {
        width: 1280
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.horizontalCenter: parent.horizontalCenter
        ColumnLayout{
            anchors.top: parent.top
            anchors.topMargin: 200
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                text: forumController.greeting
                font.pointSize: 20
                font.family: "dengxian"
                Layout.preferredWidth: 1080
                width: 1080
                onTextChanged: {
                    width = 1080
                    wrapMode = Text.Wrap
                }
            }

            ListView {
                id: boardListView
                Layout.alignment: Qt.AlignHCenter
                Layout.preferredWidth: 1080
                Layout.fillWidth: true
                model: boardListModel
                delegate: BoardItem {}
            }
        }

        ScrollBar.vertical: ScrollBar {}
    }

    RoundButton {
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.left: parent.left
        anchors.leftMargin: 100
        radius: 50
        text: qsTr("🡰")
        bottomPadding: 0
        topPadding: 0
        rightPadding: 10
        leftPadding: 10
        font.pointSize: 50
        font.family: "dengxian"
        onClicked: {
            forumController.logout()
            swipeView.setCurrentIndex(0)
        }
    }

    RoundButton {
        visible: forumController.isAdmin
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.right: parent.right
        anchors.rightMargin: 100
        radius: 50
        text: qsTr("➕")
        font.pointSize: 50
        font.family: "dengxian"
        onClicked: addBoardDialog.open()
    }

    AddBoard {
        id: addBoardDialog
    }

    Connections {
        target: forumController
        onForumOpened: {
            swipeView.setCurrentIndex(1)
            boardListView.height = boardListView.contentHeight
        }
    }

    Connections {
        target: appWindow
        onAfterSynchronizing: {
            boardListView.height = boardListView.contentHeight
        }
    }
}
