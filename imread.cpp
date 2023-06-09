#include <cfloat>
#include <cstddef>
#include <cstdio>
#include <math.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

#include "tools.hpp"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("An input image file is missing.\n");
        return EXIT_FAILURE;
    }
    cv::Mat image;
    image = cv::imread(argv[1], cv::ImreadModes::IMREAD_COLOR);
    if (image.empty()) {
        printf("Image file is not found.\n");
        return EXIT_FAILURE;
    }
    cv::Mat original = image.clone();   // imageのコピー
    if(argc < 3) {
        printf("Qfactor is missing.\n");
        return EXIT_FAILURE;
    }
    int QF = strtol(argv[2], nullptr, 10);  // Quarity Factor
    if(QF < 0 || QF > 100) {
        printf("Valid range for Qfactor is from 0 to 100\n");
        return EXIT_FAILURE;
    }
    QF = (QF == 0) ? 1 : QF;    // if(QF==0) QF = 1;
    float scale;
    if(QF < 50) {
        scale = floorf(5000.0 / QF);
    } else {
        scale = 200 - QF * 2;
    }
    scale /= 100.0;
    scale = (scale < FLT_EPSILON) ? FLT_EPSILON : scale;

    bgr2ycrcb(image);
    std::vector<cv::Mat> ycrcb;
    cv::split(image, ycrcb);

    for (int c = 0; c < image.channels(); ++c) {
        cv::Mat buf;
        ycrcb[c].convertTo(buf, CV_32F);
        
        /**
        if(c > 0) {
            cv::resize(ycrcb[c], ycrcb[c]
        }
        **/
        // encoder
        blkproc(buf, blk::dct2);
        blkproc(buf, blk::quantize, c, scale);
        // decoder
        blkproc(buf, blk::dequantize, c, scale);
        blkproc(buf, blk::idct2);

        buf.convertTo(ycrcb[c], ycrcb[c].type());
    }

    cv::merge(ycrcb, image);

    cv::cvtColor(image, image, cv::COLOR_YCrCb2BGR);
    cv::imshow("image", image);
    cv::waitKey();
    cv::destroyAllWindows();

    return EXIT_SUCCESS;
}
