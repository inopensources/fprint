//
// Created by leticia on 12/09/18.
//

#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "remote_database.h"
#include "device_utils.h"
#include "utils.h"



void cadastra_user(){

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

    printf("Opened device. It's now time to enroll your finger.\n");

    ///*Fim inicialização device*///

    data = enroll(dev);
    int length = fp_print_data_get_data(data, &ret);
    post_user(76, fprint_to_string(ret, length), length);

    ///*Encerrando device*///
    out_close:
    fp_dev_close(dev);
    out:
    fp_exit();
    return r;

}