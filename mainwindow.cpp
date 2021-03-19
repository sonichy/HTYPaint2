#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "graphicsscene.h"
#include <QDesktopWidget>
#include <QMessageBox>
#include <QPainter>
#include <QMouseEvent>
#include <QToolButton>
#include <QColorDialog>
#include <QSpinBox>
#include <QLineEdit>
#include <QDebug>
#include <QFileDialog>
#include <QFont>
#include <QFontDialog>
#include <QCheckBox>
#include <QTime>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QWidgetAction>
#include <QClipboard>
#include <QImage>
#include <QScrollArea>
#include <QLabel>
#include <QMimeData>
#include <QTextBrowser>
#include <QShortcut>
#include <QGraphicsItem>
#include <QSvgGenerator>
#include <QMetaEnum>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setStyleSheet("QScrollArea{ background-color:#cccccc; }");
    ui->splitter->setStretchFactor(4,1);
    ui->textEdit->setText("文字内容");
    connect(ui->textEdit, SIGNAL(textChanged()), this, SLOT(textChange()));
    font = qApp->font();
    ui->pushButton_text_font->setText(font.family() + "," + QString::number(font.pointSize()));
    color_background = Qt::transparent;

    //群组实现单选
    QActionGroup *AG = new QActionGroup(ui->mainToolBar);
    AG->addAction(ui->actionPencil);
    AG->addAction(ui->actionLine);
    AG->addAction(ui->actionArrow);
    AG->addAction(ui->actionRect);
    AG->addAction(ui->actionEllipse);
    AG->addAction(ui->actionText);
    AG->addAction(ui->actionColorPicker);
    AG->addAction(ui->actionFill);
    AG->addAction(ui->actionErase);
    AG->addAction(ui->actionMove);
    AG->addAction(ui->actionRectSelect);
    AG->setExclusive(true);

    //状态栏
    LSB1 = new QLabel("欢迎使用海天鹰画图！");
    LSB1->setMinimumSize(500,20);
    LSB1->setStyleSheet("padding:0px 3px;");
    //LSB1->setFrameShape(QFrame::WinPanel);
    //LSB1->setFrameShadow(QFrame::Sunken);
    LSB2 = new QLabel("");
    LSB2->setMinimumSize(150,20);
    LSB2->setStyleSheet("padding:0px 3px;");
    LSB2->setAlignment(Qt::AlignCenter);
    //LSB2->setFrameShape(QFrame::WinPanel);
    //LSB2->setFrameShadow(QFrame::Sunken);
    ui->statusBar->addWidget(LSB1);
    ui->statusBar->addWidget(LSB2);

    path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    //text = "文字内容";
    move((QApplication::desktop()->width() - width())/2, (QApplication::desktop()->height() - height())/2);

    connect(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_Up), this), SIGNAL(activated()), this, SLOT(moveUp()));
    connect(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_Down), this), SIGNAL(activated()), this, SLOT(moveDown()));
    connect(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_Left), this), SIGNAL(activated()), this, SLOT(moveLeft()));
    connect(new QShortcut(QKeySequence(Qt::ALT + Qt::Key_Right), this), SIGNAL(activated()), this, SLOT(moveRight()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up), this), SIGNAL(activated()), this, SLOT(moveTopUp()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down), this), SIGNAL(activated()), this, SLOT(moveTopDown()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Left), this), SIGNAL(activated()), this, SLOT(moveLeftLeft()));
    connect(new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Right), this), SIGNAL(activated()), this, SLOT(moveLeftRight()));
    connect(new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Left), this), SIGNAL(activated()), this, SLOT(moveRightLeft()));
    connect(new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Right), this), SIGNAL(activated()), this, SLOT(moveRightRight()));
    connect(new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Up), this), SIGNAL(activated()), this, SLOT(moveBottomUp()));
    connect(new QShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Down), this), SIGNAL(activated()), this, SLOT(moveBottomDown()));

    connect(ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(listWidgetCurrentItemChanged(QListWidgetItem*, QListWidgetItem*)));
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

    scene = new GraphicsScene;
    //scene->text = text;
    scene->font = qApp->font();
    scene->GIID = 0;
    ui->graphicsView->setScene(scene);
    ui->graphicsView->resize(800,600);
    connect(scene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()));
    connect(scene, SIGNAL(newItem(QString)), this, SLOT(listWidgetAddObject(QString)));

    //生成棋盘背景
    QPixmap pixmap(50,50);
    pixmap.fill(Qt::white);
    QPainter painter(&pixmap);
    QBrush brush(QColor(200,200,200));
    painter.fillRect(25,0,25,25,brush);
    painter.fillRect(0,25,25,25,brush);
    ui->graphicsView->setBackgroundBrush(pixmap);

    QStringList Largs = QApplication::arguments();
    qDebug() << Largs;
    if (Largs.length() > 1) {
        QString filepath = Largs.at(1);
        if(filepath.startsWith("file://")){
            QUrl url(filepath);
            filepath = url.toLocalFile();
        }
        qDebug() << filepath;
        open(filepath);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_action_changelog_triggered()
{
    QString s = "2.4\n(2021-03)\n文字、笔、刷工具移到右下。\n\n2.3\n(2020-03)\n增加：对象列表和选取。\n修复：右键不能打开图片的问题\n修复：打开qgs文件，图元无法移动的问题。\n\n2.2\n(2019-11)\n增加：属性页设置背景。\n增加：置顶和置底。\n增加：组群和散群。\n增加：文字工具可以绘制背景。\n\n2.1\n(2019-08)\n增加：视图缩放。透明度设置。\n修复：没有滚动条。但是缩放视图后，横向滚动条消失。\n\n2.0\n(2019-04)\n增加保存为SVG格式。\n粘贴剪贴板里的图片。\n水平、垂直镜像。\n修改文字内容。\n图像缩放\n路径绘制。\n实现框选。\n直线、矩形、椭圆快捷键微调大小。\n图元可以修改颜色、移动、删除。\n实现画点、线、框、圆、字。";
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("更新历史");
    dialog->setFixedSize(400,300);
    QVBoxLayout *vbox = new QVBoxLayout;
    QTextBrowser *textBrowser = new QTextBrowser;
    textBrowser->setText(s);
    textBrowser->zoomIn();
    vbox->addWidget(textBrowser);
    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *button_confirm = new QPushButton("确定");
    hbox->addStretch();
    hbox->addWidget(button_confirm);
    hbox->addStretch();
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    dialog->show();
    connect(button_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    if (dialog->exec() == QDialog::Accepted) {
        dialog->close();
    }
}

void MainWindow::on_action_aboutQt_triggered()
{
    QMessageBox::aboutQt(nullptr, "关于 Qt");
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰画图 2.4\n一款基于 QGraphicItem 的画图程序。\n作者：海天鹰\nE-mail: sonichy@163.com\n主页：https://github.com/sonichy\n参考文献：\nQGraphicsScene 管理 QGraphicsItem（单击/选择/移动/缩放/删除）：\nhttps://blog.csdn.net/liang19890820/article/details/53504323\nQGraphicsItem 的类型检测与转换 https://blog.csdn.net/liang19890820/article/details/53612446\n水平、垂直镜像算法：https://gerrysweeney.com/horizontal-and-vertical-flip-transformations-of-a-qgraphicsitem-in-qt-qgraphicsview\n保存SVG：Qt_drawcli\nQGraphicsItem 层次调整：https://blog.csdn.net/ly7969/article/details/8073883");
    aboutMB.setIconPixmap(QPixmap(":/HTYPaint2.png"));
    aboutMB.exec();
}

void MainWindow::on_actionPencil_triggered()
{
    scene->draw_type = scene->PATH_DRAW;
    ui->graphicsView->setCursor(QCursor(QPixmap(":/images/pencil.png")));
}

void MainWindow::on_actionLine_triggered()
{
    scene->draw_type = scene->LINE_DRAW;
    ui->graphicsView->setCursor(QCursor(QPixmap(":/images/line.png")));
}

void MainWindow::on_actionArrow_triggered()
{
    scene->draw_type = scene->ARROW_DRAW;
    ui->graphicsView->setCursor(QCursor(QPixmap(":/images/arrow.png")));
}

void MainWindow::on_actionRect_triggered()
{
    scene->draw_type = scene->RECT_DRAW;
    ui->graphicsView->setCursor(QCursor(QPixmap(":/images/rect.png")));
}

void MainWindow::on_actionEllipse_triggered()
{
    scene->draw_type = scene->ELLIPSE_DRAW;
    ui->graphicsView->setCursor(QCursor(QPixmap(":/images/ellipse.png")));
}

void MainWindow::on_actionText_triggered()
{
    scene->draw_type = scene->TEXT_DRAW;
    scene->text = ui->textEdit->toPlainText();
    ui->graphicsView->setCursor(QCursor(QPixmap(":/images/text.png")));
}

void MainWindow::on_actionFill_triggered()
{
    scene->draw_type = scene->FILL_DRAW;
    ui->graphicsView->setCursor(QCursor(QPixmap(":/images/fill.png")));
}

void MainWindow::on_actionErase_triggered()
{
    scene->draw_type = scene->ERASE_DRAW;
    ui->graphicsView->setCursor(QCursor(QPixmap(":/images/eraser.png")));
}

void MainWindow::on_actionMove_triggered()
{
    if(scene->draw_type == scene->SELECT_DRAW){
        //imgmove = imgtemp.copy(startPnt.x()+1,startPnt.y()+1,endPnt.x()-startPnt.x()-1,endPnt.y()-startPnt.y()-1);
    }else{
        //imgmove = imgpaste;
    }
    scene->draw_type = scene->MOVE_DRAW;
    ui->graphicsView->setCursor(QCursor(QPixmap(":/images/move.png")));
}

void MainWindow::on_actionRectSelect_triggered()
{
    scene->draw_type = scene->RECT_SELECT;
    ui->graphicsView->setCursor(QCursor(QPixmap(":/images/rectselect.png")));
}

void MainWindow::on_actionColorPicker_triggered()
{
    scene->draw_type = scene->COLORPICKER_DRAW;
    ui->graphicsView->setCursor(QCursor(QPixmap(":/images/color_picker.png")));
}

void MainWindow::on_action_selectAll_triggered()
{
    QList<QGraphicsItem*> list_item = scene->items();
    qDebug() << list_item;
    for(int i=0; i<list_item.size(); i++){
        list_item[i]->setSelected(true);
    }
    LSB2->setText("共 " + QString::number(list_item.size()) + " 个图元");
}

void MainWindow::on_action_delete_triggered()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for(int i=0; i<list_item.size(); i++){
        scene->removeItem(list_item[i]);
    }
}

void MainWindow::on_action_createGroup_triggered()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    scene->clearSelection();
    QGraphicsItemGroup *GIG = scene->createItemGroup(list_item);
    GIG->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
}

void MainWindow::on_action_destroyGroup_triggered()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        if (list_item[i]->type() == QGraphicsItemGroup::Type) {
            QGraphicsItemGroup *GIG = qgraphicsitem_cast<QGraphicsItemGroup*>(list_item[i]);
            scene->destroyItemGroup(GIG);
        }
    }
}

void MainWindow::spinValueChange(int v)
{
    scene->pen.setWidth(v);
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        if (list_item[i]->type() == QGraphicsPathItem::Type) {
            QGraphicsPathItem *GPI = qgraphicsitem_cast<QGraphicsPathItem*>(list_item[i]);
            GPI->setPen(scene->pen);
        } else if (list_item[i]->type() == QGraphicsLineItem::Type) {
            QGraphicsLineItem *GLI = qgraphicsitem_cast<QGraphicsLineItem*>(list_item[i]);
            GLI->setPen(scene->pen);
        } else if (list_item[i]->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_item[i]);
            GRI->setPen(scene->pen);
        } else if (list_item[i]->type() == QGraphicsTextItem::Type) {
            QGraphicsTextItem *GTI = qgraphicsitem_cast<QGraphicsTextItem*>(list_item[i]);
            font.setPointSize(v);
            GTI->setFont(font);
            scene->font = font;
            ui->pushButton_text_font->setText(font.family() + "," + QString::number(font.pointSize()));
        }
    }
}

void MainWindow::textChange()
{
    QString s = ui->textEdit->toPlainText();
    //scene->text = s;
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        if (list_item[i]->type() == QGraphicsTextItem::Type) {
            QGraphicsTextItem *GTI = qgraphicsitem_cast<QGraphicsTextItem*>(list_item[i]);
            GTI->setPlainText(s);
        }
    }
}

void MainWindow::on_action_new_triggered()
{
    scene->clear();
    ui->listWidget->clear();
    setWindowTitle("海天鹰画图2");
}

void MainWindow::on_action_open_triggered()
{    
    path = QFileDialog::getOpenFileName(this, "打开图片", path, "图片文件(*.jpg *.jpeg *.png *.bmp);;QGraphicsScene(*.qgs)");
    if(path.length() != 0){
        open(path);
    }
}

void MainWindow::open(QString filepath)
{
    scene->clear();
    ui->listWidget->clear();
    if (QFileInfo(filepath).suffix() == "qgs") {
        QFile *file = new QFile(filepath);
        if (file->open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream TS(file);
            TS.setCodec("UTF-8");
            QString s = TS.readAll();
            file->close();
            QStringList line = s.split("\n");
            for (int i=0; i<line.size(); i++) {
                if (line.at(i).startsWith("QGraphicsScene:")) {
                    QString s = line.at(i).mid(line.at(i).indexOf(":") + 1);
                    int w = s.mid(0, s.indexOf(",")).toInt();
                    int h = s.mid(s.indexOf(",") + 1).toInt();
                    QRect rect(0, 0, w, h);
                    scene->setSceneRect(rect);
                } else if (line.at(i).startsWith("QGraphicsRectItem:")){
                    QString s = line.at(i).mid(line.at(i).indexOf(":") + 1);
                    QStringList params = s.split(";");
                    QString xywh = params.at(0);
                    QStringList SL_xywh = xywh.split(",");
                    int x = SL_xywh.at(0).toInt();
                    int y = SL_xywh.at(1).toInt();
                    int w = SL_xywh.at(2).toInt();
                    int h = SL_xywh.at(3).toInt();
                    QStringList SL_color = params.at(1).split(",");
                    QColor color_pen(SL_color.at(0).toInt(), SL_color.at(1).toInt(), SL_color.at(2).toInt(), SL_color.at(3).toInt());
                    QPen pen(color_pen);
                    int pw = params.at(3).toInt();
                    pen.setWidth(pw);
                    SL_color = params.at(2).split(",");
                    QColor color_brush(SL_color.at(0).toInt(), SL_color.at(1).toInt(), SL_color.at(2).toInt(), SL_color.at(3).toInt());
                    QBrush brush(color_brush);
                    QGraphicsRectItem *GRI = scene->addRect(x, y, w, h, pen, brush);
                    GRI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                    GRI->setData(GRAPHICSITEM_ID, scene->GIID);
                    listWidgetAddObject("Rect");
                } else if (line.at(i).startsWith("QGraphicsEllipseItem:")) {
                    QString s = line.at(i).mid(line.at(i).indexOf(":") + 1);
                    QStringList params = s.split(";");
                    QString xywh = params.at(0);
                    QStringList SL_xywh = xywh.split(",");
                    int x = SL_xywh.at(0).toInt();
                    int y = SL_xywh.at(1).toInt();
                    int w = SL_xywh.at(2).toInt();
                    int h = SL_xywh.at(3).toInt();
                    QStringList SL_color = params.at(1).split(",");
                    QColor color_pen(SL_color.at(0).toInt(), SL_color.at(1).toInt(), SL_color.at(2).toInt(), SL_color.at(3).toInt());
                    QPen pen(color_pen);
                    int pw = params.at(3).toInt();
                    pen.setWidth(pw);
                    SL_color = params.at(2).split(",");
                    QColor color_brush(SL_color.at(0).toInt(), SL_color.at(1).toInt(), SL_color.at(2).toInt(), SL_color.at(3).toInt());
                    QBrush brush(color_brush);
                    QGraphicsEllipseItem *GEI = scene->addEllipse(x, y, w, h, pen, brush);
                    GEI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                    GEI->setData(GRAPHICSITEM_ID, scene->GIID);
                    listWidgetAddObject("Ellipse");
                } else if (line.at(i).startsWith("QGraphicsTextItem:")) {
                    QString s = line.at(i).mid(line.at(i).indexOf(":") + 1);
                    QStringList params = s.split(";");
                    QString xy = params.at(0);
                    int x = xy.mid(0, s.indexOf(",")).toInt();
                    int y = xy.mid(s.indexOf(",") + 1).toInt();
                    QStringList SL_font = params.at(2).split(",");
                    QFont font(SL_font.at(0), SL_font.at(1).toInt(), SL_font.at(2).toInt(), SL_font.at(3).toInt());
                    QGraphicsTextItem *GTI = scene->addText(params.at(1), font);
                    QStringList SL_color = params.at(3).split(",");
                    QColor color(SL_color.at(0).toInt(), SL_color.at(1).toInt(), SL_color.at(2).toInt(), SL_color.at(3).toInt());
                    GTI->setDefaultTextColor(color);
                    GTI->setPos(QPoint(x,y));
                    GTI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
                    GTI->setData(GRAPHICSITEM_ID, scene->GIID);
                    listWidgetAddObject("Text");
                }
            }
        }
    } else {
        QPixmap pixmap(filepath);
        QGraphicsPixmapItem *GPI = scene->addPixmap(pixmap);
        GPI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        GPI->setData(GRAPHICSITEM_ID, scene->GIID);
        scene->setSceneRect(0, 0, pixmap.width(), pixmap.height());
        ui->graphicsView->resize(pixmap.size() + QSize(2,2));
        listWidgetAddObject("Image");
    }
    filename = QFileInfo(filepath).fileName();
    setWindowTitle(filename + " - 海天鹰画图2");
    LSB1->setText("打开 " + filepath);
    path = filepath;
    qDebug() << path;
}

void MainWindow::on_action_import_triggered()
{
    QString filenamei = "";
    filenamei = QFileDialog::getOpenFileName(this, "导入图片", path, "图片文件(*.jpg *.png *.bmp)");
    if (path.length() != 0) {
        LSB1->setText("导入 " + filenamei);
        QPixmap pixmap(filenamei);
        QGraphicsPixmapItem *GPI = scene->addPixmap(pixmap);
        GPI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        GPI->setData(GRAPHICSITEM_ID, scene->GIID);
        listWidgetAddObject("Image");
    }
}

void MainWindow::on_action_save_triggered()
{
    qDebug() << "save path=" << path;
    if (path.length() != 0) {
        save(path);
    } else {
        on_action_saveas_triggered();
    }
}

void MainWindow::on_action_saveas_triggered()
{
    if (path=="")
        path = "./未命名.jpg";
    path = QFileDialog::getSaveFileName(this, "保存图片", path, "图片文件(*.jpg *.png *.bmp *.svg);;QGraphicsScene(*.qgs)");
    if (path.length() != 0) {
        save(path);
    }
}

void MainWindow::save(QString filepath)
{
    LSB1->setText("保存 " + filepath);
    scene->clearSelection();    //清除选区（虚线框），不然会一起保存。
    QSize size(static_cast<int>(scene->width()), static_cast<int>(scene->height()));
    if (QFileInfo(filepath).suffix() == "svg") {
        QSvgGenerator generator;
        generator.setFileName(filepath);
        generator.setSize(size);
        generator.setViewBox(QRect(0, 0, size.width(), size.height()));
        generator.setTitle(QFileInfo(filepath).baseName());
        generator.setDescription("A SVG file created by the QSvgGenerator of Qt.");
        QPainter painter;
        painter.begin(&generator);
        scene->render(&painter);
        painter.end();
    } else if(QFileInfo(filepath).suffix() == "qgs") {
        QString s = "QGraphicsScene:" + QString::number(scene->width()) + "," + QString::number(scene->height());
        QList<QGraphicsItem*> list_item = scene->items();
        for (int i=list_item.size() - 1; i>=0; i--) {
            if (list_item[i]->type() == QGraphicsLineItem::Type) {
                QGraphicsLineItem *GLI = qgraphicsitem_cast<QGraphicsLineItem*>(list_item[i]);
                QLineF line = GLI->line();
                QPen pen = GLI->pen();
                QColor color = pen.color();
                s += "\nQGraphicsLineItem:" + QString::number(line.x1()) + "," + QString::number(line.y1()) + "," + QString::number(line.x2()) + "," + QString::number(line.y2()) + ";" + QString::number(color.red()) + "," + QString::number(color.green()) + QString::number(color.blue()) + "," + QString::number(color.alpha()) + ";" + QString::number(pen.width());
            } else if (list_item[i]->type() == QGraphicsRectItem::Type) {
                QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_item[i]);
                QRectF rect = GRI->rect();
                QPen pen = GRI->pen();
                QColor color_pen = pen.color();
                QBrush brush = GRI->brush();
                QColor color_brush = brush.color();
                s += "\nQGraphicsRectItem:" + QString::number(rect.x()) + "," + QString::number(rect.y()) + "," + QString::number(rect.width()) + "," + QString::number(rect.height()) + ";" + QString::number(color_pen.red()) + "," + QString::number(color_pen.green())  + "," + QString::number(color_pen.blue()) + "," + QString::number(color_pen.alpha()) + ";" + QString::number(color_brush.red()) + "," + QString::number(color_brush.green()) + "," + QString::number(color_brush.blue()) + "," + QString::number(color_brush.alpha()) + ";" + QString::number(pen.width());
            } else if (list_item[i]->type() == QGraphicsEllipseItem::Type) {
                QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_item[i]);
                QRectF rect = GEI->rect();
                QPen pen = GEI->pen();
                QColor color_pen = pen.color();
                QBrush brush = GEI->brush();
                QColor color_brush = brush.color();
                s += "\nQGraphicsEllipseItem:" + QString::number(rect.x()) + "," + QString::number(rect.y()) + "," + QString::number(rect.width()) + "," + QString::number(rect.height()) + ";" + QString::number(color_pen.red()) + "," + QString::number(color_pen.green()) + "," + QString::number(color_pen.blue()) + "," + QString::number(color_pen.alpha()) + ";" + QString::number(color_brush.red()) + "," + QString::number(color_brush.green()) + "," + QString::number(color_brush.blue()) + "," + QString::number(color_brush.alpha()) + ";" + QString::number(pen.width());
            } else if (list_item[i]->type() == QGraphicsTextItem::Type) {
                QGraphicsTextItem *GTI = qgraphicsitem_cast<QGraphicsTextItem*>(list_item[i]);
                QColor color = GTI->defaultTextColor();
                s += "\nQGraphicsTextItem:" + QString::number(GTI->x()) + "," + QString::number(GTI->y()) + ";" + GTI->toPlainText() + ";" + GTI->font().toString() + ";" + QString::number(color.red()) + "," + QString::number(color.green()) + "," + QString::number(color.blue()) + "," + QString::number(color.alpha());
            } else if (list_item[i]->type() == QGraphicsPixmapItem::Type) {
                QGraphicsPixmapItem *GPI = qgraphicsitem_cast<QGraphicsPixmapItem*>(list_item[i]);
                QPixmap pixmap = GPI->pixmap();
                s += "\nQGraphicsPixmapItem:" + QString::number(GPI->x()) + "," + QString::number(GPI->y()) + ";" + QString::number(pixmap.width()) + ";" + QString::number(pixmap.height());
            }
        }
        QFile file(filepath);
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        file.write(s.toUtf8());
        file.close();
    } else {
        QImage image(size, QImage::Format_ARGB32);
        image.fill(color_background);    //不填充背景有些保存的png上面会有一排画图的图标
        QPainter painter(&image);
        painter.setRenderHint(QPainter::Antialiasing, true);
        scene->render(&painter);
        image.save(filepath);
    }
    filename = QFileInfo(filepath).fileName();
    setWindowTitle(filename + " - 海天鹰画图2");
}

void MainWindow::on_pushButton_text_font_pressed()
{
    bool ok;
    font = QFontDialog::getFont(&ok, font, this, "选择字体");
    if (ok) {
        scene->font = font;
        QList<QGraphicsItem*> list_item = scene->selectedItems();
        for(int i=0; i<list_item.size(); i++){
            if(list_item[i]->type() == QGraphicsTextItem::Type){
                QGraphicsTextItem *GTI = qgraphicsitem_cast<QGraphicsTextItem*>(list_item[i]);
                GTI->setFont(font);
            }
        }
        ui->pushButton_text_font->setText(font.family() + "," + QString::number(font.pointSize()));
    }
}

void MainWindow::on_action_undo_triggered()
{

}

void MainWindow::on_action_redo_triggered()
{

}

void MainWindow::on_action_property_triggered()
{
    //qDebug() << image.width() << "X" << image.height();
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("属性");
    dialog->setFixedSize(200,200);
    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;
    QLabel *label = new QLabel("宽度");
    QSpinBox *spinw = new QSpinBox;
    spinw->setRange(0, 20000);
    spinw->setValue(static_cast<int>(scene->width()));
    hbox->addWidget(label, 0, Qt::AlignCenter);
    hbox->addWidget(spinw);
    vbox->addLayout(hbox);
    label = new QLabel("高度");
    QSpinBox *spinh = new QSpinBox;
    spinh->setRange(0, 20000);
    spinh->setValue(static_cast<int>(scene->height()));
    hbox = new QHBoxLayout;
    hbox->addWidget(label, 0, Qt::AlignCenter);
    hbox->addWidget(spinh);
    vbox->addLayout(hbox);

    label = new QLabel("背景");
    QCheckBox *checkBox_backgroundcolor = new QCheckBox;
    QPushButton *pushButton_backgroundcolor = new QPushButton("■");
    QPalette plt = pushButton_backgroundcolor->palette();
    plt.setColor(QPalette::ButtonText, color_background);
    pushButton_backgroundcolor->setPalette(plt);
    connect(pushButton_backgroundcolor, &QPushButton::pressed, [=](){
        QPalette plt = pushButton_backgroundcolor->palette();
        QBrush brush = plt.color(QPalette::ButtonText);
        QColor color = QColorDialog::getColor(brush.color(), this);
        if (color.isValid()) {
            color_background = color;
            plt.setColor(QPalette::ButtonText, color);
            pushButton_backgroundcolor->setPalette(plt);
        }
    });
    connect(checkBox_backgroundcolor, static_cast<void (QCheckBox::*)(int)>(&QCheckBox::stateChanged),[=](int state){
        switch (state) {
        case Qt::Unchecked:
            color_background = Qt::transparent;
            pushButton_backgroundcolor->setEnabled(false);
            break;
        case Qt::Checked:
            pushButton_backgroundcolor->setEnabled(true);
            break;
        default:
            break;
        }
    });
    if (color_background != Qt::transparent) {
        checkBox_backgroundcolor->setChecked(true);
    }
    hbox = new QHBoxLayout;
    hbox->addWidget(label, 0, Qt::AlignCenter);
    hbox->addWidget(checkBox_backgroundcolor);
    hbox->addWidget(pushButton_backgroundcolor);
    vbox->addLayout(hbox);

    QPushButton *pushButton_confirm = new QPushButton("确定");
    QPushButton *pushButton_cancel = new QPushButton("取消");
    hbox = new QHBoxLayout;
    hbox->addWidget(pushButton_confirm);
    hbox->addWidget(pushButton_cancel);
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    connect(pushButton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(pushButton_cancel, SIGNAL(clicked()), dialog, SLOT(reject()));
    if (dialog->exec() == QDialog::Accepted) {
        QRect rect(0, 0, spinw->value(), spinh->value());
        scene->setSceneRect(rect);
        ui->graphicsView->resize(rect.size() + QSize(5,5));
        ui->graphicsView->setBackgroundBrush(QBrush(color_background));
        //ui->graphicsView->viewport()->update();//无效，改变窗口大小才能更新
    }
    dialog->close();
}

void MainWindow::on_action_extrude_triggered()
{
    QGraphicsPixmapItem *GPI = nullptr;
    QPixmap pixmap;
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        if(list_item[i]->type() == QGraphicsPixmapItem::Type){
            GPI = qgraphicsitem_cast<QGraphicsPixmapItem*>(list_item[i]);
            pixmap = GPI->pixmap();
            wo = pixmap.width();
            ho = pixmap.height();
            break;  //只设置第一个位图的大小
        }
    }
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("缩放");

    QVBoxLayout *vbox = new QVBoxLayout;
    QHBoxLayout *hbox = new QHBoxLayout;
    QLabel *label = new QLabel("宽度：");
    spinw = new QSpinBox;
    spinw->setRange(1,10000);
    spinw->setValue(wo);
    hbox->addWidget(label,0,Qt::AlignCenter);
    hbox->addWidget(spinw);
    label = new QLabel("px");
    hbox->addWidget(label);
    spinwr = new QSpinBox;
    spinwr->setRange(10,1000);
    spinwr->setValue(100);
    hbox->addWidget(spinwr);
    label = new QLabel("%");
    hbox->addWidget(label);
    vbox->addLayout(hbox);

    hbox = new QHBoxLayout;
    label = new QLabel("高度：");
    hbox->addWidget(label,0,Qt::AlignCenter);
    spinh = new QSpinBox;
    spinh->setRange(1,10000);
    spinh->setValue(ho);
    hbox->addWidget(spinh);
    label = new QLabel("px");
    hbox->addWidget(label);
    spinhr = new QSpinBox;
    spinhr->setRange(10,1000);
    spinhr->setValue(100);
    hbox->addWidget(spinhr);
    label = new QLabel("%");
    hbox->addWidget(label);
    vbox->addLayout(hbox);

    QPushButton *pushButton_confirm = new QPushButton("确定");
    QPushButton *pushButton_cancel = new QPushButton("取消");
    hbox = new QHBoxLayout;
    hbox->addStretch();
    hbox->addWidget(pushButton_confirm);
    hbox->addWidget(pushButton_cancel);
    hbox->addStretch();
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);

    connect(spinw, SIGNAL(valueChanged(int)), this, SLOT(onSpinwChanged(int)));
    //    connect(spinw, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){
    //        spinwr->setValue(i*100/wo);
    //    });
    connect(spinh, SIGNAL(valueChanged(int)), this, SLOT(onSpinhChanged(int)));
    //    connect(spinh, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){
    //        spinhr->setValue(i*100/ho);
    //    });
    connect(spinwr, SIGNAL(valueChanged(int)), this, SLOT(onSpinwrChanged(int)));
    //    connect(spinwr, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){
    //        spinw->setValue(wo*i/100);
    //    });
    connect(spinhr, SIGNAL(valueChanged(int)), this, SLOT(onSpinhrChanged(int)));
    //    connect(spinhr, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i){
    //        spinh->setValue(ho*i/100);
    //    });
    connect(pushButton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(pushButton_cancel, SIGNAL(clicked()), dialog, SLOT(reject()));
    if (dialog->exec() == QDialog::Accepted) {
        GPI->setPixmap(pixmap.scaled(spinw->value(), spinh->value(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
        QMessageBox MB(QMessageBox::Question, "画布", "是否缩放画布到图像大小？", QMessageBox::Yes | QMessageBox::No, nullptr);
        if (MB.exec() == QMessageBox::Yes) {
            scene->setSceneRect(0, 0, spinw->value(), spinh->value());
        }
    }
    dialog->close();
}

void MainWindow::onSpinwChanged(int i)
{
    disconnect(spinwr, SIGNAL(valueChanged(int)), this, SLOT(onSpinwrChanged(int)));
    spinwr->setValue(i*100/wo);
    connect(spinwr, SIGNAL(valueChanged(int)), this, SLOT(onSpinwrChanged(int)));
}

void MainWindow::onSpinhChanged(int i)
{
    disconnect(spinhr, SIGNAL(valueChanged(int)), this, SLOT(onSpinhrChanged(int)));
    spinhr->setValue(i*100/ho);
    connect(spinhr, SIGNAL(valueChanged(int)), this, SLOT(onSpinhrChanged(int)));
}

void MainWindow::onSpinwrChanged(int i)
{
    disconnect(spinw, SIGNAL(valueChanged(int)), this, SLOT(onSpinwChanged(int)));
    spinw->setValue(wo*i/100);
    connect(spinw, SIGNAL(valueChanged(int)), this, SLOT(onSpinwChanged(int)));
}

void MainWindow::onSpinhrChanged(int i)
{
    disconnect(spinh, SIGNAL(valueChanged(int)), this, SLOT(onSpinhChanged(int)));
    spinh->setValue(ho*i/100);
    connect(spinh, SIGNAL(valueChanged(int)), this, SLOT(onSpinhChanged(int)));
}

void MainWindow::on_action_copy_triggered()
{
    //scene->copy();
    QRectF rect(scene->startPnt, scene->endPnt);
    QPixmap pixmap = ui->graphicsView->grab(rect.toRect());
    QApplication::clipboard()->setPixmap(pixmap, QClipboard::Clipboard);
    LSB1->setText("选区已复制到剪贴板");
}

void MainWindow::on_action_paste_triggered()
{
    QPixmap pixmap = QApplication::clipboard()->pixmap();
    QGraphicsPixmapItem *GPI = scene->addPixmap(pixmap);
    GPI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    LSB1->setText("剪贴板已粘贴");
}

void MainWindow::on_action_layerTop_triggered()
{
    if (scene->selectedItems().size() > 0) {
        QList<QGraphicsItem*> collidingItems = scene->collidingItems(scene->selectedItems().first());
        //QList<QGraphicsItem*> collidingItems1 = scene->collidingItems(collidingItems.first());
        for (int i=0; i<collidingItems.size(); i++) {
            collidingItems.at(i)->stackBefore(scene->selectedItems().first());
        }
        //LSB2->setText("层：" + QString::number(scene->selectedItems().first()->zValue()));
        scene->update();
    }
}

void MainWindow::on_action_layerBottom_triggered()
{
    if (scene->selectedItems().size() > 0) {
        QList<QGraphicsItem*> collidingItems = scene->collidingItems(scene->selectedItems().first());
        //QList<QGraphicsItem*> collidingItems1 = scene->collidingItems(collidingItems.first());
        for (int i=0; i<collidingItems.size(); i++) {
            scene->selectedItems().first()->stackBefore(collidingItems.at(i));
        }
        //LSB2->setText("层：" + QString::number(scene->selectedItems().first()->zValue()));
        scene->update();
    }
}

void MainWindow::on_actionRotateLeft_triggered()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        list_item[i]->setTransformOriginPoint(list_item[i]->boundingRect().center());
        list_item[i]->setRotation(list_item[i]->rotation()-1);
        LSB2->setText("旋转 " + QString::number(list_item[i]->rotation()) + "°");
    }
}

void MainWindow::on_actionRotateRight_triggered()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        list_item[i]->setTransformOriginPoint(list_item[i]->boundingRect().center());
        list_item[i]->setRotation(list_item[i]->rotation() + 1);
        LSB2->setText("旋转 " + QString::number(list_item[i]->rotation()) + "°");
    }
}

void MainWindow::on_actionMirrorHorizontal_triggered()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for(int i=0; i<list_item.size(); i++){
        //list_item[i]->setTransformOriginPoint(list_item[i]->boundingRect().center());
        // https://gerrysweeney.com/horizontal-and-vertical-flip-transformations-of-a-qgraphicsitem-in-qt-qgraphicsview
        QTransform transform(list_item[i]->transform());
        qreal m11 = transform.m11();    // Horizontal scaling 缩放
        qreal m12 = transform.m12();    // Vertical shearing 剪切
        qreal m13 = transform.m13();    // Horizontal Projection 投影
        qreal m21 = transform.m21();    // Horizontal shearing
        qreal m22 = transform.m22();    // vertical scaling
        qreal m23 = transform.m23();    // Vertical Projection
        qreal m31 = transform.m31();    // Horizontal Position (DX)
        qreal m32 = transform.m32();    // Vertical Position (DY)
        qreal m33 = transform.m33();    // Addtional Projection Factor
        qDebug() << m11 << m12 << m13 << m21 << m22 << m23 << m31 << m32 << m33;
        if (m31 > 0)
            m31 = 0;
        else
            m31 = (list_item[i]->boundingRect().width() * m11);
        m11 = -m11;
        qDebug() << m11 << m12 << m13 << m21 << m22 << m23 << m31 << m32 << m33;
        transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);
        list_item[i]->setTransform(transform);
        LSB2->setText("水平镜像");
    }
}

void MainWindow::on_actionMirrorVertical_triggered()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for(int i=0; i<list_item.size(); i++){
        // https://gerrysweeney.com/horizontal-and-vertical-flip-transformations-of-a-qgraphicsitem-in-qt-qgraphicsview
        QTransform transform(list_item[i]->transform());
        qreal m11 = transform.m11();    // Horizontal scaling 缩放
        qreal m12 = transform.m12();    // Vertical shearing 剪切
        qreal m13 = transform.m13();    // Horizontal Projection 投影
        qreal m21 = transform.m21();    // Horizontal shearing
        qreal m22 = transform.m22();    // vertical scaling
        qreal m23 = transform.m23();    // Vertical Projection
        qreal m31 = transform.m31();    // Horizontal Position (DX)
        qreal m32 = transform.m32();    // Vertical Position (DY)
        qreal m33 = transform.m33();    // Addtional Projection Factor
        qDebug() << m11 << m12 << m13 << m21 << m22 << m23 << m31 << m32 << m33;
        if (m32 > 0)
            m32 = 0;
        else
            m32 = (list_item[i]->boundingRect().height() * m22);
        m22 = -m22;
        qDebug() << m11 << m12 << m13 << m21 << m22 << m23 << m31 << m32 << m33;
        transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);
        list_item[i]->setTransform(transform);
        LSB2->setText("垂直镜像");
    }
}

void MainWindow::on_action_help_triggered()
{
    QString s = "<h3 align='center'>快捷键</h3>"
                "<table align='center'>"
                "<tr><td>Alt + ↑</td><td>选区上移</td></tr>"
                "<tr><td>Alt + ↓</td><td>选区下移</td></tr>"
                "<tr><td>Alt + ←</td><td>选区左移</td></tr>"
                "<tr><td>Alt + →</td><td>选区右移</td></tr>"
                "<tr><td>Ctrl + ↑</td><td>选区上边上移</td></tr>"
                "<tr><td>Ctrl + ↓</td><td>选区上边下移</td></tr>"
                "<tr><td>Ctrl + ←</td><td>选区左边左移</td></tr>"
                "<tr><td>Ctrl + →</td><td>选区左边右移</td></tr>"
                "<tr><td>Shift + ↑</td><td>选区下边上移</td></tr>"
                "<tr><td>Shift + ↓</td><td>选区下边下移</td></tr>"
                "<tr><td>Shift + ←</td><td>选区右边左移</td></tr>"
                "<tr><td>Shift + →</td><td>选区右边右移</td></tr>"
                "<tr><td>Ctrl + 鼠标滚轮</td><td>改变线粗或字体大小</td></tr>"
                "<table>";
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("帮助");
    dialog->setFixedSize(400,300);
    QVBoxLayout *vbox = new QVBoxLayout;
    QTextBrowser *textBrowser = new QTextBrowser;
    textBrowser->setHtml(s);
    textBrowser->zoomIn();
    vbox->addWidget(textBrowser);
    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *pushButton_confirm = new QPushButton("确定");
    hbox->addStretch();
    hbox->addWidget(pushButton_confirm);
    hbox->addStretch();
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    dialog->show();
    connect(pushButton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    if (dialog->exec() == QDialog::Accepted) {
        dialog->close();
    }
}

void MainWindow::on_action_gray_triggered()
{
    //imageWidget->gray();
}

void MainWindow::on_action_invert_triggered()
{
    //imageWidget->invert();
}

void MainWindow::on_action_opacity_triggered()
{
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("透明度");
    dialog->setFixedSize(300,100);
    QVBoxLayout *vbox = new QVBoxLayout;
    QSlider *slider = new QSlider(Qt::Horizontal);
    slider->setRange(0, 10);
    slider->setValue(10);
    connect(slider, &QSlider::valueChanged, [&](int v){
        QList<QGraphicsItem*> list_GI = scene->selectedItems();
        for (int i=0; i<list_GI.size(); i++) {
            list_GI[i]->setOpacity(v/10.0);
        }
    });
    vbox->addWidget(slider);
    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *pushButton_confirm = new QPushButton("确定");
    QPushButton *pushButton_cancel = new QPushButton("取消");
    hbox->addStretch();
    hbox->addWidget(pushButton_confirm);
    hbox->addWidget(pushButton_cancel);
    hbox->addStretch();
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    connect(pushButton_confirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    connect(pushButton_cancel, SIGNAL(clicked()), dialog, SLOT(reject()));
    if (dialog->exec() == QDialog::Accepted) {
        dialog->close();
    } else {
        dialog->close();
    }
}

void MainWindow::on_action_blur_triggered()
{
    //imageWidget->blur(spinbox->value());
}

void MainWindow::on_action_mosaic_triggered()
{
    //imageWidget->mosaic(spinbox->value());
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    //if(e->mimeData()->hasFormat("text/uri-list")) //只能打开文本文件
    e->acceptProposedAction(); //可以在这个窗口部件上拖放对象
}

void MainWindow::dropEvent(QDropEvent *e) //释放对方时，执行的操作
{
    QList<QUrl> urls = e->mimeData()->urls();
    if(urls.isEmpty())
        return ;

    QString fileName = urls.first().toLocalFile();

    //    foreach (QUrl u, urls) {
    //        qDebug() << u.toString();
    //    }
    //    qDebug() << urls.size();

    if(fileName.isEmpty())
        return;

    open(fileName);
}

void MainWindow::setPicker(QColor color)
{
//    QPalette plt = btnColorBorder->palette();
//    plt.setColor(QPalette::ButtonText, color);
//    btnColorBorder->setPalette(plt);

//    plt = btnColorFill->palette();
//    plt.setColor(QPalette::ButtonText, color);
//    btnColorFill->setPalette(plt);
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
    if (QApplication::keyboardModifiers() == Qt::ControlModifier) {
        if (e->delta() > 0) {
            QList<QGraphicsItem*> list_GI = scene->selectedItems();
            for (int i=0; i<list_GI.size(); i++) {
                if (list_GI[i]->type() == QGraphicsTextItem::Type) {
                    font = scene->font;
                    font.setPointSize(font.pointSize()+1);
                    QGraphicsTextItem *GTI = qgraphicsitem_cast<QGraphicsTextItem*>(list_GI[i]);
                    GTI->setFont(font);
                    scene->font = font;
                    ui->pushButton_text_font->setText(font.family() + "," + QString::number(font.pointSize()));
                } else {
                    ui->spinBox_pen_width->setValue(ui->spinBox_pen_width->value() + 1);
                }
            }
        } else {
            if (scene->draw_type == scene->TEXT_DRAW) {
                font.setPointSize(font.pointSize()-1);
                scene->font = font;
                ui->pushButton_text_font->setText(font.family() + "," + QString::number(font.pointSize()));
            } else {
                ui->spinBox_pen_width->setValue(ui->spinBox_pen_width->value() - 1);
            }
        }
    } else {
        QWidget::wheelEvent(e);
    }
}

void MainWindow::moveUp()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        list_item[i]->setPos(list_item[i]->pos() + QPoint(0,-1));
    }
}

void MainWindow::moveDown()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        list_item[i]->setPos(list_item[i]->pos() + QPoint(0,1));
    }
}

void MainWindow::moveLeft()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        list_item[i]->setPos(list_item[i]->pos() + QPoint(-1,0));
    }
}

void MainWindow::moveRight()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        list_item[i]->setPos(list_item[i]->pos() + QPoint(1,0));
    }
}

void MainWindow::moveTopUp()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        if (list_item[i]->type() == QGraphicsLineItem::Type) {
            QGraphicsLineItem *GLI = qgraphicsitem_cast<QGraphicsLineItem*>(list_item[i]);
            GLI->setLine(QLineF(GLI->line().p1() + QPoint(0,-1), GLI->line().p2()));
        } else if (list_item[i]->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_item[i]);
            GRI->setRect(GRI->rect() + QMargins(0,1,0,0));
        } else if (list_item[i]->type() == QGraphicsEllipseItem::Type) {
            QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_item[i]);
            GEI->setRect(GEI->rect() + QMargins(0,1,0,0));
        } else if (list_item[i]->type() == QGraphicsPixmapItem::Type) {
            //QGraphicsPixmapItem *GPI = qgraphicsitem_cast<QGraphicsPixmapItem*>(list_item[i]);
            //GPI->setRect(GPI->rect() + QMargins(0,1,0,0));
        }
    }
}

void MainWindow::moveTopDown()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        if (list_item[i]->type() == QGraphicsLineItem::Type) {
            QGraphicsLineItem *GLI = qgraphicsitem_cast<QGraphicsLineItem*>(list_item[i]);
            GLI->setLine(QLineF(GLI->line().p1() + QPoint(0,1), GLI->line().p2()));
        } else if (list_item[i]->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_item[i]);
            GRI->setRect(GRI->rect() + QMargins(0,-1,0,0));
        } else if (list_item[i]->type() == QGraphicsEllipseItem::Type) {
            QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_item[i]);
            GEI->setRect(GEI->rect() + QMargins(0,-1,0,0));
        }
    }
}

void MainWindow::moveBottomUp()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        if (list_item[i]->type() == QGraphicsLineItem::Type) {
            QGraphicsLineItem *GLI = qgraphicsitem_cast<QGraphicsLineItem*>(list_item[i]);
            GLI->setLine(QLineF(GLI->line().p1(), GLI->line().p2() + QPoint(0,-1)));
        } else if(list_item[i]->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_item[i]);
            GRI->setRect(GRI->rect() + QMargins(0,0,0,-1));
        } else if(list_item[i]->type() == QGraphicsEllipseItem::Type) {
            QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_item[i]);
            GEI->setRect(GEI->rect() + QMargins(0,0,0,-1));
        }
    }
}

void MainWindow::moveBottomDown()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        if (list_item[i]->type() == QGraphicsLineItem::Type) {
            QGraphicsLineItem *GLI = qgraphicsitem_cast<QGraphicsLineItem*>(list_item[i]);
            GLI->setLine(QLineF(GLI->line().p1(), GLI->line().p2() + QPoint(0,1)));
        } else if(list_item[i]->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_item[i]);
            GRI->setRect(GRI->rect() + QMargins(0,0,0,1));
        } else if(list_item[i]->type() == QGraphicsEllipseItem::Type) {
            QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_item[i]);
            GEI->setRect(GEI->rect() + QMargins(0,0,0,1));
        }
    }
}

void MainWindow::moveLeftLeft()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        if (list_item[i]->type() == QGraphicsLineItem::Type) {
            QGraphicsLineItem *GLI = qgraphicsitem_cast<QGraphicsLineItem*>(list_item[i]);
            GLI->setLine(QLineF(GLI->line().p1() + QPoint(-1,0), GLI->line().p2()));
        } else if (list_item[i]->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_item[i]);
            GRI->setRect(GRI->rect() + QMargins(1,0,0,0));
        } else if (list_item[i]->type() == QGraphicsEllipseItem::Type) {
            QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_item[i]);
            GEI->setRect(GEI->rect() + QMargins(1,0,0,0));
        }
    }
}

void MainWindow::moveLeftRight()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for(int i=0; i<list_item.size(); i++){
        if(list_item[i]->type() == QGraphicsLineItem::Type){
            QGraphicsLineItem *GLI = qgraphicsitem_cast<QGraphicsLineItem*>(list_item[i]);
            GLI->setLine(QLineF(GLI->line().p1() + QPoint(1,0), GLI->line().p2()));
        }else if(list_item[i]->type() == QGraphicsRectItem::Type){
            QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_item[i]);
            GRI->setRect(GRI->rect() + QMargins(-1,0,0,0));
        }else if(list_item[i]->type() == QGraphicsEllipseItem::Type){
            QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_item[i]);
            GEI->setRect(GEI->rect() + QMargins(-1,0,0,0));
        }
    }
}

void MainWindow::moveRightLeft()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        if (list_item[i]->type() == QGraphicsLineItem::Type) {
            QGraphicsLineItem *GLI = qgraphicsitem_cast<QGraphicsLineItem*>(list_item[i]);
            GLI->setLine(QLineF(GLI->line().p1(), GLI->line().p2() + QPoint(-1,0)));
        } else if (list_item[i]->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_item[i]);
            GRI->setRect(GRI->rect() + QMargins(0,0,-1,0));
        } else if (list_item[i]->type() == QGraphicsEllipseItem::Type) {
            QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_item[i]);
            GEI->setRect(GEI->rect() + QMargins(0,0,-1,0));
        }
    }
}

void MainWindow::moveRightRight()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        if (list_item[i]->type() == QGraphicsLineItem::Type) {
            QGraphicsLineItem *GLI = qgraphicsitem_cast<QGraphicsLineItem*>(list_item[i]);
            GLI->setLine(QLineF(GLI->line().p1(), GLI->line().p2() + QPoint(1,0)));
        } else if (list_item[i]->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_item[i]);
            GRI->setRect(GRI->rect() + QMargins(0,0,1,0));
        } else if (list_item[i]->type() == QGraphicsEllipseItem::Type) {
            QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_item[i]);
            GEI->setRect(GEI->rect() + QMargins(0,0,1,0));
        }
    }
}

void MainWindow::on_action_zoomin_item_triggered()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        list_item[i]->setTransformOriginPoint(list_item[i]->boundingRect().center());
        list_item[i]->setScale(list_item[i]->scale() + 0.1);
        LSB2->setText("缩放：" + QString::number(list_item[i]->scale()));
    }
}

void MainWindow::on_action_zoomout_item_triggered()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        list_item[i]->setTransformOriginPoint(list_item[i]->boundingRect().center());
        list_item[i]->setScale(list_item[i]->scale()-0.1);
        LSB2->setText("缩放：" + QString::number(list_item[i]->scale()));
    }
}

void MainWindow::on_action_zoom1_item_triggered()
{
    QList<QGraphicsItem*> list_item = scene->selectedItems();
    for (int i=0; i<list_item.size(); i++) {
        list_item[i]->setTransformOriginPoint(list_item[i]->boundingRect().center());
        list_item[i]->setScale(1.0);
        LSB2->setText("缩放：" + QString::number(list_item[i]->scale()));
    }
}

void MainWindow::on_actionZoomout_triggered()
{
    ui->graphicsView->scale(0.9, 0.9);
}

void MainWindow::on_actionZoomin_triggered()
{
    ui->graphicsView->scale(1.1, 1.1);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    //qDebug() << event->size();
    //sui->graphicsView->resize(event->size());
}

void MainWindow::listWidgetCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current) {
        Q_UNUSED(previous);
        scene->clearSelection();
        int id = current->data(GRAPHICSITEM_ID).toInt();
        QList<QGraphicsItem*> list_GI = scene->items(Qt::AscendingOrder);
        for (int i=0; i<list_GI.count(); i++) {
            //qDebug() << id << list_GI.at(i)->data(GRAPHICSITEM_ID).toInt();
            if (list_GI.at(i)->data(GRAPHICSITEM_ID).toInt() == id) {
                list_GI.at(i)->setSelected(true);
                if (list_GI.at(i)->type() == QGraphicsTextItem::Type){
                   //ui->property_text->show();
                }
            }
        }
    }
}

void MainWindow::listWidgetAddObject(QString drawtype)
{
    QListWidgetItem *LWI;
    if (drawtype == "Path") {
        LWI = new QListWidgetItem(QIcon(":/images/pencil.png"), drawtype);
    } else if (drawtype == "Image"){
        LWI = new QListWidgetItem(QIcon::fromTheme("image-x-generic"), drawtype);
    } else {
        LWI = new QListWidgetItem(QIcon(":/images/" + drawtype.toLower() + ".png"), drawtype);
    }
    LWI->setData(GRAPHICSITEM_ID, scene->GIID);
    scene->GIID++;
    ui->listWidget->addItem(LWI);
}

void MainWindow::customContextMenu(const QPoint &pos)
{
    QAction *action_layer_top = nullptr, *action_layer_bottom = nullptr;
    QList<QAction*> actions;
    QModelIndex index = ui->listWidget->indexAt(pos);
    if (index.isValid()) {
        action_layer_top = new QAction(this);
        action_layer_top->setText("置顶");
        action_layer_top->setIcon(QIcon::fromTheme("go-top"));
        actions.append(action_layer_top);
        action_layer_bottom = new QAction(this);
        action_layer_bottom->setText("置底");
        action_layer_bottom->setIcon(QIcon::fromTheme("go-bottom"));
        actions.append(action_layer_bottom);
    }
    QAction *result_action = QMenu::exec(actions, ui->listWidget->mapToGlobal(pos));
    if (result_action == action_layer_top) {
        on_action_layerTop_triggered();
        return;
    }
    if (result_action == action_layer_bottom) {
       on_action_layerBottom_triggered();
       return;
    }
}

void MainWindow::on_pushButton_text_color_pressed()
{
    QPalette palette = ui->pushButton_text_color->palette();
    QBrush brush1 = palette.color(QPalette::ButtonText);
    QColor color = QColorDialog::getColor(brush1.color(), this);
    if (color.isValid()) {
        scene->pen.setColor(color);
        palette.setColor(QPalette::ButtonText, color);
        ui->pushButton_text_color->setPalette(palette);
        //修改选中图元文字颜色
        QList<QGraphicsItem*> list_GI = scene->selectedItems();       
        for(int i=0; i<list_GI.size(); i++){
            qDebug() << list_GI[i] << list_GI[i]->type();
            if(list_GI[i]->type() == QGraphicsTextItem::Type){
                QGraphicsTextItem *GTI = qgraphicsitem_cast<QGraphicsTextItem*>(list_GI[i]);
                GTI->setDefaultTextColor(color);
            }
        }
    }
}

void MainWindow::on_pushButton_pen_color_pressed()
{
    QPalette palette = ui->pushButton_pen_color->palette();
    QBrush brush = palette.color(QPalette::ButtonText);
    QColor color = QColorDialog::getColor(brush.color(), this);
    if (color.isValid()) {
        scene->pen.setColor(color);
        palette.setColor(QPalette::ButtonText, color);
        ui->pushButton_pen_color->setPalette(palette);
        //修改选中图元边框颜色
        QList<QGraphicsItem*> list_GI = scene->selectedItems();
        for (int i=0; i<list_GI.size(); i++) {
            qDebug() << list_GI[i] << list_GI[i]->type();
            if (list_GI[i]->type() == QGraphicsPathItem::Type){
                QGraphicsPathItem *GPI = qgraphicsitem_cast<QGraphicsPathItem*>(list_GI[i]);
                GPI->setPen(scene->pen);
            } else if (list_GI[i]->type() == QGraphicsLineItem::Type) {
                QGraphicsLineItem *GLI = qgraphicsitem_cast<QGraphicsLineItem*>(list_GI[i]);
                GLI->setPen(scene->pen);
            } else if (list_GI[i]->type() == QGraphicsRectItem::Type) {
                QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_GI[i]);
                GRI->setPen(scene->pen);
            } else if (list_GI[i]->type() == QGraphicsEllipseItem::Type) {
                QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_GI[i]);
                GEI->setPen(scene->pen);
            }
        }
    }
}

void MainWindow::on_pushButton_brush_color_pressed()
{
    QPalette palette = ui->pushButton_brush_color->palette();
    QBrush brush = palette.color(QPalette::ButtonText);
    QColor color = QColorDialog::getColor(brush.color(), this);
    if (color.isValid()) {
        scene->brush.setColor(color);
        palette.setColor(QPalette::ButtonText, color);
        ui->pushButton_brush_color->setPalette(palette);
        //修改选中图元背景颜色
        QList<QGraphicsItem*> list_GI = scene->selectedItems();
        for (int i=0; i<list_GI.size(); i++) {
            qDebug() << list_GI[i] << list_GI[i]->type();
            if(list_GI[i]->type() == QGraphicsRectItem::Type){
                QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_GI[i]);
                GRI->setBrush(scene->brush);
            } else if(list_GI[i]->type() == QGraphicsEllipseItem::Type){
                QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_GI[i]);
                GEI->setBrush(scene->brush);
            }
        }
    }
}

void MainWindow::on_spinBox_pen_width_valueChanged(int v)
{
    scene->pen.setWidth(v);
    //修改选中图元边框粗细
    QList<QGraphicsItem*> list_GI = scene->selectedItems();
    for (int i=0; i<list_GI.size(); i++) {
        //QMetaEnum metaEnum = QMetaEnum::fromType<QGraphicsItem::anonymous>();//无法转换
        //qDebug() << list_GI[i] << metaEnum.valueToKey(list_GI[i]->type());
        qDebug() << list_GI[i] << list_GI[i]->type();
        if (list_GI[i]->type() == QGraphicsPathItem::Type) {
            QGraphicsPathItem *GPI = qgraphicsitem_cast<QGraphicsPathItem*>(list_GI[i]);
            GPI->setPen(scene->pen);
        } else if (list_GI[i]->type() == QGraphicsLineItem::Type) {
            QGraphicsLineItem *GLI = qgraphicsitem_cast<QGraphicsLineItem*>(list_GI[i]);
            GLI->setPen(scene->pen);
        } else if (list_GI[i]->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_GI[i]);
            GRI->setPen(scene->pen);
        } else if(list_GI[i]->type() == QGraphicsEllipseItem::Type){
            QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_GI[i]);
            GEI->setPen(scene->pen);
        }
    }
}

void MainWindow::on_checkBox_pen_toggled(bool b)
{
    if (b) {
        QPalette palette = ui->pushButton_pen_color->palette();
        QBrush brush = palette.color(QPalette::ButtonText);
        scene->pen.setColor(brush.color());
        scene->pen.setWidth(ui->spinBox_pen_width->value());
    } else {
        scene->pen.setColor(Qt::transparent);
    }
    QList<QGraphicsItem*> list_GI = scene->selectedItems();
    for (int i=0; i<list_GI.size(); i++) {
        //QMetaEnum metaEnum = QMetaEnum::fromType<QGraphicsItem::anonymous>();//无法转换
        //qDebug() << list_GI[i] << metaEnum.valueToKey(list_GI[i]->type());
        qDebug() << list_GI[i] << list_GI[i]->type();
        if (list_GI[i]->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_GI[i]);
            GRI->setPen(scene->pen);
        } else if(list_GI[i]->type() == QGraphicsEllipseItem::Type){
            QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_GI[i]);
            GEI->setPen(scene->pen);
        }
    }
}

void MainWindow::on_checkBox_brush_toggled(bool b)
{
    if (b) {
        QPalette palette = ui->pushButton_brush_color->palette();
        scene->brush.setColor(palette.color(QPalette::ButtonText));
    } else {
        scene->brush.setColor(Qt::transparent);
    }
    QList<QGraphicsItem*> list_GI = scene->selectedItems();
    for (int i=0; i<list_GI.size(); i++) {
        //QMetaEnum metaEnum = QMetaEnum::fromType<QGraphicsItem::type()>();//无法转换
        //qDebug() << list_GI[i] << metaEnum.valueToKey(list_GI[i]->type());        
        qDebug() << list_GI[i] << list_GI[i]->type();
        if (list_GI[i]->type() == QGraphicsRectItem::Type) {
            QGraphicsRectItem *GRI = qgraphicsitem_cast<QGraphicsRectItem*>(list_GI[i]);
            GRI->setBrush(scene->brush);
        } else if(list_GI[i]->type() == QGraphicsEllipseItem::Type){
            QGraphicsEllipseItem *GEI = qgraphicsitem_cast<QGraphicsEllipseItem*>(list_GI[i]);
            GEI->setBrush(scene->brush);
        }
    }
}

void MainWindow::sceneSelectionChanged()
{
    QString s = "";
    QList<QGraphicsItem*> list_GI = scene->selectedItems();
    for (int i=0; i<list_GI.size(); i++) {
        if (list_GI[i]->type() == QGraphicsPathItem::Type){
            s += "Path";
        } else if (list_GI[i]->type() == QGraphicsLineItem::Type) {
            s += "Line";
        } else if (list_GI[i]->type() == QGraphicsRectItem::Type) {
            s += "Rect";
        } else if (list_GI[i]->type() == QGraphicsEllipseItem::Type) {
            s += "Ellipse";
        } else if (list_GI[i]->type() == QGraphicsTextItem::Type) {
            s += "Text";
        } else if (list_GI[i]->type() == QGraphicsPixmapItem::Type) {
            s += "Pixmap";
        } else if (list_GI[i]->type() == QGraphicsItemGroup::Type) {
            s += "Group";
        }
        if (i < list_GI.size() - 1)
            s += " + ";
    }
    ui->label_type->setText(s);
}