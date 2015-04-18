#include "imagedocument.h"

#include "util.h"
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QFileDialog>

ImageDocument::ImageDocument(const QSize &size, TextureData::Format::Id format, QObject *parent) :
    Document(QString(), parent), m_imageData(nullptr), m_paletteData(nullptr)
{
    GLContextGrabber grab(&APP->shareWidget);

    Colour fillColour;
    if (format == TextureData::Format::Indexed) {
        m_paletteData = new PaletteData(2);
        m_paletteData->setColour(0, Rgba(0, 0, 0, 0));
        m_paletteData->setColour(1, Rgba(255, 255, 255, 255));
        fillColour.index = 1;
//        m_contextColours[ContextColour::Primary] = 1;
//        m_contextColours[ContextColour::Secondary] = m_contextColours[ContextColour::Eraser] = 0;
    }
    else if (format == TextureData::Format::RGBA) {
        fillColour = Colour(0, 0, 0, 0);
//        m_contextColours[ContextColour::Primary] = qRgba(255, 255, 255, 255);
//        m_contextColours[ContextColour::Secondary] = m_contextColours[ContextColour::Eraser] = qRgba(0, 0, 0, 0);
    }

    m_imageData = new ImageData(size, format);
    clear(fillColour);
}

ImageDocument::ImageDocument(const QString &fileName, const char *fileFormat, QObject *parent) :
    Document(fileName, parent), m_imageData(nullptr), m_paletteData(nullptr)
{
    GLContextGrabber grab(&APP->shareWidget);

    QImage image(fileInfo.fileName(), fileFormat);
    if (!image.isNull()) {
        switch (image.format()) {
        case QImage::Format_Invalid:
        case QImage::Format_Indexed8:
        case QImage::Format_ARGB32:
            break;
        case QImage::Format_Mono:
        case QImage::Format_MonoLSB:
            image = image.convertToFormat(QImage::Format_Indexed8);
            break;
        default:
            image = image.convertToFormat(QImage::Format_ARGB32);
            break;
        }
    }

    TextureData::Format::Id format;
    switch (image.format()) {
    case QImage::Format_Indexed8:
        format = TextureData::Format::Indexed;
        break;
    case QImage::Format_ARGB32:
        format = TextureData::Format::RGBA;
        break;
    }

    m_paletteData = nullptr;
    if (!image.colorTable().isEmpty()) {
        m_paletteData = new PaletteData(image.colorTable().size(), (GLubyte *)image.colorTable().constData());
    }

    switch (format) {
    case TextureData::Format::Indexed:
        if (m_paletteData && m_paletteData->length() > 0) {
//            m_contextColours[ContextColour::Primary] = m_paletteData->colour(m_paletteData->length() - 1);
        }
        else {
//            m_contextColours[ContextColour::Primary] = 255;
        }
//        m_contextColours[ContextColour::Secondary] = 0;
        break;
    case TextureData::Format::RGBA:
//        m_contextColours[ContextColour::Primary] = qRgba(255, 255, 255, 255);
//        m_contextColours[ContextColour::Secondary] = qRgba(0, 0, 0, 0);
        break;
    }

    m_imageData = new ImageData(image.size(), format, image.constBits());
}

ImageDocument::~ImageDocument()
{
    GLContextGrabber grab(&APP->shareWidget);
    delete m_imageData;
    delete m_paletteData;
}

Colour ImageDocument::pixel(const QPoint &position)
{
    GLubyte index = -1;
    Rgba rgba;
    if (m_imageData->format == TextureData::Format::Indexed) {
        m_imageData->readPixel(position, &index);
        if (m_paletteData) {
            rgba = m_paletteData->colour(index);
        }
        else {
            rgba = Rgba(index, index, index, 255);
        }
    }
    else if (m_imageData->format == TextureData::Format::RGBA) {
        m_imageData->readPixel(position, rgba.bytes);
    }
    return Colour(rgba, index);
}

void ImageDocument::setPixel(const QPoint &position, const Colour &colour)
{
    if (m_imageData->format == TextureData::Format::Indexed) {
        GLubyte index = colour.index;
        m_imageData->writePixel(position, &index);
    }
    else if (m_imageData->format == TextureData::Format::RGBA) {
        m_imageData->writePixel(position, colour.bytes);
    }
}

void ImageDocument::clear(const Colour &colour)
{
    if (m_imageData->format == TextureData::Format::Indexed) {
        GLubyte index = colour.index;
        m_imageData->clear(&index);
    }
    else if (m_imageData->format == TextureData::Format::RGBA) {
        GLubyte buffer[4] = {colour.r, colour.g, colour.b, colour.a};
        m_imageData->clear(buffer);
    }
}

Editor *ImageDocument::createEditor() {
    ImageEditor *editor = new ImageEditor(*this);
    return editor;
}

ImageDocument *ImageDocument::newGui(QWidget *const parent)
{
    ImageDocument *image = nullptr;
    return image;
}

ImageDocument *ImageDocument::openGui(QWidget *const parent)
{
    ImageDocument *image = nullptr;
    return image;
}

bool ImageDocument::saveGui(QWidget *const parent)
{
    QSettings settings;
    settings.beginGroup("file");
    if (fileInfo.fileName().isNull()) {
        return saveAsGui();
    }
    if (save()) {
        settings.setValue("lastSaved", fileInfo.fileName());
        return true;
    }
    else {
        QMessageBox::critical(parent, QString(), QString(tr("Error saving file <b>\"%1\"</b>")).arg(QFileInfo(fileInfo.fileName()).fileName()));
    }
    settings.endGroup();
}

void ImageDocument::closeGui(QWidget *const parent)
{
}

bool ImageDocument::saveAsGui(QWidget *const parent)
{
    QSettings settings;
    settings.beginGroup("file");
    QString newFileName;
    if (!fileInfo.fileName().isNull()) {
        newFileName = fileInfo.fileName();
    }
    else {
        QFileInfo fileInfo(settings.value("lastSaved", QDir::homePath()).toString());
        newFileName = fileInfo.dir().path();
    }
    newFileName = QFileDialog::getSaveFileName(parent, tr("Save Image"), newFileName, APP->fileDialogFilterString);
    if (!newFileName.isNull()) {
        if (save(newFileName)) {
            settings.setValue("lastSaved", newFileName);
        }
        else {
            QMessageBox::critical(parent, QString(), QString(tr("Error saving file <b>\"%1\"</b>")).arg(QFileInfo(newFileName).fileName()));
        }
    }
    settings.endGroup();
}

bool ImageDocument::doOpen(QString fileName)
{
    return true;
}

bool ImageDocument::doSave(QString fileName)
{

    GLContextGrabber grab(&APP->shareWidget);
    uchar *data = m_imageData->readData();
    QImage::Format format;
    switch (m_imageData->format) {
    case TextureData::Format::Indexed:
        format = QImage::Format_Indexed8;
        break;
    case TextureData::Format::RGBA:
        format = QImage::Format_ARGB32;
        break;
    default:
        delete data;
        return false;
    }
    QImage qImage = QImage(data, m_imageData->size.width(), m_imageData->size.height(), format);
    if (m_paletteData && (format == QImage::Format_Indexed8)) {
        uchar *palette = m_paletteData->readData();
        std::vector<QRgb> vector((QRgb *)palette, ((QRgb *)palette) + m_paletteData->length());
        qImage.setColorTable(QVector<QRgb>::fromStdVector(vector));
        delete palette;
    }
    delete data;
    return qImage.save(fileName);

}
