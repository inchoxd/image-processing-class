#pragma once

#include <vector>
#include <opencv2/core.hpp>

#define BSIZE 8

int cvtYCbCr(cv::Mat &data);
void mozaic(cv::Mat &data);
