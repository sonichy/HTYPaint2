#ifndef GRAPHICSRECTITEM_H
#define GRAPHICSRECTITEM_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPen>

class GraphicsRectItem : public QGraphicsRectItem
{
public:
    GraphicsRectItem();
    QPointF startPnt;
    QPointF endPnt;
    QPen pen;
    QBrush brush;

protected:
    //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

};

#endif // GRAPHICSRECTITEM_H