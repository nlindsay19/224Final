#ifndef BILATERALFILTER_H
#define BILATERALFILTER_H

#include "imagereader.h"

class BilateralFilter
{
public:
    BilateralFilter();
    float euclideanDistance(int row1, int col1, int row2, int col2);
    QRgb applyBilateralFilter(int row, int col, ImageReader im, float sigmac, float sigmar);

    float applyBilateralFilter(int row, int col, ImageReader im, std::vector<float> luminances, float sigma, float sigmar);

    QImage convolve(ImageReader im, float sigmac, float sigmar);
    std::vector<float> convolve(ImageReader im, std::vector<float> luminances, float sigmac, float sigmar);
    int m_kernelRows;
    int m_kernelCols;
    int m_kernelRadius;

};

#endif // BILATERALFILTER_H
