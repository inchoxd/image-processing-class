#pragma once

#include <vector>
#include <string.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#define BSIZE 8

int gcm(int a, int b);

void checkered_flag(cv::Mat &data);
void checkered_flag(cv::Mat &data);
void blacken_upper_left_corner(cv::Mat &data);
void mozaic(cv::Mat &data);

int cvtYCbCr(cv::Mat &data);

void checkDisplayType(char *display, cv::Mat &input);
void imgTiled(cv::Mat &data);
void imgSingle(cv::Mat &data);
void imgPattern(char *pattern, cv::Mat &data);

cv::Mat inputImg(char *f_path, char *color);

void errorExt(char *arg);
