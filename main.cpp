#include "mainwindow.h"
#include <QApplication>

#include <QCoreApplication>
#include <QCommandLineParser>

#include <iostream>
#include <QImage>
#include "imagereader.h"
#include "bilateralfilter.h"
#include "shapeestimation.h"
#include "incidentlight.h"
#include "retexture.h"
#include "histogram.h"
#include "Eigen/Dense"
#include "causticmaker.h"
#include "brdfreplacement.h"
#include "materialmanager.h"

using namespace Eigen;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    MaterialManager mm;
    mm.materialParams.backgroundFile = "images/background.jpg";
    mm.materialParams.mainImageFile = "images/han.jpg";
    mm.materialParams.bilateralSmoothing = 0.004f;
    mm.materialParams.curvature = 1.0f;
    mm.materialParams.maskFile = "images/han_mask.jpg";
    mm.materialParams.textureFile = "images/marble.jpg";

    mm.materialParams.diffuse = Vector3f(0.5, 0.5, 0.5);
    mm.materialParams.specular = Vector3f(0.5,0.5,0.5);


    mm.materialParams.s = 50;
    mm.materialParams.frosty = 5;
    mm.materialParams.glassColor = Vector3f(1.0,1.0,1.0);
    mm.materialParams.darkness = 1.8f;

    mm.materialParams.makeMaterial = GLASS;
    mm.transformMaterial();

    std::cout << "done" << std::endl;
    return a.exec();
}
