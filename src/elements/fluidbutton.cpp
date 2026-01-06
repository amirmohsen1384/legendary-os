#include <QPainter>
#include "fluidbutton.h"
#include <QEventTransition>
#include <QPropertyAnimation>

void FluidButton::initialize()
{
    setFlat(true);
    setText(QString());
    setCursor(Qt::PointingHandCursor);

    setupStateMachine();
}

qreal FluidButton::getTransition() const
{
    return transition;
}

void FluidButton::setTransition(qreal value)
{
    transition = value;
    update();
}

void FluidButton::setupStateMachine()
{
    coordinator = new QStateMachine(this);
    normalState = new QState;
    hoverState = new QState;
    clickState = new QState;

    normalState->assignProperty(this, "transition", qreal(Group::Normal));
    hoverState->assignProperty(this, "transition", qreal(Group::Hovered));
    clickState->assignProperty(this, "transition", qreal(Group::Clicked));

    auto normalToHover = new QEventTransition(this, QEvent::Enter);
    normalToHover->setTargetState(hoverState);
    normalState->addTransition(normalToHover);

    auto normalToHoverAnimation = new QPropertyAnimation(this, "transition");
    normalToHoverAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    normalToHoverAnimation->setDuration(duration);
    normalToHover->addAnimation(normalToHoverAnimation);

    auto hoverToNormal = new QEventTransition(this, QEvent::Leave);
    hoverToNormal->setTargetState(normalState);
    hoverState->addTransition(hoverToNormal);

    auto hoverToNormalAnimation = new QPropertyAnimation(this, "transition");
    hoverToNormalAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    hoverToNormalAnimation->setDuration(duration);
    hoverToNormal->addAnimation(hoverToNormalAnimation);

    auto hoverToClick = new QEventTransition(this, QEvent::MouseButtonPress);
    hoverToClick->setTargetState(clickState);
    hoverState->addTransition(hoverToClick);

    auto hoverToClickAnimation = new QPropertyAnimation(this, "transition");
    hoverToClickAnimation->setDuration(duration / 2);
    hoverToClickAnimation->setEasingCurve(QEasingCurve::OutQuad);
    hoverToClick->addAnimation(hoverToClickAnimation);

    auto clickToHover = new QEventTransition(this, QEvent::MouseButtonRelease);
    clickToHover->setTargetState(hoverState);
    clickState->addTransition(clickToHover);

    auto clickToHoverAnimation = new QPropertyAnimation(this, "transition");
    clickToHoverAnimation->setEasingCurve(QEasingCurve::OutBounce);
    clickToHoverAnimation->setDuration(duration / 2);
    clickToHover->addAnimation(clickToHoverAnimation);

    auto clickToNormal = new QEventTransition(this, QEvent::Leave);
    clickToNormal->setTargetState(normalState);
    clickState->addTransition(clickToNormal);

    auto clickToNormalAnimation = new QPropertyAnimation(this, "transition");
    clickToNormalAnimation->setDuration(duration);
    clickToNormalAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    clickToNormal->addAnimation(clickToNormalAnimation);

    coordinator->addState(normalState);
    coordinator->addState(hoverState);
    coordinator->addState(clickState);

    coordinator->setInitialState(normalState);
    coordinator->start();
}

FluidButton::FluidButton(QWidget *parent) : QPushButton(parent)
{
    initialize();
}

QPixmap FluidButton::getPixmap(FluidButton::Group group)
{
    return pixmaps[group];
}

void FluidButton::setPixmap(Group group, const QPixmap &value)
{
    pixmaps[group] = value;
}

void FluidButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    QRect iconRect((width() - iconSize().width()) / 2, (height() - iconSize().height()) / 2, iconSize().width(), iconSize().height());
    if (transition < 1.0)
    {
        if (pixmaps.contains(Group::Normal))
        {
            qreal opacity = 1.0 - transition;
            painter.setOpacity(opacity);
            QPixmap normalPixmap = pixmaps[Group::Normal].scaled(iconSize());
            painter.drawPixmap(iconRect, normalPixmap);
        }

        if (pixmaps.contains(Group::Hovered))
        {
            qreal opacity = transition;
            painter. setOpacity(opacity);
            QPixmap hoverPixmap = pixmaps[Group::Hovered].scaled(iconSize());
            painter.drawPixmap(iconRect, hoverPixmap);
        }
    }
    else if (transition >= 1.0 && transition < 2.0)
    {
        qreal clickProgress = transition - 1.0;

        if (pixmaps.contains(Group::Hovered))
        {
            qreal opacity = 1.0 - clickProgress;
            painter.setOpacity(opacity);
            QPixmap hoverPixmap = pixmaps[Group::Hovered].scaled(iconSize());
            painter.drawPixmap(iconRect, hoverPixmap);
        }

        if (pixmaps.contains(Group::Clicked))
        {
            qreal opacity = clickProgress;
            painter.setOpacity(opacity);
            QPixmap clickPixmap = pixmaps[Group::Clicked].scaled(iconSize());

            painter.save();
            qreal scale = 1.0 - (clickProgress * 0.1);
            painter.translate(iconRect.center());
            painter.scale(scale, scale);
            painter.translate(-iconRect. center());
            painter.drawPixmap(iconRect, clickPixmap);
            painter.restore();
        }
    }
    else
    {
        if (pixmaps.contains(Group::Clicked))
        {
            qreal opacity = 1.0;
            painter.setOpacity(opacity);
            QPixmap clickPixmap = pixmaps[Group::Clicked].scaled(iconSize());

            painter.save();
            painter.translate(iconRect.center());
            painter.scale(0.9, 0.9);
            painter.translate(-iconRect.center());
            painter.drawPixmap(iconRect, clickPixmap);
            painter.restore();
        }
    }
}

QSize FluidButton::sizeHint() const
{
    return iconSize();
}
