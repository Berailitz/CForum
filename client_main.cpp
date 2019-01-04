/**
 * @file client_main.cpp
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 客户端主程序，即客户端程序的入口函数。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "client_controller.h"
#include "cfcommon.h"
#include "error_handler.h"

using namespace std;

int main(int argc, char *argv[])
{
    int returnValue; // 暂存程序的返回值
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    // 创建客户端控制器
    cforum::ClientController *forumController = new cforum::ClientController(engine);

    // 禁用OpenGL以支持在没有OpenGL的电脑上运行
    QCoreApplication::setAttribute(Qt::AA_UseOpenGLES);

    // 禁用DPI缩放以避免UI错位
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

    // 建立控制器与view层的连接
    engine.rootContext()->setContextProperty("forumController",
        QVariant::fromValue(&*forumController));
    QObject::connect(&*errorHandler,
        &cforum::ErrorHandler::errorRaised,
        &*forumController,
        &cforum::ClientController::onErrorRaised);

    // 加载界面
    forumController->refreshViews();
    engine.load(QUrl(QString::fromUtf8("qrc:/qml/client_main.qml")));

    // 连接服务器，并运行程序
    forumController->connect();
    returnValue = app.exec();
    return returnValue;
}
