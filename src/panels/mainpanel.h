#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QMainWindow>
#include "core/system.h"

namespace Ui
{
    class MainPanel;
}

class MainPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainPanel(QWidget *parent = nullptr);
    ~MainPanel();

private:
    std::unique_ptr<Ui::MainPanel> ui;
};

#endif // MAINPANEL_H
