#include <cstdio>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

int main(int argc, char *argv[]) {
  const char *f_path = argv[1];
  if(argc < 2 && f_path == NULL) {
      printf("input file_path >>");
      scanf("%s", f_path);
  }

  cv::Mat image;
  image = cv::imread(f_path, cv::ImreadModes::IMREAD_GRAYSCALE);
  if(image.empty()) {
    printf("Image file is not found.\n");
    return EXIT_FAILURE;
  }
  printf("width=%d, height=%d\n", image.cols, image.rows);
  for(int y = 0; y < image.rows; ++y) {
      for(int x = 0; x < image.cols; ++x) {
          if(x < image.cols / 2 && y < image.rows / 2) {
              int val = image.data[y * image.cols + x];       // x, y座標を指定して画素を取得．ストライド．
              val = 0;
              image.data[y * image.cols + x] = val;
          }
      }
  }
  cv::imshow("image", image);
  cv::waitKey();
  cv::destroyAllWindows();

  return EXIT_SUCCESS;
}
