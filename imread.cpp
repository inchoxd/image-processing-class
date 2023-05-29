#include <cstdio>
#include <getopt.h>

#include "tools.hpp"

int main(int argc, char *argv[]) {
    int i, opt;
    const char *optstr = "f:c:d:p:";
    char *f_path = NULL;
    char *color = NULL;
    char *display = NULL;
    char *disp_ptn = NULL;

    cv::Mat input;
    while((opt = getopt(argc, argv, optstr)) != -1) {
        switch(opt) {
            case 'f':
                f_path = optarg;
                break;

            case 'c':
                color = optarg;
                break;

            case 'd':
                display = optarg;
                break;

            case 'p':
                disp_ptn = optarg;
                break;
        }
    }

    for(i = optind; i < argc; i++) {
        if(f_path == NULL) {
            f_path = argv[i];
        }
    }

    if(color != NULL) {
        input = inputImg(f_path, color);
    } else {
        input = inputImg(f_path, "color");
    }

    if(disp_ptn != NULL) {
        if(display == NULL) {
            display = "single";
        }
        if(strcmp(display, "single") > 0 || strcmp(display, "single") < 0) {
            errorExt(display);
        }
        imgPattern(disp_ptn, input);
    }

    if(display != NULL) {
        checkDisplayType(display, input);
    } else {
        checkDisplayType("single", input);
    }
    
    return EXIT_SUCCESS;
}
