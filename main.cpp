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
    QQmlContext *ctxt = engine.rootContext();
    cforum::Controller forumController;
    forumController.registerUser("admin", "admin");
    forumController.addBoard("TestBoard");
    forumController.postThread("Test thread", "This is content.");
    forumController.postComment("Comment 1");
    forumController.setModerator("admin");
	// forumController.load("data");
//	forumController.save("data");
    ctxt->setContextProperty("forumController", QVariant::fromValue(&forumController));
    ctxt->setContextProperty("boardListModel", QVariant::fromValue(*forumController.cforum->boards));
    ctxt->setContextProperty("threadListModel", QVariant::fromValue(*forumController.board->threads));
    ctxt->setContextProperty("commentListModel", QVariant::fromValue(*forumController.thread->comments));
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
