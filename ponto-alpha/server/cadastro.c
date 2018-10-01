//
// Created by leticia on 12/09/18.
//

#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

struct fp_print_data *enroll(struct fp_dev *dev);
char *fprint_to_string(char * ret, int length);
void post_user(int id_usuario, char* digital, int tamanho_array);


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

char * fprint_to_string(char * ret, int length){
    int i;
    char *digital = calloc(4*length, sizeof(char));
    for (i = 0; i<length; i++)
    {
        char digital2[50];
        if (i == 0){
            sprintf(digital2, "[%d, ", (int)(*(ret+i)));
        }else if(i == length-1 ){
            sprintf(digital2, "%d] ", (int)(*(ret+i)));
        }else{
            sprintf(digital2, "%d, ", (int)(*(ret+i)));
        }
        strncat(digital, digital2, strlen(digital2));
    }
    return digital;

}


/*De cadastro.c*/
void cadastra_user(int user_id){

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
    post_user(user_id, fprint_to_string(ret, length), length);

    ///*Encerrando device*///
    out_close:
    fp_dev_close(dev);
    out:
    fp_exit();
    return r;

}

struct fp_print_data *enroll(struct fp_dev *dev) {

    struct fp_print_data *enrolled_print = NULL;
    int r;


    printf("You will need to successfully scan your finger %d times to "
           "complete the process.\n", fp_dev_get_nr_enroll_stages(dev));

    do {

        sleep(1);

        printf("\nScan your finger now.\n");


        r = fp_enroll_finger(dev, &enrolled_print);
        if (r < 0) {
            printf("Enroll failed with error %d\n", r);
            return NULL;
        }
        switch (r) {
            case FP_ENROLL_COMPLETE:
                printf("Enroll completed!\n");
                break;
            case FP_ENROLL_FAIL:
                printf("Enroll failed, something wen't wrong :(\n");
                return NULL;
            case FP_ENROLL_PASS:
                printf("Enroll stage passed. Yay!\n");
                break;
            case FP_ENROLL_RETRY:
                printf("Didn't quite catch that. Please try again.\n");
                break;
            case FP_ENROLL_RETRY_TOO_SHORT:
                printf("Your swipe was too short, please try again.\n");
                break;
            case FP_ENROLL_RETRY_CENTER_FINGER:
                printf("Didn't catch that, please center your finger on the "
                       "sensor and try again.\n");
                break;
            case FP_ENROLL_RETRY_REMOVE_FINGER:
                printf("Scan failed, please remove your finger and then try "
                       "again.\n");
                break;
        }
    } while (r != FP_ENROLL_COMPLETE);

    if (!enrolled_print) {
        fprintf(stderr, "Enroll complete but no print?\n");
        return NULL;
    }

    return enrolled_print;
}


void post_user(int id_usuario, char* digital, int tamanho_array){

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

    char *result = malloc(strlen(requestBody1) + strlen(userIdAsStr) + strlen(requestBody2) + strlen(digital) + strlen(requestBody3) + strlen(tamanhoArrayAsStr) + strlen(requestBody4) + 1);
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

    return 0;
}