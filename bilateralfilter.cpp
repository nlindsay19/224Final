#include "bilateralfilter.h"
#include "imagereader.h"
#include <math.h>
BilateralFilter::BilateralFilter()
{
    m_kernelRows = 5;
    m_kernelCols = 5;

    m_kernelRadius = int(floor(float(m_kernelRows) / 2));
    std::cout << m_kernelRadius << std::endl;
   // sigmac = 100.0f;
   // sigmar = 2500.0f;
}

QImage BilateralFilter::convolve(ImageReader im, float sigmac, float sigmar){
    int rows = im.getImageHeight();
    int cols = im.getImageWidth();
    QImage output(cols, rows, QImage::Format_RGB32);
    QRgb *imOut = reinterpret_cast<QRgb *>(output.bits());
    int counter = 0;
    for(int row = 0; row < rows; row++){
        #pragma omp parallel for
        for(int col = 0; col < cols; col++){

            if(row - m_kernelRadius < 0 || col - m_kernelRadius < 0){
                QColor black = QColor(0,0,0);
                imOut[counter] = black.rgb();
            } else if(row + m_kernelRadius >= rows || col + m_kernelRadius >= cols){
                QColor black = QColor(0,0,0);
                imOut[counter] = black.rgb();
            } else {
                imOut[counter]  = applyBilateralFilter(row, col, im, sigmac, sigmar);

            }
            counter += 1;
        }
    }
    std::cout << "done" << std::endl;
    return output;
}

std::vector<float> BilateralFilter::convolve(ImageReader im, std::vector<float> luminances, float sigmac, float sigmar){
    int rows = im.getImageHeight();
    int cols = im.getImageWidth();
    std::vector<float> imOut;
    for(int row = 0; row < rows; row++){

        #pragma omp parallel for
        for(int col = 0; col < cols; col++){
            if(row - m_kernelRadius < 0 || col - m_kernelRadius < 0){
                imOut.push_back(0.0f);
            } else if(row + m_kernelRadius >= rows || col + m_kernelRadius >= cols){
                imOut.push_back(0.0f);
            } else {
                imOut.push_back(applyBilateralFilter(row, col, im, luminances, sigmac, sigmar));
            }
        }
    }
    std::cout << "done" << std::endl;
    return imOut;
}

QRgb BilateralFilter::applyBilateralFilter(int row, int col, ImageReader im, float sigmac, float sigmar){
    QRgb centerRgb = im.pixelAt(row, col);
    QColor centerColor = QColor(centerRgb);
    float colorAcc = 0.0f;
    float weightAcc = 0.0f;

    for(int i = 0; i < m_kernelRows; i++){
        for(int j = 0; j < m_kernelCols; j++){
            int y = row - (m_kernelRadius - i);
            int x = col - (m_kernelRadius - j);


            QRgb neighborRgb = im.pixelAt(y, x);
            QColor neighborColor = QColor(neighborRgb);

            float redDistance = (float(centerColor.red()) - float(neighborColor.red()));
            float colorDistance = redDistance * redDistance;

            float xf = float(m_kernelRadius - j);
            float yf = float(m_kernelRadius - i);
            float pixelDistance = xf * xf + yf * yf;

            float wColor =  exp(-0.5f * colorDistance / (sigmac * sigmac));
            float wDistance = exp( -0.5f * (pixelDistance) / (sigmar * sigmar) );
            colorAcc += (wDistance * wColor) * float(neighborColor.red());
            weightAcc += (wDistance * wColor);
        }
    }

    if(weightAcc == 0.0f){
        weightAcc = 0.001f;
    }
    colorAcc /= weightAcc;
    if(colorAcc > 255.0f){
        colorAcc = 255.0f;
    } else if(colorAcc < 0.0f){
        colorAcc = 0.0f;
    }

    QColor colorOut = QColor(floor(colorAcc ),floor(colorAcc ),floor(colorAcc));

    return colorOut.rgb();
}

float BilateralFilter::applyBilateralFilter(int row, int col, ImageReader im, std::vector<float> luminances, float sigmaSpatial, float sigmaL){
    float centerColor = luminances[im.indexAt(row, col)];
    float colorAcc = 0.0f;
    float weightAcc = 0.0f;

    for(int i = 0; i < m_kernelRows; i++){
        for(int j = 0; j < m_kernelCols; j++){
            int y = row - (m_kernelRadius - i);
            int x = col - (m_kernelRadius - j);

            float neighborColor = luminances[im.indexAt(y, x)];

            float luminanceDistance = pow(float(centerColor) - float(neighborColor), 2.0f);

            float xf = float(m_kernelRadius - j);
            float yf = float(m_kernelRadius - i);
            float pixelDistance = xf * xf + yf * yf;

            float wColor =  exp(-0.5f * luminanceDistance / (sigmaL * sigmaL));

            float wDistance = exp( -0.5f * (pixelDistance) / (sigmaSpatial * sigmaSpatial) );
            colorAcc += (wDistance * wColor) * float(neighborColor);
            weightAcc += (wDistance * wColor);
        }
    }

    if(weightAcc == 0.0f){
        weightAcc = 0.001f;
    }

    colorAcc /= weightAcc;
    if(colorAcc > 1.0f){
        colorAcc = 1.0f;
    } else if(colorAcc < 0.0f){
        colorAcc = 0.0f;
    }

    return colorAcc;
}

float BilateralFilter::euclideanDistance(int row1, int col1, int row2, int col2){
    return float( (row1 - row2) * (row1 - row2) + (col1 - col2) * (col1-col2));
}
