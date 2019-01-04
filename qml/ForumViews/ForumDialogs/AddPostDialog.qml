import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

Dialog {
    id: addPostDialog
    title: qsTr("发布主题帖")
    contentItem: ColumnLayout {
        RowLayout {
            Layout.preferredWidth: 1080
            Text {
                text: qsTr("标题")
                Layout.preferredWidth: 100
                horizontalAlignment: Text.AlignHCenter
                font.family: "dengxian"
                font.pointSize: 20
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            }

            TextField {
                id: postTitleTextField
                Layout.preferredWidth: 970
                font.family: "dengxian"
                placeholderText: qsTr("标题")
                font.pointSize: 20
                selectByMouse: true
            }
        }

        RowLayout {
            Layout.preferredWidth: 1080
            Layout.preferredHeight: 550
            Text {
                text: qsTr("正文")
                Layout.preferredWidth: 100
                horizontalAlignment: Text.AlignHCenter
                font.family: "dengxian"
                font.pointSize: 20
                Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
            }
            TextArea {
                    id: postContentTextArea
                    Layout.preferredWidth: 970
                    Layout.preferredHeight: parent.Layout.preferredHeight
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignHCenter
                    font.family: "dengxian"
                    placeholderText: qsTr("正文")
                    font.pointSize: 20
                    selectByMouse: true
                    background: Rectangle {
                            border.color: "#4d90fe"
                        }
                }
        }

        RowLayout {
            Layout.preferredWidth: 1080
            Layout.preferredHeight: 70
            Button {
                text: qsTr("取消")
                font.family: "dengxian"
                font.pointSize: 20
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    postTitleTextField.text = ""
                    postContentTextArea.text = ""
                    addPostDialog.close()
                }
            }

            Button {
                text: qsTr("发布")
                font.family: "dengxian"
                font.pointSize: 20
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    forumController.addPost(postTitleTextField.text, postContentTextArea.text)
                    postTitleTextField.text = ""
                    postContentTextArea.text = ""
                    addPostDialog.close()
                }
            }
        }
    }

}
