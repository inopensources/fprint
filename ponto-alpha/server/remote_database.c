//
// Created by leticia on 14/09/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <json-c/json.h>
#include <curl/curl.h>
#include "structs.h"
#include "utils.h"

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void post_user(int id_usuario, char digital[], int tamanho_array){

          char url[] = "http://licenca.infarma.com.br/ponto/cadastro_digital";
 //       char url[] = "http://192.168.16.111/ponto/cadastro_digital";
//         char url[] = "http://localhost:8080/ponto/cadastro_digital";

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

char *get_user_list_mini() {
    CURL *curl_handle;
    CURLcode res;
    char *buf2;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    char url[] = "http://licenca.infarma.com.br/ponto/lista_usuarios_mini";
//  char url[] = "http://localhost:8080/ponto/lista_usuarios_mini";

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Postman-Token: 193ba1fd-48a2-4777-a0fc-f8600d0251ac");
    headers = curl_slist_append(headers, "Cache-Control: no-cache");
    headers = curl_slist_append(headers, "Authorization: Basic VVNFUlRFU1RFOjEyMzQ1");
    headers = curl_slist_append(headers, "usuarioId: 76");
    headers = curl_slist_append(headers,
                                "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS,
                     "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"usuarioId\"\r\n\r\n76\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"digital\"\r\n\r\n\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--");

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);

    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    } else {
        printf("%lu bytes retrieved\n", (long) chunk.size);
        unsigned char *buf2 = calloc(chunk.size, sizeof(int));
        int i = 0;
        for (i = 0; i < chunk.size; i++) {
            buf2[i] = chunk.memory[i];
        }
        return buf2;
    }
    return buf2;
}

char *get_full_user_list() {
    CURL *curl_handle;
    CURLcode res;
    char *buf2;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    char url[] = "http://licenca.infarma.com.br/ponto/lista_usuarios";
//    char url[] = "http://localhost:8080/ponto/lista_usuarios";

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Postman-Token: 193ba1fd-48a2-4777-a0fc-f8600d0251ac");
    headers = curl_slist_append(headers, "Cache-Control: no-cache");
    headers = curl_slist_append(headers, "Authorization: Basic VVNFUlRFU1RFOjEyMzQ1");
    headers = curl_slist_append(headers, "usuarioId: 76");
    headers = curl_slist_append(headers,
                                "content-type: multipart/form-data; boundary=----WebKitFormBoundary7MA4YWxkTrZu0gW");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS,
                     "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"usuarioId\"\r\n\r\n76\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"digital\"\r\n\r\n\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--");

    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);

    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
    } else {
        printf("%lu bytes retrieved\n", (long) chunk.size);
        unsigned char *buf2 = calloc(chunk.size, sizeof(int));
        int i = 0;
        for (i = 0; i < chunk.size; i++) {
            buf2[i] = chunk.memory[i];
        }
        return buf2;
    }
    return buf2;
}

int create_list_users(){
    //free this later on
    char *json = get_full_user_list();
    //printf("%s", json);

    int number_of_users;
    get_number_of_users(json, &number_of_users);
    //printf("%d", number_of_users);

    //Allocating structs to fill with user data
    //PS: Free this later on
    struct user_list *list_of_users = malloc(number_of_users * sizeof(struct user_list));
    deal_with_json(json, list_of_users);

    printf("\n NUmber of users: %d\n", number_of_users);
    compose_json_answer("CONSOLE_LOG", "SUCCESS", "create_list_users", "Number of users retrieved.", "");

    return number_of_users;
}

char *post_ponto(int id_usuario){
    unsigned char buf2;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    char url[] = "http://licenca.infarma.com.br/ponto/bate_ponto";
    //char url[] = "http://localhost:8080/ponto/bate_ponto";
    //  char url[] = "http://192.168.16.111/ponto/bate_ponto";

    char requestBody1[] = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"usuarioId\"\r\n\r\n";
    char requestBody4[] = "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--";

    char userIdAsStr[12];
    sprintf(userIdAsStr, "%d", id_usuario);

    char *result = calloc(strlen(requestBody1) + strlen(userIdAsStr) + strlen(requestBody4) + 1, sizeof(unsigned char));
    strcat(result, requestBody1);
    strcat(result, userIdAsStr);
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
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *) &chunk);

    CURLcode ret = curl_easy_perform(hnd);

    if (ret != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(ret));
    } else {
        printf("%lu bytes retrieved\n", (long) chunk.size);
        unsigned char buffer2[chunk.size+5];
        unsigned char *buf2 = calloc(chunk.size, sizeof(int));
        int i = 0;
        for (i = 0; i < chunk.size; i++) {
            buf2[i] = chunk.memory[i];
        }
        return buf2;
    }
    free(result);
}
