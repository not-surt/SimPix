#ifndef IMAGE_H
#define IMAGE_H

#include <QImage>
#include <QQueue>
#include <QUndoStack>

class Image : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QImage data READ data)
    Q_PROPERTY(Image::Format format READ format)
    Q_PROPERTY(uint currentColour READ currentColour WRITE setCurrentColour NOTIFY currentColourChanged)
    Q_PROPERTY(bool dirty READ dirty)
    Q_ENUMS(data format primaryColour secondaryColour)
public:
    enum Format {
        Invalid = QImage::Format_Invalid,
        Indexed = QImage::Format_Indexed8,
        RGBA = QImage::Format_ARGB32,
    };
    explicit Image(const Image &image, QObject *parent = nullptr);
    explicit Image(const QSize &size, Format format, QObject *parent = nullptr);
    explicit Image(const QString &fileName, const char *format = nullptr, QObject *parent = nullptr);
    ~Image();

    QImage &data();
    const QImage &constData() const;

    Image::Format format() const;

    uint currentColour(const bool secondary = false) const;

    QUndoStack *undoStack = new QUndoStack(this);

    const QString &fileName() const;
    bool save(QString fileName = QString());

    bool dirty() const;

signals:
    void fileNameChanged(const QString &fileName);
    void changed(const QRegion &region);
    void currentColourChanged(uint currentColour, bool secondary);    

public slots:
    void point(const QPoint &position, const bool secondary = false);
    void stroke(const QPoint &a, const QPoint &b, const bool secondary = false);
    void pick(const QPoint &position, const bool secondary = false);
    void setCurrentColour(uint currentColour, const bool secondary = false);
    void setFileName(const QString &fileName);

private:
    QString m_fileName;
    QImage m_data;
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
    bool m_dirty;
};



class StrokeCommand : public QUndoCommand
{
public:
    StrokeCommand(const Image &image, const QPoint &point0, const QPoint &point1, QUndoCommand *parent = nullptr);
    ~StrokeCommand();

    void undo();
    void redo();

private:
    const QPoint &point0, &point1;
    const Image &image;
//    Image dirty;
};

#endif // IMAGE_H
