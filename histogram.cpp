#include "histogram.h"
#include "math.h"

Histogram::Histogram(std::vector<float> luminances)
{
    m_histogram.reserve(m_size);
    std::fill(m_histogram.begin(), m_histogram.end(), 0);

    // Take the log of all the luminances and ignore negative values
    for (int i = 0; i < luminances; i++) {
        float logVal = log(luminances[i]);

        m_luminances.push_back(logVal);
    }

    float max = -1;
    for (int i  = 0; i < m_luminances.size(); i++) {
        if (m_luminances[i] > max) {
            max = m_luminances[i];
        }
    }

    m_binSize = max / (float)m_size;
    createHistogram();

}

void Histogram::createHistogram()
{

    for (int i = 0; i < m_luminances.size(); i++) {
        float lum = m_luminances[i];
        if (lum < 0) {
            continue;
        }
        int bin = (int) ((lum - fmod(lum, m_binSize)) / m_binSize);
        m_histogram[bin]++;
    }

}

float Histogram::findSlope(int bin)
{
    int x1 = bin;
    int x2 = bin + 1;
    int y1 = m_histogram[x1];
    int y2 = m_histogram[x2];

    float slope = ((float)(y2 - y1)) / ((float)(x2 - x1));
    return slope;
}

float Histogram::findLowestSlope()
{
    float minSlope = 100000000.f;
    int minIndex = 0;
    for (int i = 0; i < m_histogram.size() - 1; i++) {
        float slope = findSlope(i);
        if (slope < minSlope) {
            minSlope = slope;
            minIndex = i;
        }
    }

    // Returning beginning of highlight vals
    return ((float)minIndex) * m_binSize;

}

std::vector<int> Histogram::findHighlights()
{
    float lowestLog = findLowestSlope();
    std::vector<int> highlights;
    for (int i = 0; i < m_luminances.size(); i++) {
        float lum = m_luminances[i];
        if (lum > lowestLog) {
            highlights.push_back(i);
        }
    }

    return highlights;
}
