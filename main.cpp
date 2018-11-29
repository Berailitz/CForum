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
    //cforum::Controller forumController(&engine);
	qmlRegisterType<cforum::Controller>("CForum", 1, 0, "Controller");
	qmlRegisterType<cforum::User>("CForum", 1, 0, "User");
	qmlRegisterType<cforum::Board>("CForum", 1, 0, "Board");
	qmlRegisterType<cforum::Thread>("CForum", 1, 0, "Thread");
	qmlRegisterType<cforum::Comment>("CForum", 1, 0, "Comment");
	//forumController.load("data");
 //   forumController.registerUser("admin", "admin");
	//forumController.cforum->admins->insert(1); // admin
 //   forumController.addBoard("TestBoard");
 //   forumController.postThread("Test thread", "This is content.");
 //   forumController.postComment("Comment 1");
 //   forumController.setModerator("admin");
	//forumController.save("data");
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    return app.exec();
}
