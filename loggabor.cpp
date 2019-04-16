#include "loggabor.h"
#include "math.h"

#define M_PI 3.14159265358979323846264338327950288

LogGabor::LogGabor()
{
    m_kernelRows = 5;
    m_kernelCols = 5;

    m_kernelRadius = int(floor(float(m_kernelRows) / 2));
}

std::vector<float> LogGabor::createFilterBank(){

}

////http://sse.tongji.edu.cn/linzhang/files/Phase%20congruency%20induced%20local%20features%20for%20finger-knuckle-print%20recognition.pdf
//std::vector<float> LogGabor::combineKernels(std::vector<std::vector<float>> kernelBank){
//    for(int i = 0; i < kernelBank.size(); i++){
//        std::vector<float> currentKernel = kernelBank[i];
//        std::vector<float> shiftedFilter = fftshift(currentKernel);

//        std::vector<float> convoled =
//    }
//}
