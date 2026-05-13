#include "HomeNoExperiment.h"

HomeNoExperiment::HomeNoExperiment(QWidget* parent)
    : HomeEmptyState(parent)
{
    showNoExperimentLoaded();
}

void HomeNoExperiment::showNoExperimentLoaded()
{
    setTitle(QStringLiteral("暂无实验加载"));
    setMessage(QStringLiteral("实验配置暂未加载，确认资源文件存在后可重新进入。"));
}

void HomeNoExperiment::showNoSearchResult()
{
    setTitle(QStringLiteral("没有匹配的实验"));
    setMessage(QStringLiteral("换个关键词试试，或点击全部实验清除搜索。"));
}
