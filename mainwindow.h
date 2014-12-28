#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileInfoList>
#include <QDir>

#include "crosshair.h"
#include "rotatedrect.h"

class QPixmap;
class CrossHair;
class QGraphicsPixmapItem;
class Scene;


namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void loadFromDrawingsFile();

private:
    void setImage();
    void updateFileList();
    void writeToDrawingsFile();
    void removeDrawings();
    void scaleToImage();
    void deleteSelectedItems();

public slots:
    void onRotatedRectFinished(RotatedRect* rotRect);

protected:
    virtual void keyPressEvent(QKeyEvent* event);

private slots:
	void on_actionOpen_triggered();
    void on_actionNext_triggered();
    void on_actionPrev_triggered();
    void on_actionExit_triggered();

    void on_actionAdd_Page_triggered();
    void on_actionAdd_Thumb_triggered();

    void on_actionExportCurrentImage_triggered();

    void on_actionExportAllImages_triggered();

public:
    static MainWindow* app;
    Ui::MainWindow *ui;
	Scene* scene; // is in View
    QPixmap pixmap;
    QString fileName; // holds full filename including path

    QGraphicsPixmapItem* pixmapItem;
    CrossHair* crossHair;

    QDir dir;
    QDir tempDir;
    QFileInfoList files;
    int curFileIndex;

private:

    QList<RotatedRect*> rotRects;
    int recentPageNumber; // is for suggestions for the pagename of a new page
    int recentThumbNumber; // is for enumeration

};

#endif // MAINWINDOW_H
