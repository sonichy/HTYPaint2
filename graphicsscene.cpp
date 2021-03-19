#include "graphicsscene.h"
#include <graphicsrectitem.h>
#include <QDebug>
#include <QGraphicsTextItem>
#include <QtMath>
#include <QApplication>
#include <QDesktopWidget>
#include <QClipboard>

GraphicsScene::GraphicsScene(QObject *parent):QGraphicsScene(parent)
{
    brush = QBrush(Qt::transparent, Qt::SolidPattern);
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    startPnt = event->scenePos();
    endPnt = event->scenePos();
    switch(draw_type){
    case PATH_DRAW:
        PP = new QPainterPath(startPnt);
        break;
    default:
        break;
    }
    QGraphicsScene::mousePressEvent(event);//不加这句图元无法点选和拖动
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    endPnt = event->scenePos();
    switch(draw_type){
    case PATH_DRAW:
        PP->lineTo(endPnt);
        break;
    default:
        break;
    }
    QGraphicsScene::mouseMoveEvent(event);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    //qDebug() << startPnt << endPnt;
    endPnt = event->scenePos();
    if (draw_type == PATH_DRAW) {
        QGraphicsPathItem *GPI = addPath(*PP, pen, brush);
        GPI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        GPI->setData(GRAPHICSITEM_ID, GIID);
        emit newItem("Path");
    } else if (draw_type == LINE_DRAW) {
        QGraphicsLineItem *GLI = addLine(QLineF(startPnt, endPnt));;
        GLI->setPen(pen);
        GLI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        GLI->setData(GRAPHICSITEM_ID, GIID);
        emit newItem("Line");
    } else if (draw_type == RECT_DRAW) {
        QGraphicsRectItem *GRI = addRect(QRectF(startPnt, endPnt), pen, brush);
        GRI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemClipsToShape |QGraphicsItem::ItemClipsChildrenToShape);
        GRI->setData(GRAPHICSITEM_ID, GIID);
        emit newItem("Rect");
    } else if (draw_type == ELLIPSE_DRAW) {
        QGraphicsEllipseItem *GEI = addEllipse(QRectF(startPnt, endPnt), pen, brush);
        GEI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        GEI->setData(GRAPHICSITEM_ID, GIID);
        emit newItem("Ellipse");
    } else if (draw_type == TEXT_DRAW) {
        QGraphicsTextItem *GTI = addText(text, font);
        GTI->setDefaultTextColor(pen.color());
        GTI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        GTI->setPos(endPnt);
        GTI->setData(GRAPHICSITEM_ID, GIID);
        emit newItem("Text");
    } else if (draw_type == RECT_SELECT) {
        PP = new QPainterPath;
        PP->addRect(QRectF(startPnt, endPnt));
        setSelectionArea(*PP);
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void GraphicsScene::copy()
{
    QRectF rect(startPnt, endPnt);
    QPixmap pixmap = QPixmap::grabWindow(QApplication::desktop()->winId(), static_cast<int>(rect.x()), static_cast<int>(rect.y()), static_cast<int>(rect.width()), static_cast<int>(rect.height()));
    //QPixmap pixmap = this->grab(rect);
    QApplication::clipboard()->setPixmap(pixmap, QClipboard::Clipboard);
}