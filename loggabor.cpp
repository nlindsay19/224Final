#include "loggabor.h"
#include <math.h>

#define M_PI 3.14159265358979323846264338327950288

LogGabor::LogGabor()
{
    m_kernelRows = 5;
    m_kernelCols = 5;

    m_kernelRadius = int(floor(float(m_kernelRows) / 2));
    m_filterBank = createFilterBank(20,2);
}

std::vector<std::vector<std::vector<float>>> LogGabor::createFilterBank(int numOrientations, int numScales){
    std::vector<std::vector<std::vector<float>>> filterBank;

    float thetaStd = M_PI / float(numOrientations) / 1.2f;
    float wavelengthIncrement = 1.6f;
    float sigmaF = 0.55f;

    for(int o = 1; o < numOrientations + 1; o++){
        std::vector<std::vector<float>> filtersForOrientation;
        float centerAngle = (float(o) - 1.0f) * M_PI / float(numOrientations);
        float wavelength = 3.0f;

        for(int s = 0; s < numScales; s++ ){
            std::vector<float> filterForScale;
            float centerFrequency = 1/wavelength;
            for(int i = 0; i < m_kernelRows; i++){
                for(int j = 0; j < m_kernelCols; j++){
                   float y = float(i) - float(m_kernelRows)/2.0f;
                   float x = float(j) - float(m_kernelCols)/2.0f;

                   float normalizedY = y / (float(m_kernelRows)/2.0f);
                   float normalizedX = x / (float(m_kernelRows)/2.0f);

                   float normalizedRadius = sqrtf(normalizedY * normalizedY + normalizedX * normalizedX);
                   float elementRadial = exp(-1.0f * pow( (normalizedRadius/(centerFrequency/0.5f)), 2) / (2.0f * sigmaF * sigmaF));
                   float theta = atan2(-1.0f * y , x); //normalizedY, normalizedX ?

                   float deltaSin = sinf(theta) * cosf(centerAngle) - cosf(theta) * sinf(centerAngle);
                   float deltaCosine = cosf(theta) * cosf(centerAngle) + sinf(theta) * sinf(centerAngle);

                   float deltaTheta = fabs(atan2(deltaSin, deltaCosine));

                   float elementAngular = exp((-1.0f * deltaTheta * deltaTheta) / (2.0f * thetaStd * thetaStd));
                   filterForScale.push_back(elementAngular * elementRadial); // clamp at 0?
                }
            }
            wavelength *= wavelengthIncrement;
            filtersForOrientation.push_back(filterForScale);
        }
        filterBank.push_back(filtersForOrientation);
    }
    return filterBank;
}

////http://sse.tongji.edu.cn/linzhang/files/Phase%20congruency%20induced%20local%20features%20for%20finger-knuckle-print%20recognition.pdf
//std::vector<float> LogGabor::combineKernels(std::vector<std::vector<float>> kernelBank){
//    for(int i = 0; i < kernelBank.size(); i++){
//        std::vector<float> currentKernel = kernelBank[i];
//        std::vector<float> shiftedFilter = fftshift(currentKernel);

//        std::vector<float> convoled =
//    }
//}
