#ifndef SESSION_H
#define SESSION_H

#include <QList>
#include "document.h"
#include "imageeditor.h"

#include <QObject>

class Session : public QObject
{
    Q_OBJECT
public:
    explicit Session(QObject *parent = nullptr);

    QList<ImageDocument *> images() const;
    QList<ImageEditor *> editors() const;

signals:

public slots:

private:
    QList<ImageDocument *> m_images;
    QList<ImageEditor *> m_editors;
};

#endif // SESSION_H
