#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "client_controller.h"
#include "cfcommon.h"
#include "error_handler.h"

using namespace std;

int main(int argc, char *argv[])
{
	// _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
	int returnValue;
	bool doLoad;
    QGuiApplication app(argc, argv);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    QQmlApplicationEngine engine;
	cforum::ClientController *forumController = new cforum::ClientController(engine);
	engine.rootContext()->setContextProperty("forumController", QVariant::fromValue(&*forumController));
	QObject::connect(&*errorHandler, &cforum::ErrorHandler::errorRaised, &*forumController, &cforum::ClientController::errorRaised);
	engine.load(QUrl(QString::fromUtf8("qrc:/qml/main.qml")));
	forumController->open(QString::fromUtf8("ws://localhost:8118/"));
	forumController->refreshViews();
	returnValue = app.exec();
	return returnValue;
}
