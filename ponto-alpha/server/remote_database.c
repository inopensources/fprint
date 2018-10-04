//
// Created by leticia on 14/09/18.
//

#include "user.h"
#include "curl_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
        strncat(result, requestBody1);
        strcat(result, userIdAsStr);
        strcat(result, requestBody2);
        strcat(result, digital);
        strcat(result, requestBody3);
        strcat(result, tamanhoArrayAsStr);
        strcat(result, requestBody4);

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
}