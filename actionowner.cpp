#include "actionowner.h"

#include <QDebug>

QAction *ActionOwner::ActionDefinition::toAction() const {
    QAction *action = new QAction(QIcon::fromTheme(icon), text, nullptr);
    action->setCheckable(checkable);
    action->setChecked(checked);
    if (standardShortcut >= 0) {
        action->setShortcut(static_cast<enum QKeySequence::StandardKey>(standardShortcut));
        action->setShortcutContext(Qt::ApplicationShortcut);
    }
    else if (!customShortcut.isEmpty()) {
        action->setShortcut(QKeySequence(customShortcut));
        action->setShortcutContext(Qt::ApplicationShortcut);
    }
    if (!toolTip.isEmpty()) {
        action->setToolTip(toolTip);
    }

    return action;
}


QMenu *ActionOwner::MenuDefinition::toMenu(QHash<QString, QAction *> &actions, QMenu *menu) const {
    if (!menu) {
        menu = new QMenu(text);
    }
    for (int j = 0; j < actionNames.size(); j++) {
        if (!actionNames[j].isEmpty()) {
            menu->addAction(actions[actionNames[j]]);
        }
        else {
            menu->addSeparator();
        }
    }

    return menu;
}


ActionOwner::ActionOwner(const ActionOwner &other)
    : actions(other.actions), actionGroups(other.actionGroups), menus(other.menus) {
}


ActionOwner::ActionOwner(const QHash<QString, ActionOwner::ActionDefinition> &actionDefinitions, const QHash<QString, ActionOwner::MenuDefinition> &menuDefinitions)
    : actions(), actionGroups(), menus() {
    init(actionDefinitions, menuDefinitions);
}


ActionOwner::ActionOwner(const QHash<QString, ActionOwner::ActionDefinition> &actionDefinitions, const QHash<QString, ActionOwner::MenuDefinition> &menuDefinitions, const ActionOwner &inherit)
    : ActionOwner(inherit) {
    init(actionDefinitions, menuDefinitions);
}


void ActionOwner::init(const QHash<QString, ActionOwner::ActionDefinition> &actionDefinitions, const QHash<QString, ActionOwner::MenuDefinition> &menuDefinitions) {
    // Create actions
    for (const QString key : actionDefinitions.keys()) {
        actions[key] = actionDefinitions[key].toAction();
    }
    // Set action groups
    for (const QString key : actionDefinitions.keys()) {
        const ActionDefinition &definition = actionDefinitions[key];
        if (!definition.groupName.isEmpty()) {
            if (!actionGroups[definition.groupName]) {
                actionGroups[definition.groupName] = new QActionGroup(nullptr);
            }
            actions[key]->setActionGroup(actionGroups[definition.groupName]);
        }
    }
    // Create menus
    for (const QString key : menuDefinitions.keys()) {
        menus[key] = menuDefinitions[key].toMenu(actions, menus[key]);
    }
    // Set menu actions
    for (const QString key : actionDefinitions.keys()) {
        const ActionDefinition &definition = actionDefinitions[key];
        if (!definition.menuName.isEmpty()) {
            actions[key]->setMenu(menus[definition.menuName]);
        }
    }
}
