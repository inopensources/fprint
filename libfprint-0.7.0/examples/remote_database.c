//
// Created by leticia on 14/09/18.
//

#include "user.c"
#include "curl_utils.c"

struct node_user * get_users(void);
void post_user(int id_usuario, char* digital, int tamanho_array);

struct node_user * get_users(void){

    printf("Connnecting to mock database\n");
    struct node_user * head = create_list_users();

    CURLcode ret;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    CURL *hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_URL, "http://licenca.infarma.com.br/ponto/lista_usuarios");
    /* send all data to this function  */
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&chunk);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Postman-Token: a3646a34-01e4-47d5-8f1f-5a7db9987a84");
    headers = curl_slist_append(headers, "Cache-Control: no-cache");
    headers = curl_slist_append(headers, "Authorization: Basic TUFSQ1VTOjEyMzQ1");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    ret = curl_easy_perform(hnd);

    /* check for errors */
    if(ret != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(ret));
    }
    else {
        /*
         * Now, our chunk.memory points to a memory block that is chunk.size
         * bytes big and contains the remote file.
         */

        printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
        //printf("%s\n", chunk.memory);

        FILE *fp = fopen("users.txt", "w");
        fprintf(fp, "%s", chunk.memory);
    }

    /* cleanup curl stuff*/
    curl_easy_cleanup(hnd);
    free(chunk.memory);
    curl_global_cleanup();

    return head;
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