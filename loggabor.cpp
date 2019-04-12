#include "loggabor.h"
#include "math.h"

LogGabor::LogGabor()
{
    m_kernelRows = 5;
    m_kernelCols = 5;

    m_kernelRadius = int(floor(float(m_kernelRows) / 2));
}

//std::vector<float> LogGabor::convolve(ImageReader im, std::vector<float> depthMap){
//    int rows = im.getImageHeight();
//    int cols = im.getImageWidth();
//    std::vector<float> imOut;
//    for(int row = 0; row < rows; row++){

//        #pragma omp parallel for
//        for(int col = 0; col < cols; col++){
//            if(row - m_kernelRadius < 0 || col - m_kernelRadius < 0){
//                imOut.push_back(0.0f);
//            } else if(row + m_kernelRadius >= rows || col + m_kernelRadius >= cols){
//                imOut.push_back(0.0f);
//            } else {
//                imOut.push_back(applyBilateralFilter(row, col, im, luminances, sigmac, sigmar));
//            }
//        }
//    }
//    std::cout << "done" << std::endl;
//    return imOut;
//}
