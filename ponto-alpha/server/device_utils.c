//
// Created by root on 09/10/18.
//

//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include "utils.h"
//#include <libfprint/fprint.h>

//int verify(struct fp_print_data *data)
//{
//    int r;
//    int result = -1;
//    ///*Iniciando device*///
//    struct fp_dscv_dev *ddev;
//    struct fp_dscv_dev **discovered_devs;
//    struct fp_dev *dev;
//
//    r = fp_init();
//
//    if (r < 0) {
//        fprintf(stderr, "Failed to initialize libfprint\n");
//        compose_json_answer("SCREEN_UPDATE", "ERROR", "verify", "Falha ao inicializar a libfprint", "");
//        return 1;
//    }
//
//    fp_set_debug(3);
//
//    discovered_devs = fp_discover_devs();
//    if (!discovered_devs) {
//        fprintf(stderr, "Could not discover devices\n");
//        compose_json_answer("SCREEN_UPDATE", "ERROR", "verify", "Falha ao descobrir dispositivos", "");
//        return 1;
//    }
//    ddev = discover_device(discovered_devs);
//    if (!ddev) {
//        fprintf(stderr, "No devices detected.\n");
//        compose_json_answer("SCREEN_UPDATE", "ERROR", "verify", "Nenhum dispositivo encontrado", "");
//        return 1;
//    }
//    dev = fp_dev_open(ddev);
//    fp_dscv_devs_free(discovered_devs);
//    if (!dev) {
//        fprintf(stderr, "Could not open device.\n");
//        compose_json_answer("SCREEN_UPDATE", "ERROR", "verify", "Falha ao abrir dispositivo", "");
//        return 1;
//    }
//
//    printf("Opened device. It's now time to enroll your finger.\n");
//    compose_json_answer("SCREEN_UPDATE", "SUCCESS", "verify", "Dispositivo inicializado. Verifique sua digital.", "");
//
//    ///*Fim inicialização device*///
//
//    do {
//        printf("\nScan your finger now.\n");
//
//        r = fp_verify_finger(dev, data);
//
//        ///*Encerrando device*///
//        fp_dev_close(dev);
//        fp_exit();
//
//        if (r < 0) {
//            printf("verification failed with error %d :(\n", r);
//            return r;
//        }
//        switch (r) {
//            case FP_VERIFY_NO_MATCH:
//                printf("NO MATCH!\n");
//                return 0;
//                break;
//            case FP_VERIFY_MATCH:
//                printf("MATCH!\n");
//                return 1;
//                break;
//            case FP_VERIFY_RETRY:
//                printf("Scan didn't quite work. Please try again.\n");
//                break;
//            case FP_VERIFY_RETRY_TOO_SHORT:
//                printf("Swipe was too short, please try again.\n");
//                break;
//            case FP_VERIFY_RETRY_CENTER_FINGER:
//                printf("Please center your finger on the sensor and try again.\n");
//                break;
//            case FP_VERIFY_RETRY_REMOVE_FINGER:
//                printf("Please remove finger from the sensor and try again.\n");
//                break;
//        }
//    } while (1);
//
//
//}
