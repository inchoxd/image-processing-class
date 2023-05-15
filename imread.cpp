#include <cstdio>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

int euclidean_alg(int a, int b) {
    int tmp = 0;
    while((tmp = a % b) != 0) {
        a = b;
        b = tmp;
    }
    return b;
}

cv::Mat checkered_flag(cv::Mat input) {
    bool fill = true;
    int x = input.cols, y = input.rows;
    int euc = euclidean_alg(x, y);

    for(int y = 0; y < input.rows; ++y) {
        for(int x = 0; x < input.cols; ++x) {
            if(fill == true) {
                int val = input.data[y * input.cols + x];       // x, y座標を指定して画素を取得．ストライド．
                val = 0;
                input.data[y * input.cols + x] = val;
            }
            if(x % euc == 0) {
                if(fill == true) {
                    fill = false;
                } else {
                    fill = true;
                }
            }
        }
        if(y % euc >= 31) {
            if(fill == true) {
                fill = false;
            } else {
                fill = true;
            }
        }
    }

    return input;
}

cv::Mat zebra(cv::Mat input) {
    bool fill = true;
    int x = input.cols, y = input.rows;
    int euc = euclidean_alg(x, y);

    for(int y = 0; y < input.rows; ++y) {
        for(int x = 0; x < input.cols; ++x) {
            if(fill == true) {
                int val = input.data[y * input.cols + x];       // x, y座標を指定して画素を取得．ストライド．
                val = 0;
                input.data[y * input.cols + x] = val;
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

    return input;
}

cv::Mat blacken_upper_left_corner(cv::Mat input) {
    for(int y = 0; y < input.rows; ++y) {
        for(int x = 0; x < input.cols; ++x) {
            if(x < input.cols / 2 && y < input.rows / 2) {
                int val = input.data[y * input.cols + x];       // x, y座標を指定して画素を取得．ストライド．
                val = 0;
                input.data[y * input.cols + x] = val;
            }
        }
    }

    return input;
}

int main(int argc, char *argv[]) {
    const char *f_path = argv[1];
    if(argc < 2 && f_path == NULL) {
        printf("input file_path >>");
        scanf("%s", f_path);
    }

    cv::Mat data, image;
    data = cv::imread(f_path, cv::ImreadModes::IMREAD_GRAYSCALE);
    if(data.empty()) {
        printf("Image file is not found.\n");
        return EXIT_FAILURE;
    }
    printf("width=%d, height=%d\n", data.cols, data.rows);
    image = checkered_flag(data);
    //image = zebra(data);
    //image = blacken_upper_left_corner(data);
    cv::imshow("image", data);
    cv::waitKey();
    cv::destroyAllWindows();

    return EXIT_SUCCESS;
}
