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
                printf("\t-c\tchoose color type: color, mono, ycrcb. default is color.\n\t-d\tchoose display type: single, tiled. default is single.\n\t-f\tinput filepath\n\t-p\tchoose pattern: ulc, zebra, checkerd, mozaic\n\t-q\tinput range from 0 to 100. compresses image use jpeg.");
                return EXIT_SUCCESS;
        }
    }

    if(argc < 2) {
        printf("An input image file is missing. File path before options or using -f option. Running bin/imread -h and check options recommend.\n");
        return EXIT_FAILURE;
    } else {
        if((argc - optind) > 0) {
            for(i = optind; i < argc; i++) {
                if(f_path == NULL && (argc - optind) < 2) {
                    f_path = argv[i];
                } else {
                    printf("too many filepath. filepath may input up to one. Running bin/imread -h and check options recommend.\n");
                    return EXIT_FAILURE;
                }
            }
        } else {
            if(f_path == NULL) {
                return EXIT_FAILURE;
            }
        }
    }

    if(color != NULL) {
        input = inputImg(f_path, color);
    } else {
        color = "color";
        input = inputImg(f_path, color);
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
        if(strcmp(color, "mono") == 0) {
            errorExt(color);
        }
        int QF = strtol(quarity, nullptr, 10);  // Quarity Factor
        if(QF < 0 || QF > 100) {
            printf("Valid range for Qfactor is from 0 to 100\n");
            return EXIT_FAILURE;
        }
        procJpg(input, QF);
    }

    if(display != NULL) {
        checkDisplayType(display, input);
    } else {
        checkDisplayType("single", input);
    }
    
    return EXIT_SUCCESS;
}
