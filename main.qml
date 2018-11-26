import QtQuick 2.9
import QtQuick.Controls 2.2

ApplicationWindow {
    visible: true
    width: 640
    height: 480

    ListView {
        width: 100
        height: 100

        model: boardListModel
        delegate: Rectangle {
            height: 25
            width: 100
            Text {
                text: name
            }
        }
    }
}
