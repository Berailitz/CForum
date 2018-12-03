import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

RowLayout {
    width: 1080
    Layout.preferredWidth: 1080
    visible: !model.isRemoved
    height: model.isRemoved ? 0 : implicitHeight
    Text {
        text: qsTr("#") + model.id + " " + model.title + qsTr(" -- by: ") + forumController.getUsername(model.authorID) + ", " + model.time
        Layout.preferredWidth: 850
        font.family: "dengxian"
        font.pointSize: 20
    }

    Button {
        visible: forumController.canRemovePost(model.id) && (forumController.isModerator || model.authorID === forumController.userID)
        text: qsTr("❌")
        Layout.preferredWidth: 100
        font.family: "dengxian"
        font.pointSize: 20
        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        onClicked: forumController.removePost(model.id)
    }

    Button {
        text: qsTr("🡲")
        Layout.preferredWidth: 100
        font.family: "dengxian"
        font.pointSize: 20
        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        onClicked: forumController.viewPost(model.id)
    }
}
