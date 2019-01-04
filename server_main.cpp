#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "forum_server.h"
#include "cfcommon.h"

using namespace std;

int main(int argc, char *argv[])
{
    // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
    QGuiApplication app(argc, argv);
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);
    QQmlApplicationEngine engine;
    cforum::ForumServer *server = new cforum::ForumServer;
    engine.rootContext()->setContextProperty("forumServer", QVariant::fromValue(&*server));
    QObject::connect(&*errorHandler, &cforum::ErrorHandler::errorRaised, &*server, &cforum::ForumServer::onErrorRaised);
    engine.load(QUrl(QString::fromUtf8("qrc:/qml/server_main.qml")));
    server->load(cforum::DEFAULT_DATABASE_FOLDER_PATH);
    server->start();
    return app.exec();
}
