#include "imagereader.h"
#include "assert.h"
#include "QtWidgets"


ImageReader::ImageReader(QString filename)
{
    bool success = m_image.load(filename);
    if(success){
        m_imageData = reinterpret_cast<QRgb *>(m_image.bits());
        m_filename = filename;
    } else {
        std::cout << "Could not read image." << std::endl;
    }

    findMinAndMax();
}

ImageReader::ImageReader(int width, int height)
{
    m_image = QImage(width, height, QImage::Format_RGB32);
    m_filename = "";
}

ImageReader::~ImageReader(){

}

void ImageReader::saveImage(QString filenameOut){
    m_image.save(filenameOut);
}

int ImageReader::getImageWidth(){
    return m_image.width();
}

int ImageReader::getImageHeight(){
    return m_image.height();
}

int ImageReader::indexAt(int row, int col){
    assert(row < m_image.height());
    assert(col < m_image.width());
    assert(row * m_image.width() + col < m_image.height() * m_image.width());
    return row * m_image.width() + col;
}

QRgb ImageReader::pixelAt(int row, int col){
    return m_imageData[indexAt(row, col)];
}

void ImageReader::findMinAndMax()
{
    int min = 10000000000;
    int max = -1;
    for (int i = 0; i < getImageHeight(); i++) {
        for (int j = 0; j < getImageWidth(); j++) {
            if (QColor(pixelAt(i,j)).red() > 150) {
                if (j > max) {
                    max = j;
                }
                if (j < min) {
                    min = j;
                }
            }
        }
    }
    assert(max > min);
    m_xMax = max;
    m_xMin = min;
}

int ImageReader::getXMax()
{
    return m_xMax;
}

int ImageReader::getXMin()
{
    return m_xMin;
}

