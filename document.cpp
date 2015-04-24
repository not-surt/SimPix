#include "document.h"

#include "util.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include "newdialog.h"

Document::Document(Session &session, const QString &fileName) :
    QObject(), fileInfo(fileName), editors(), session(session)
{
   session.documents.append(this);
}

Document::~Document()
{
    session.documents.removeOne(this);
}

bool Document::save(QString fileName)
{
    bool saved = false;
    if (fileInfo.fileName().isNull()) {
        fileName = fileInfo.fileName();
    }
    if (!fileName.isNull()) {
        saved = doSave(fileName);
        if (saved) {
            fileInfo.setFileName(fileName);
            fileInfo.clearDirty();
        }
    }
    return saved;
}

TreeModelItem *Document::child(int row) {
    return editors.value(row);
}

//const struct DocumentType DOCUMENT_TYPES[] = {
////    {"image", (char *[]){"png", "gif", "bmp"}, &ImageDocument::create, &ImageDocument::open},
//    {"palette", (char *[]){"gpl", "pal"}, nullptr, nullptr},
//};
