#include "cfcommon.h"

namespace cforum
{
    /**
     * @brief 检查字符串是否被正则表达式匹配。
     *
     * @param text
     * @param regularExpression
     * @return true
     * @return false
     */
    bool matchRegular(const QString text, const QString regularExpression)
    {
        QRegExp regular(regularExpression);
        return regular.exactMatch(text);
    }

    /**
     * @brief 计算文件夹下有多少个文件。
     *
     * @param path
     * @return int
     */
    int count_files(const fs::path path)
    {
        return distance(fs::directory_iterator(path), fs::directory_iterator{});
    }
}
