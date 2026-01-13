#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QMainWindow>

namespace Ui
{
    class MainPanel;
}

class MainPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainPanel(const Config::Info &info, QWidget *parent = nullptr);
    ~MainPanel();

private:
    std::unique_ptr<Ui::MainPanel> ui;
};

#endif // MAINPANEL_H
