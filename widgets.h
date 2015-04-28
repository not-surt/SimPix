#ifndef WIDGETS_H
#define WIDGETS_H

#include <QMenu>
#include <QSpinBox>
#include <QToolButton>
#include <QWidgetAction>
#include "colourswatchwidget.h"

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
    explicit MenuToolButtonAction(QObject *const parent = nullptr)
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

template <typename Enum>
class ModeActionGroup : public QActionGroup
{
public:
    explicit ModeActionGroup(QObject *const parent = nullptr)
        : QActionGroup(parent) {
    }
    QAction *addAction(QAction *const action, const Enum &mode) {
        QActionGroup::addAction(action);
        m_actionModes[action] = mode;
        return action;
    }
    void removeAction(QAction *const action) {
        m_actionModes.remove(action);
        QActionGroup::removeAction(action);
    }
    Enum &mode(QAction *const action = checkedAction()) {
        return m_actionModes[action];
    }
    void setMode(const Enum &mode) {
        QList<QAction *> actions = m_actionModes.keys(mode);
        if(actions.length() > 0) {
            actions[0]->setChecked(true);
        }
    }

protected:
    QHash<QAction *, Enum> m_actionModes;
};

template <typename Enum>
class ModeMenuToolButton : public MenuToolButton
{
public:
    explicit ModeMenuToolButton(ModeActionGroup<Enum> &group, QWidget *const parent = nullptr)
        : MenuToolButton(parent), m_group(group) {
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

protected:
    ModeActionGroup<Enum> &m_group;
};

template <typename Enum>
class ModeToolButtonAction : public QWidgetAction
{
public:
    explicit ModeToolButtonAction(ModeActionGroup<Enum> &group, QObject *const parent = nullptr)
        : QWidgetAction(parent), m_group(group) {
    }

protected:
    ModeActionGroup<Enum> &m_group;

    virtual QWidget *createWidget(QWidget *parent) {
        ModeMenuToolButton<Enum> *button = new ModeMenuToolButton<Enum>(m_group, parent);
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
    explicit IntegerFieldAction(QObject *const parent = nullptr)
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
    explicit FloatFieldAction(QObject *const parent = nullptr)
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
    explicit ColourSwatchAction(QObject *const parent = nullptr)
        : QWidgetAction(parent) {
    }

protected:
    virtual QWidget *createWidget(QWidget *parent) {
        ColourSwatchWidget *swatch = new ColourSwatchWidget(parent);
        return swatch;
    }
};

#endif // WIDGETS_H
