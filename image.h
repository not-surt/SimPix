#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QQueue>
#include <QUndoStack>

class Image : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QImage data READ data)
    Q_PROPERTY(Image::Format format READ format)
    Q_PROPERTY(uint currentColour READ currentColour WRITE setCurrentColour NOTIFY currentColourChanged)
    Q_ENUMS(data format primaryColour secondaryColour)
public:
    enum Format {
        Invalid = QImage::Format_Invalid,
        Indexed = QImage::Format_Indexed8,
        RGBA = QImage::Format_ARGB32,
    };
    explicit Image();
    explicit Image(const QSize &size, Format format);
    explicit Image(const QString &fileName, const char *format=0);
    explicit Image(Image & image);
    ~Image();

    QImage &data();

    Image::Format format() const;

    uint currentColour(const bool secondary = false) const;

signals:
    void changed(const QRegion &region);
    void currentColourChanged(uint currentColour, bool secondary);

public slots:
    void point(const QPoint &position, const bool secondary = false);
    void stroke(const QPoint &a, const QPoint &b, const bool secondary = false);
    void pick(const QPoint &position, const bool secondary = false);
    void setCurrentColour(uint currentColour, const bool secondary = false);

private:
    QImage m_data;
    QUndoStack *undoStack = new QUndoStack(this);
    QQueue<QPoint> recentPoints;
    static const int recentPointsMax = 5;
    void addRecentPoint(const QPoint &point)
    {
        recentPoints.enqueue(point);
        while (recentPoints.length() > recentPointsMax) {
            recentPoints.dequeue();
        }
    }
    uint m_primaryColour;
    uint m_secondaryColour;
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
