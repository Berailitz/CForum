import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

Dialog {
    title: qsTr("发布回复帖")
    standardButtons: Dialog.Ok | Dialog.Cancel
    TextField {
        id: commentContentTextField
        font.family: "dengxian"
        placeholderText: qsTr("回复")
        font.pointSize: 20
    }

    onAccepted: {
        forumController.postComment(user.id, board.id, thread.id, commentContentTextField.text)
        commentContentTextField.text = ""
    }
}
