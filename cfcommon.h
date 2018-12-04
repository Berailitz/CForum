#ifndef CFORUM_CFCOMMON_H
#define CFORUM_CFCOMMON_H

#include <QRegExp>
#include <QString>

using namespace std;

const QString FRONT_END_DATETIME_FORMAT = QString::fromUtf8("M月d日 HH:mm:ss");


bool matchRegular(const QString text, const QString regularExpression);

#endif // CFORUM_CFCOMMON_H
