#ifndef MATERIALMANAGER_H
#define MATERIALMANAGER_H

#include <QApplication>

#include <QCoreApplication>
#include <QCommandLineParser>

#include <iostream>
#include <QImage>
#include <Eigen/Dense>

using namespace Eigen;

enum MaterialChangeType{
    BRDF,
    RETEXTURE,
    GLASS,
    CAUSTIC
};

struct MaterialParams{
    QString mainImageFile = "";
    QString maskFile = "";
    QString textureFile = "";
    QString backgroundFile = "";

    // BRDF REPLACEMENT
    Vector3f diffuse = Vector3f(-1,-1,-1);
    Vector3f specular = Vector3f(0,0,0);

    // SHAPE ESTIMATION
    float curvature = -1.0f;
    float bilateralSmoothing = -1.0f;

    // GLASS
    float s = 50.0f;
    int frosty = 5;
    Vector3f glassColor = Vector3f(1,1,1);
    float darkness = 1.8;

    MaterialChangeType makeMaterial = BRDF;
};

class MaterialManager
{
public:
    MaterialManager();
    MaterialParams materialParams;

    bool changeBrdf();
    bool areBasicParamsValid();
    void vectorToFile(std::vector<Vector3f> data, QString filenameOut, int rows, int cols);
    bool areBrdfParamsValid();
    bool areShapeEstimationParamsValid();
    bool areRetextureParamsValid();
    bool areGlassParametersValid();
    bool retexture();
    bool makeGlass();
    bool makeCaustic();
    bool transformMaterial();
};

#endif // MATERIALMANAGER_H
