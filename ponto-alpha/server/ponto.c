//
// Created by leticia on 14/09/18.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
int do_point();

char *get_user_list_mini();
char *get_full_user_list();

void get_number_of_users(char* json_str, int* number_of_users);
int create_list_users();

//new
void string_to_fprint(char fprint_string[], unsigned char file[]);
int size_of_file(char fprint_string[]);

struct user_list{
    int user_id;
    char *name;
    char *role;
    char *fingerprint;
};

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

char *get_full_user_list() {
    CURL *curl_handle;
    CURLcode res;
    char *buf2;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl_handle, CURLOPT_URL, "http://licenca.infarma.com.br/ponto/lista_usuarios");

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

char *get_user_list_mini() {
    CURL *curl_handle;
    CURLcode res;
    char *buf2;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */

    curl_global_init(CURL_GLOBAL_ALL);

    /* init the curl session */
    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curl_handle, CURLOPT_URL, "http://licenca.infarma.com.br/ponto/lista_usuarios_mini");

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

void get_number_of_users(char* json_str, int* number_of_users){
    json_object * jobj = json_tokener_parse(json_str);
    int length_array = json_object_array_length(jobj);

    //Writing the number of users in the JSON to number_of_users
    *number_of_users = length_array;

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

int do_point(){

    //lista de digitais
    int num_digitais = 0;
    unsigned char *ret;
    int num_ret = 0;


    //free this later on
    char *json = get_full_user_list();

    int number_of_users;
    get_number_of_users(json, &number_of_users);

    //Allocating structs to fill with user data
    //PS: Free this later on
    struct user_list *list_of_users = malloc(number_of_users * sizeof(struct user_list));
    num_digitais = deal_with_json(json, list_of_users);

    //criando lista de digitais
    unsigned char digitais[num_digitais][12050];
    int ids_list[num_digitais];
    for (int i = 0; i < number_of_users; i++){

        if (strcmp((list_of_users)[i].fingerprint, "") != 0){
//            printf("Id: %d\n", (list_of_users)[i].user_id);
//            printf("Name: %s\n", (list_of_users)[i].name);
            ids_list[num_ret] = (list_of_users)[i].user_id;
            string_to_fprint((list_of_users)[i].fingerprint, digitais[num_ret]);
            num_ret++;
        }
    }


    ///*Iniciando device*///

    int r = 1;
    struct fp_dscv_dev *ddev;
    struct fp_dscv_dev **discovered_devs;
    struct fp_dev *dev;
    // struct fp_print_data *data;


    r = fp_init();

    if (r < 0) {
        fprintf(stderr, "Failed to initialize libfprint\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Falha ao inicializar a libfprint", "");
        return 1;
    }

    fp_set_debug(3);

    discovered_devs = fp_discover_devs();
    if (!discovered_devs) {
        fprintf(stderr, "Could not discover devices\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Falha ao descobrir dispositivos", "");
        return 1;
    }
    ddev = discover_device(discovered_devs);
    if (!ddev) {
        fprintf(stderr, "No devices detected.\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Nenhum dispositivo encontrado", "");
        return 1;
    }
    dev = fp_dev_open(ddev);
    fp_dscv_devs_free(discovered_devs);
    if (!dev) {
        fprintf(stderr, "Could not open device.\n");
        compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Falha ao abrir dispositivo", "");
        return 1;
    }

    printf("Opened device. It's now time to enroll your finger.\n");
    compose_json_answer("SCREEN_UPDATE", "SUCCESS", "cadastra_user", "Dispositivo inicializado. Cadastre sua digital.", "");

    ///Fim inicialização device


    int result = compare_digital(dev, digitais, num_digitais, ids_list); //chamada em data.c

    out_close:
    fp_dev_close(dev);
    out:

    fp_exit();

    return 0;
}

void post_ponto(int id_usuario){

    char url[] = "http://licenca.infarma.com.br/ponto/bate_ponto";

    char requestBody1[] = "------WebKitFormBoundary7MA4YWxkTrZu0gW\r\nContent-Disposition: form-data; name=\"usuarioId\"\r\n\r\n";
    char requestBody4[] = "\r\n------WebKitFormBoundary7MA4YWxkTrZu0gW--";

    char userIdAsStr[12];
    sprintf(userIdAsStr, "%d", id_usuario);

    char *result = malloc(strlen(requestBody1) + strlen(userIdAsStr) + strlen(requestBody4) + 1);
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

    CURLcode ret = curl_easy_perform(hnd);
}

//remote_database após parser:


///new

void string_to_fprint(char fprint_string[], unsigned char file[]) {

    int fprint_string_length = strlen(fprint_string);
    int cnt = 0;

    int file_size = size_of_file(fprint_string);
    unsigned char fprint_file[file_size];
    int fprint_file_pos = 0;

    for (int i = 0; i < fprint_string_length; i++) {
        char * num;
        if (fprint_string[i] != ' ') {
            if (fprint_string[i] == '[') {
                i++;
                cnt = 0;
                num = calloc(3, sizeof(char));
                while (fprint_string[i] != ',' && fprint_string[i] != ']') {
                    num[cnt] = fprint_string[i];
                    cnt++;
                    i++;
                }
                i--;
            }
            if (fprint_string[i] == ',') {
                i++;
                cnt = 0;
                num = calloc(3, sizeof(char));
                while (fprint_string[i] != ',' && fprint_string[i] != ']') {
                    num[cnt] = fprint_string[i];
                    cnt++;
                    i++;
                }
                i--;
            }
        }
        file[fprint_file_pos++] = atoi(num);
    }
}

int size_of_file(char fprint_string[]) {
    int length = strlen(fprint_string);
    int file_size = 1;

    for (int i = 0; i < length; i++) {
        if (fprint_string[i] == ',') {
            file_size++;
        }
    }
    return file_size;
}