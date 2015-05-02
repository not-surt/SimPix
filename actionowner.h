#ifndef ACTIONOWNER_H
#define ACTIONOWNER_H

#include <QHash>
#include <QString>
#include <QList>
#include <QAction>
#include <QMenu>
#include <QDebug>

struct ActionOwner {
    struct ActionDefinition {
        QString text;
        QString icon;
        bool checkable;
        bool checked;
        int standardShortcut;
        QString customShortcut;
        QString toolTip;
        QString menuName;
        QString groupName;

        QAction *toAction() const {
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
    };
    struct MenuDefinition {
        QString text;
        QList<QString> actionNames;

        QMenu *toMenu(QHash<QString, QAction *> &actions, const QMenu *const inherit = nullptr) const {
            QMenu *menu = new QMenu(text);
            if (inherit) {
                for (QAction *const action : inherit->actions()) {
                    menu->addAction(action);
                }
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
    };

    QHash<QString, QAction *> actions;
    QHash<QString, QActionGroup *> actionGroups;
    QHash<QString, QMenu *> menus;

    explicit ActionOwner(const ActionOwner &other)
        : actions(other.actions), actionGroups(other.actionGroups), menus(other.menus) {}
    explicit ActionOwner(const QHash<QString, ActionDefinition> &actionDefinitions, const QHash<QString, MenuDefinition> &menuDefinitions)
        : actions(), actionGroups(), menus() {
        init(actionDefinitions, menuDefinitions);
    }
    explicit ActionOwner(const QHash<QString, ActionDefinition> &actionDefinitions, const QHash<QString, MenuDefinition> &menuDefinitions, const ActionOwner &inherit)
        : ActionOwner(inherit) {
        init(actionDefinitions, menuDefinitions);
    }
    void init(const QHash<QString, ActionDefinition> &actionDefinitions, const QHash<QString, MenuDefinition> &menuDefinitions) {
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
            qDebug() << key;
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
};

#endif // ACTIONOWNER_H
