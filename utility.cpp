#include "utility.h"

template <class T>
string container_to_string(T &container, string separator, const int start_index)
{
	string result = "";
	if (container.size() > start_index)
	{
		auto it = container.begin();
		advance(it, start_index);
		result = *it;
		it++;
		for (; it != container.end(); it++)
		{
			result += (separator + *it);
		}
	}
	return result;
}
