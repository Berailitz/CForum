#ifndef CFORUM_UTILITY_H
#define CFORUM_UTILITY_H

#include <QRegExp>
#include <QString>

using namespace std;

const QString FRONT_END_DATETIME_FORMAT = QString::fromUtf8("M月d日 H:m:s");


bool matchRegular(const QString text, const QString regularExpression);

#endif // CFORUM_UTILITY_H
