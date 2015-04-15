#ifndef ACTIONS_H
#define ACTIONS_H

#include <QKeySequence>
#include <vector>

struct ActionGroupDefinition {
    const char *name;
};

struct ActionDefinition {
    const char *const name;
    const char *const icon;
    const char *const text;
    const QKeySequence key;
    const char *const group;
};

struct MenuDefinition {
    const char *const name;
    const char *const text;
    const std::vector<const char *> actions;
};

extern const std::vector<struct ActionDefinition> actionDefinitions;
extern const std::vector<struct MenuDefinition> menuDefinitions;

#endif // ACTIONS_H
