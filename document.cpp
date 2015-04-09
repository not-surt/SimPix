#include "document.h"

#include "util.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include "newdialog.h"

Document::Document(const QString &fileName, QObject *parent) :
    QObject(parent), m_fileName(fileName), m_dirty(false)
{

}

bool Document::save(QString fileName)
{
    bool saved = false;
    if (fileName.isNull()) {
        fileName = m_fileName;
    }
    if (!fileName.isNull()) {
        saved = doSave(fileName);
        if (saved) {
            setFileName(fileName);
        }
    }
    return saved;
}
