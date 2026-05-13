#pragma once

#include "HomeEmptyState.h"

class HomeNoExperiment : public HomeEmptyState
{
    Q_OBJECT

public:
    explicit HomeNoExperiment(QWidget* parent = nullptr);

    void showNoExperimentLoaded();
    void showNoSearchResult();
};
