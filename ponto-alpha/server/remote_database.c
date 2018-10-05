//
// Created by leticia on 14/09/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
#include <curl/curl.h>
#include "structs.h"

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

int deal_with_json(char* json_str, struct user_list *list){

        int val_type, i;
        int number_of_digitals = 0;
        char *val_type_str, *str;

        json_object * jobj = json_tokener_parse(json_str);
        enum json_type type;

        int length_array = json_object_array_length(jobj);

        json_object * jUobject;

        for (int i = 0; i < length_array; i++){
                jUobject = json_object_array_get_idx(jobj, i);

                //Writing null to the fingerprint attribute to make sure the .fingerprint attribute isn't ever null
                list[i].fingerprint =(char *)malloc(strlen(""));
                list[i].fingerprint = "";

                json_object_object_foreach(jUobject, key, val) {
                        //To read the value of a new field, just copy the following structure
                        //and you should be good to go.
                        if (strcmp(key, "usuarioId") == 0){
                                list[i].user_id = json_object_get_int(val);
                        }
                        if (strcmp(key, "nome") == 0){
                                list[i].name = (char *)malloc(json_object_get_string_len(val)+1);
                                strcpy(list[i].name, json_object_get_string(val));
                        }
                        if (strcmp(key, "perfil") == 0){
                                list[i].role = (char *)malloc(json_object_get_string_len(val)+1);
                                strcpy(list[i].role, json_object_get_string(val));
                        }
                        if (strcmp(key, "digital") == 0){
                                list[i].fingerprint = (char *)malloc(json_object_get_string_len(val)+1);
                                strcpy(list[i].fingerprint, json_object_get_string(val));
                                if (strcmp(list[i].fingerprint, "") != 0){
                                        number_of_digitals++;
                                }
                        }
                }
        }

        return number_of_digitals;
}
