import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3

Dialog {
    title: qsTr("设置版主")
    standardButtons: Dialog.Ok | Dialog.Cancel
    TextField {
        id: moderatorUsernameTextField
        font.family: "dengxian"
        placeholderText: qsTr("版主用户名")
        font.pointSize: 20
    }

    onAccepted: {
        forumController.setModerator(moderatorUsernameTextField.text)
        moderatorUsernameTextField.text = ""
    }
}
