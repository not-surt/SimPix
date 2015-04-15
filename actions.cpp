#include "actions.h"

const std::vector<struct ActionDefinition> actionDefinitions = {
    {"about", nullptr, "About", QKeySequence(), nullptr},
    {"aboutQt", nullptr, "About Qt", QKeySequence(), nullptr},
};
const std::vector<struct MenuDefinition> menuDefinitions = {
    {"program", "Program", {"about", "aboutQt"}},
};
