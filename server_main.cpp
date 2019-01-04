#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "forum_server.h"
#include "cfcommon.h"

using namespace std;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    // 创建服务端控制器
    cforum::ForumServer *server = new cforum::ForumServer;

    // 禁用OpenGL以支持在没有OpenGL的电脑上运行
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);

    // 建立控制器与view层的连接
    engine.rootContext()->setContextProperty("forumServer", QVariant::fromValue(&*server));
    QObject::connect(&*errorHandler,
        &cforum::ErrorHandler::errorRaised,
        &*server,
        &cforum::ForumServer::onErrorRaised);

    // 加载界面
    engine.load(QUrl(QString::fromUtf8("qrc:/qml/server_main.qml")));
    // 读取数据库
    server->load(cforum::DEFAULT_DATABASE_FOLDER_PATH);

    // 启动服务器，并运行程序
    server->start();
    return app.exec();
}
