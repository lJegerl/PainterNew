#include <QtWidgets>
#include "scribblearea.h"



ScribbleArea::ScribbleArea(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StaticContents);
    modified = false;
    scribbling = false;
    myPenWidth = 1;
    myPenColor = Qt::blue;
}


bool ScribbleArea::openImage(const QString &fileName)
{
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;
    QSize newSize = loadedImage.size().expandedTo(size());
    resizeImage(&loadedImage, newSize);
    image = loadedImage;
    modified = false;
    update();
    return true;
}


bool ScribbleArea::saveImage(const QString &fileName, const char *fileFormat)
{
    QImage visibleImage = image;
    resizeImage(&visibleImage, size());
    if (visibleImage.save(fileName, fileFormat)) {
        modified = false;
        return true;
    } else {
        return false;
    }
}


void ScribbleArea::setPenColor(const QColor &newColor)
{
    myPenColor = newColor;
}

void ScribbleArea::setPenWidth(int newWidth)
{
    myPenWidth = newWidth;
}

void ScribbleArea::setFigure(int _Count)
{
    Count = _Count;
}

void ScribbleArea::clearImage()
{
    image.fill(qRgb(255, 255, 255));
    modified = true;
    update();
}


void ScribbleArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        lastPoint = event->pos();
        FirstPoint = event->pos();
        scribbling = true;
    }
}

void ScribbleArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && scribbling) {
        if(Count == 1) {
            drawLineTo(event->pos());
        }
        else if (Count == 2) {

            drawEllipseTo(event->pos());
        }
        else if (Count == 3) {
            drawSquareTo(event->pos());

        }


        scribbling = false;
    }
}


void ScribbleArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QRect dirtyRect = event->rect();
    painter.drawImage(dirtyRect, image, dirtyRect);
}

void ScribbleArea::resizeEvent(QResizeEvent *event)
{
    if (width() > image.width() || height() > image.height()) {
        int newWidth = qMax(width() + 128, image.width());
        int newHeight = qMax(height() + 128, image.height());
        resizeImage(&image, QSize(newWidth, newHeight));
        update();
    }
    QWidget::resizeEvent(event);
}


void ScribbleArea::drawLineTo(const QPoint &endPoint)
{
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap,
                        Qt::RoundJoin));
    painter.drawLine(FirstPoint, endPoint);
    modified = true;
    int rad = (myPenWidth / 2) + 2;
    update(QRect(lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
}

void ScribbleArea::drawSquareTo(const QPoint &endPoint)
{
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(FirstPoint.x(),FirstPoint.y(), endPoint.x(), FirstPoint.y());
    painter.drawLine(FirstPoint.x(),FirstPoint.y(), FirstPoint.x(),endPoint.y());
    painter.drawLine(endPoint.x(),FirstPoint.y(), endPoint.x(), endPoint.y());
    painter.drawLine(FirstPoint.x(),endPoint.y(), endPoint.x(), endPoint.y());
    modified = true;
    int rad = (myPenWidth / 2) + 2;

    update(QRect(lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
}

void ScribbleArea::drawEllipseTo(const QPoint &endPoint)
{
    QPainter painter(&image);
    painter.setPen(QPen(myPenColor, myPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if(endPoint.y() > FirstPoint.y()) {
        if(endPoint.x() > FirstPoint.x()) {
            painter.drawEllipse(FirstPoint.x(),FirstPoint.y(), endPoint.x()-FirstPoint.x(), endPoint.y()-FirstPoint.y());
        }
        else {
            painter.drawEllipse(endPoint.x(),FirstPoint.y(), FirstPoint.x()-endPoint.x(), endPoint.y()-FirstPoint.y());
        }
    }
    else {
        if(endPoint.x() > FirstPoint.x()) {
            painter.drawEllipse(FirstPoint.x(),endPoint.y(), endPoint.x()-FirstPoint.x(), FirstPoint.y()-endPoint.y());
        }
        else {
            painter.drawEllipse(endPoint.x(),endPoint.y(), FirstPoint.x()-endPoint.x(), FirstPoint.y()-endPoint.y());
        }
    }
    modified = true;
    int rad = (myPenWidth / 2) + 2;

    update(QRect(lastPoint, endPoint).normalized().adjusted(-rad, -rad, +rad, +rad));
    lastPoint = endPoint;
}


void ScribbleArea::resizeImage(QImage *image, const QSize &newSize)
{
    if (image->size() == newSize)
        return;
    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(qRgb(255, 255, 255));
    QPainter painter(&newImage);
    painter.drawImage(QPoint(0, 0), *image);
    *image = newImage;
}
