#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "controller.h"

using namespace std;

int main(int argc, char *argv[])
{
	// _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
    QGuiApplication app(argc, argv);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    QQmlApplicationEngine engine;
    cforum::Controller forumController(engine);
    forumController.registerUser("admin", "admin");
    forumController.addBoard("TestBoard");
    forumController.postThread("Test thread", "This is content.");
    forumController.postComment("Comment 1");
    forumController.setModerator("admin");
    forumController.refreshViews();
	// forumController.load("data");
//	forumController.save("data");
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    return app.exec();
}
