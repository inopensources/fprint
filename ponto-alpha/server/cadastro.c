//
// Created by leticia on 12/09/18.
//

#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"
#include "remote_database.h"
#include "device_utils.h"

struct fp_print_data *enroll(struct fp_dev *dev);

struct fp_dscv_dev *discover_device(struct fp_dscv_dev **discovered_devs)
{
    struct fp_dscv_dev *ddev = discovered_devs[0];
    struct fp_driver *drv;
    if (!ddev)
        return NULL;

    drv = fp_dscv_dev_get_driver(ddev);
    printf("Found device claimed by %s driver\n", fp_driver_get_full_name(drv));
    return ddev;
}

int verify(struct fp_print_data *data)
{
    int r;
    int result = -1;
    ///*Iniciando device*///
    struct fp_dscv_dev *ddev;
    struct fp_dscv_dev **discovered_devs;
    struct fp_dev *dev;

    r = fp_init();

    if (r < 0) {
        fprintf(stderr, "Failed to initialize libfprint\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "verify", "Falha ao inicializar a libfprint", "");
        return 1;
    }

    fp_set_debug(3);

    discovered_devs = fp_discover_devs();
    if (!discovered_devs) {
        fprintf(stderr, "Could not discover devices\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "verify", "Falha ao descobrir dispositivos", "");
        return 1;
    }
    ddev = discover_device(discovered_devs);
    if (!ddev) {
        fprintf(stderr, "No devices detected.\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "verify", "Nenhum dispositivo encontrado", "");
        return 1;
    }
    dev = fp_dev_open(ddev);
    fp_dscv_devs_free(discovered_devs);
    if (!dev) {
        fprintf(stderr, "Could not open device.\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "verify", "Falha ao abrir dispositivo", "");
        return 1;
    }

    printf("Opened device. It's now time to enroll your finger.\n");


    ///*Fim inicialização device*///

    do {
        printf("\nScan your finger now.\n");
        compose_json_answer("SCREEN_UPDATE", "SUCCESS", "verify", "Por favor, posicione sua digital", "");
        r = fp_verify_finger(dev, data);

        ///*Encerrando device*///
        fp_dev_close(dev);
        fp_exit();

        if (r < 0) {
            printf("verification failed with error %d :(\n", r);
            return r;
        }
        switch (r) {
            case FP_VERIFY_NO_MATCH:
                printf("NO MATCH!\n");
                return 0;
                break;
            case FP_VERIFY_MATCH:
                printf("MATCH!\n");
                return 1;
                break;
            case FP_VERIFY_RETRY:
                printf("Scan didn't quite work. Please try again.\n");
                break;
            case FP_VERIFY_RETRY_TOO_SHORT:
                printf("Swipe was too short, please try again.\n");
                break;
            case FP_VERIFY_RETRY_CENTER_FINGER:
                printf("Please center your finger on the sensor and try again.\n");
                break;
            case FP_VERIFY_RETRY_REMOVE_FINGER:
                printf("Please remove finger from the sensor and try again.\n");
                break;
        }
    } while (1);


}


/*De cadastro.c*/
int cadastra_user(int user_id){

    ///*Iniciando device*///
    compose_json_answer("SCREEN_UPDATE", "SUCCESS", "cadastra_user", "Iniciando dispositivo de leitura. Por favor, aguarde um pouco", "");

    int r = 1;
    struct fp_dscv_dev *ddev;
    struct fp_dscv_dev **discovered_devs;
    struct fp_dev *dev;
    unsigned char *ret;
    struct fp_print_data *data;

    r = fp_init();

    if (r < 0) {
        fprintf(stderr, "Failed to initialize libfprint\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "cadastra_user", "Falha ao inicializar a libfprint", "");
        return 1;
    }

    fp_set_debug(3);

    discovered_devs = fp_discover_devs();
    if (!discovered_devs) {
        fprintf(stderr, "Could not discover devices\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "cadastra_user", "Falha ao descobrir dispositivos", "");
        return 1;
    }
    ddev = discover_device(discovered_devs);
    if (!ddev) {
        fprintf(stderr, "No devices detected.\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "cadastra_user", "Nenhum dispositivo encontrado", "");
        return 1;
    }
    dev = fp_dev_open(ddev);
    fp_dscv_devs_free(discovered_devs);
    if (!dev) {
        fprintf(stderr, "Could not open device.\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "cadastra_user", "Falha ao abrir dispositivo", "");
        return 1;
    }

    printf("Opened device. It's now time to enroll your finger.\n");
//    compose_json_answer("SCREEN_UPDATE", "SUCCESS", "cadastra_user", "Posicione sua digital.", "");

    ///*Fim inicialização device*///

    data = enroll(dev);
    int length = fp_print_data_get_data(data, &ret);

    int length_dig = 0;
    int i;
    int ret_temp;

    length_dig = get_length_digital(ret, length);
    printf("length_dig %d\n", length_dig);
    char digital[length_dig];
    fprint_to_string(ret, length, digital);

    //todo: modularizar verificação em um só método
//    compose_json_answer("SCREEN_UPDATE", "SUCCESS", "verify", "verificação agora", "");


    ///*Encerrando device*///
    fp_dev_close(dev);
    fp_exit();

    printf("\nVerifing..\n");
    sleep(2);

    int trying = 1;
    int result_verify = -22; //iniciando resultado com erro

    while(result_verify  == -22 && trying < 5){
        result_verify = verify(data);
        printf("\n trying: %d, result_verify : %d\n", trying, result_verify);
        trying +=1;
    }


    if(result_verify == 0){
        ///don't match
        compose_json_answer("SCREEN_UPDATE", "ERROR", "verify", "Sua verificação falhou :( <br> Realize o cadastro novamente certificando-se de posicionar sua digital sempre da mesma forma, ok?", "");
    }else{
        ///match
        post_user(user_id, digital, length_dig);
        compose_json_answer("SCREEN_UPDATE", "SUCCESS", "verify_final", "Seu cadastro foi realizado com sucesso :)", "");

    }

    return 0;
}

struct fp_print_data *enroll(struct fp_dev *dev) {

    struct fp_print_data *enrolled_print = NULL;
    int r;


    printf("You will need to successfully scan your finger %d times to "
           "complete the process.\n", fp_dev_get_nr_enroll_stages(dev));

    compose_json_answer("SCREEN_UPDATE", "SUCCESS", "enroll", "Você precisará escanear sua digital 5 vezes para realizar o cadastro", "");
    sleep(2);
    do {

        sleep(1);

        printf("\nScan your finger now.\n");
        compose_json_answer("SCREEN_UPDATE", "SUCCESS", "enroll", "Por favor, posicione sua digital no sensor", "");


        r = fp_enroll_finger(dev, &enrolled_print);
        if (r < 0) {
            printf("Enroll failed with error %d\n", r);
            compose_json_answer("SCREEN_UPDATE", "ERROR", "enroll", "Opa, houve uma falha ao escanear. Tente mais um vez.", "");
            return NULL;
        }
        switch (r) {
            case FP_ENROLL_COMPLETE:
                printf("Enroll completed!\n");
                compose_json_answer("SCREEN_UPDATE", "SUCCESS", "enroll_final", "Quase lá. Agora vamos testar se seu cadastro foi bem sucedido.", "");
                break;
            case FP_ENROLL_FAIL:
                printf("Enroll failed, something wen't wrong :(\n");
                compose_json_answer("SCREEN_UPDATE", "ERROR", "enroll", "Falha ao cadastrar.", "");
                return NULL;
            case FP_ENROLL_PASS:
                printf("Enroll stage passed. Yay!\n");
                compose_json_answer("SCREEN_UPDATE", "SUCCESS", "enroll", "Captura de digital bem sucedida", "");
                break;
            case FP_ENROLL_RETRY:
                printf("Didn't quite catch that. Please try again.\n");
                compose_json_answer("SCREEN_UPDATE", "ERROR", "enroll", "Houve uma falha ao capturar digital. Por favor, tente novamente.", "");
                break;
            case FP_ENROLL_RETRY_TOO_SHORT:
                printf("Your swipe was too short, please try again.\n");
                compose_json_answer("SCREEN_UPDATE", "ERROR", "enroll", "Você deixou sua digital por pouco tempo. Por favor, tente novamente.", "");
                break;
            case FP_ENROLL_RETRY_CENTER_FINGER:
                printf("Didn't catch that, please center your finger on the "
                       "sensor and try again.\n");
                compose_json_answer("SCREEN_UPDATE", "ERROR", "enroll", "Falha ao capturar digital. Por favor, posicione seu dedo sobre o sensor e tente novamente.", "");
                break;
            case FP_ENROLL_RETRY_REMOVE_FINGER:
                printf("Scan failed, please remove your finger and then try "
                       "again.\n");
                compose_json_answer("SCREEN_UPDATE", "ERROR", "enroll", "Falha ao capturar digital. Por favor, remova seu dedo e tente novamente.", "");
                break;
        }
    } while (r != FP_ENROLL_COMPLETE);

    if (!enrolled_print) {
        fprintf(stderr, "Enroll complete but no print?\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "enroll", "Falha ao cadastrar.", "");
        return NULL;
    }

    return enrolled_print;
}
