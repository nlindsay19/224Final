#include "mainwindow.h"
#include <QApplication>

#include <QCoreApplication>
#include <QCommandLineParser>

#include <iostream>
#include <QImage>
#include "imagereader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QString fileIn = "full path to input file";
    ImageReader im(fileIn);
    QString fileOut = "full path to output file";

    QImage imOut(im.getImageWidth(), im.getImageHeight(), QImage::Format_RGB32);
    QRgb *data = reinterpret_cast<QRgb *>(imOut.bits());
    int counter = 0;
    for(int i = 0; i < im.getImageHeight(); i ++){
        for(int j = 0; j < im.getImageWidth(); j++){
            QRgb color = im.pixelAt(i, j);
            data[counter] = color;
            counter += 1;
        }
    }
    imOut.save(fileOut);

    return a.exec();
}
