//
// Created by leticia on 12/09/18.
//

#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>
#include "utils.h"


struct fp_print_data *enroll(struct fp_dev *dev);

void post_user(int id_usuario, char digital[], int tamanho_array);

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

/*De cadastro.c*/
int cadastra_user(int user_id){

    ///*Iniciando device*///
    compose_json_answer("SCREEN_UPDATE", "SUCCESS", "cadastra_user", "Iniciando dispositivo de leitura.", "");

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
    compose_json_answer("SCREEN_UPDATE", "SUCCESS", "cadastra_user", "Dispositivo inicializado. Cadastre sua digital.", "");

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
    post_user(user_id, digital, length_dig);

    ///*Encerrando device*///
    fp_dev_close(dev);
    return 0;
}

struct fp_print_data *enroll(struct fp_dev *dev) {

    struct fp_print_data *enrolled_print = NULL;
    int r;


    printf("You will need to successfully scan your finger %d times to "
           "complete the process.\n", fp_dev_get_nr_enroll_stages(dev));

    compose_json_answer("SCREEN_UPDATE", "SUCCESS", "enroll", "Você precisará escanear sua digital 5 vezes para concluir o processo.", "");

    do {

        sleep(1);

        printf("\nScan your finger now.\n");
        compose_json_answer("SCREEN_UPDATE", "SUCCESS", "enroll", "Escaneie seu dedo agora.", "");


        r = fp_enroll_finger(dev, &enrolled_print);
        if (r < 0) {
            printf("Enroll failed with error %d\n", r);
            compose_json_answer("SCREEN_UPDATE", "ERROR", "enroll", "Falha ao escanear.", "");
            return NULL;
        }
        switch (r) {
            case FP_ENROLL_COMPLETE:
                printf("Enroll completed!\n");
                compose_json_answer("SCREEN_UPDATE", "SUCCESS", "enroll", "Cadastro concluído com sucesso.", "");
                break;
            case FP_ENROLL_FAIL:
                printf("Enroll failed, something wen't wrong :(\n");
                compose_json_answer("SCREEN_UPDATE", "ERROR", "enroll", "Falha ao cadastrar.", "");
                return NULL;
            case FP_ENROLL_PASS:
                printf("Enroll stage passed. Yay!\n");
                compose_json_answer("SCREEN_UPDATE", "SUCCESS", "enroll", "Captura bem sucedida.", "");
                break;
            case FP_ENROLL_RETRY:
                printf("Didn't quite catch that. Please try again.\n");
                compose_json_answer("SCREEN_UPDATE", "ERROR", "enroll", "Falha ao capturar digital. Por favor, tente novamente.", "");
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

void post_user(int id_usuario, char digital[], int tamanho_array){

    char url[] = "http://licenca.infarma.com.br/ponto/cadastro_digital";
    // char url[] = "http://192.168.16.111:8080/ponto/cadastro_digital";

    char requestBody1[] = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"usuarioId\"\r\n\r\n";
    char requestBody2[] = "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"digital\"\r\n\r\n";
    char requestBody3[] = "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"tamanhoArray\"\r\n\r\n";
    char requestBody4[] = "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--";

    char userIdAsStr[12];
    sprintf(userIdAsStr, "%d", id_usuario);

    char tamanhoArrayAsStr[12];
    sprintf(tamanhoArrayAsStr, "%d", tamanho_array);

    char *result = calloc(strlen(requestBody1) + strlen(userIdAsStr) + strlen(requestBody2) + strlen(digital) + strlen(requestBody3) + strlen(tamanhoArrayAsStr) + strlen(requestBody4) + 1, sizeof(char));
    strcat(result, requestBody1);
    strcat(result, userIdAsStr);
    strcat(result, requestBody2);
    strcat(result, digital);
    strcat(result, requestBody3);
    strcat(result, tamanhoArrayAsStr);
    strcat(result, requestBody4);

    printf("\n\n\nKKKK: %s", digital);

    CURL *hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(hnd, CURLOPT_URL, url);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Postman-Token: 6b970eb6-28cd-4f90-8703-cfea648b0c79");
    headers = curl_slist_append(headers, "Cache-Control: no-cache");
    headers = curl_slist_append(headers, "Authorization: Basic VVNFUlRFU1RFOjEyMzQ1");
    headers = curl_slist_append(headers, "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(hnd, CURLOPT_POSTFIELDS, result);

    CURLcode ret = curl_easy_perform(hnd);

    free(result);

}