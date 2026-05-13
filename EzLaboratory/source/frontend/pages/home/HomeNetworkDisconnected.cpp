#include "HomeNetworkDisconnected.h"

HomeNetworkDisconnected::HomeNetworkDisconnected(QWidget* parent)
    : HomeEmptyState(parent)
{
    setTitle(QStringLiteral("网络未连接"));
    setMessage(QStringLiteral("请检查网络连接或网络代理，实验将在正常连接服务器后加载。"));
}
