#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "controller.h"

using namespace std;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    QQmlContext *ctxt = engine.rootContext();
    cforum::Controller c;
    c.registerUser("admin", "admin");
    c.addBoard("TestBoard");
    c.postThread("Test thread", "This is content.");
    c.postComment("Comment 1");
    c.setModerator("admin");
	// c.load("data");
//	c.save("data");
    ctxt->setContextProperty("boardListModel", QVariant::fromValue(*c.cforum->boards));
    return app.exec();
}
