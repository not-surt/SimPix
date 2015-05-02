#ifndef ACTIONOWNER_H
#define ACTIONOWNER_H

#include <QHash>
#include <QString>
#include <QList>
#include <QAction>
#include <QMenu>

class ActionOwner {
public:
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

        QAction *toAction() const;
    };
    struct MenuDefinition {
        QString text;
        QList<QString> actionNames;

        QMenu *toMenu(QHash<QString, QAction *> &actions, QMenu *menu = nullptr) const;
    };

    QHash<QString, QAction *> actions;
    QHash<QString, QActionGroup *> actionGroups;
    QHash<QString, QMenu *> menus;

    explicit ActionOwner(const ActionOwner &other);
    explicit ActionOwner(const QHash<QString, ActionDefinition> &actionDefinitions, const QHash<QString, MenuDefinition> &menuDefinitions);
    explicit ActionOwner(const QHash<QString, ActionDefinition> &actionDefinitions, const QHash<QString, MenuDefinition> &menuDefinitions, const ActionOwner &inherit);

private:
    void init(const QHash<QString, ActionDefinition> &actionDefinitions, const QHash<QString, MenuDefinition> &menuDefinitions);
};

#endif // ACTIONOWNER_H
