#ifndef CFORUM_CFCOMMON_H
#define CFORUM_CFCOMMON_H

#include <filesystem>
#include <QRegExp>
#include <QString>

using namespace std;
namespace fs = std::experimental::filesystem;

namespace cforum
{
	const QString FRONT_END_DATETIME_FORMAT = QString::fromUtf8("M月d日 HH:mm:ss");

	class Controller;

	bool matchRegular(const QString text, const QString regularExpression);
	int count_files(const fs::path path);
}

#endif // CFORUM_CFCOMMON_H
