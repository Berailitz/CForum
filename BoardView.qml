import QtQuick 2.9
import QtQuick.Controls 2.2

Page {
    ListView {
        model: threadListModel
        delegate: Rectangle {
            Text {
                text: content
            }
        }
    }
}
