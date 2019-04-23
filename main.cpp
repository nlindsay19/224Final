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
#include "histogram.h"
#include "Eigen/Dense"

using namespace Eigen;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QString imageFile = "images/han.jpg";
    ImageReader im(imageFile);
    std::cout << "read image 1" << std::endl;
    QString maskFile = "images/han_mask.jpg";
    ImageReader mask(maskFile);
    std::cout << "read mask" << std::endl;
    QString backgroundFile = "images/background.jpg";
    ImageReader background(backgroundFile);
    std::cout << "read background" << std::endl;

    QString marbleFile = "images/marble.jpg";
    ImageReader marble(marbleFile);


    std::vector<float> depth;
    std::vector<Eigen::Vector3f> normals;
    ShapeEstimator se;
    std::vector<float> gradientX;
    std::vector<float> gradientY;
    se.estimateShape(im,mask, depth, normals, gradientX, gradientY);
    std::cout << "finished estimating" << std::endl;
    incidentlight incidentObj;

    std::vector<Vector3f> inpainting = incidentObj.inPaint(mask, background.toVector());

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
    std::vector<Vector3f> blurred = retextureObj.applyGaussianFilter(inpainting, cols, rows, 0);
    QImage outputBlur(cols, rows, QImage::Format_RGB32);
    QRgb *inpaintedBlur = reinterpret_cast<QRgb *>(outputBlur.bits());
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            int index = im.indexAt(i, j);
            Vector3f color = blurred[index];
            float colorR = color(0);
            float colorG = color(1);
            float colorB = color(2);
            QColor colorOut = QColor(int(colorR), int(colorG), int(colorB));
            inpaintedBlur[im.indexAt(i, j)] = colorOut.rgb();
        }
    }
    outputBlur.save("images/inpaintBlur.png");
    std::cout<< "Did blur" << std::endl;

    std::vector<Vector3f> retexturing;
    retextureObj.calculate(marble.toVector(), inpainting, im.toVector(), gradientX, gradientY, retexturing, mask );

    Histogram hist(se.getLuminances());
    std::vector<int> highlights = hist.findHighlights();

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

    std::vector<Vector3f> originalImage = im.toVector();
    std::cout << "Highlights size: " << highlights.size() << std::endl;
    for (int i = 0; i < highlights.size(); i++) {
        int index = highlights[i];
        Vector3f originalVal = originalImage[index];
        int gray = (int(originalVal[0]) + int(originalVal[1]) + int(originalVal[2]))/3;
        QColor color = QColor(gray, gray, gray);
        retextured[index] = color.rgb();
    }
    outputF.save("images/retextured.png");


    std::cout << "done" << std::endl;
    return a.exec();
}
