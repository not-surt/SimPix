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
    Q_PROPERTY(uint primaryColour READ primaryColour WRITE setPrimaryColour NOTIFY primaryColourChanged)
    Q_PROPERTY(uint secondaryColour READ secondaryColour WRITE setSecondaryColour NOTIFY secondaryColourChanged)
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
    explicit Image(const QImage & image);
    ~Image();

    QImage &data();

    Image::Format format() const;

    uint primaryColour() const;

    uint secondaryColour() const;

signals:
    void changed(const QRegion &region);
    void primaryColourChanged(uint arg);
    void secondaryColourChanged(uint arg);

public slots:
    void point(const QPoint &position);
    void stroke(const QPoint &a, const QPoint &b);

    void setPrimaryColour(uint arg)
    {
        if (m_primaryColour != arg) {
            m_primaryColour = arg;
            emit primaryColourChanged(arg);
        }
    }

    void setSecondaryColour(uint arg)
    {
        if (m_secondaryColour != arg) {
            m_secondaryColour = arg;
            emit secondaryColourChanged(arg);
        }
    }

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
