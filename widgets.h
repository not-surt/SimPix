#ifndef WIDGETS_H
#define WIDGETS_H

#include <QMenu>
#include <QSpinBox>
#include <QToolButton>
#include <QWidgetAction>
#include "colourswatch.h"

class ActionWidget : public QWidget
{
public:
    explicit ActionWidget(QWidget *const parent = nullptr)
        : QWidget(parent) {
    }
    void setAction(QAction *const action) { m_action = action; }
    QAction *action() { return m_action; }
protected:
    QAction *m_action;
};

class MenuToolButton : public QToolButton
{
public:
    explicit MenuToolButton(QWidget *const parent = nullptr)
        : QToolButton(parent) {
        setPopupMode(QToolButton::InstantPopup);
    }
};

class MenuToolButtonAction : public QWidgetAction
{
public:
    explicit MenuToolButtonAction(QWidget *const parent = nullptr)
        : QWidgetAction(parent) {
    }

protected:
    virtual QWidget *createWidget(QWidget *parent) {
        MenuToolButton *button = new MenuToolButton(parent);
        button->setMenu(menu());
        button->setDefaultAction(menu()->menuAction());
        return button;
    }
};

class ModeMenuToolButton : public MenuToolButton
{
public:
    explicit ModeMenuToolButton(QWidget *const parent = nullptr)
        : MenuToolButton(parent) {
    }
    void setCurrentAction(QAction *const action) {
        setDefaultAction(action);
        setCheckable(false);
        setChecked(false);
    }
    void setMenu(QMenu *const _menu) {
        if (menu()) {
            QObject::disconnect(menu(), &QMenu::triggered, this, &ModeMenuToolButton::setCurrentAction);
        }
        QToolButton::setMenu(_menu);
        if (_menu) {
            QObject::connect(_menu, &QMenu::triggered, this, &ModeMenuToolButton::setCurrentAction);
        }
    }
};

class ModeToolButtonAction : public QWidgetAction
{
public:
    explicit ModeToolButtonAction(QWidget *const parent = nullptr)
        : QWidgetAction(parent) {
    }

protected:
    virtual QWidget *createWidget(QWidget *parent) {
        ModeMenuToolButton *button = new ModeMenuToolButton(parent);
        button->setMenu(menu());
        QList<QAction *> actions = menu()->actions();
        QAction *action = nullptr;
        if (actions.length() > 0) {
            QActionGroup *group = actions[0]->actionGroup();
            if (group) {
                action = group->checkedAction();
            }
            else {
                action = actions[0];
            }
        }
        button->setCurrentAction(action);
        return button;
    }
};

class IntegerField : public QSpinBox
{
public:
    explicit IntegerField(QWidget *const parent = nullptr)
        : QSpinBox(parent) {
    }
};

class IntegerFieldAction : public QWidgetAction
{
public:
    explicit IntegerFieldAction(QWidget *const parent = nullptr)
        : QWidgetAction(parent) {
    }

protected:
    virtual QWidget *createWidget(QWidget *parent) {
        IntegerField *field = new IntegerField(parent);
        return field;
    }
};

class FloatField : public QDoubleSpinBox
{
public:
    explicit FloatField(QWidget *const parent = nullptr)
        : QDoubleSpinBox(parent) {
    }
};

class FloatFieldAction : public QWidgetAction
{
public:
    explicit FloatFieldAction(QWidget *const parent = nullptr)
        : QWidgetAction(parent) {
    }

protected:
    virtual QWidget *createWidget(QWidget *parent) {
        FloatField *field = new FloatField(parent);
        return field;
    }
};

class ColourSwatchAction : public QWidgetAction
{
public:
    explicit ColourSwatchAction(QWidget *const parent = nullptr)
        : QWidgetAction(parent) {
    }

protected:
    virtual QWidget *createWidget(QWidget *parent) {
        ColourSwatch *swatch = new ColourSwatch(parent);
        return swatch;
    }
};

#endif // WIDGETS_H
