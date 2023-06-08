#pragma once
#include <functional>
#include <opencv2/core.hpp>

#define BSIZE 8

void bgr2ycrcb(cv::Mat &image);

void blkproc(cv::Mat &, std::function<void(cv::Mat &, int, float)>, int = 0,
             float = 0.0F);

namespace blk {
void mozaic(cv::Mat &in, int p0, float p1);
void dct2(cv::Mat &in, int p0, float p1);
void idct2(cv::Mat &in, int p0, float p1);
void quantize(cv::Mat &in, int p0, float p1);
void dequantize(cv::Mat &in, int p0, float p1);
}
