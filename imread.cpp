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

cv::Mat checkered_flag(cv::Mat data) {
    bool fill = true;
    int x = data.cols, y = data.rows;
    int euc = euclidean_alg(x, y);

    for(int y = 0; y < data.rows; ++y) {
        for(int x = 0; x < data.cols; ++x) {
            if(fill == true) {
                int val = data.data[y * data.cols + x];       // x, y座標を指定して画素を取得．ストライド．
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
        if(y % euc >= 31) {
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
    int a = 0, b = 0, tmp = 0;
    int x = data.cols, y = data.rows;

    a = x;
    b = y;
    while((tmp = a % b) != 0) {
        a = b;
        b = tmp;
    }

    for(int y = 0; y < data.rows; ++y) {
        for(int x = 0; x < data.cols; ++x) {
            if(fill == true) {
                int val = data.data[y * data.cols + x];       // x, y座標を指定して画素を取得．ストライド．
                val = 0;
                data.data[y * data.cols + x] = val;
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

    return data;
}

cv::Mat blacken_upper_left_corner(cv::Mat data) {
    for(int y = 0; y < data.rows; ++y) {
        for(int x = 0; x < data.cols; ++x) {
            if(x < data.cols / 2 && y < data.rows / 2) {
                int val = data.data[y * data.cols + x];       // x, y座標を指定して画素を取得．ストライド．
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
    if(input.empty()) {
        printf("Image file is not found.\n");
        return EXIT_FAILURE;
    }
    printf("width=%d, height=%d\n", input.cols, input.rows);
    cv::imshow("input_data", input);

    while(i < 3) {
        data = cv::imread(f_path, cv::ImreadModes::IMREAD_GRAYSCALE);
        switch(i) {
            case 0:
                image[0] = checkered_flag(data);
                cv::imshow("checkered_flag", image[0]);
                break;
            case 1:
                image[1] = zebra(data);
                cv::imshow("zebra", image[1]);
                break;
            case 2:
                image[2] = blacken_upper_left_corner(data);
                cv::imshow("blacken_upper_left_corner", image[2]);
               break; 
        }
        i++;
    }
    //image = zebra(input);
    //image = blacken_upper_left_corner(input);
    cv::waitKey();
    cv::destroyAllWindows();

    return EXIT_SUCCESS;
}
