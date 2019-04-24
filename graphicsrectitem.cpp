#include "graphicsrectitem.h"
#include <QPainter>
#include <QDebug>

GraphicsRectItem::GraphicsRectItem()
{
    setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

//void QGraphicsRectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR)
//{
    //painter->setPen(pen);
    //painter->setBrush(brush);
    //QRectF rect(startPnt, endPnt);
    //painter->drawRect(rect);
//}

void GraphicsRectItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    startPnt = event->scenePos();
}

void GraphicsRectItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << startPnt << endPnt;
    endPnt = event->scenePos();
    QRectF rect(startPnt, endPnt);
    setRect(rect);
}

void GraphicsRectItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    endPnt = event->scenePos();
    qDebug() << startPnt << endPnt;
    QRectF rect(startPnt, endPnt);
    setRect(rect);
}

