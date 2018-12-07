import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

RowLayout {
    width: 1080
    visible: !model.isRemoved
    height: model.isRemoved ? 0 : implicitHeight
    Text {
        text: qsTr("#") + model.id + " " + model.content + qsTr(" -- by: ") + forumController.getUsername(model.authorID) + ", " + model.time
        font.family: "dengxian"
        font.pointSize: 20
        Layout.preferredWidth: 960
        width: 960
        onTextChanged: {
            width = 960
            wrapMode = Text.Wrap
        }
    }

    Button {
        visible: !model.isRemoved && (forumController.isModerator || model.authorID === forumController.userID)
        text: qsTr("❌")
        Layout.preferredWidth: 100
        font.family: "dengxian"
        font.pointSize: 20
        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        onClicked: forumController.removeComment(model.id)
    }
}
