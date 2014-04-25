#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QUndoStack>

class Image : public QObject
{
    Q_OBJECT
//    Q_PROPERTY(QImage &data READ data)
    Q_PROPERTY(Image::Format format READ format)
    Q_ENUMS(data)
public:
    enum Format {
        Invalid = QImage::Format_Invalid,
        Indexed = QImage::Format_Indexed8,
        RGBA = QImage::Format_ARGB32,
    };
    explicit Image();
    explicit Image(const QSize &size, Format format);
    explicit Image(const QString &fileName, const char *format=0);
    explicit Image(const QImage & image);
    ~Image();

    QImage &data();

    Image::Format format() const;

signals:
    void changed(const QRegion &region);

public slots:
    void point(const QPoint &position);
    void stroke(const QPoint &a, const QPoint &b);

private:
    QImage m_data;
    QUndoStack *undoStack = new QUndoStack(this);
};

class StrokeCommand : public QUndoCommand
{
public:
    StrokeCommand(const Image &image, const QPoint &a, const QPoint &b, QUndoCommand *parent = 0);
    ~StrokeCommand();

    void undo();
    void redo();

private:
    QPointF a, b;
    const Image &image;
    Image dirty;
};

#endif // IMAGE_H
