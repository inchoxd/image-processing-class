#include "tools.hpp"

int cvtYCbCr(cv::Mat &data) {
    const int w = data.cols, h = data.rows, NC = data.channels();
    // const int STRIDE = w * NC;
    if(NC < 3)
        return 1;

    uchar *p0, *p1, *p2;
    p0 = data.data;
    p1 = data.data + 1;
    p2 = data.data + 2;

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x, p0 += NC, p1 += NC, p2 += NC) {
            int B = *p0, G = *p1, R = *p2;

            double Y = 0.299 * R + 0.587 * G + 0.114 * B;
            double Cb = -0.1687 * R - 0.3313 * G + 0.5 * B + 128;
            double Cr = 0.5 * R -0.4187 * G - 0.0813 * B + 128;

            *p0 = static_cast<uchar>(roundl(Y));
            *p1 = static_cast<uchar>(roundl(Cr));
            *p2 = static_cast<uchar>(roundl(Cb));
        }
    }

    return 0;
}


