#include <cstdio>
#include <cstdlib>
#include <getopt.h>

#include "tools.hpp"

int main(int argc, char *argv[]) {
    int i, opt;
    const char *optstr = "f:c:d:p:q:h";
    char *f_path = NULL;
    char *color = NULL;
    char *display = NULL;
    char *quarity = NULL;
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

            case 'q':
                quarity = optarg;
                break;

            case 'h':
                printf("\t-c\tchoose color: color, mono, ycrcb\n\t-d\tchoose display type: single, tiled\n\t-f\tiput file path\n\t-p\tchoose pattern: ulc, zebra, checkerd, mozaic\n");
                return EXIT_SUCCESS;
        }
    }

    if(argc < 2) {
        printf("An input image file is missing. File path before options or using -f option. Running bin/imread -h and check options recommend.\n");
        return EXIT_FAILURE;
    } else {
        if(optind < argc) {
            for(i = optind; i < argc; i++) {
                if(f_path == NULL) {
                    f_path = argv[i];
                }
            }
        } else {
            printf("An input image file is missing. File path before options or using -f option. Running bin/imread -h and check options recommend.\n");
            return EXIT_FAILURE;
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

    if(quarity != NULL) {
        if(display == NULL) {
            display = "single";
        }
        if(strcmp(display, "single") > 0 || strcmp(display, "single") < 0) {
            errorExt(display);
        }
    }

    if(display != NULL) {
        checkDisplayType(display, input);
    } else {
        checkDisplayType("single", input);
    }
    
    return EXIT_SUCCESS;
}
