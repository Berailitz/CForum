import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

Dialog {
    title: qsTr("发布主题帖")
    standardButtons: Dialog.Ok | Dialog.Cancel
    ColumnLayout {
        RowLayout {
            Text {
                text: qsTr("标题")
                font.family: "dengxian"
                font.pointSize: 20
            }

            TextField {
                id: postTitleTextField
                font.family: "dengxian"
                placeholderText: qsTr("标题")
                font.pointSize: 20
            }
        }

        RowLayout {
            Text {
                text: qsTr("正文")
                font.family: "dengxian"
                font.pointSize: 20
            }

            TextField {
                id: postContentTextField
                font.family: "dengxian"
                placeholderText: qsTr("正文")
                font.pointSize: 20
            }
        }
    }

    onAccepted: {
        forumController.postPost(postTitleTextField.text, postContentTextField.text)
        postTitleTextField.text = ""
        postContentTextField.text = ""
    }
}
