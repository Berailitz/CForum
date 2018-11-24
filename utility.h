#ifndef CFORUM_UTILITY_H
#define CFORUM_UTILITY_H

#include <string>

using namespace std;

namespace cforum
{
    template <class T>
    string container_to_string(T &container, string separator = " ", const int start_index = 0);
    // 将一个内部表示的容器（符号列表等）转化为字符串以便输出
}

#endif // CFORUM_UTILITY_H
