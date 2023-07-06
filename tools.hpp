#pragma once

#include <cfloat>
#include <cstddef>
#include <cstdio>
#include <math.h>
#include <string.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
//#include <opencv2/core/types.hpp>
//#include <opencv2/quality/qualitybase.hpp>
#include <vector>
#include "bitstream.hpp"

#define BSIZE 8

/***************************************************************
 * functions of display pattern
***************************************************************/

int gcm(int a, int b);

void checkered_flag(cv::Mat &data);
void checkered_flag(cv::Mat &data);
void blacken_upper_left_corner(cv::Mat &data);

/***************************************************************
 * functions of processing to jpeg
***************************************************************/

int cvtYCbCr(cv::Mat &data);

void blkProc(cv::Mat &, std::function<void(cv::Mat &, int *)>, int * = nullptr);
void Encode_MCUs(std::vector<cv::Mat> &, bitstream &);
namespace blk {
    constexpr float qmatrix[3][64] = {
        {
            16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55,
            14, 13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62,
            18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 
            49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99
        }, 
        {
            17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99,
            24, 26, 56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99,
            99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
            99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99
        }, 
        {
            17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99,
            24, 26, 56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99,
            99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99,
            99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99
        }
    }; // constexpr <- 完全なる定数．型をつけられる．コンパイル前から値を決められる．
    void mozaic(cv::Mat &data, int * = nullptr);
    void dct2(cv::Mat &data, int * = nullptr);
    void idct2(cv::Mat &data, int * = nullptr);
    void quantize(cv::Mat &data, int * = nullptr);
    void dequantize(cv::Mat &data, int * = nullptr);
}

/***************************************************************
 * functions of cli option
***************************************************************/
void createQtable(int, float, int *);
void checkDisplayType(char *display, cv::Mat &input);
void imgTiled(cv::Mat &data);
void imgSingle(cv::Mat &data);
void imgPattern(char *pattern, cv::Mat &data);
void psnr(cv::Mat &orgn, cv::Mat &data);
//void mypsnr(cv::Mat &ref, cv::Mat &img);
void procJpg(cv::Mat &data, int QF, int PSNR = 0);
cv::Mat inputImg(char *f_path, char *color);
void errorExt(char *arg);
