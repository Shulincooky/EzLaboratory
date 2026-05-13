#pragma once

#include "HomeEmptyState.h"

class HomeNetworkDisconnected : public HomeEmptyState
{
    Q_OBJECT

public:
    explicit HomeNetworkDisconnected(QWidget* parent = nullptr);
};
