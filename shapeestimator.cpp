#include "shapeestimator.h"
#include "math.h"
#include "assert.h"

#include "Eigen/Dense"

#define DEBUG 1
#define DEPTHMAPBACKGROUND 0

using namespace Eigen;

ShapeEstimator::ShapeEstimator()
{
    m_bilateralSmoothing = 0.008;
}

void ShapeEstimator::estimateShape(ImageReader imageIn, ImageReader mask, std::vector<float>& depthMap, std::vector<Vector3f>& normalMap, std::vector<float> &gradientX, std::vector<float> &gradientY){
    BilateralFilter bf;
    int rows = imageIn.getImageHeight();
    int cols = imageIn.getImageWidth();

    float sigma = 0.0f;

    std::vector<float> luminances = computePixelLuminance(imageIn, mask, sigma);
    m_luminances = luminances;

    sigmoidalCompression(luminances, sigma);
    std::cout << rows << " " << cols << std::endl;
    float bilateralSigmaSpatial = m_bilateralSmoothing * float(cols);
    float bilateralSigmaL = 255.0f;
    std::cout << "convolve" << std::endl;
    luminances = bf.convolve(imageIn, luminances, bilateralSigmaSpatial, bilateralSigmaL);
    std::cout << "inversion" << std::endl;
    sigmoidalInversion(luminances, sigma);
    //cropMask(mask, luminances);

    std::vector<Vector3f> normals = gradientField(mask, luminances, gradientX, gradientY);

    if(DEBUG){
        QImage output(cols, rows, QImage::Format_RGB32);
        QRgb *depthMap = reinterpret_cast<QRgb *>(output.bits());
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                int index = imageIn.indexAt(i, j);
                float color = luminances[index] * 255.0f;
                QColor colorOut = QColor(floor(color), floor(color), floor(color));
                depthMap[imageIn.indexAt(i, j)] = colorOut.rgb();
            }
        }
        output.save("images/depthMap.png");
    }

    // write out normal map
    if(DEBUG){
        float maxRed = 0.0f;
        float maxGreen = 0.0f;
        float minRed = 10000.0f;
        float minGreen = 10000.0f;
        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                int index = imageIn.indexAt(i, j);
                float red = normals[index](0);
                if(red > maxRed){
                    maxRed = red;
                }
                if(red < minRed){
                    minRed = red;
                }
                float green = normals[index](1) ;
                if(green > maxGreen){
                    maxGreen = green;
                }
                if(green < minGreen){
                    minGreen = green;
                }
            }
        }
        QImage output(cols, rows, QImage::Format_RGB32);
        QRgb *normalMap = reinterpret_cast<QRgb *>(output.bits());

        for(int i = 0; i < rows; i++){
            for(int j = 0; j < cols; j++){
                int index = mask.indexAt(i, j);
                QColor colorOut = QColor(0,0,0);
                if(QColor(mask.pixelAt(i,j)).red() > 150){
                    float red = (255) * (normals[index](0)  - minRed)/(maxRed - minRed);
                    float green = (255) * (normals[index](1)  - minGreen)/(maxGreen - minGreen);
                    float blue = normals[index](2) * 255.0f;

                    colorOut = QColor(fabs(floor(red)), fabs(floor(green)), floor(blue));
                }
                normalMap[mask.indexAt(i, j)] = colorOut.rgb();
            }
        }
        output.save("images/normalmap.png");
    }
    depthMap = luminances;
    normalMap = normals;
}

std::vector<float> ShapeEstimator::computePixelLuminance(ImageReader imageIn, ImageReader mask, float &sigma){
    int rows = imageIn.getImageHeight();
    int cols = imageIn.getImageWidth();
    std::vector<float> pixelLuminances;

    int pixelsInObject = 0;
    float objectLuminanceSum = 0.0f;
    for(int row = 0; row < rows; row++){
        for(int col = 0; col < cols; col++){
            QColor imageColor = QColor(imageIn.pixelAt(row, col));
            QColor maskColor = QColor(mask.pixelAt(row, col));
            if((maskColor.red() > 150)){
                pixelsInObject += 1;
                float luminance =  0.213f * float(imageColor.red()) + 0.715f * float(imageColor.green()) + 0.072f * float(imageColor.blue());
                if(luminance == 0.0f){
                    luminance = 0.0001f;
                }
                pixelLuminances.push_back(luminance/ 255.0f);
                objectLuminanceSum += log(luminance / 255.0f);
            } else {
                if(DEPTHMAPBACKGROUND){
                     pixelLuminances.push_back(1.0f);
                } else {
                     pixelLuminances.push_back(0.0f);
                }
            }
        }
    }
    sigma = exp(objectLuminanceSum / float(pixelsInObject));
    return pixelLuminances;
}

void ShapeEstimator::sigmoidalCompression(std::vector<float> &pixelLuminances, float sigma){
    float n = 0.73;
    for(int i = 0; i < pixelLuminances.size(); i++){
        pixelLuminances[i] = pow(pixelLuminances[i], n)/(pow(pixelLuminances[i], n) + pow(sigma, n));
    }
}

void ShapeEstimator::sigmoidalInversion(std::vector<float> &pixelLuminances, float sigma){
    float n = 0.73;
    for(int i = 0; i < pixelLuminances.size(); i++){
        pixelLuminances[i] = pow(-1.0f * pow(sigma, n) * pixelLuminances[i] / (pixelLuminances[i] - 1.0f), 1.0f/n);
    }
}

std::vector<Vector3f> ShapeEstimator::gradientField(ImageReader mask, std::vector<float> &pixelLuminances, std::vector<float> &gradientX, std::vector<float> &gradientY){
    int rows = mask.getImageHeight();
    int cols = mask.getImageWidth();

    float gxNormalize = 0.0f;
    float gyNormalize = 0.0f;

    for(int row = 0; row < rows; row++){
        for(int col = 0; col < cols; col++){
           int index = mask.indexAt(row, col);
           if(row + 1 < rows){
               int indexUp = mask.indexAt(row + 1, col);
               float dY = pixelLuminances[indexUp] - pixelLuminances[index];
               gradientY.push_back(dY);
               if(fabs(dY) > gyNormalize){
                   gyNormalize = fabs(dY);
               }

           } else {
               gradientY.push_back(0.0f);
           }

           if(col + 1 < cols){
               int indexRight = mask.indexAt(row, col+1);
               float dX = pixelLuminances[indexRight] - pixelLuminances[index];
               gradientX.push_back(dX);
               if(fabs(dX) > gxNormalize){
                   gxNormalize = fabs(dX);
               }
           } else {
               gradientX.push_back(0.0f);
           }

        }
    }
    assert(gradientX.size() == gradientY.size());

    for(int i = 0; i < gradientX.size(); i++){
        gradientX[i] = gradientReshapeRecursive(gradientX[i]/gxNormalize, m_curvature) * gxNormalize;

    }
    for(int i = 0; i < gradientY.size(); i++){
        gradientY[i] = gradientReshapeRecursive(gradientY[i]/gyNormalize, m_curvature) * gyNormalize;
    }

    std::vector<Vector3f> normals;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            QColor maskColor = QColor(mask.pixelAt(i,j));
            if(maskColor.red() > 150){
                Eigen::Vector3f gx = Vector3f(1.0f, 0.0f, gradientX[mask.indexAt(i,j)]);
                Eigen::Vector3f gy = Vector3f(0.0f, 1.0f, gradientY[mask.indexAt(i,j)]);


                Eigen::Vector3f normal = (gx.cross(gy));

                normal = normal.normalized();
                normals.push_back(normal);
            } else {
                normals.push_back(Vector3f(0.0,0.0,0.0));
            }
        }
    }
    //pixelLuminances = gradientX;
    return normals;
}

float ShapeEstimator::gradientReshapeRecursive(float in, int itr){
    float x = fabs(in);
    for(int i = 0; i < itr; i++){
        x = gradientReshape(x);
    }
    if(in < 0){
        return -1.0f * x;
    }
    return x;
}

float ShapeEstimator::gradientReshape(float x){
    return (3.0f + (-6.0f + 4.0f * x) * x) * x;
}

void ShapeEstimator::cropMask(ImageReader mask, std::vector<float> &pixelLuminances){
    int rows = mask.getImageHeight();
    int cols = mask.getImageWidth();

    for(int row = 0; row < rows; row++){
        for(int col = 0; col < cols; col++){
           QColor maskColor = QColor(mask.pixelAt(row, col));
           int index = mask.indexAt(row, col);

           if(maskColor.red() > 150){

           } else {
               if(DEPTHMAPBACKGROUND){
                    pixelLuminances[index] = 1.0f;
               } else {
                    pixelLuminances[index] = 0.0f;
               }
           }
        }
    }
}

std::vector<float> ShapeEstimator::getLuminances()
{
    return m_luminances;
}
