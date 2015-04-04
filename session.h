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

    QList<Image *> images() const;
    QList<ImageEditor *> editors() const;

signals:

public slots:

private:
    QList<Image *> m_images;
    QList<ImageEditor *> m_editors;
};

#endif // SESSION_H
