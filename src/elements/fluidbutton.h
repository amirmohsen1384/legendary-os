#ifndef FLUIDBUTTON_H
#define FLUIDBUTTON_H

#include <QStateMachine>
#include <QPushButton>
#include <QState>
#include <QIcon>
#include <QSize>
#include <QHash>

class FluidButton : public QPushButton
{
    Q_OBJECT
    Q_PROPERTY(qreal transition READ getTransition WRITE setTransition)
    void setTransition(qreal value);
    qreal getTransition() const;
    void setupStateMachine();
    void initialize();

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const;

public:
    enum Group {Normal, Hovered, Clicked};

    explicit FluidButton(QWidget *parent = nullptr);
    QPixmap getPixmap(FluidButton::Group group);

public slots:
    void setPixmap(FluidButton::Group group, const QPixmap &value);

private:
    QHash<FluidButton::Group, QPixmap> pixmaps {};
    QStateMachine *coordinator {};
    QState *normalState {};
    QState *hoverState {};
    QState *clickState {};
    int duration { 300 };
    qreal transition {};
};

#endif
