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
    QImage imOut = se.estimateShape(im,mask, depth, normals, gradientX, gradientY);
    incidentlight incidentObj;
    std::vector<Vector3f> inpainting = incidentObj.inPaint(mask, im);
    Retexture retextureObj;
    //std::vector<Vector3f> retexturing = retextureObj.calculate(inpainting, im, gradientX, gradientY, );
    imOut.save(fileOut);
    std::cout << "done" << std::endl;
    return a.exec();
}
