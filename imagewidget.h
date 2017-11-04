#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>
#include <QPen>
#include <QLabel>
//线段
//typedef struct myLine{
//    QPoint startPnt;
//    QPoint endPnt;
//}myLine;

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    enum				/*定义图形的类型 */
    {
        NONE_DRAW,
        POINT_DRAW,
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
        COLORPICKER_DRAW
    } draw_type;
    ImageWidget(QWidget *parent = 0);
    ~ImageWidget();    
    QImage image,imgtemp,imgbuf[10],imgmove;
    QPoint startPnt;   //起点
    QPoint endPnt;     //终点
    QPen pen;
    QBrush brush;
    QString text;
    QLabel *labelFont;
    bool boolBorder,boolFill;    
    void load(QString fileName);
    void save(QString path);
    void setAngle(qreal rotateAngle);
    void rotate(qreal degrees);
    void mirror(bool bh,bool bv);
    void copy();
    void paste();
    void draw(QImage &img);    
    void newsize(int width,int height);
    void scale(int ratioW,int ratioH);
    void moveImgbuf();

    void gray();
    void invert();
    void blur(int p);

public slots:
    void newfile();
    void drawPoint();
    void drawLine();
    void drawArrow();
    void drawRect();
    void drawEllipse();
    void drawText();
    void drawFill();
    void drawErase();
    void drawMove();
    void drawRectselect();
    void colorPicker();
    void cutSelect();
    void delSelect();
    void zoomin();
    void zoomout();
    void zoom1();
    void selectAll();
    void undo();
    void redo();

private slots:    
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void moveTopUp();
    void moveTopDown();
    void moveLeftLeft();
    void moveLeftRight();
    void moveRightLeft();
    void moveRightRight();
    void moveBottomUp();
    void moveBottomDown();

protected:
    void paintEvent(QPaintEvent *);

private:
    QImage imgload,imgpaste;
    int cundo;
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);

signals:
    void statusbar2Message(QString);
};

#endif // IMAGEWIDGET_H
