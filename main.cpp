#include "mainwindow.h"
#include <QApplication>

#include <QCoreApplication>
#include <QCommandLineParser>

#include <iostream>
#include <QImage>
#include "imagereader.h"
#include "bilateralfilter.h"
#include "shapeestimator.h"
#include "incidentlight.h"
#include "retexture.h"
#include "Eigen/Dense"

using namespace Eigen;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QString imageFile = "images/bottle.png";
    ImageReader im(imageFile);
    std::cout << "read image 1" << std::endl;
    QString maskFile = "images/mask.png";
    ImageReader mask(maskFile);
    std::cout << "read mask" << std::endl;

    QString fileOut = "images/shapeout.png";

    std::vector<float> depth;
    std::vector<Eigen::Vector3f> normals;
    ShapeEstimator se;
    std::vector<float> gradientX;
    std::vector<float> gradientY;
    se.estimateShape(im,mask, depth, normals, gradientX, gradientY);
 std::cout << "finished estimating" << std::endl;
    incidentlight incidentObj;

    std::vector<Vector3f> inpainting = incidentObj.inPaint(mask, im.toVector());

    std::cout << "inpainting" << std::endl;
    int cols = im.getImageWidth();
    int rows = im.getImageHeight();
    QImage output(cols, rows, QImage::Format_RGB32);
    QRgb *inpainted = reinterpret_cast<QRgb *>(output.bits());
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            int index = im.indexAt(i, j);
            Vector3f color = inpainting[index];
            float colorR = color(0);
            float colorG = color(1);
            float colorB = color(2);
            QColor colorOut = QColor(int(colorR), int(colorG), int(colorB));
            inpainted[im.indexAt(i, j)] = colorOut.rgb();
        }
    }
    output.save("images/inpaint.png");

    Retexture retextureObj;
    std::vector<Vector3f> retexturing;
    retextureObj.calculate(inpainting, im.toVector(), gradientX, gradientY, retexturing, mask );

    QImage outputF(cols, rows, QImage::Format_RGB32);
    QRgb *retextured = reinterpret_cast<QRgb *>(outputF.bits());
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            int index = im.indexAt(i, j);
            Vector3f color = retexturing[index];
            float colorR = color(0);
            float colorG = color(1);
            float colorB = color(2);
            QColor colorOut = QColor(int(colorR), int(colorG), int(colorB));
            retextured[im.indexAt(i, j)] = colorOut.rgb();
        }
    }
    outputF.save("images/retextured.png");


    std::cout << "done" << std::endl;
    return a.exec();
}
