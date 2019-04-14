#include "loggabor.h"
#include "math.h"

#define M_PI 3.14159265358979323846264338327950288

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
//                imOut.push_back(applyGaborFilter(row, col, im, luminances, sigmac, sigmar));
//            }
//        }
//    }
//    std::cout << "done" << std::endl;
//    return imOut;
//}

//// explanation :https://www.peterkovesi.com/matlabfns/PhaseCongruency/Docs/convexpl.html
//std::vector<float> LogGabor::getKernel(float f, float sigmaF, float theta, float sigmaTheta){
//    float sigmaOnF = 0.0f;
//    float fo = 0.0f;
//    float centerAngle = 0.0f;
//    float thetaStd = centerAngle / 1.2f;
//    std::vector<float> kernel;
//    //MULTIPLY RADIAL BY LOW PASS FILTER
//    for(int i = 0; i < m_kernelRows; i++){
//        for(int j = 0; j < m_kernelCols; j++){
//            int y = fabs(m_kernelRadius - i);
//            int x = fabs(m_kernelRadius - j);

//            float normalizedY = float(y) / m_kernelRadius;
//            float normalizedX = float(x) / m_kernelRadius;
//            float normalizedRadius = sqrtf( normalizedY * normalizedY + normalizedX * normalizedX);

//            float elementRadial = exp(pow( (-1.0f * log(normalizedRadius/fo)), 2.0f) / (2.0f * pow(log(sigmaOnF), 2.0f)));

//            float theta = atan2(-1.0f * y, x);
//            float sintheta = sin(theta);
//            float costheta = cos(theta);

//            float deltaSin = sintheta * cosf(centerAngle) - costheta * sinf(centerAngle);
//            float deltaCosine = costheta * cosf(centerAngle) + sintheta * sinf(centerAngle);

//            float deltaTheta = fabs(atan2(deltaSin, deltaCosine));

//            float elementAngular = exp((-1.0f * deltaTheta * deltaTheta) / (2.0f * thetaStd * thetaStd));

//            kernel.push_back(elementAngular * elementRadial);

//        }
//    }
//}

//std::vector<float> LogGabor::computeOrientations(int maxO, std::vector<float> &orientations, std::vector<float> &scales){
//    for(int z = 0; z < maxO; z++){
//        float nextOrientation = float(z) * M_PI / float(maxO);
//        orientations.push_back(nextOrientation);
//    }
//    for(int s = 0; s < maxScale; s++){
//        scales.push_back(float(s));
//    }
//}

//QRgb LogGabor::applyGaborFilter(int row, int col, std::vector<float> im){
//    QRgb centerRgb = im.pixelAt(row, col);
//    QColor centerColor = QColor(centerRgb);



//    if(weightAcc == 0.0f){
//        weightAcc = 0.001f;
//    }
//    colorAcc /= weightAcc;
//    if(colorAcc > 255.0f){
//        colorAcc = 255.0f;
//    } else if(colorAcc < 0.0f){
//        colorAcc = 0.0f;
//    }

//    QColor colorOut = QColor(floor(colorAcc ),floor(colorAcc ),floor(colorAcc));

//    return colorOut.rgb();
//}

////http://sse.tongji.edu.cn/linzhang/files/Phase%20congruency%20induced%20local%20features%20for%20finger-knuckle-print%20recognition.pdf
//std::vector<float> LogGabor::combineKernels(std::vector<std::vector<float>> kernelBank){
//    for(int i = 0; i < kernelBank.size(); i++){
//        std::vector<float> currentKernel = kernelBank[i];
//        std::vector<float> shiftedFilter = fftshift(currentKernel);

//        std::vector<float> convoled =
//    }
//}
