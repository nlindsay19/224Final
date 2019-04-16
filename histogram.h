#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <vector>


class Histogram
{
public:
    Histogram(std::vector<float> luminances);

    void createHistogram();
    float findSlope(int bin);
    float findLowestSlope();
    std::vector<int> findHighlights();

private:
    std::vector<float> m_luminances;
    int m_size = 100;
    std::vector<int> m_histogram;
    float m_binSize;

};

#endif // HISTOGRAM_H
