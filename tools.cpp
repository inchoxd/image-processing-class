#include "tools.hpp"
#include "zigzag_order.hpp"
#include <cstdint>
#include <ctime>
#include <math.h>
#include <opencv2/imgproc.hpp>
#include "huffman_tables.hpp"
#include "bitstream.hpp"
#include "jpgheaders.hpp"
#include "ycctype.hpp"
#include <x86intrin.h>

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


void blk::mozaic(cv::Mat &data, int *p) {
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

void blk::quantize(cv::Mat &data, int *qtable) {
    data.forEach<float>([&](float &v, const int *pos) -> void {     // opencvの関数で，画素それぞれに処理をかける．無名関数でスコープ内のv, poswを参照．戻り値の型はvoid
            v /= qtable[pos[0] * data.cols + pos[1]];  // 画素を1/stepsize
            v = roundf(v);
            });
    // float *p = (float *)data.data;
    // for(int i = 0; i < 64; i++) {
    //     printf("%d ", (int)p[scan[i]]);
    // }
    // printf("\n");
}

void blk::dequantize(cv::Mat &data, int *qtable) {
    data.forEach<float>([&](float &v, const int *pos) -> void {
            v *= qtable[pos[0] * data.cols + pos[1]];  // 画素を1/stepsize
            v = roundf(v);
            });
}

void blk::dct2(cv::Mat &data, int *p) { cv::dct(data, data); }

void blk::idct2(cv::Mat &data, int *p) { cv::idct(data, data); }

void blkProc(cv::Mat &data, std::function<void(cv::Mat &, int *p)> func, int *p) {
    for (int y = 0; y < data.rows; y += BSIZE) {
        for (int x = 0; x < data.cols; x += BSIZE) {
            cv::Mat blk_in = data(cv::Rect(x, y, BSIZE, BSIZE)).clone();
            cv::Mat blk_out = data(cv::Rect(x, y, BSIZE, BSIZE));
            func(blk_in, p);
            blk_in.convertTo(blk_out, blk_out.type());
        }
    }
}

void createQtable(int c, float scale, int *qtable) {
    for(int i = 0; i < 64; ++i) {
        float stepsize = (blk::qmatrix[c][i] * scale + 50.0) / 100.0;
        stepsize = floor(stepsize);
        if(stepsize < 1.0) {
            stepsize = 1;
        }
        if(stepsize > 255) {
            stepsize = 255;
        }
        qtable[i] = stepsize;
    }
}


void psnr(cv::Mat &orgn, cv::Mat &data) {
    float PSNR = 0, MSE = 0, diff = 0;
    int M = orgn.cols, N = orgn.rows;
    int c = orgn.channels();
    uchar *op0, *op1, *op2, *dp0, *dp1, *dp2;
    float depth = (orgn.depth() == 0) ? 8 : 0;

    for(int i = 0; i < c; ++i){
        switch(i) {
            case 0:
                op0 = orgn.data;
                dp0 = data.data;
                break;
            case 1:
                op1 = orgn.data + 1;
                dp1 = data.data + 1;
                break;
            case 2:
                op2 = orgn.data + 2;
                dp2 = data.data + 2;
                break;
        }
    }
    for(int y = 0; y < N; ++y) {
        for(int x = 0; x < M; ++x, op0 += c, op1 += c, op2 += c, dp0 += c, dp1 += c, dp2 += c) {
            float orgn_pxl_avg = (*op0 + *op1 + *op2) / c;
            float data_pxl_avg = (*dp0 + *dp1 + *dp2) / c;
            diff += orgn_pxl_avg - data_pxl_avg;
        }
    }
    MSE = pow(diff, 2.0) / (M * N);
    PSNR = 10 * log10(pow((pow(2.0, depth) - 1), 2.0) / MSE);
    printf("PSNR = %f\n", PSNR);
}

/**
void mypsnr(cv::Mat &ref, cv::Mat &img) {
    auto PSNR = cv::quality::QualityPSNR::create(ref);
    cv::Scalar a = PSNR ->compute(img);
    double tmp = 0.0;
    for(int c = 0; c < ref.channels(); ++c) {
        tmp += a[c];
    }
    tmp /= ref.channels();
    printf("PSNR = %f [dB]\n", tmp);
}
**/

void EncodeDC(int diff, const uint32_t *Ctable, const uint32_t *Ltable, bitstream &enc) {
    uint32_t uval = (diff < 0) ? -diff:diff;
    uint32_t s = 32 - _lzcnt_u32(uval);    
    enc.put_bits(Ctable[s], Ltable[s]);
    if(s != 0) {
        if(diff < 0) {
            diff -= 1;

        }
        enc.put_bits(diff, s);
    }
}

void EncodeAC(int run, int val, const uint32_t *Ctable, const uint32_t *Ltable, bitstream &enc) {
    uint32_t uval = (val < 0) ? -val:val;
    uint32_t s = 32 - _lzcnt_u32(uval);    
    enc.put_bits(Ctable[(run << 4) + s], Ltable[(run << 4) + s]);
    if(s != 0) {
        if(val < 0) {
            val -= 1;

        }
        enc.put_bits(val, s);
    }
}

void EncodeBlock(cv::Mat &data, int c, int &prev_dc, bitstream &enc) {
    float *p = (float *)data.data;
    // DC
    int diff = p[0] - prev_dc;
    prev_dc = p[0];
    // EncodeDC with Huffman
    EncodeDC(diff, DC_cwd[c], DC_len[c], enc);

    int run = 0;    // 0が連続する個数
    // AC
    for(int i = 1; i < 64; ++i) {
        int ac = p[scan[i]];
        if(ac == 0) {
            run++;
        } else {
            while(run > 15) {
                // Encode ZRL whith Huffman
                EncodeAC(0xF, 0x0,  AC_cwd[c], AC_len[c], enc);
                run -= 16;
            }
            // Encode non-zero AC with Huffman
            EncodeAC(run, ac,  AC_cwd[c], AC_len[c], enc);
            run = 0;
        }
    }
    if(run) {
        // Encode EOB with Huffman
        EncodeAC(0x0, 0x0,  AC_cwd[c], AC_len[c], enc);
    }
}

void Encode_MCUs(std::vector<cv::Mat> &buf, bitstream &enc, int YCCtype) {
    const int height = buf[0].rows;
    const int width = buf[0].cols;
    int H, V;
    switch(YCCtype) {
        case YCC::YUV420:
            H = V = 2;
            break;
        case YCC::GRAY:
            H = V = 1;
            break;
        default:
            H = V = 0;
            break;
    }
    cv::Mat blk;
    int prev_dc[3] = {0};

    for(int Ly = 0, Cy = 0; Ly < height; Ly += BSIZE * V, Cy += BSIZE) {
        for(int Lx = 0, Cx = 0; Lx < width; Lx += BSIZE * H, Cx += BSIZE) {
            for(int y = 0; y < V; y++) {
                for(int x = 0; x < H; x++) {
                    blk = buf[0](cv::Rect(Lx + BSIZE * x, Ly + BSIZE * y, BSIZE, BSIZE)).clone();
                    EncodeBlock(blk, 0, prev_dc[0], enc);
                }
            }
            if(buf.size() == 3) {
                //Cb
                blk = buf[2](cv::Rect(Cx, Cy, BSIZE, BSIZE)).clone();
                EncodeBlock(blk, 1, prev_dc[1], enc);
                //Cr
                blk = buf[1](cv::Rect(Cx, Cy, BSIZE, BSIZE)).clone();
                EncodeBlock(blk, 1, prev_dc[2], enc);
            }
        }
    }
}

void procJpg(cv::Mat &image, int QF, int PSNR) {
    timer_counter prcJpgStm;
    prcJpgStm.procJpg_start_time = clock();
    PSNR = 1;
    cv::Mat orgn;
    if(PSNR)
        orgn = image.clone();
    QF = (QF == 0) ? 1 : QF;    // if(QF==0) QF = 1;
    float scale;
    if(QF < 50) {
        scale = floorf(5000.0 / QF);
    } else {
        scale = 200 - QF * 2;
    }

    int qtable_L[64], qtable_C[64];
    prcJpgStm.start_time = clock();
    createQtable(0, scale, qtable_L);
    createQtable(0, scale, qtable_C);
    prcJpgStm.func_createQtable = clock() - prcJpgStm.start_time;

    bitstream enc;
    int YCCtype = (image.channels() == 3) ? YCC::YUV420 : YCC::GRAY;
    
    prcJpgStm.start_time = clock();
    create_mainheader(image.cols, image.rows, image.channels(), qtable_L, qtable_C, YCCtype, enc);
    prcJpgStm.func_create_main_header = clock() - prcJpgStm.start_time;

    if(image.channels() == 3) cvtYCbCr(image);
    std::vector<cv::Mat> ycrcb;
    std::vector<cv::Mat> buf(image.channels());
    cv::split(image, ycrcb);

    // constexpr float D = 2;
    // encoder
    prcJpgStm.start_time = clock();
    for (int c = 0; c < image.channels(); ++c) {
        int *qtable = qtable_L;
        if(c > 0) {
            qtable = qtable_C;
            cv::resize(ycrcb[c], ycrcb[c], cv::Size(), 0.5, 0.5, cv::INTER_LINEAR_EXACT);
        }
        ycrcb[c].convertTo(buf[c], CV_32F);
        buf[c] -= 128.0;    // DC level shift
        blkProc(buf[c], blk::dct2);
        blkProc(buf[c], blk::quantize, qtable);
    }
    Encode_MCUs(buf, enc, YCCtype);
    const std::vector<uint8_t> codestream = enc.finalize();
    printf("codestream size = %d\n", codestream.size());
    prcJpgStm.proc_encoder = clock() - prcJpgStm.start_time;

    FILE *fout = fopen("myjpeg.jpg", "wb");
    fwrite(codestream.data(), sizeof(unsigned char), codestream.size(), fout);
    fclose(fout);
    // decoder
    prcJpgStm.start_time = clock();
    for (int c = 0; c < image.channels(); ++c) {
        int *qtable = qtable_L;
        if(c > 0) {
            qtable = qtable_C;
        }
        blkProc(buf[c], blk::dequantize, qtable);
        blkProc(buf[c], blk::idct2);
        buf[c] += 128.0;    // Inverse DC level shift

        buf[c].convertTo(ycrcb[c], ycrcb[c].type());
        if(c > 0) {
            cv::resize(ycrcb[c], ycrcb[c], cv::Size(), 2, 2, cv::INTER_LINEAR_EXACT);
        }
    }
    prcJpgStm.proc_decoder = clock() - prcJpgStm.start_time;

    cv::merge(ycrcb, image);
    cv::cvtColor(image, image, cv::COLOR_YCrCb2BGR);

    /**
    // psnr
    if(PSNR)
        psnr(orgn, image);
    **/
    prcJpgStm.func_procJpg = clock() - prcJpgStm.procJpg_start_time;
    printf("createQtable:%lf\n", (double)prcJpgStm.func_createQtable/CLOCKS_PER_SEC);
    printf("create_mainheader:%lf\n", (double)prcJpgStm.func_create_main_header/CLOCKS_PER_SEC);
    printf("encoder:%lf\n", (double)prcJpgStm.proc_encoder/CLOCKS_PER_SEC);
    printf("decoder:%lf\n", (double)prcJpgStm.proc_decoder/CLOCKS_PER_SEC);
    printf("ttl:%lf\n", (double)prcJpgStm.func_procJpg/CLOCKS_PER_SEC);
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
    if(strcmp(color, "rgb") == 0) {
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
