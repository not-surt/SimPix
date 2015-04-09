#ifndef SESSION_H
#define SESSION_H

#include <QList>
#include "document.h"
#include "editor.h"

#include <QObject>

class Session : public QObject
{
    Q_OBJECT
public:
    explicit Session(QObject *parent = nullptr);
    explicit Session(const QString &fileName, QObject *parent = nullptr);
    virtual ~Session() {}

    bool save(QString fileName = QString());

    FileInfo fileInfo;
    QList<Document *> documents;
    QList<Editor *> editors;

signals:

public slots:

private:
};

#endif // SESSION_H
