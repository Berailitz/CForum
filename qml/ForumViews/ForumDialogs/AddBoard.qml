import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

Dialog {
    title: qsTr("新建版面")
    standardButtons: Dialog.Ok | Dialog.Cancel
    TextField {
        id: boardNameTextField
        font.family: "dengxian"
        placeholderText: qsTr("版主名")
        font.pointSize: 20
    }

    onAccepted: {
        forumController.addBoard(boardNameTextField.text)
        boardNameTextField.text = ""
    }
}
