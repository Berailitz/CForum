#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "controller.h"
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
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    QQmlApplicationEngine engine;
	cforum::Controller *forumController;
	forumController = new cforum::Controller(engine);
	QObject::connect(&*errorHandler, &cforum::ErrorHandler::errorRaised, &*forumController, &cforum::Controller::errorRaised);
	doLoad = forumController->load("data");
	if (doLoad)
	{
		forumController->refreshViews();
		engine.load(QUrl(QString::fromUtf8("qrc:/qml/main.qml")));
		returnValue = app.exec();
		forumController->save("data");
	}
	else
	{
		qDebug() << "Cannot load data";
		returnValue = 1;
	}
	return returnValue;
}
