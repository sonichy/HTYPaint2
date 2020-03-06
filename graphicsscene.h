#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#define GRAPHICSITEM_ID Qt::UserRole

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit GraphicsScene(QObject *parent = 0);
    QPointF startPnt;   //起点
    QPointF endPnt;     //终点
    QPen pen;
    QBrush brush;
    QFont font;
    QString text;
    QPainterPath *PP;
    enum { /*定义图形的类型 */
        NONE_DRAW,
        //POINT_DRAW,
        PATH_DRAW,
        LINE_DRAW,
        ARROW_DRAW,
        RECT_DRAW,
        ELLIPSE_DRAW,
        TEXT_DRAW,
        FILL_DRAW,
        ERASE_DRAW,
        MOVE_DRAW,
        SELECT_DRAW,
        CLIP_DRAW,
        IMAGE_DRAW,
        DEL_DRAW,
        COLORPICKER_DRAW,
        RECT_SELECT
    } draw_type;
    void copy();
    bool isFill;
    int GIID;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:

signals:
    void newItem(QString s);

};

#endif // GRAPHICSSCENE_H