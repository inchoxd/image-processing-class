#include <cstdio>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

cv::Mat zebra(cv::Mat input) {
    bool fill = true;
    int a = 0, b = 0, tmp = 0;
    int x = input.cols, y = input.rows;

    a = x;
    b = y;
    while((tmp = a % b) != 0) {
        a = b;
        b = tmp;
    }

    for(int y = 0; y < input.rows; ++y) {
        for(int x = 0; x < input.cols; ++x) {
            if(fill == true) {
                int val = input.data[y * input.cols + x];       // x, y座標を指定して画素を取得．ストライド．
                val = 0;
                input.data[y * input.cols + x] = val;
            }
            if(x % b == 0) {
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
    image = zebra(data);
    //image = blacken_upper_left_corner(data);
    cv::imshow("image", data);
    cv::waitKey();
    cv::destroyAllWindows();

    return EXIT_SUCCESS;
}
