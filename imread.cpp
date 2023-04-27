#include <cstdio>
#include <opencv2/opencv.hpp>

int main(int argc, char *argv[]) {
  const char *f_path = argv[1];
  if(f_path == NULL)
      f_path = "barbara.pgm";

  cv::Mat image;
  image = cv::imread(f_path);
  if (image.empty()) {
    printf("Image file is not found.\n");
    return EXIT_FAILURE;
  }
  cv::imshow("image", image);
  cv::waitKey();
  cv::destroyAllWindows();

  return EXIT_SUCCESS;
}
