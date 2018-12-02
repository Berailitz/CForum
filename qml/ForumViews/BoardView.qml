import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

import "ForumItems"
import "ForumDialogs"

Page {
    Layout.fillWidth: true
    Layout.preferredWidth: 1080
    Layout.preferredHeight: 720
    Flickable {
        width: parent.width
        height: parent.height
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.horizontalCenter: parent.horizontalCenter
        ColumnLayout{
            anchors.top: parent.top
            anchors.topMargin: 200
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                text: forumController.boardName
                font.pointSize: 20
                font.family: "dengxian"
                height: 60
                width: 1080
                Layout.fillWidth: true
            }

            ListView {
                id: postListView
                Layout.alignment: Qt.AlignHCenter
                width: 1080
                Layout.fillWidth: true
                model: postListModel
                delegate: PostItem {}
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
        onClicked: forumController.viewForum()
    }

    RoundButton {
        id: setModeratorButton
        visible: forumController.isAdmin
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.right: parent.right
        anchors.rightMargin: 300
        radius: 50
        text: qsTr("⚙")
        font.pointSize: 50
        font.family: "dengxian"
        onClicked: setModeratorPopup.open()
    }

    RoundButton {
        visible: !forumController.isAdmin
        anchors.top: parent.top
        anchors.topMargin: 100
        anchors.right: parent.right
        anchors.rightMargin: 100
        radius: 50
        text: qsTr("➕")
        font.pointSize: 50
        font.family: "dengxian"
        onClicked: newPostDialog.open()
    }

    AddPostDialog {
        id: newPostDialog
    }

    Popup {
        id: setModeratorPopup
        x: setModeratorButton.x - width - 50
        y: setModeratorButton.y
        height: contentHeight + 20
        width: contentWidth + 20
        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.fillHeight: true
            RowLayout {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: true
                Layout.fillHeight: true
                TextField {
                    id: moderatorUsernameTextField
                    font.family: "dengxian"
                    placeholderText: qsTr("用户名")
                    font.pointSize: 20
                }
            }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                Layout.fillWidth: true
                Layout.fillHeight: true
                Button {
                    text: qsTr("设置为版主")
                    font.family: "dengxian"
                    font.pointSize: 20
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onClicked: {
                        forumController.setModerator(moderatorUsernameTextField.text)
                        moderatorUsernameTextField.text = ""
                    }
                }

                Button {
                    text: qsTr("撤销版主")
                    font.family: "dengxian"
                    font.pointSize: 20
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    onClicked: {
                        forumController.removeModerator(moderatorUsernameTextField.text)
                        moderatorUsernameTextField.text = ""
                    }
                }
            }
        }
    }

    Connections {
        target: forumController
        onBoardOpened: {
            swipeView.setCurrentIndex(2)
            postListView.height = postListView.contentHeight
        }
    }

    Connections {
        target: appWindow
        onAfterSynchronizing: {
            postListView.height = postListView.contentHeight
        }
    }
}
