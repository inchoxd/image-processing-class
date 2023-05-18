#include <cstdio>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

int euclidean_alg(int a, int b) {
    int r = 0;
    while((r = a % b) != 0) {
        a = b;
        b = r;
    }

    return b;
}

cv::Mat checkered_flag(cv::Mat data) {
    bool fill = true;
    int x = data.cols, y = data.rows;
    int euc = euclidean_alg(x, y);

    for(int y = 0; y < data.rows; ++y) {
        for(int x = 0; x < data.cols; ++x) {
            if(fill == true) {
                int val = data.data[y * data.cols + x];
                val = 0;
                data.data[y * data.cols + x] = val;
            }
            if(x % euc == 0) {
                if(fill == true) {
                    fill = false;
                } else {
                    fill = true;
                }
            }
        }
        if(y % euc >= euc - 1) {
            if(fill == true) {
                fill = false;
            } else {
                fill = true;
            }
        }
    }

    return data;
}

cv::Mat zebra(cv::Mat data) {
    bool fill = true;
    int x = data.cols, y = data.rows;
    int euc = euclidean_alg(x, y);

    for(int y = 0; y < data.rows; ++y) {
        for(int x = 0; x < data.cols; ++x) {
            if(fill == true) {
                int val = data.data[y * data.cols + x];
                val = 0;
                data.data[y * data.cols + x] = val;
            }
            if(x % euc == 0) {
                if(fill == true) {
                    fill = false;
                } else {
                    fill = true;
                }
            }
        }
    }

    return data;
}

cv::Mat blacken_upper_left_corner(cv::Mat data) {
    for(int y = 0; y < data.rows; ++y) {
        for(int x = 0; x < data.cols; ++x) {
            if(x < data.cols / 2 && y < data.rows / 2) {
                int val = data.data[y * data.cols + x];       // x, y座標を指定して画素を取得．ストライドアクセス．
                val = 0;
                data.data[y * data.cols + x] = val;
            }
        }
    }

    return data;
}

int main(int argc, char *argv[]) {
    int i = 0;
    const char *f_path = argv[1];

    if(argc < 2 && f_path == NULL) {
        printf("input file_path >>");
        scanf("%s", f_path);
    }

    cv::Mat input, data, image[3];
    input = cv::imread(f_path, cv::ImreadModes::IMREAD_GRAYSCALE);
    cv::Mat tiled(input.rows * 2, input.cols * 2, input.type());
    if(input.empty()) {
        printf("Image file is not found.\n");
        return EXIT_FAILURE;
    }

    const int width = input.cols, height = input.rows;
    printf("width=%d, height=%d\n", width, height);
    input.copyTo(tiled(cv::Rect(0, 0, width, height)));

    for(; i < 3; i++) {
        data = cv::imread(f_path, cv::ImreadModes::IMREAD_GRAYSCALE);
        switch(i) {
            case 0:
                image[0] = blacken_upper_left_corner(data);
                image[0].copyTo(tiled(cv::Rect(width, 0, width, height)));
                break;
            case 1:
                image[1] = zebra(data);
                image[1].copyTo(tiled(cv::Rect(0, height, width, height)));
                break;
            case 2:
                image[2] = checkered_flag(data);
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
