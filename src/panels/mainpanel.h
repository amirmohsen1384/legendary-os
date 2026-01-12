#ifndef MAINPANEL_H
#define MAINPANEL_H

#include <QMainWindow>

namespace Ui {
class MainPanel;
}

class MainPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainPanel(QWidget *parent = nullptr);
    ~MainPanel();

private:
    Ui::MainPanel *ui;
};

#endif // MAINPANEL_H
