#include "shapeestimator.h"
#include "math.h"
#include "assert.h"

#include "Eigen/Dense"

using namespace Eigen;

ShapeEstimator::ShapeEstimator()
{

}

QImage ShapeEstimator::estimateShape(ImageReader imageIn, ImageReader mask, std::vector<float>& depthMap){
    BilateralFilter bf;
    int rows = imageIn.getImageHeight();
    int cols = imageIn.getImageWidth();


    QImage output(cols, rows, QImage::Format_RGB32);
    QRgb *imOut = reinterpret_cast<QRgb *>(output.bits());

    float sigma = 0.0f;

    std::vector<float> luminances = computePixelLuminance(imageIn, mask, sigma);

    sigmoidalCompression(luminances, sigma);

    float bilateralSigmaSpatial = 0.002f * float(cols);
    float bilateralSigmaL = 255.0f;
    luminances = bf.convolve(imageIn, luminances, bilateralSigmaSpatial, bilateralSigmaL);
    sigmoidalInversion(luminances, sigma);
    cropMask(mask, luminances);

    std::vector<Vector3f> normals = gradientField(mask, luminances);

//    for(int i = 0; i < rows; i++){
//        for(int j = 0; j < cols; j++){
//            int index = imageIn.indexAt(i, j);
//            float color = luminances[index] * 255.0f;
//            QColor colorOut = QColor(floor(color), floor(color), floor(color));
//            imOut[imageIn.indexAt(i, j)] = colorOut.rgb();
//        }
//    }

    float maxRed = 0.0f;
    float maxGreen = 0.0f;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            int index = imageIn.indexAt(i, j);
            float red = normals[index](0);
            if(fabs(red) > maxRed){
                maxRed = fabs(red);
            }
            float green = normals[index](1) ;
            if(fabs(green) > maxGreen){
                maxGreen = fabs(green);
            }
        }
    }

    // write out normal map
    maxRed = 255.0f / maxRed;
    maxGreen = 255.0f / maxGreen;

    for(int i = 0; i < rows; i++){
        for(int j = 0; j < cols; j++){
            int index = imageIn.indexAt(i, j);
            float red = normals[index](0) * maxRed;
            float green = normals[index](1) * maxGreen;
            float blue = normals[index](2) * 255.0f;

            QColor colorOut = QColor(fabs(floor(red)), fabs(floor(green)), floor(blue));
            imOut[imageIn.indexAt(i, j)] = colorOut.rgb();
        }
    }
    depthMap = luminances;
    return output;

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
            if((maskColor.red() > 200)){
                pixelsInObject += 1;
                float luminance =  0.213f * float(imageColor.red()) + 0.715f * float(imageColor.green()) + 0.072f * float(imageColor.blue());
                pixelLuminances.push_back(luminance/ 255.0f);
                objectLuminanceSum += log(luminance / 255.0f);
            } else {
                pixelLuminances.push_back(0.0f);
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

std::vector<Vector3f> ShapeEstimator::gradientField(ImageReader mask, std::vector<float> &pixelLuminances){
    int rows = mask.getImageHeight();
    int cols = mask.getImageWidth();

    std::vector<float> gradientX;
    std::vector<float> gradientY;

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
        gradientX[i] = gradientReshapeRecursive(gradientX[i]/gxNormalize, 1) * gxNormalize;

    }
    for(int i = 0; i < gradientY.size(); i++){
        gradientY[i] = gradientReshapeRecursive(gradientY[i]/gyNormalize, 1) * gyNormalize;
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
               pixelLuminances[index] = 0.0f;
           }

        }
    }
}
