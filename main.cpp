#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "controller.h"
#include "cftools.h"

using namespace std;

int main(int argc, char *argv[])
{
	// _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
	int returnValue;
    QGuiApplication app(argc, argv);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    QQmlApplicationEngine engine;
    cforum::Controller forumController(engine);
	forumController.load("data");
 //   forumController.addUser("admin", "admin");
	//forumController.cforum->admins->insert(1); // admin
 //   forumController.addBoard("TestBoard");
 //   forumController.addPost("Test post", "This is content.");
 //   forumController.addComment("Comment 1");
 //   forumController.setModerator("admin");
    forumController.refreshViews();
    engine.load(QUrl(QString::fromUtf8("qrc:/qml/main.qml")));
	returnValue = app.exec();
	forumController.save("data");
	return returnValue;
}
