//
// Created by leticia on 14/09/18.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "device_utils.h"
#include <libfprint/fprint.h>


void do_point(){

    ///*Iniciando device*///

    int r = 1;
    struct fp_dscv_dev *ddev;
    struct fp_dscv_dev **discovered_devs;
    struct fp_dev *dev;
    unsigned char *ret;
    struct fp_print_data *data;

    r = fp_init();

    if (r < 0) {
        fprintf(stderr, "Failed to initialize libfprint\n");
        exit(1);
    }

    fp_set_debug(3);

    discovered_devs = fp_discover_devs();
    if (!discovered_devs) {
        fprintf(stderr, "Could not discover devices\n");
        goto out;
    }
    ddev = discover_device(discovered_devs);
    if (!ddev) {
        fprintf(stderr, "No devices detected.\n");
        goto out;
    }
    dev = fp_dev_open(ddev);
    fp_dscv_devs_free(discovered_devs);
    if (!dev) {
        fprintf(stderr, "Could not open device.\n");
        goto out;
    }

    printf("Opened device. It's now time to enroll your finger!\n");

    ///Fim inicialização device


    ///*pegando digital de txt*///
    unsigned char *ret_from_file = read_digital();
    int length = 12050;

    //data = enroll(dev);
    int result = compare_digital(dev, ret_from_file, length, 1); //chamada em data.c

    out_close:
    fp_dev_close(dev);
    out:
    fp_exit();
    return r;


}
