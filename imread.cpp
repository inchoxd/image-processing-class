#include <cstdio>
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

int cvt_YCbCr(cv::Mat data) {
    const int w = data.cols, h = data.rows, NC = data.channels();
    if(NC < 3)
        return 1;


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

void checkered_flag(cv::Mat data) {
    const int w = data.cols, h = data.rows, NC = data.channels();
    bool fill = true;
    int gcm_val = gcm(w, h);

    for(int y = 0; y < data.rows; ++y) {
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

void zebra(cv::Mat data) {
    const int w = data.cols, h = data.rows, NC = data.channels();
    bool fill = true;
    int gcm_val = gcm(w, h);

    for(int y = 0; y < w; ++y) {
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

void blacken_upper_left_corner(cv::Mat data) {
    const int w = data.cols, NC = data.channels();

    for(int y = 0; y < data.rows / 2; ++y) {
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

int main(int argc, char *argv[]) {
    int i = 0;
    const char *f_path = argv[1];
    const char *arg_mode = argv[2];

    if(f_path == NULL) {
        printf("input file_path >>");
        scanf("%s", f_path);
    }

    cv::Mat input, data, rst, image[3];
    if(arg_mode != NULL && strcmp(arg_mode, "color") == 0) {
        input = cv::imread(f_path, cv::ImreadModes::IMREAD_COLOR);
    } else {
        input = cv::imread(f_path, cv::ImreadModes::IMREAD_GRAYSCALE);
    }

    for(i = 0; i < 3; i++) {
        input.copyTo(image[i]);
    }

    if(input.empty()) {
        printf("Image file is not found.\n");
        return EXIT_FAILURE;
    }

    cv::Mat tiled(input.rows * 2, input.cols * 2, input.type());
    const int width = input.cols, height = input.rows;
    printf("width=%d, height=%d\n", width, height);
    input.copyTo(tiled(cv::Rect(0, 0, width, height)));

    for(i = 0; i < 3; i++) {
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

    return EXIT_SUCCESS;
}
