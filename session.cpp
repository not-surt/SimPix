#include "session.h"

#include "document.h"

TreeModelItem *Session::child(int row) {
    return documents.value(row);
}
