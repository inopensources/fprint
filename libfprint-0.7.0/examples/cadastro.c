//
// Created by leticia on 12/09/18.
//

#include <stdio.h>
#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>


int cadastrar_digital(int id_usuario, char* digital, int tamanho_array);

char * fprint_to_string(char * ret, int length);

struct node_user * connect_postman(void);
struct node_user * createList(int id, unsigned char *digital);


/*int main(void)
{
    printf("We are posting!!\n");
    char digital[] = "digital\n";
    cadastrar_digital(50, digital, 600);
}*/


char * fprint_to_string(char * ret, int length){


    int i;
    char *digital = alloca(4*length);
    for (i = 0; i<length; i++)
    {
        unsigned char digital2[12];
        if (i == 0){
            sprintf(digital2, "[%d, ", (int)(*(ret+i)));
        }else if(i == length-1 ){
            sprintf(digital2, "%d] ", (int)(*(ret+i)));

        }else{
            sprintf(digital2, "%d, ", (int)(*(ret+i)));
        }
        ;
        strcat(digital, digital2);
    }

    return digital;

}

int cadastrar_digital(int id_usuario, char* digital, int tamanho_array) {
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