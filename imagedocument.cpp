#include "imagedocument.h"

#include "util.h"
#include <QMessageBox>
#include <QSettings>
#include <QDir>
#include <QFileDialog>

ImageDocument::ImageDocument(const QSize &size, TextureDataFormat format, QObject *parent) :
    Document(QString(), parent), m_imageData(nullptr), m_paletteData(nullptr)
{
    ContextGrabber grab(APP->shareWidget());

    if (format == TextureDataFormat::Invalid) {
        qDebug() << "Invalid image format";
        return;
    }

    uint fillColour = 0;
    if (format == TextureDataFormat::Indexed) {
        m_paletteData = new PaletteData(APP->shareWidget(), 2);
        m_paletteData->setColour(0, qRgba(0, 0, 0, 255));
        m_paletteData->setColour(1, qRgba(255, 255, 255, 255));
        fillColour = 1;
//        m_contextColours[ContextColour::Primary] = 1;
//        m_contextColours[ContextColour::Secondary] = m_contextColours[ContextColour::Eraser] = 0;
    }
    else if (format == TextureDataFormat::RGBA) {
        fillColour = qRgba(255, 0, 0, 255);
//        m_contextColours[ContextColour::Primary] = qRgba(255, 255, 255, 255);
//        m_contextColours[ContextColour::Secondary] = m_contextColours[ContextColour::Eraser] = qRgba(0, 0, 0, 0);
    }

    m_imageData = new ImageData(APP->shareWidget(), size, format);
    m_imageData->clear(fillColour);
}

ImageDocument::ImageDocument(const QString &fileName, const char *fileFormat, QObject *parent) :
    Document(fileName, parent), m_imageData(nullptr), m_paletteData(nullptr)
{
    ContextGrabber grab(APP->shareWidget());

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

    TextureDataFormat format;
    switch (image.format()) {
    case QImage::Format_Indexed8:
        format = TextureDataFormat::Indexed;
        break;
    case QImage::Format_ARGB32:
        format = TextureDataFormat::RGBA;
        break;
    default:
        format = TextureDataFormat::Invalid;
        qDebug() << "Invalid image format";
        return;
    }

    m_paletteData = nullptr;
    if (!image.colorTable().isEmpty()) {
        m_paletteData = new PaletteData(APP->shareWidget(), image.colorTable().size(), (GLubyte *)image.colorTable().constData());
    }

    switch (format) {
    case TextureDataFormat::Indexed:
        if (m_paletteData && m_paletteData->length() > 0) {
//            m_contextColours[ContextColour::Primary] = m_paletteData->colour(m_paletteData->length() - 1);
        }
        else {
//            m_contextColours[ContextColour::Primary] = 255;
        }
//        m_contextColours[ContextColour::Secondary] = 0;
        break;
    case TextureDataFormat::RGBA:
//        m_contextColours[ContextColour::Primary] = qRgba(255, 255, 255, 255);
//        m_contextColours[ContextColour::Secondary] = qRgba(0, 0, 0, 0);
        break;
    }

    m_imageData = new ImageData(APP->shareWidget(), image.size(), format, image.constBits());
}

ImageDocument::~ImageDocument()
{
    ContextGrabber grab(APP->shareWidget());
    delete m_imageData;
    delete m_paletteData;
}

Editor *ImageDocument::createEditor() {
    ImageEditor *editor = new ImageEditor(*this);
    return editor;
}

ImageDocument *ImageDocument::newGui(QWidget *const parent)
{
    ImageDocument *image = nullptr;
//    NewDialog *dialog = new NewDialog(parent);
//    if (dialog->exec()) {
//        Image *image = new Image(dialog->imageSize(), dialog->mode());
//        m_images.append(image);
//        ImageEditor *editor = newEditor(image);
//    }
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

    APP->shareWidget()->makeCurrent();
    uchar *data = m_imageData->readData();
    QImage::Format format;
    switch (m_imageData->format()) {
    case TextureDataFormat::Indexed:
        format = QImage::Format_Indexed8;
        break;
    case TextureDataFormat::RGBA:
        format = QImage::Format_ARGB32;
        break;
    default:
        delete data;
        return false;
    }
    QImage qImage = QImage(data, m_imageData->size().width(), m_imageData->size().height(), format);
    if (m_paletteData && (format == QImage::Format_Indexed8)) {
        uchar *palette = m_paletteData->readData();
        std::vector<QRgb> vector((QRgb *)palette, ((QRgb *)palette) + m_paletteData->length());
        qImage.setColorTable(QVector<QRgb>::fromStdVector(vector));
        delete palette;
    }
    delete data;
    return qImage.save(fileName);

}
