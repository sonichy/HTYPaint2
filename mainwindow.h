#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define GRAPHICSITEM_ID Qt::UserRole

#include "imagewidget.h"
#include "graphicsscene.h"
#include <QMainWindow>
#include <QScrollArea>
#include <QToolButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QListWidgetItem>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
    QToolButton *btnColorFill, *btnColorBorder;
    QLineEdit *lineEdit;
    QSpinBox *spinbox;
    QString text, path;
    QColor colorb, colorf, color_background;
    QCheckBox *checkBox_color_fill, *checkBox_color_border;
    QFont font;
    QString filename;

protected:
    void dragEnterEvent(QDragEnterEvent*);
    void dropEvent(QDropEvent*);
    void wheelEvent(QWheelEvent *e);
    void resizeEvent(QResizeEvent *event);

private:
    QScrollArea *scrollArea;
    ImageWidget *imageWidget;
    QLabel *LSB1,*LSB2;
    GraphicsScene *scene;
    void save(QString filepath);
    QSpinBox *spinw, *spinwr, *spinh, *spinhr;
    int wo, ho;

private slots:
    void on_action_new_triggered();
    void on_action_open_triggered();
    void on_action_save_triggered();
    void on_action_saveas_triggered();
    void on_action_import_triggered();
    void on_action_changelog_triggered();
    void on_action_aboutQt_triggered();
    void on_action_about_triggered();
    void on_action_undo_triggered();
    void on_action_redo_triggered();
    void on_action_copy_triggered();
    void on_action_paste_triggered();
    void on_action_selectAll_triggered();
    void on_action_delete_triggered();
    void on_action_layerTop_triggered();
    void on_action_layerBottom_triggered();
    void on_action_setWallpaper_triggered();
    void on_action_property_triggered();
    void on_action_help_triggered();

    void on_action_extrude_triggered();
    void on_action_gray_triggered();
    void on_action_invert_triggered();
    void on_action_blur_triggered();
    void on_action_mosaic_triggered();
    void on_action_opacity_triggered();
    void on_action_zoomin_item_triggered();
    void on_action_zoomout_item_triggered();
    void on_action_zoom1_item_triggered();

    void on_actionPencil_triggered();
    void on_actionLine_triggered();
    void on_actionArrow_triggered();
    void on_actionRect_triggered();
    void on_actionEllipse_triggered();
    void on_actionText_triggered();
    void on_actionFill_triggered();
    void on_actionErase_triggered();
    void on_actionMove_triggered();
    void on_actionRectSelect_triggered();
    void on_actionColorPicker_triggered();

    void on_actionRotateLeft_triggered();
    void on_actionRotateRight_triggered();
    void on_actionMirrorHorizontal_triggered();
    void on_actionMirrorVertical_triggered();
    void on_actionFont_triggered();
    void on_actionZoomin_triggered();
    void on_actionZoomout_triggered();
    //void on_actionZoom1_triggered();
    void on_action_createGroup_triggered();
    void on_action_destroyGroup_triggered();

    void setColorFill();
    void setColorBorder();
    void spinValueChange(int);
    void textChange(QString);
    void onSpinwChanged(int);
    void onSpinhChanged(int);
    void onSpinwrChanged(int);
    void onSpinhrChanged(int);
    void checkBox_color_border_Changed(int);
    void checkBox_color_fill_Changed(int);
    void open(QString);
    void setPicker(QColor color);
    void addPenWidth();
    void reducePenWidth();

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void moveTopUp();
    void moveTopDown();
    void moveBottomUp();
    void moveBottomDown();
    void moveLeftLeft();
    void moveLeftRight();
    void moveRightLeft();
    void moveRightRight();

    void listWidgetAddObject(QString drawtype);
    void listWidgetCurrentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

};

#endif // MAINWINDOW_H