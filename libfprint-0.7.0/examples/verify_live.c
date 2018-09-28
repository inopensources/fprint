#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libfprint/fprint.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <json-c/json.h>
#include <string.h>
#include <pthread.h>
#include "ws.c"


/*

struct lws_context * context_client;

void cadastra_user();
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
//struct fp_dscv_dev *discover_device(struct fp_dscv_dev **discovered_devs);
//struct fp_print_data *enroll(struct fp_dev *dev);
void do_point();
struct node_user * get_users(void);
void post_user(int id_usuario, char* digital, int tamanho_array);

//create a list with the content of users
struct node_user * createList(int id, unsigned char *digital);
//do a iter over the list of users
void iterOverList(struct node_user * head);
//add a user to the end og the list
void append(struct node_user * head, int id);
char** str_split(char* a_str, const char a_delim);
void read_digital(char * digital, unsigned char * ret_returned);
//char * fprint_to_string(char * ret, int length);
//void deal_with_json(char* json_str, struct user_list *list);
char *get_user_list();
void get_number_of_users(char* json_str, int* number_of_users);

int create_list_users();
*/


/*De curl_utils.c*/
/*
struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        *//* out of memory! *//*
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}*/

/*De remote_database.c*/
/*
struct node_user * get_users(void){

    printf("Connnecting to mock database\n");
    struct node_user * head = create_list_users();

    CURLcode ret;
    struct MemoryStruct chunk;
    chunk.memory = malloc(1);  *//* will be grown as needed by the realloc above *//*
    chunk.size = 0;    *//* no data at this point *//*

    CURL *hnd = curl_easy_init();

    curl_easy_setopt(hnd, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(hnd, CURLOPT_URL, "http://licenca.infarma.com.br/ponto/lista_usuarios");
    *//* send all data to this function  *//*
    curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    *//* we pass our 'chunk' struct to the callback function *//*
    curl_easy_setopt(hnd, CURLOPT_WRITEDATA, (void *)&chunk);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Postman-Token: a3646a34-01e4-47d5-8f1f-5a7db9987a84");
    headers = curl_slist_append(headers, "Cache-Control: no-cache");
    headers = curl_slist_append(headers, "Authorization: Basic TUFSQ1VTOjEyMzQ1");
    curl_easy_setopt(hnd, CURLOPT_HTTPHEADER, headers);

    ret = curl_easy_perform(hnd);

    *//* check for errors *//*
    if(ret != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(ret));
    }
    else {
        *//*
         * Now, our chunk.memory points to a memory block that is chunk.size
         * bytes big and contains the remote file.
         *//*

        printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
        //printf("%s\n", chunk.memory);

        FILE *fp = fopen("users.txt", "w");
        fprintf(fp, "%s", chunk.memory);
    }

    *//* cleanup curl stuff*//*
    curl_easy_cleanup(hnd);
    free(chunk.memory);
    curl_global_cleanup();

    return head;
}*/
/*

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
*/

/*

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

    return 0;
}

//remote_database após parser:

struct user_list{
    int user_id;
    char *name;
    char *role;
    char *fingerprint;
};

void get_number_of_users(char* json_str, int* number_of_users){
    json_object * jobj = json_tokener_parse(json_str);
    int length_array = json_object_array_length(jobj);

    //Writing the number of users in the JSON to number_of_users
    *number_of_users = length_array;

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

char *get_user_list() {
    CURL *curl_handle;
    CURLcode res;
    char *buf2;
    struct MemoryStruct chunk;

    chunk.memory = malloc(1);  */
/* will be grown as needed by the realloc above *//*

    chunk.size = 0;    */
/* no data at this point *//*


    curl_global_init(CURL_GLOBAL_ALL);

    */
/* init the curl session *//*

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
        char *buf2 = (char *) malloc(chunk.size);
        for (int i = 0; i < chunk.size; i++) {
            buf2[i] = chunk.memory[i];
        }
        return buf2;
    }
    return buf2;
}
*/


/*De user.c*/

//struct node_user * createList(int id, unsigned char *digital){

//
//    struct node_user * head = NULL;
//    //head = malloc(struct node_user);
//    if (head == NULL) {
//        return head;
//    }
//
//    //memcpy(head->digital,(const unsigned char*)&digital,sizeof(digital));
//    //strcpy(head->digital, digital);
//    //int lengthDigital = sizeof(head->digital)/sizeof(char);
//
//    head->id = id;
//    head->next = NULL;
//    return head;
/*}

void iterOverList(struct node_user * head){

    struct node_user * current = head;
    int lengthDigital = 0;

    while (current != NULL) {

        printf("id %d\n", current->id);

        lengthDigital = sizeof(current->digital)/sizeof(char);
        printf("length digital %d\n", lengthDigital);

        for(int i = 0; i<= lengthDigital; i++){

            printf("%c\n", current->digital[i]);
        }

        current = current->next;
    }
}

void append(struct node_user * head, int id) {*/

//	struct node_user *current = head;
//	while (current->next != NULL) {
//		current = current->next;
//	}
//
//	*//* now we can add a new variable *//*
//	current->next = malloc(sizeof(struct node_user));
//	current->next->id = id;
//	current->next->next = NULL;

//}


/*De utils.c*/

/*char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    *//* Count how many elements will be extracted. *//*
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    *//* Add space for trailing token. *//*
    count += last_comma < (a_str + strlen(a_str) - 1);

    *//* Add space for terminating null string so caller
       knows where the list of returned strings ends. *//*
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == count - 1);
        *(result + idx) = 0;
    }*/

 /*   return result;
}

void removeChar(char *str, char c) {
    int i = 0;
    int j = 0;

    while (str[i]) {
        if (str[i] != c) {
            str[j++] = str[i];
        }
        i++;
    }
    str[j]=0;
}*/
//
//void read_digital(char * digital, unsigned char * ret_returned){
//
//    ///*get a string and return the array of char*///
//    char digitael[42682];
//    strcpy(digitael, digital);
//
//    //unsigned char *ret_returned = alloca(12050); // todo: generalizar length
//
//    char** tokens;
//
//    //tokens é uma array de char
//    tokens = str_split(digitael, ',');
//    if (tokens)
//    {
//        int  i;
//        int num;
//        for (i = 0; *(tokens + i); i++)
//        {
//            //printf("%c ", atoi(*(tokens + i)));
//
//            if((strchr(*(tokens + i), '[')) != NULL )
//            {
//                removeChar(*(tokens + i), '[');
//            }
//            if((strchr(*(tokens + i), ']')) != NULL)
//            {
//                removeChar(*(tokens + i), ']');
//            }
//            //num é um int com valor  inteiro equivalente ao token
//            num = atoi(*(tokens + i));
//            //printf("%d ",num);
//            //printf("%c ", num);
//            *(ret_returned + i) = num;
//            //printf("%d ", ret_returned[i]);
//            free(*(tokens + i));
//        }
//
//        printf("\nLength digital ON read_digital(): %d\n", i);
//        free(tokens);
//    }
//
//}
/*
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
        strcat(digital, digital2);
    }

    return digital;

}*/

/*De devices_utils.c*//*
struct fp_dscv_dev *discover_device(struct fp_dscv_dev **discovered_devs)
{
    struct fp_dscv_dev *ddev = discovered_devs[0];
    struct fp_driver *drv;
    if (!ddev)
        return NULL;

    drv = fp_dscv_dev_get_driver(ddev);
    printf("Found device claimed by %s driver\n", fp_driver_get_full_name(drv));
    return ddev;
}*/
/*

struct lws_context * get_context();

void set_context(struct lws_context * context);
*/

/*
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
}*/


//
///*de ponto.c*/
//void do_point(){
//
//    //lista de digitais
//    unsigned char **digitais = 0;
//    int num_digitais = 0;
//    unsigned char *ret;
//    int num_ret = 0;
//
//
//    //free this later on
//    char *json = get_user_list();
//    printf("%s\n", json);
//
//    int number_of_users;
//    get_number_of_users(json, &number_of_users);
//    printf("number_of_users: %d\n", number_of_users);
//
//    //Allocating structs to fill with user data
//    //PS: Free this later on
//    struct user_list *list_of_users = malloc(number_of_users * sizeof(struct user_list));
//    num_digitais = deal_with_json(json, list_of_users);
//
//    //criando lista de digitais
//    digitais = malloc(sizeof(unsigned char *) * num_digitais);
//    int ids_list[num_digitais];
//
//    for (int i = 0; i < number_of_users; i++){
//
//        if (strcmp((list_of_users)[i].fingerprint, "") != 0){
//
//            printf("Id: %d\n", (list_of_users)[i].user_id);
//            printf("Name: %s\n", (list_of_users)[i].name);
//            ids_list[num_ret] = (list_of_users)[i].user_id;
//            digitais[num_ret] = malloc(sizeof(unsigned char) * 12050);
//            read_digital((list_of_users)[i].fingerprint, digitais[num_ret]);
//            num_ret++;
//
//        }
//    }
//
//    ///*Iniciando device*///
//
//    int r = 1;
//    struct fp_dscv_dev *ddev;
//    struct fp_dscv_dev **discovered_devs;
//    struct fp_dev *dev;
//    // struct fp_print_data *data;
//
//
//    r = fp_init();
//
//    if (r < 0) {
//        fprintf(stderr, "Failed to initialize libfprint\n");
//        exit(1);
//    }
//
//    fp_set_debug(3);
//
//    discovered_devs = fp_discover_devs();
//    if (!discovered_devs) {
//        fprintf(stderr, "Could not discover devices\n");
//        goto out;
//    }
//    ddev = discover_device(discovered_devs);
//    if (!ddev) {
//        fprintf(stderr, "No devices detected.\n");
//        goto out;
//    }
//    dev = fp_dev_open(ddev);
//    fp_dscv_devs_free(discovered_devs);
//    if (!dev) {
//        fprintf(stderr, "Could not open device.\n");
//        goto out;
//    }
//
//    printf("Opened device. It's now time to enroll your finger.\n");
//
//    ///Fim inicialização device
//
//
//    //ret = read_digital(digital);
//    //int length = 12050;
//    //data = enroll(dev);
//    //int result = compare_digital(dev, ret, length); //chamada em data.c
//
//    int result = compare_digital(dev, digitais, num_digitais, ids_list); //chamada em data.c
//
//    printf("\nResult: %d\n", result);
//
//   /* if(result != -1){
//        client(result, get_context());
//        sleep(1);
//        post_ponto(result);
//    }
//    else{
//        client(-1, get_context());
//        sleep(1);
//    }*/
//
//
//    free(digitais);
//
//    out_close:
//    fp_dev_close(dev);
//    out:
//    fp_exit();
//    return r;
//
//
//}

///*De cadastro.c*/
//void cadastra_user(){
//
//    ///*Iniciando device*///
//
//    int r = 1;
//    struct fp_dscv_dev *ddev;
//    struct fp_dscv_dev **discovered_devs;
//    struct fp_dev *dev;
//    unsigned char *ret;
//    struct fp_print_data *data;
//
//    r = fp_init();
//
//    if (r < 0) {
//        fprintf(stderr, "Failed to initialize libfprint\n");
//        exit(1);
//    }
//
//    fp_set_debug(3);
//
//    discovered_devs = fp_discover_devs();
//    if (!discovered_devs) {
//        fprintf(stderr, "Could not discover devices\n");
//        goto out;
//    }
//    ddev = discover_device(discovered_devs);
//    if (!ddev) {
//        fprintf(stderr, "No devices detected.\n");
//        goto out;
//    }
//    dev = fp_dev_open(ddev);
//    fp_dscv_devs_free(discovered_devs);
//    if (!dev) {
//        fprintf(stderr, "Could not open device.\n");
//        goto out;
//    }
//
//    printf("Opened device. It's now time to enroll your finger.\n");
//
//    ///*Fim inicialização device*///
//
//
//    data = enroll(dev);
//    int length = fp_print_data_get_data(data, &ret);
//    post_user(76, fprint_to_string(ret, length), length);
//
//    ///*Encerrando device*///
//    out_close:
//    fp_dev_close(dev);
//    out:
//    fp_exit();
//    return r;
//
//}

//todo: adicionar em User.c
/*
int create_list_users(){
    //free this later on
    char *json = get_user_list();
    //printf("%s", json);

    int number_of_users;
    get_number_of_users(json, &number_of_users);
    //printf("%d", number_of_users);

    //Allocating structs to fill with user data
    //PS: Free this later on
    struct user_list *list_of_users = malloc(number_of_users * sizeof(struct user_list));
    deal_with_json(json, list_of_users);

    printf("\n NUmber of users: %d\n", number_of_users);

    return number_of_users;
}
*/


/*Método main*/

int main() {

   /* pthread_t thread_id_server;
    pthread_create(&thread_id_server, NULL, server, NULL);

    set_context(init_context_client());

    pthread_t thread_id_serving;
    pthread_create(&thread_id_serving, NULL, serving, get_context());*/
    run_ws();
    //cadastra_user();
    //do_point();

    //destroy_context_client(get_context());

    return 0;
}













































































