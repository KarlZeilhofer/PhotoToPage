#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QtGui>
#include "scene.h"

/*
 * TODO:
 * add handling for ESC, to stop inputs
 * add delete Drawing Item Action
 * add thumb hiding
 *      class is the same as cutout2, define property in cutout2, which distinguish the type
 * imlement the processing
 */


MainWindow* MainWindow::app = NULL;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    app = this;
    ui->setupUi(this);

    QSettings set;

    recentPageNumber = set.value("recentPageNumber", 0).toInt();
    recentThumbNumber = 0;
    exportingAll=false;
    timer = new QTimer(this);

	scene = ui->graphicsView->getScene();

	//ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);

    fileName = QString("test.jpg");

    pixmapItem = new QGraphicsPixmapItem();


    ui->mainToolBar->addAction(ui->actionOpen);
    ui->mainToolBar->addAction(ui->actionPrev);
    ui->mainToolBar->addAction(ui->actionNext);

    // TODO: save and restore window-position and size
    showMaximized();


    dir.setPath(set.value("dir", QDir::homePath()).toString());

    qDebug() << "dir = " << dir.path();

    updateFileList();
    if(!files.isEmpty()){
            curFileIndex = set.value("curFileIndex", 0).toInt();
    }else{
            curFileIndex = -1;
            qDebug("files-list has NO items");
    }
    setImage();

    connect(ui->graphicsView, SIGNAL(finishedRotatedRect(RotatedRect*)), this, SLOT(onRotatedRectFinished(RotatedRect*)));
    timer->singleShot(350, ui->actionView_FitToScreen, SIGNAL(triggered())); // fit to screen, after window is shown
}

MainWindow::~MainWindow()
{
    QSettings set;
    set.setValue("recentPageNumber", recentPageNumber);
    set.setValue("curFileIndex", curFileIndex);
    set.setValue("dir", dir.absolutePath());
    delete ui;
}


void MainWindow::on_actionOpen_triggered()
{
    fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), dir.path(), tr("Image Files (*.jpg)"));
    QFileInfo info(fileName);
    dir = info.dir();
    updateFileList();
    // search this file in the file list
    for(int n=0; n< files.size(); n++){
        if(files.at(n) == info){
            curFileIndex = n;
            break;
        }
    }

    setImage();
}

void MainWindow::setImage()
{
    if(!files.isEmpty()){
        qDebug("setImage().files-list has items");
            if(curFileIndex >= 0 && curFileIndex < files.size()){
            qDebug("open().opening...");
            fileName=files.at(curFileIndex).absoluteFilePath();
            pixmap = QPixmap(fileName);

            if(ui->actionView_AutofitToScreen->isChecked()){
                scaleToImage();
            }

            pixmapItem->setPixmap(pixmap);
			if(pixmapItem->scene() == NULL){
                scene->addItem(pixmapItem);
			}
            setWindowTitle("PhotoToPage - " + fileName + " - " + QString::number(curFileIndex+1) +" von " + QString::number(files.size()));
            qDebug("done");

            // delete all drawing items
            removeDrawings();

            loadFromDrawingsFile();
        }
    }else{
        qDebug("setImage().files-list has NO items");
    }
}

void MainWindow::scaleToImage()
{
    ui->graphicsView->setVisibleRectangle(QRectF(0,0,pixmap.width(), pixmap.height()));
}

void MainWindow::deleteSelectedItems()
{
    for(int i=0; i<rotRects.size(); i++){
        RotatedRect* rr = rotRects.at(i);
        if(rr->isSelected()){
            rotRects.removeAt(i);
            rr->~QGraphicsItem();
            i--; // repeat this index
        }
    }
    writeToDrawingsFile();
}


void MainWindow::enhanceBlackWhiteText(QImage* image, QString name)
{
    if(!image){
        qDebug()<<"Nullpointer passed to " << __func__;
        return;
    }

#define DoSaveHistogram

    QRgb col;
    int gray;
    int width = image->width();
    int height = image->height();
    double hist[256]={0}; // histogramm
    int histMax = 0;
    int histMaxGrayValue; // 0...255

    for (int i = 0; i < width; ++i)
    {
        for (int j = 0; j < height; ++j)
        {
            col = image->pixel(i, j);
            gray = qGray(col);
            hist[gray]++;

            image->setPixel(i, j, qRgb(gray, gray, gray));
        }
    }

#ifdef DoSaveHistogram
    QPixmap histIm(256,256);
    QPainter histPainter(&histIm);
    // clear image:
    histPainter.fillRect(QRectF(0,0,265,256), Qt::white);
#endif

    // find maximum in histogram:
    for(int i=0;i<256;i++){
        if(hist[i]>histMax){
            histMax = hist[i];
            histMaxGrayValue = i;
        }
    }

    // scale histogram to 0..1:
    for(int i=0;i<256;i++){
        hist[i] /= hist[histMaxGrayValue];
    }


    // enhance image, by spreading the histogram:
    // TODO: very good method is:
    int blackBorder = histMaxGrayValue-110;
    int whiteBorder = histMaxGrayValue-30;


#ifdef DoSaveHistogram
    histPainter.setPen(Qt::darkGreen);
    // draw histogram-bars:
    for(int i=0;i<256;i++){
        histPainter.drawLine(i, 255, i, 255 - (int)(10*256*hist[i])); // ATTENTION, only lower 10% are shown.
    }
    // draw borders:
    histPainter.setPen(Qt::red);
    histPainter.drawLine(blackBorder, 0, blackBorder, 255);
    histPainter.drawLine(whiteBorder, 0, whiteBorder, 255);

    // save histogram image
    QFileInfo fi = files.at(curFileIndex);
    static int suffix=0;
    char numStr[10]={0};
    if(name.isEmpty())
        sprintf(numStr, "%03d", suffix++);
    fi.setFile(fi.absolutePath() + "/pages/"+name+"_hist"+QString(numStr)+".png");
    qDebug() << fi.absoluteFilePath();
    histIm.save(fi.absoluteFilePath(), "png");
#endif




//    // white-border: from black to white: value before the large peak comes.
//    //   we make use of small noise in the histogramm, which is dominated by the big slope of the peak
//    //  start at the peak of the histogramm,
//    //   go towards black, Steps of 1% downwards must occure. When we find
//    //   a value, that is higher, than it's whiter neighbour, set this as white-border
//    // black-border: start at black, and find the first value that is larger than 1.5%.

//    // Find black border:
//    int blackBorder = 0;
//    for(blackBorder=0; blackBorder<histMaxGrayValue; blackBorder++){
//        if(hist[blackBorder]>0.015)
//            break;
//    }
//    // blackBorder is now in the range [0,peak[

//    // Find white border:
//    int whiteBorder;
//    bool steepDeclineFound = false;
//    for(whiteBorder=histMaxGrayValue; blackBorder+1 < whiteBorder; whiteBorder--){
//        if(!steepDeclineFound && hist[whiteBorder+1]-hist[whiteBorder] > 0.01)
//            steepDeclineFound = true;
//        if(steepDeclineFound && hist[whiteBorder] > hist[whiteBorder+1]){
//            whiteBorder++;
//            break;
//        }
//    }
//    // whiteBorder is now in the range of ]blackBorder, peak-2]



    qDebug() << "black/white border = (" << blackBorder << "/" << whiteBorder << ")";
    if(blackBorder < whiteBorder){ // plausability check
        // transform the gray values, where x are the old values
        // and y are the new values
        // y(x) = k*(x-x0) + y0
        // y(x) = 255/(xW-xB)*(x-xB) + 0

        double x,y;
        //double xB=blackBorder;
        double xB=blackBorder;
        double xW=whiteBorder;

        // transform each pixel!
        for (int i = 0; i < width; ++i)
        {
            for (int j = 0; j < height; ++j)
            {
                x = image->pixel(i, j) & 0xff; // get gray-value (mask blue channel)


                switch(1){
                case 1:// linear scaling:
                    y = 255.0/(xW-xB)*(x-xB) + 0;
                    break;
                case 2: // move non-white pixel towards black
                    // (führt zu verpixelterm Schriftbild, weil die Graustufen zu Weiß hin fehlen.)
                    if(x>=xW){
                        y = 255;
                    }else{
                        y = x-xB;
                    }
                    break;
                }




                gray = round(y);
                gray = fmax(gray,0);
                gray = fmin(gray, 255);

                image->setPixel(i, j, qRgb(gray, gray, gray));
            }
        }

        // TODO: blacken thin, light-gray lines
        // what algorithm?
    }
}

void MainWindow::exportOfImageFinished()
{
    if(ui->actionAuto_Advance->isChecked() && !exportingAll){
        ui->actionNext->trigger();
    }

    if(exportingAll){
        ui->actionNext->trigger();
        if(curFileIndex == exportingAllStartingFileIndex) { // turned one round
            exportingAll = false;
        }else{
            ui->actionExportCurrentImage->trigger();
        }
    }
}

void MainWindow::on_actionNext_triggered()
{
    if(!(files.size()==0 || files.size()==1)){
        if(curFileIndex >= files.size()-1){
                curFileIndex = 0;
        }else{
                curFileIndex++;
        }
        setImage();

    }
}

void MainWindow::updateFileList(){
    QStringList filter("*.jpg"); // TODO: extend file types list
    files = dir.entryInfoList(filter, QDir::Files, QDir::Name);
    if(files.isEmpty()){
        qDebug("No Files found in this directory");
    }
    // correctArrowButtons(); // TODO
}

void MainWindow::writeToDrawingsFile()
{
    QString drawingFN = fileName + ".ptp"; // extension stands for photo to page
    QSettings set(drawingFN, QSettings::IniFormat, this);
    qDebug() << __func__ << " with " << drawingFN;


    set.remove(""); // clear all entries in ini-file
    set.beginWriteArray("RotatedRectangles", rotRects.size());

    for(int i=0; i<rotRects.size(); i++){
        RotatedRect* rr = rotRects.at(i);

        set.setArrayIndex(i);
        set.setValue("name", rr->name);
        set.setValue("type", (int)(rr->type));

        // write a cutout2, by writing an array of QPointF entries, of the user input points in scene-coordinates.
        set.beginWriteArray(rr->name, 4); // prefix is the object-name
        bool ok;
        QList<QPointF> scenePoints = rr->allInputPoints(&ok);
        if(ok){
            for(int ii=0; ii<scenePoints.size(); ii++){
                set.setArrayIndex(ii);
                QPoint p(scenePoints.at(ii).x(),scenePoints.at(ii).y());
                set.setValue("scenePoint", p );
                qDebug()<< "wrote point["<<ii<<"] = "<<p;
            }
        }
        set.endArray();
    }
    set.endArray();
    set.sync();
}


// version with nested groups,
// nested arrays didn't work! see http://www.qtforum.de/forum/viewtopic.php?f=1&t=16671
void MainWindow::loadFromDrawingsFile()
{
    bool ok;
    QString drawingFN = fileName + ".ptp"; // extension stands for photo to page
    QSettings set(drawingFN, QSettings::IniFormat, this);
    qDebug() << __func__ << " with " << drawingFN;



    set.beginGroup("RotatedRectangles");
    int size = set.value("size").toInt(&ok);
    if(!ok) qDebug() << "Error on reding Int in "<< __FILE__<<":" << __LINE__;
    qDebug()<<"size = " << size;


    for(int i=0; i<size; i++){
        set.beginGroup(QString::number(i+1));

        RotatedRect* rr;
        QString name = set.value("name").toString();
        qDebug()<<"name = " << name;
        rr = new RotatedRect(0, ui->graphicsView->getScene());
        rr->name = name;
        rr->type = (RotatedRect::Type)(set.value("type").toInt(&ok));
        if(!ok) qDebug() << "Error on reding Int in "<< __FILE__<<":" << __LINE__;

        set.beginGroup(rr->name);

        int sizeP = set.value("size").toInt(&ok);
        if(!ok) qDebug() << "Error on reding Int in "<< __FILE__<<":" << __LINE__;
        qDebug()<<"sizeP = " << sizeP;
        for(int ii=0; ii<sizeP; ii++){
            set.beginGroup(QString::number(ii+1));

            QPointF p = set.value("scenePoint").toPoint();
            qDebug()<< "read point["<<ii<<"] = "<<p;
            rr->addPoint(p);
            set.endGroup();
        }
        set.endGroup();
        rotRects.append(rr);
        qDebug() << "loaded " << rr;

        set.endGroup();
    }
    set.endGroup();
}

// ask user to enter page-nuber of the cutout, and
// add this cutout to the list of cutouts!
// save it in a separate file

void MainWindow::onRotatedRectFinished(RotatedRect *rotRect)
{
    if(rotRect->type == RotatedRect::Page){
        bool ok;
        char hstr[10];
        sprintf(hstr, "%03d", ++recentPageNumber);

        rotRect->name = QInputDialog::getText(this, "New Page", "Enter page name:",
                                               QLineEdit::Normal, QString(hstr), &ok, 0, 0);
        if(!ok){
            rotRect->name = "unnamedPage"+QString::number(recentPageNumber);
        }

        // update recentPageNumber
        int n = rotRect->name.toInt(&ok);
        if(ok)
            recentPageNumber = n;
    }else if(rotRect->type == RotatedRect::Thumb){
        rotRect->name = QString("Nr")+QString::number(++recentThumbNumber);
    }



    rotRects.append(rotRect);
    qDebug() << "added" << rotRects.last();

    writeToDrawingsFile();

    //restart action:
    if(rotRect->type == RotatedRect::Page){
        ui->actionAdd_Page->trigger();
    }else if(rotRect->type == RotatedRect::Thumb){
        ui->actionAdd_Thumb->trigger();
    }
}

void MainWindow::on_actionPrev_triggered()
{
    if(!(files.size()==0 || files.size()==1)){
        if(curFileIndex == 0){
                curFileIndex = files.size()-1;
        }else{
                curFileIndex--;
        }
        setImage();
    }
}

void MainWindow::on_actionExit_triggered()
{
    qApp->exit(0);
}


void MainWindow::on_actionAdd_Page_triggered()
{
    ui->graphicsView->newRotatedRect(RotatedRect::Page);
}



/*
 * for character names refer to:
 * http://www.user-archiv.de/zeichen-tabelle.html
 */
void MainWindow::keyPressEvent(QKeyEvent* event){

    // When no CTRL and ALT pressed:
    if(!(event->modifiers() & Qt::ControlModifier) &&
            !(event->modifiers() & Qt::AltModifier)){
        switch(event->key()){
        case Qt::Key_F:
            ui->actionAdd_Page->trigger();
            break;
        case Qt::Key_T:
            ui->actionAdd_Thumb->trigger();
            break;
        case Qt::Key_Escape:
            ui->graphicsView->stopCurrentTool();
            break;
        case Qt::Key_P:
            ui->actionPrev->trigger();
            break;
        case Qt::Key_Space:
        case Qt::Key_N:
            ui->actionNext->trigger();
            break;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            ui->actionExportCurrentImage->trigger();
            break;

        case Qt::Key_Delete:
            deleteSelectedItems();
            break;

        default:
            QWidget::keyPressEvent(event);
            break;
        }// switch
    }else if((event->modifiers() & Qt::ControlModifier))
    {
        switch(event->key()){
        case Qt::Key_Enter:
        case Qt::Key_Return:
            ui->actionExportAllImages->trigger();
            break;
        default:
            QWidget::keyPressEvent(event);
            break;
        }
    }else{
        QWidget::keyPressEvent(event);
    }

}

void MainWindow::removeDrawings()
{
    qDebug() << __func__;
    for(int i=0; i<rotRects.size(); i++){
        qDebug() << "delete" << rotRects.at(i);
        rotRects.at(i)->~QGraphicsItem();
    }
    rotRects.clear();
}

void MainWindow::on_actionAdd_Thumb_triggered()
{
    ui->graphicsView->newRotatedRect(RotatedRect::Thumb);
}


void MainWindow::on_actionExportCurrentImage_triggered()
{
    QDir pageDir(dir);
    pageDir.mkdir("pages");
    pageDir.cd("pages");

    QGraphicsScene renderScene(this);

    // render thubs into the pixmap
    QPixmap thumbedPixmap(pixmap);
    { // block for visibility
        QPainter painter(&thumbedPixmap);
        for(int i=0; i<rotRects.size(); i++){
            RotatedRect* rr = rotRects.at(i);
            if(rr->type == RotatedRect::Thumb){
                QPolygonF poly = rr->getPolygonInScene();
                painter.setBrush(Qt::white);
                painter.setPen(Qt::white);
                painter.drawPolygon(poly);
            }
        }
    }


    for(int i=0; i<rotRects.size(); i++){
        RotatedRect* rr = rotRects.at(i);
        if(rr->type == RotatedRect::Page){
            renderScene.clear();
            QGraphicsPixmapItem pmi(thumbedPixmap);
            renderScene.addItem(&pmi);

            // ACHTUNG: seltsamerweiser gibt rotation() nicht das gleiche zurück
            // wie degrees (selbst implementiert)
            qDebug()<< "rr("<<rr->name<<")->degrees =" << rr->degrees;
            QPointF o = rr->originInScene();

            QTransform t = QTransform().translate(o.x(), o.y()).rotate(-rr->degrees).
                    translate(-o.x(), -o.y());
            pmi.setTransform(t); // move and rotate item

            QRectF r = rr->getRectInItem();
            r.translate(o.x(), o.y());

            QImage* im = new QImage(r.width(), r.height(), QImage::Format_RGB32);
            QPainter painter(im);
            renderScene.render(&painter, QRectF(), r);

            im->save(pageDir.absolutePath()+"/"+rr->name+"_1color.jpg", "jpg", 100);
            enhanceBlackWhiteText(im, rr->name);
            im->save(pageDir.absolutePath()+"/"+rr->name+"_2enhanced.jpg", "jpg", 100);
        }
    }


    exportOfImageFinished();
}

void MainWindow::on_actionExportAllImages_triggered()
{
    exportingAll = true;
    exportingAllStartingFileIndex = curFileIndex;
    ui->actionExportCurrentImage->trigger();
}

void MainWindow::on_actionView_AutofitToScreen(bool checked)
{
    if(checked){
        on_actionView_FitToScreen_triggered();
    }
}

void MainWindow::on_actionView_FitToScreen_triggered()
{
    scaleToImage();
}

void MainWindow::on_actionAbout_triggered()
{
    QMessageBox::about(this, "PhotoToPage", QString(
                           "Based on Qt 4.8.3\n"
                           "Developed by Mechatronik Karl Zeilhofer\n"
                           "http://www.zeilhofer.co.at \n"
                           "Source Code on Github \n"
                           "https://github.com/KarlZeilhofer/PhotoToPage \n"
                           "License: GPL v3"));
}
