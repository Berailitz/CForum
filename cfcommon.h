/**
 * @file cfcommon.h
 * @author 熊光正 (xgz@bupt.edu.cn)
 * @brief 公共头文件，定义常用字符串，封装正则匹配函数等。
 * @version 3.3
 * @date 2019-01-04
 *
 * @copyright Copyright (c) 2019
 *
 */


#ifndef CFORUM_CFCOMMON_H
#define CFORUM_CFCOMMON_H

#include <filesystem>
#include <QRegExp>
#include <QString>

using namespace std;
namespace fs = std::experimental::filesystem;

/**
 * @brief cforum命名空间为程序的主命名空间，用于避免命名冲突。
 *
 */
namespace cforum
{
    // 用于显示的日期、时间格式
    const QString FRONT_DATETIME_FORMAT = QString::fromUtf8("M月d日 HH:mm:ss");
    // 用于保存等内部表示的日期、时间格式
    const QString BACK_DATETIME_FORMAT = QString::fromUtf8("yyyyMMddHHmmss");

    const string STD_LINE_BREAK = "\n";
    const QString Q_LINE_BREAK = QString::fromStdString(STD_LINE_BREAK);

    class ClientController;

    bool matchRegular(const QString text, const QString regularExpression);
    int count_files(const fs::path path);
}

#endif // CFORUM_CFCOMMON_H
