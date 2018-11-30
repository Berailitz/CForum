#include "cftools.h"

bool matchRegular(const QString text, const QString regularExpression)
{
	QRegExp regular(regularExpression);
	return regular.exactMatch(text);
}
