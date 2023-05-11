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
  image = cv::imread(f_path);
  if(image.empty()) {
    printf("Image file is not found.\n");
    return EXIT_FAILURE;
  }
  printf("width=%d, height=%d\n", image.cols, image.rows);
  cv::imshow("image", image);
  cv::waitKey();
  cv::destroyAllWindows();

  return EXIT_SUCCESS;
}
