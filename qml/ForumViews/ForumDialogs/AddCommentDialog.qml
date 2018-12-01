import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

Dialog {
    id: addCommentDialog
    visible: false
    title: qsTr("发布回复帖")
    contentItem: ColumnLayout {
        RowLayout {
            TextArea {
                    id: commentContentTextField
                    Layout.preferredWidth: 1080
                    Layout.preferredHeight: 580
                    font.family: "dengxian"
                    placeholderText: qsTr("回复")
                    font.pointSize: 20
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
                    commentContentTextField.text = ""
                    addCommentDialog.close()
                }
            }

            Button {
                text: qsTr("发布")
                font.family: "dengxian"
                font.pointSize: 20
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                onClicked: {
                    forumController.addComment(commentContentTextField.text)
                    commentContentTextField.text = ""
                    addCommentDialog.close()
                }
            }
        }
    }
}
