#ifndef IMAGEREADER_H
#define IMAGEREADER_H

#include <iostream>
#include <QImage>
class ImageReader
{
public:
    ImageReader(QString filename);
    ImageReader(int width, int height);
    ~ImageReader();
    void saveImage(QString filenameOut);
    int getImageWidth();
    int getImageHeight();
    QRgb pixelAt(int row, int col);
    int indexAt(int row, int col);
private:
    QRgb *m_imageData;
    QString m_filename;
    QImage m_image;

};

#endif // IMAGEREADER_H
