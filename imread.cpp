#include <cstdio>
#include <getopt.h>
#include <string.h>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

typedef struct {
    int B;
    int G;
    int R;
} BGR;

typedef struct {
    int Y;
    int Cb;
    int Cr;
} YCbCr;

int cvtYCbCr(cv::Mat &data) {
    const int w = data.cols, h = data.rows, NC = data.channels();
    if(NC < 3)
        return 1;

    for (int y = 0; y < h; ++y) {
        const int STRIDE = w * NC;
        for (int x = 0; x < w; ++x) {
            double out[3] = {0.0, 0.0, 0.0};
            double wY[3] = {0.114, 0.587, 0.299};
            double wCb[3] = {0.5, -.3313, -.1687};
            double wCr[3] = {-.0813, -.4187, 0.5};
            for (int c = 0; c < NC; ++c) {
                out[0] += data.data[y * STRIDE + x * NC + c] * wY[c];
                out[1] += data.data[y * STRIDE + x * NC + c] * wCb[c];
                out[2] += data.data[y * STRIDE + x * NC + c] * wCr[c];
            }
            for (int c = 0; c < NC; ++c) {
                int val = (c != 0) ? 128 : 0;
                data.data[y * STRIDE + x * NC + c] = out[c] + val;
            }
        }
    }

    return 0;
}

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

void errorExt(char *arg) {
    printf("%s is an option that does not exist.\n", arg);
    exit(1);
}

void imgSimple(cv::Mat data) {
    cv::imshow("results", data);
    cv::waitKey();
    cv::destroyAllWindows();
}

void checkDisplayType(char *display, cv::Mat input) {
    if(strcmp("tiled", display) == 0) {
        imgTiled(input);
    } else if(strcmp("single", display) == 0) {
        imgSimple(input);
    } else {
        errorExt(display);
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

int main(int argc, char *argv[]) {
    int opt;
    const char *optstr = "f:c:d:";
    char *f_path = NULL;
    char *color = NULL;
    char *display = NULL;

    cv::Mat input;
    while((opt = getopt(argc, argv, optstr)) != -1) {
        switch(opt) {
            case 'f':
                if(optarg) {
                    f_path = optarg;
                }
                if(color != NULL) {
                    input = inputImg(f_path, color);
                }
                break;

            case 'c':
                color = optarg;
                if(f_path == NULL) {
                    continue;
                }
                if(optarg) {
                    input = inputImg(f_path, optarg);
                }

                if(input.empty()) {
                    printf("Image file is not found.\n");
                    return EXIT_FAILURE;
                }

                if(display != NULL) {
                    checkDisplayType(display, input);
                }

                break;

            case 'd':
                display = optarg;
                if(f_path == NULL) {
                    continue;
                }
                if(color == NULL) {
                    continue;
                }

                if(optarg) {
                    checkDisplayType(optarg, input);
                }
                break;
        }
    }

    if(color == NULL) {
        input = inputImg(f_path, "color");
    }
    if(display == NULL) {
        checkDisplayType("single", input);
    }
    
    return EXIT_SUCCESS;
}
