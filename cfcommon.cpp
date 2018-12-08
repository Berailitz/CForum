#include "cfcommon.h"

namespace cforum
{
	bool matchRegular(const QString text, const QString regularExpression)
	{
		QRegExp regular(regularExpression);
		return regular.exactMatch(text);
	}

	int count_files(const fs::path path)
	{
		return distance(fs::directory_iterator(path), fs::directory_iterator{});
	}
}
