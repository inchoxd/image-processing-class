#include "tools.hpp"

/***************************************************************
 * functions of display pattern
 ***************************************************************/

int gcm(int a, int b) {
    /**
     * NOTE: MAXIMUM VAL IS 128!!!
     *       MINIMUM VAL IS 2!!!
     **/
    int tmp, r = 0;
    tmp = a;
    if(a > b) {
        tmp = b;
    }

    for(int i = 2; i <= tmp; i++) {
        if(a % i == 0 && b % i == 0) {
            if(r < 128) {
                r = i;
            }
        }
    }

    return r;
}

void checkered_flag(cv::Mat &data) {
    const int w = data.cols, h = data.rows, NC = data.channels();
    bool fill = true;
    int gcm_val = gcm(w, h);

    for(int y = 0; y < h; ++y) {
        const int STRIDE = w * NC;
        for(int x = 0; x < w; ++x) {
            if(fill == true) {
                for(int c = 0; c < NC; c++) {
                    int val = data.data[y * STRIDE + x * NC + c];       // x, y座標を指定して画素を取得．ストライドアクセス．
                    val = 0;
                    data.data[y * STRIDE + x * NC + c] = val;
                }
            }
            if(x % gcm_val == 0) {
                if(fill == true) {
                    fill = false;
                } else {
                    fill = true;
                }
            }
        }
        if(y % gcm_val >= gcm_val - 1) {
            if(fill == true) {
                fill = false;
            } else {
                fill = true;
            }
        }
    }
}

void zebra(cv::Mat &data) {
    const int w = data.cols, h = data.rows, NC = data.channels();
    bool fill = true;
    int gcm_val = gcm(w, h);

    for(int y = 0; y < h; ++y) {
        const int STRIDE = w * NC;
        for(int x = 0; x < w; ++x) {
            if(fill == true) {
                for(int c = 0; c < NC; c++) {
                    int val = data.data[y * STRIDE + x * NC + c];       // x, y座標を指定して画素を取得．ストライドアクセス．
                    val = 0;
                    data.data[y * STRIDE + x * NC + c] = val;
                }
            }
            if(x % gcm_val == 0) {
                if(fill == true) {
                    fill = false;
                } else {
                    fill = true;
                }
            }
        }
    }
}

void blacken_upper_left_corner(cv::Mat &data) {
    const int w = data.cols, h = data.rows, NC = data.channels();

    for(int y = 0; y < h / 2; ++y) {
        const int STRIDE = w * NC;
        for(int x = 0; x < w / 2; ++x) {
            for(int c = 0; c < NC; c++) {
                int val = data.data[y * STRIDE + x * NC + c];       // x, y座標を指定して画素を取得．ストライドアクセス．
                val = (c != 0) ? 0 : val;
                data.data[y * STRIDE + x * NC + c] = val;
            }
        }
    }
}


void blk::mozaic(cv::Mat &data, int p0, float p1) {
    std::vector<cv::Mat> ycrcb;
    cv::split(data, ycrcb);
    for(int y = 0; y < ycrcb[0].rows; y += BSIZE) {
        for(int x = 0; x < ycrcb[0].cols; x += BSIZE) {
            cv::Mat blk = ycrcb[0](cv::Rect(x, y, BSIZE, BSIZE));
            for(int i = 0; i < BSIZE; i++) {
                for(int j = 0; j < BSIZE; j++) {
                    blk.data[i * ycrcb[0].cols + j] = blk.data[0];
                }
            }
        }
    }
    cv::merge(ycrcb, data);
    cv::cvtColor(data, data, cv::COLOR_YCrCb2BGR);
}

/***************************************************************
 * functions of processing to jpeg
 ***************************************************************/

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

void blk::quantize(cv::Mat &data, int c, float scale) {
    data.forEach<float>([&](float &v, const int *pos) -> void {     // opencvの関数で，画素それぞれに処理をかける．無名関数でスコープ内のv, poswを参照．戻り値の型はvoid
            float stepsize = blk::qmatrix[c][pos[0] * data.cols + pos[1]] * scale;
            v /= stepsize;  // 画素を1/stepsize
            v = roundf(v);
            });
}

void blk::dequantize(cv::Mat &data, int c, float scale) {
    data.forEach<float>([&](float &v, const int *pos) -> void {
            float stepsize = blk::qmatrix[c][pos[0] * data.cols + pos[1]] * scale;
            v *= stepsize;  // 画素を1/stepsize
            v = roundf(v);
            });
}

void blk::dct2(cv::Mat &data, int p0, float p1) { cv::dct(data, data); }

void blk::idct2(cv::Mat &data, int p0, float p1) { cv::idct(data, data); }

void blkProc(cv::Mat &data, std::function<void(cv::Mat &, int, float)> func, int p0, float p1) {
    for (int y = 0; y < data.rows; y += BSIZE) {
        for (int x = 0; x < data.cols; x += BSIZE) {
            cv::Mat blk_in = data(cv::Rect(x, y, BSIZE, BSIZE)).clone();
            cv::Mat blk_out = data(cv::Rect(x, y, BSIZE, BSIZE));
            func(blk_in, p0, p1);
            blk_in.convertTo(blk_out, blk_out.type());
        }
    }
}

void procJpg(cv::Mat &data, int QF) {
    QF = (QF == 0) ? 1 : QF;    // if(QF==0) QF = 1;
    float scale;
    if(QF < 50) {
        scale = floorf(5000.0 / QF);
    } else {
        scale = 200 - QF * 2;
    }
    scale /= 100.0;
    scale = (scale < FLT_EPSILON) ? FLT_EPSILON : scale;

    cvtYCbCr(data);
    std::vector<cv::Mat> ycrcb;
    cv::split(data, ycrcb);

    for (int c = 0; c < data.channels(); ++c) {
        cv::Mat buf;
        ycrcb[c].convertTo(buf, CV_32F);
        
        // encoder
        blkProc(buf, blk::dct2);
        blkProc(buf, blk::quantize, c, scale);
        // decoder
        blkProc(buf, blk::dequantize, c, scale);
        blkProc(buf, blk::idct2);

        buf.convertTo(ycrcb[c], ycrcb[c].type());
    }

    cv::merge(ycrcb, data);

    cv::cvtColor(data, data, cv::COLOR_YCrCb2BGR);
}

/***************************************************************
 * functions of cli option
 ***************************************************************/

void checkDisplayType(char *display, cv::Mat &input) {
    if(strcmp("tiled", display) == 0) {
        imgTiled(input);
    } else if(strcmp("single", display) == 0) {
        imgSingle(input);
    } else {
        errorExt(display);
    }
}

void imgTiled(cv::Mat &data) {
    const int width = data.cols, height = data.rows;
    cv::Mat image[3];
    cv::Mat tiled(height * 2, width * 2, data.type());
    data.copyTo(tiled(cv::Rect(0, 0, width, height)));

    for(int i = 0; i < 3; i++) {
        data.copyTo(image[i]);
    }

    for(int i = 0; i < 3; i++) {
        switch(i) {
            case 0:
                blacken_upper_left_corner(image[0]);
                image[0].copyTo(tiled(cv::Rect(width, 0, width, height)));
                break;
            case 1:
                zebra(image[1]);
                image[1].copyTo(tiled(cv::Rect(0, height, width, height)));
                break;
            case 2:
                checkered_flag(image[2]);
                image[2].copyTo(tiled(cv::Rect(width, height, width, height)));
                break; 
        }
    }

    cv::resize(tiled, tiled, cv::Size(), 0.65, 0.65);
    cv::imshow("results", tiled);
    cv::waitKey();
    cv::destroyAllWindows();
}

void imgSingle(cv::Mat &data) {
    cv::imshow("results", data);
    cv::waitKey();
    cv::destroyAllWindows();
}

void imgPattern(char *pattern, cv::Mat &data) {
    if(strcmp(pattern, "ulc") == 0) {
        blacken_upper_left_corner(data);
    } else if(strcmp(pattern, "zebra") == 0) {
        zebra(data);
    } else if(strcmp(pattern, "checkerd") == 0) {
        checkered_flag(data);
    } else if(strcmp(pattern, "mozaic") == 0) {
        blkProc(data, blk::mozaic);
    }
}

cv::Mat inputImg(char *f_path, char *color) {
    cv::Mat input;
    if(strcmp(color, "color") == 0) {
        input = cv::imread(f_path, cv::ImreadModes::IMREAD_COLOR);
    } else if(strcmp(color, "ycrcb") == 0) {
        input = cv::imread(f_path, cv::ImreadModes::IMREAD_COLOR);
        cvtYCbCr(input);
    } else if (strcmp(color, "mono") == 0) {
        input = cv::imread(f_path, cv::ImreadModes::IMREAD_GRAYSCALE);
    } else {
        errorExt(color);
    }

    const int width = input.cols, height = input.rows;
    printf("width=%d, height=%d\n", width, height);

    return input;
}

void errorExt(char *arg) {
    printf("%s is an option that does not exist.\n", arg);
    exit(1);
}
