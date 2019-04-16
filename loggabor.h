#ifndef LOGGABOR_H
#define LOGGABOR_H


#include "imagereader.h"

class LogGabor
{
public:
    LogGabor();


    int m_kernelRows;
    int m_kernelCols;
    int m_kernelRadius;
    std::vector<std::vector<std::vector<float>>> m_filterBank;
    std::vector<std::vector<std::vector<float>>> createFilterBank(int numOrientations, int numScales);
};

#endif // LOGGABOR_H
