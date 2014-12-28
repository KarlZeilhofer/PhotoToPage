#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QTextCodec>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));		// all messages in tr() are interpreted in UTF-8 so the file encoding for all source-files should be UTF-8 too.
    QCoreApplication::setOrganizationName("zeilhofer.co.at");
    QCoreApplication::setApplicationName("PhotoToPage");
    MainWindow w;
    w.show();

    return a.exec();
}
