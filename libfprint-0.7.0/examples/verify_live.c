#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <libfprint/fprint.h>
#include <assert.h>
#include <math.h>
#include <limits.h>
#include <json-c/json.h>
#include <string.h>


void cadastra_user();
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
struct fp_dscv_dev *discover_device(struct fp_dscv_dev **discovered_devs);
struct fp_print_data *enroll(struct fp_dev *dev);
void do_point(char ** digital, int num_digitais);
struct node_user * get_users(void);
void post_user(int id_usuario, char* digital, int tamanho_array);

//create a list with the content of users
struct node_user * createList(int id, unsigned char *digital);
//do a iter over the list of users
void iterOverList(struct node_user * head);
//add a user to the end og the list
void append(struct node_user * head, int id);
char** str_split(char* a_str, const char a_delim);
unsigned char * read_digital();
char * fprint_to_string(char * ret, int length);
//void deal_with_json(char* json_str, struct user_list *list);
char *get_user_list();
void get_number_of_users(char* json_str, int* number_of_users);

int create_list_users();


/*De curl_utils.c*/

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
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

/*De remote_database.c*/

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
        list[i].fingerprint =(char *)malloc(strlen("null"));
        list[i].fingerprint = "null";

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
                if (strcmp(list[i].fingerprint, "null") != 0){
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
        char *buf2 = (char *) malloc(chunk.size);
        for (int i = 0; i < chunk.size; i++) {
            buf2[i] = chunk.memory[i];
        }
        return buf2;
    }
    return buf2;
}


/*De user.c*/

struct node_user * createList(int id, unsigned char *digital){

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
}

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

void append(struct node_user * head, int id) {

//	struct node_user *current = head;
//	while (current->next != NULL) {
//		current = current->next;
//	}
//
//	*//* now we can add a new variable *//*
//	current->next = malloc(sizeof(struct node_user));
//	current->next->id = id;
//	current->next->next = NULL;

}


/*De utils.c*/

char** str_split(char* a_str, const char a_delim)
{
    char** result    = 0;
    size_t count     = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

    /* Count how many elements will be extracted. */
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
       knows where the list of returned strings ends. */
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
    }

    return result;
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
}

unsigned char * read_digital(char * digital){

    ///*get a string and return the array of char*///
    char digitael[42682];
    strcpy(digitael, digital);

    unsigned char *ret_returned = alloca(12050); // todo: generalizar length

    char** tokens;

    //tokens é uma array de char
    tokens = str_split(digitael, ',');
    if (tokens)
    {
        int  i;
        int num;
        for (i = 0; *(tokens + i); i++)
        {
            //printf("%c ", atoi(*(tokens + i)));

            if((strchr(*(tokens + i), '[')) != NULL )
            {
                removeChar(*(tokens + i), '[');
            }
            if((strchr(*(tokens + i), ']')) != NULL)
            {
                removeChar(*(tokens + i), ']');
            }
            //num é um int com valor  inteiro equivalente ao token
            num = atoi(*(tokens + i));
            //printf("%d ",num);
            //printf("%c ", num);
            *(ret_returned + i) = num;
            //printf("%d ", ret_returned[i]);
            free(*(tokens + i));
        }

        printf("\nLength digital ON read_digital(): %d\n", i);
        free(tokens);
    }

    printf("\nRet ON read_digital(): %s\n", ret_returned);

    return ret_returned;
}

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


/*De devices_utils.c*/

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
                printf("Enroll complete!\n");
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

    printf("Enrollment completed!\n\n");
    return enrolled_print;
}


/*de ponto.c*/

void do_point(char ** digitais, int num_digitais){

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

//    printf("iterando lista de digitais para conversão\n");
//    for (int i = 0; i < num_digitais; i++){
//        //printf("\ndigitais: %s\n", digitais);
//        digital = *(digitais + i);
//        ret = read_digital(digital);
//    }

//    ret = read_digital(digital);
//    int length = 12050;
//    //data = enroll(dev);
//    int result = compare_digital(dev, ret, length); //chamada em data.c

    out_close:
    fp_dev_close(dev);
    out:
    fp_exit();
    return r;


}

/*De cadastro.c*/
void cadastra_user(){

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
    post_user(76, fprint_to_string(ret, length), length);

    ///*Encerrando device*///
    out_close:
    fp_dev_close(dev);
    out:
    fp_exit();
    return r;

}

//todo: adicionar em User.c
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

/*Método main*/

int main() {

    //lista de digitais
    char **digitais = 0;
    int num_digitais = 0;
    unsigned char *ret;


    //Only for tests
    //char *json = "[{\"usuarioId\":57,\"nome\":\"ANDRE\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":80,\"nome\":\"ANTONYS\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":12,\"nome\":\"APP_API\",\"perfil\":\"ROLE_ADM\"},{\"usuarioId\":87,\"nome\":\"ARIADNA\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":40,\"nome\":\"ARMANDO\",\"perfil\":\"ROLE_ADM\"},{\"usuarioId\":78,\"nome\":\"ARTENIO\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":54,\"nome\":\"DAYANNA\",\"perfil\":\"ROLE_FIN\"},{\"usuarioId\":59,\"nome\":\"DEBORA\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":71,\"nome\":\"ESDRAS\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":88,\"nome\":\"FELIPE\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":65,\"nome\":\"FERNANDO\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":6,\"nome\":\"FLAVIO\",\"perfil\":\"ROLE_ADM\"},{\"usuarioId\":70,\"nome\":\"FLORISVALDO\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":37,\"nome\":\"GUSTAVOS\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":77,\"nome\":\"GUSTAVOSOUZA\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":81,\"nome\":\"ICARO\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":10,\"nome\":\"IRLANE\",\"perfil\":\"ROLE_ADM\"},{\"usuarioId\":42,\"nome\":\"JACELYNE\",\"perfil\":\"ROLE_FIN\"},{\"usuarioId\":31,\"nome\":\"JESSICA\",\"perfil\":\"ROLE_ADM\"},{\"usuarioId\":86,\"nome\":\"JUAN\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":7,\"nome\":\"LAYRA\",\"perfil\":\"ROLE_ADM\"},{\"usuarioId\":75,\"nome\":\"LEONARDO\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":83,\"nome\":\"LEONARDOJR\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":30,\"nome\":\"LUCAS\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":56,\"nome\":\"MARCIO\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":84,\"nome\":\"MARCOSLIMA\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":73,\"nome\":\"MARCOSROBERTO\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":94,\"nome\":\"MARCUS\",\"perfil\":\"ROLE_SUP\",\"digital\":\"[70, 80, 50, 2, 0, 0, 0, 0, 0, 1, 100, 9, 0, 0, 42, 0, 0, 0, 24, 0, 0, 0, 43, 0, 0, 0, 74, 0, 0, 0, 101, 0, 0, 0, 102, 0, 0, 0, 118, 0, 0, 0, 127, 0, 0, 0, -127, 0, 0, 0, -120, 0, 0, 0, -112, 0, 0, 0, -105, 0, 0, 0, -103, 0, 0, 0, -101, 0, 0, 0, -89, 0, 0, 0, -82, 0, 0, 0, -79, 0, 0, 0, -73, 0, 0, 0, -64, 0, 0, 0, -40, 0, 0, 0, -33, 0, 0, 0, -22, 0, 0, 0, -21, 0, 0, 0, -11, 0, 0, 0, 8, 1, 0, 0, 20, 1, 0, 0, 31, 1, 0, 0, 45, 1, 0, 0, 56, 1, 0, 0, 59, 1, 0, 0, 63, 1, 0, 0, 64, 1, 0, 0, 68, 1, 0, 0, 68, 1, 0, 0, 75, 1, 0, 0, 77, 1, 0, 0, 78, 1, 0, 0, 82, 1, 0, 0, 83, 1, 0, 0, 88, 1, 0, 0, 88, 1, 0, 0, 94, 1, 0, 0, 95, 1, 0, 0, -127, 0, 0, 0, -126, 0, 0, 0, -126, 0, 0, 0, -126, 0, 0, 0, -127, 0, 0, 0, -127, 0, 0, 0, -127, 0, 0, 0, -127, 0, 0, 0, -127, 0, 0, 0, -127, 0, 0, 0, 0, 0, 0, 0, -16, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 112, 64, 19, 8, 80, 127, 0, 0, 41, 0, 0, 0, -114, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -21, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 22, 8, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -16, 26, 19, 8, 80, 127, 0, 0, -64, 34, 19, 8, 80, 127, 0, 0, 126, 0, 0, 0, 125, 0, 0, 0, 125, 0, 0, 0, 124, 0, 0, 0, 123, 0, 0, 0, 122, 0, 0, 0, 123, 0, 0, 0, 123, 0, 0, 0, 123, 0, 0, 0, 123, 0, 0, 0, 122, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -48, 116, 20, 8, 80, 127, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 113, 0, 0, 0, 113, 0, 0, 0, 113, 0, 0, 0, 113, 0, 0, 0, 113, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -16, 36, 19, 8, 80, 127, 0, 0, 74, 1, 0, 0, 127, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -80, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 2, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -80, 68, 18, 8, 80, 127, 0, 0, 88, 1, 0, 0, 16, 1, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 71, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -29, 20, 0, 0, -79, 4, 0, 0, 0, 0, 0, 0, 64, 19, 19, 8, 80, 127, 0, 0, -128, 0, 0, 8, 80, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -34, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 122, 14, 0, 0, 65, 1, 0, 0, 0, 0, 0, 0, -32, 0, 20, 8, 80, 127, 0, 0, -96, 23, 18, 8, 80, 127, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -26, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 8, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 80, 51, 20, 8, 80, 127, 0, 0, 112, 81, 20, 8, -95, 0, 0, 0, -105, 0, 0, 0, -82, 0, 0, 0, 76, 0, 0, 0, -56, 0, 0, 0, -32, 0, 0, 0, -127, 0, 0, 0, 111, 0, 0, 0, -108, 0, 0, 0, 76, 0, 0, 0, 86, 0, 0, 0, 125, 0, 0, 0, -25, 0, 0, 0, -32, 0, 0, 0, -24, 0, 0, 0, 71, 0, 0, 0, -27, 0, 0, 0, -81, 0, 0, 0, 105, 0, 0, 0, 23, 0, 0, 0, -48, 0, 0, 0, -100, 0, 0, 0, -127, 0, 0, 0, 74, 0, 0, 0, -90, 0, 0, 0, -70, 0, 0, 0, 31, 0, 0, 0, -79, 0, 0, 0, 127, 0, 0, 0, 81, 0, 0, 0, 47, 0, 0, 0, 120, 0, 0, 0, -119, 0, 0, 0, -86, 0, 0, 0, 81, 0, 0, 0, -78, 0, 0, 0, -119, 0, 0, 0, 14, 0, 0, 0, 102, 0, 0, 0, -72, 0, 0, 0, 110, 0, 0, 0, 93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -85, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -112, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -64, 13, 19, 8, 80, 127, 0, 0, 85, 1, 0, 0, 16, 1, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 66, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -34, 20, 0, 0, -47, 2, 0, 0, 0, 0, 0, 0, 96, 77, 20, 8, 80, 127, 0, 0, -112, 11, 19, 8, 80, 127, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -39, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 117, 14, 0, 0, -127, 2, 0, 0, 0, 0, 0, 0, -128, 33, 19, 8, 80, 127, 0, 0, 64, -69, 19, 8, 80, 127, 0, 0, 126, 0, 0, 0, 127, 0, 0, 0, 127, 0, 0, 0, 127, 0, 0, 0, 127, 0, 0, 0, 127, 0, 0, 0, 126, 0, 0, 0, 126, 0, 0, 0, 126, 0, 0, 0, 126, 0, 0, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 8, 0, 0, 65, 1, 0, 0, 0, 0, 0, 0, 48, 12, 19, 8, 80, 127, 0, 0, -128, 0, 0, 8, 80, 127, 0, 0, -75, 0, 0, 0, -75, 0, 0, 0, -74, 0, 0, 0, -73, 0, 0, 0, -72, 0, 0, 0, -71, 0, 0, 0, -70, 0, 0, 0, -70, 0, 0, 0, -69, 0, 0, 0, -69, 0, 0, 0, -68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -93, 1, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -128, -18, 19, 8, 80, 127, 0, 0, 48, 1, 20, 8, 80, 127, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 15, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 64, 16, 19, 8, 80, 127, 0, 0, -128, 0, 0, 0, -128, 0, 0, 0, -128, 0, 0, 0, -128, 0, 0, 0, 126, 0, 0, 0, 126, 0, 0, 0, 126, 0, 0, 0, 126, 0, 0, 0, 127, 0, 0, 0, 127, 0, 0, 0, 127, 0, 0, 0, -128, 0, 0, 0, -126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 66, 27, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 64, 32, 19, 8, 80, 127, 0, 0, 48, -85, 19, 8, 80, 127, 0, 0, -122, 0, 0, 0, -122, 0, 0, 0, -120, 0, 0, 0, -120, 0, 0, 0, -120, 0, 0, 0, -121, 0, 0, 0, -121, 0, 0, 0, -121, 0, 0, 0, -123, 0, 0, 0, -123, 0, 0, 0, -123, 0, 0, 0, 0, 0, 0, 0, -112, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 32, 64, 19, 8, 80, 127, 0, 0, 117, 0, 0, 0, -124, -1, -1, -1, 110, -1, -1, -1, -23, -1, -1, -1, -99, 0, 0, 0, -110, 0, 0, 0, -110, 0, 0, 0, -23, -1, -1, -1, -99, 0, 0, 0, -87, 0, 0, 0, -11, -1, -1, -1, -87, 0, 0, 0, -87, 0, 0, 0, -11, -1, -1, -1, 11, 0, 0, 0, 22, 0, 0, 0, -87, 0, 0, 0, 34, 0, 0, 0, 87, -1, -1, -1, 11, 0, 0, 0, -76, 0, 0, 0, 110, -1, -1, -1, 98, -1, -1, -1, 98, -1, -1, -1, 22, 0, 0, 0, 22, 0, 0, 0, 22, 0, 0, 0, 22, 0, 0, 0, 34, 0, 0, 0, 110, -1, -1, -1, 34, 0, 0, 0, 98, -1, -1, -1, 34, 0, 0, 0, 34, 0, 0, 0, 98, -1, -1, -1, 110, -1, -1, -1, 34, 0, 0, 0, 45, 0, 0, 0, 22, 0, 0, 0, 45, 0, 0, 0, 34, 0, 0, 0, 56, 0, 0, 0, 56, 0, 0, 0, -127, 0, 0, 0, -126, 0, 0, 0, -126, 0, 0, 0, -127, 0, 0, 0, -127, 0, 0, 0, 127, 0, 0, 0, 126, 0, 0, 0, 125, 0, 0, 0, 125, 0, 0, 0, 125, 0, 0, 0, 0, 0, 0, 0, 48, 7, 0, 0, 0, 0, 0, 0, -124, 0, 0, 0, 0, 0, 0, 0, -48, -82, 19, 8, 80, 127, 0, 0, 19, 1, 0, 0, 19, 1, 0, 0, 18, 1, 0, 0, 17, 1, 0, 0, 17, 1, 0, 0, 18, 1, 0, 0, 18, 1, 0, 0, 18, 1, 0, 0, 18, 1, 0, 0, 18, 1, 0, 0, 17, 1, 0, 0, 17, 1, 0, 0, 17, 1, 0, 0, 18, 1, 0, 0, 19, 1, 0, 0, 20, 1, 0, 0, 20, 1, 0, 0, 21, 1, 0, 0, 21, 1, 0, 0, 21, 1, 0, 0, 22, 1, 0, 0, 22, 1, 0, 0, 22, 1, 0, 0, 22, 1, 0, 0, 22, 1, 0, 0, 22, 1, 0, 0, 22, 1, 0, 0, 23, 1, 0, 0, -31, 1, 0, 0, 0, 0, 0, 0, -16, 26, 19, 8, 80, 127, 0, 0, -64, 67, 18, 8, 80, 127, 0, 0, 19, 0, 0, 0, 61, 27, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -39, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 8, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -96, 42, 19, 8, 80, 127, 0, 0, -80, 35, 19, 8, 80, 127, 0, 0, -120, 0, 0, 0, -119, 0, 0, 0, -118, 0, 0, 0, -117, 0, 0, 0, -116, 0, 0, 0, -115, 0, 0, 0, -115, 0, 0, 0, -115, 0, 0, 0, -116, 0, 0, 0, -116, 0, 0, 0, -117, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -64, 64, 19, 8, 80, 127, 0, 0, 30, 0, 0, 0, -127, 0, 0, 0, 19, 0, 0, 0, 107, 14, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 7, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -93, 33, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, 80, 26, 19, 8, 80, 127, 0, 0, -96, -50, 19, 8, 80, 127, 0, 0, -128, 0, 0, 0, -128, 0, 0, 0, -128, 0, 0, 0, -128, 0, 0, 0, -128, 0, 0, 0, -128, 0, 0, 0, -128, 0, 0, 0, -127, 0, 0, 0, -127, 0, 0, 0, -127, 0, 0, 0, -127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 58, 27, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 80, 26, 19, 8, 80, 127, 0, 0, 80, 15, 19, 8, 80, 127, 0, 0, -127, 0, 0, 0, -125, 0, 0, 0, -125, 0, 0, 0, -125, 0, 0, 0, -125, 0, 0, 0, -125, 0, 0, 0, -126, 0, 0, 0, -126, 0, 0, 0, -126, 0, 0, 0, -125, 0, 0, 0, -125, 0, 0, 0, 0, 0, 0, 0, -112, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -96, 15, 19, 8, 80, 127, 0, 0, 84, 1, 0, 0, 15, 1, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 100, 9, 0, 0, 35, 0, 0, 0, 18, 0, 0, 0, 20, 0, 0, 0, 23, 0, 0, 0, 24, 0, 0, 0, 24, 0, 0, 0, 24, 0, 0, 0, 24, 0, 0, 0, 27, 0, 0, 0, 36, 0, 0, 0, 70, 0, 0, 0, 79, 0, 0, 0, 94, 0, 0, 0, 96, 0, 0, 0, 98, 0, 0, 0, 108, 0, 0, 0, 110, 0, 0, 0, 120, 0, 0, 0, 123, 0, 0, 0, -124, 0, 0, 0, -115, 0, 0, 0, -114, 0, 0, 0, -106, 0, 0, 0, -91, 0, 0, 0, -73, 0, 0, 0, -71, 0, 0, 0, -50, 0, 0, 0, -48, 0, 0, 0, -43, 0, 0, 0, -26, 0, 0, 0, 7, 1, 0, 0, 23, 1, 0, 0, 31, 1, 0, 0, 35, 1, 0, 0, 43, 1, 0, 0, 108, 1, 0, 0, -47, 2, 0, 0, 0, 0, 0, 0, 32, -5, 19, 8, 80, 127, 0, 0, 112, 117, 20, 8, 80, 127, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -112, -7, 19, 8, 80, 127, 0, 0, 5, 1, 0, 0, 5, 1, 0, 0, 3, 1, 0, 0, 3, 1, 0, 0, 3, 1, 0, 0, 2, 1, 0, 0, 2, 1, 0, 0, 3, 1, 0, 0, 3, 1, 0, 0, 3, 1, 0, 0, 4, 1, 0, 0, 6, 1, 0, 0, 7, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -32, 70, 18, 8, 80, 127, 0, 0, -128, 0, 0, 8, 80, 127, 0, 0, 10, 1, 0, 0, 10, 1, 0, 0, 8, 1, 0, 0, 8, 1, 0, 0, 8, 1, 0, 0, 9, 1, 0, 0, 9, 1, 0, 0, 9, 1, 0, 0, 9, 1, 0, 0, 9, 1, 0, 0, 8, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -64, 54, 20, 8, 80, 127, 0, 0, -32, -7, 19, 8, 80, 127, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, -82, 71, -31, 122, 20, -82, -17, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 80, -8, 19, 8, 80, 127, 0, 0, -1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 2, 1, 0, 0, 3, 1, 0, 0, 4, 1, 0, 0, 4, 1, 0, 0, 4, 1, 0, 0, 4, 1, 0, 0, 5, 1, 0, 0, 5, 1, 0, 0, 5, 1, 0, 0, 0, 0, 0, 0, -112, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -32, -12, 19, 8, 80, 127, 0, 0, 2, 1, 0, 0, 2, 1, 0, 0, 2, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 2, 1, 0, 0, 2, 1, 0, 0, 2, 1, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -64, -5, 19, 8, 80, 127, 0, 0, -32, 70, 18, 8, 80, 127, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -32, 84, 20, 8, 80, 127, 0, 0, 27, 0, 0, 0, 10, 1, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -32, 70, 18, 8, 80, 127, 0, 0, 8, 1, 0, 0, 80, 0, 0, 0, 114, 0, 0, 0, 91, 0, 0, 0, 96, 0, 0, 0, -120, 0, 0, 0, -101, 0, 0, 0, -83, 0, 0, 0, -72, 0, 0, 0, 34, 0, 0, 0, 85, 0, 0, 0, -46, 0, 0, 0, -23, 0, 0, 0, -116, 0, 0, 0, 121, 0, 0, 0, -98, 0, 0, 0, 87, 0, 0, 0, 97, 0, 0, 0, -122, 0, 0, 0, -15, 0, 0, 0, 81, 0, 0, 0, -24, 0, 0, 0, -16, 0, 0, 0, -72, 0, 0, 0, 113, 0, 0, 0, 33, 0, 0, 0, -92, 0, 0, 0, -40, 0, 0, 0, -116, 0, 0, 0, 83, 0, 0, 0, 38, 0, 0, 0, 36, 0, 0, 0, 127, 0, 0, 0, 61, 0, 0, 0, 57, 0, 0, 0, -118, 0, 0, 0, -127, 2, 0, 0, 0, 0, 0, 0, -64, 0, 20, 8, 80, 127, 0, 0, 0, 53, 19, 8, 80, 127, 0, 0, 9, 1, 0, 0, 9, 1, 0, 0, 10, 1, 0, 0, 10, 1, 0, 0, 9, 1, 0, 0, 9, 1, 0, 0, 9, 1, 0, 0, 9, 1, 0, 0, 9, 1, 0, 0, 9, 1, 0, 0, 8, 1, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -128, -16, 19, 8, 80, 127, 0, 0, 17, 1, 0, 0, 7, 1, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -31, 1, 0, 0, 0, 0, 0, 0, -80, -14, 19, 8, 80, 127, 0, 0, 32, 43, 19, 8, 80, 127, 0, 0, 8, 1, 0, 0, 8, 1, 0, 0, 8, 1, 0, 0, 7, 1, 0, 0, 7, 1, 0, 0, 8, 1, 0, 0, 8, 1, 0, 0, 8, 1, 0, 0, 8, 1, 0, 0, 8, 1, 0, 0, 8, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -128, -1, 19, 8, 80, 127, 0, 0, -96, -8, 19, 8, 80, 127, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -96, -8, 19, 8, 80, 127, 0, 0, 12, 1, 0, 0, 13, 1, 0, 0, 13, 1, 0, 0, 13, 1, 0, 0, 13, 1, 0, 0, 13, 1, 0, 0, 13, 1, 0, 0, 13, 1, 0, 0, 13, 1, 0, 0, 12, 1, 0, 0, 12, 1, 0, 0, 12, 1, 0, 0, 12, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -32, 3, 20, 8, 80, 127, 0, 0, 16, 1, 20, 8, 80, 127, 0, 0, 20, 1, 0, 0, 20, 1, 0, 0, 21, 1, 0, 0, 21, 1, 0, 0, 21, 1, 0, 0, 20, 1, 0, 0, 20, 1, 0, 0, 20, 1, 0, 0, 20, 1, 0, 0, 20, 1, 0, 0, 20, 1, 0, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 32, 43, 19, 8, 80, 127, 0, 0, 109, 0, 0, 0, 109, 0, 0, 0, 109, 0, 0, 0, 109, 0, 0, 0, 107, 0, 0, 0, 107, 0, 0, 0, 107, 0, 0, 0, 108, 0, 0, 0, 109, 0, 0, 0, 109, 0, 0, 0, 109, 0, 0, 0, 109, 0, 0, 0, 111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -80, 118, 20, 8, 80, 127, 0, 0, 96, -9, 19, 8, 80, 127, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 6, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 96, 97, 20, 8, 80, 127, 0, 0, 28, 0, 0, 0, 121, -1, -1, -1, -76, 0, 0, 0, 90, 0, 0, 0, -110, 0, 0, 0, -76, 0, 0, 0, -87, 0, 0, 0, -76, 0, 0, 0, -87, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -110, 0, 0, 0, -110, 0, 0, 0, -23, -1, -1, -1, -99, 0, 0, 0, -99, 0, 0, 0, -23, -1, -1, -1, -87, 0, 0, 0, -87, 0, 0, 0, -11, -1, -1, -1, -87, 0, 0, 0, 11, 0, 0, 0, 22, 0, 0, 0, 87, -1, -1, -1, 11, 0, 0, 0, -76, 0, 0, 0, 98, -1, -1, -1, 110, -1, -1, -1, 98, -1, -1, -1, 11, 0, 0, 0, 22, 0, 0, 0, 98, -1, -1, -1, 98, -1, -1, -1, 34, 0, 0, 0, 98, -1, -1, -1, -90, -1, -1, -1, 81, 0, 0, 0, 0, 0, 0, 0, 112, 18, 19, 8, 80, 127, 0, 0, 16, 60, 19, 8, 80, 127, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 48, -1, 19, 8, 80, 127, 0, 0, 13, 0, 0, 0, 11, 1, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 64, 57, 19, 8, 80, 127, 0, 0, 80, 98, 20, 8, 80, 127, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 112, 0, 20, 8, 80, 127, 0, 0, 25, 0, 0, 0, 12, 1, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -64, 76, 20, 8, 80, 127, 0, 0, -112, 55, 18, 8, 80, 127, 0, 0, 16, 1, 0, 0, 16, 1, 0, 0, 16, 1, 0, 0, 16, 1, 0, 0, 16, 1, 0, 0, 16, 1, 0, 0, 16, 1, 0, 0, 16, 1, 0, 0, 16, 1, 0, 0, 16, 1, 0, 0, 16, 1, 0, 0, 0, 0, 0, 0, 112, 8, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -112, 62, 19, 8, 80, 127, 0, 0, 17, 1, 0, 0, 15, 1, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -95, 5, 0, 0, 0, 0, 0, 0, -112, 60, 18, 8, 80, 127, 0, 0, 32, 96, 20, 8, 80, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 107, 0, 0, 0, 107, 0, 0, 0, 107, 0, 0, 0, 107, 0, 0, 0, 107, 0, 0, 0, 107, 0, 0, 0, 107, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -96, -3, 19, 8, 80, 127, 0, 0, 20, 0, 0, 0, 15, 1, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5, 0, 0, 0, 0, 0, 0, 96, -9, 19, 8, 80, 127, 0, 0, -16, -3, 19, 8, 100, 9, 0, 0, 37, 0, 0, 0, 13, 0, 0, 0, 17, 0, 0, 0, 20, 0, 0, 0, 22, 0, 0, 0, 28, 0, 0, 0, 32, 0, 0, 0, 34, 0, 0, 0, 45, 0, 0, 0, 53, 0, 0, 0, 55, 0, 0, 0, 66, 0, 0, 0, 71, 0, 0, 0, 79, 0, 0, 0, 98, 0, 0, 0, 102, 0, 0, 0, 112, 0, 0, 0, 114, 0, 0, 0, 123, 0, 0, 0, -122, 0, 0, 0, -109, 0, 0, 0, -88, 0, 0, 0, -82, 0, 0, 0, -69, 0, 0, 0, -66, 0, 0, 0, -47, 0, 0, 0, -41, 0, 0, 0, -22, 0, 0, 0, 11, 1, 0, 0, 29, 1, 0, 0, 32, 1, 0, 0, 38, 1, 0, 0, 41, 1, 0, 0, 43, 1, 0, 0, 53, 1, 0, 0, 58, 1, 0, 0, 69, 1, 0, 0, 79, 1, 0, 0, 80, 81, 20, 8, 80, 127, 0, 0, -80, 99, 19, 8, 80, 127, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, -95, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -97, 17, 0, 0, 65, 1, 0, 0, 0, 0, 0, 0, -64, 118, 19, 8, 80, 127, 0, 0, -16, 35, 19, 8, 80, 127, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 58, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -48, 68, 20, 8, 80, 127, 0, 0, -86, 0, 0, 0, -85, 0, 0, 0, -85, 0, 0, 0, -85, 0, 0, 0, -85, 0, 0, 0, -86, 0, 0, 0, -86, 0, 0, 0, -85, 0, 0, 0, -85, 0, 0, 0, -85, 0, 0, 0, -84, 0, 0, 0, -84, 0, 0, 0, -83, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -16, 35, 19, 8, 80, 127, 0, 0, -82, 0, 0, 0, -83, 0, 0, 0, -84, 0, 0, 0, -84, 0, 0, 0, -85, 0, 0, 0, -86, 0, 0, 0, -87, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -89, 0, 0, 0, -90, 0, 0, 0, -91, 0, 0, 0, -91, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 16, 55, 20, 8, 80, 127, 0, 0, -128, 0, 0, 8, 80, 127, 0, 0, -85, 0, 0, 0, -85, 0, 0, 0, -84, 0, 0, 0, -84, 0, 0, 0, -84, 0, 0, 0, -85, 0, 0, 0, -85, 0, 0, 0, -85, 0, 0, 0, -86, 0, 0, 0, -86, 0, 0, 0, -86, 0, 0, 0, 0, 0, 0, 0, 48, 2, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -64, 72, 18, 8, 80, 127, 0, 0, 57, 1, 0, 0, -86, 0, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -36, 36, 0, 0, 65, 1, 0, 0, 0, 0, 0, 0, -16, 71, 20, 8, 80, 127, 0, 0, -64, 118, 19, 8, 80, 127, 0, 0, -84, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -84, 0, 0, 0, -84, 0, 0, 0, -84, 0, 0, 0, -84, 0, 0, 0, -84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 70, 35, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -64, 78, 19, 8, 80, 127, 0, 0, 64, 76, 19, 8, 80, 127, 0, 0, -18, 0, 0, 0, -18, 0, 0, 0, -18, 0, 0, 0, -18, 0, 0, 0, -18, 0, 0, 0, -18, 0, 0, 0, -18, 0, 0, 0, -18, 0, 0, 0, -18, 0, 0, 0, -18, 0, 0, 0, -18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, 64, 60, 18, 8, 80, 127, 0, 0, -80, 100, 20, 8, 7, 0, 0, 0, 125, 0, 0, 0, 115, 0, 0, 0, 46, 0, 0, 0, -78, 0, 0, 0, -94, 0, 0, 0, 15, 0, 0, 0, -80, 0, 0, 0, 12, 0, 0, 0, -74, 0, 0, 0, -82, 0, 0, 0, 82, 0, 0, 0, -50, 0, 0, 0, -27, 0, 0, 0, 115, 0, 0, 0, -104, 0, 0, 0, 80, 0, 0, 0, 90, 0, 0, 0, -21, 0, 0, 0, 74, 0, 0, 0, -78, 0, 0, 0, -22, 0, 0, 0, 107, 0, 0, 0, 26, 0, 0, 0, -97, 0, 0, 0, -122, 0, 0, 0, 77, 0, 0, 0, 32, 0, 0, 0, 126, 0, 0, 0, 50, 0, 0, 0, 67, 0, 0, 0, -69, 0, 0, 0, 32, 0, 0, 0, 103, 0, 0, 0, -107, 0, 0, 0, -121, 0, 0, 0, -118, 0, 0, 0, -96, 71, 20, 8, 80, 127, 0, 0, 84, 1, 0, 0, -85, 0, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 112, 30, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 48, 73, 20, 8, 80, 127, 0, 0, -128, 37, 19, 8, 80, 127, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 96, 4, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 96, 29, 19, 8, 80, 127, 0, 0, 86, 1, 0, 0, -83, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 68, 27, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 32, 79, 20, 8, 80, 127, 0, 0, -64, 118, 19, 8, 80, 127, 0, 0, -80, 0, 0, 0, -81, 0, 0, 0, -82, 0, 0, 0, -83, 0, 0, 0, -84, 0, 0, 0, -85, 0, 0, 0, -86, 0, 0, 0, -86, 0, 0, 0, -88, 0, 0, 0, -89, 0, 0, 0, -90, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -80, 60, 19, 8, 80, 127, 0, 0, 30, 1, 0, 0, 25, 1, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -96, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 24, 0, 0, 97, 4, 0, 0, 0, 0, 0, 0, -32, 87, 20, 8, 80, 127, 0, 0, -32, 97, 20, 8, 80, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 10, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 32, 74, 20, 8, 80, 127, 0, 0, 86, 1, 0, 0, -82, 0, 0, 0, 19, 0, 0, 0, 7, 1, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 8, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 110, 22, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 80, 51, 20, 8, 80, 127, 0, 0, -128, 37, 19, 8, 80, 127, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -34, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -64, 10, 18, 8, 80, 127, 0, 0, -128, 61, 18, 8, 110, -1, -1, -1, -113, -1, -1, -1, 98, -1, -1, -1, -87, 0, 0, 0, 110, -1, -1, -1, 87, -1, -1, -1, 11, 0, 0, 0, -11, -1, -1, -1, 0, 0, 0, 0, -23, -1, -1, -1, -110, 0, 0, 0, -99, 0, 0, 0, -121, 0, 0, 0, -110, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -11, -1, -1, -1, -87, 0, 0, 0, -23, -1, -1, -1, -87, 0, 0, 0, 87, -1, -1, -1, 34, 0, 0, 0, 11, 0, 0, 0, -76, 0, 0, 0, 98, -1, -1, -1, 87, -1, -1, -1, 22, 0, 0, 0, 22, 0, 0, 0, 98, -1, -1, -1, 98, -1, -1, -1, 98, -1, -1, -1, 22, 0, 0, 0, 98, -1, -1, -1, 98, -1, -1, -1, 22, 0, 0, 0, 34, 0, 0, 0, 34, 0, 0, 0, -48, 78, 20, 8, 80, 127, 0, 0, 88, 1, 0, 0, -80, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -98, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -80, 19, 19, 8, 80, 127, 0, 0, 20, 0, 0, 0, -36, 0, 0, 0, 24, 0, 0, 0, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -128, 14, 0, 0, 49, 2, 0, 0, 0, 0, 0, 0, -112, 55, 18, 8, 80, 127, 0, 0, -80, 60, 20, 8, 80, 127, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, -78, 0, 0, 0, -78, 0, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 96, 14, 19, 8, 80, 127, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, -80, 0, 0, 0, -80, 0, 0, 0, -80, 0, 0, 0, -80, 0, 0, 0, -80, 0, 0, 0, -80, 0, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 48, 78, 20, 8, 80, 127, 0, 0, -128, 0, 0, 8, 80, 127, 0, 0, -78, 0, 0, 0, -78, 0, 0, 0, -78, 0, 0, 0, -79, 0, 0, 0, -77, 0, 0, 0, -76, 0, 0, 0, -75, 0, 0, 0, -74, 0, 0, 0, -74, 0, 0, 0, -74, 0, 0, 0, -74, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 11, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, 48, 78, 20, 8, 80, 127, 0, 0, -128, 0, 0, 8, 80, 127, 0, 0, -78, 0, 0, 0, -78, 0, 0, 0, -77, 0, 0, 0, -77, 0, 0, 0, -77, 0, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, -81, 0, 0, 0, -82, 0, 0, 0, -83, 0, 0, 0, -84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -86, 9, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 32, 79, 20, 8, 80, 127, 0, 0, 48, 78, 20, 8, 80, 127, 0, 0, -77, 0, 0, 0, -77, 0, 0, 0, -78, 0, 0, 0, -78, 0, 0, 0, -78, 0, 0, 0, -77, 0, 0, 0, -77, 0, 0, 0, -77, 0, 0, 0, -77, 0, 0, 0, -77, 0, 0, 0, -77, 0, 0, 0, 0, 0, 0, 0, -112, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -48, 53, 20, 8, 80, 127, 0, 0, 109, 1, 0, 0, -112, 0, 0, 0, 1, 0, 0, 0, -20, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 126, 6, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 48, 35, 18, 8, 80, 127, 0, 0, -128, 7, 20, 8, 100, 9, 0, 0, 41, 0, 0, 0, 16, 0, 0, 0, 19, 0, 0, 0, 21, 0, 0, 0, 21, 0, 0, 0, 25, 0, 0, 0, 28, 0, 0, 0, 33, 0, 0, 0, 37, 0, 0, 0, 39, 0, 0, 0, 41, 0, 0, 0, 52, 0, 0, 0, 62, 0, 0, 0, 75, 0, 0, 0, 94, 0, 0, 0, 100, 0, 0, 0, 109, 0, 0, 0, 111, 0, 0, 0, 119, 0, 0, 0, 122, 0, 0, 0, -127, 0, 0, 0, -107, 0, 0, 0, -104, 0, 0, 0, -101, 0, 0, 0, -97, 0, 0, 0, -75, 0, 0, 0, -59, 0, 0, 0, -34, 0, 0, 0, -34, 0, 0, 0, -32, 0, 0, 0, -27, 0, 0, 0, -12, 0, 0, 0, 3, 1, 0, 0, 22, 1, 0, 0, 24, 1, 0, 0, 32, 1, 0, 0, 48, 1, 0, 0, 53, 1, 0, 0, 55, 1, 0, 0, 64, 1, 0, 0, 81, 1, 0, 0, 101, 1, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -40, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 112, 117, 20, 8, 80, 127, 0, 0, -128, 83, 20, 8, 80, 127, 0, 0, 109, 0, 0, 0, 110, 0, 0, 0, 111, 0, 0, 0, 112, 0, 0, 0, 113, 0, 0, 0, 114, 0, 0, 0, 115, 0, 0, 0, 116, 0, 0, 0, 117, 0, 0, 0, 118, 0, 0, 0, 119, 0, 0, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -64, 117, 20, 8, 80, 127, 0, 0, 93, 0, 0, 0, 55, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -104, 108, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -56, 108, 0, 0, -127, 2, 0, 0, 0, 0, 0, 0, 48, 87, 19, 8, 80, 127, 0, 0, -80, 80, 20, 8, 80, 127, 0, 0, 112, 0, 0, 0, 114, 0, 0, 0, 114, 0, 0, 0, 114, 0, 0, 0, 114, 0, 0, 0, 113, 0, 0, 0, 113, 0, 0, 0, 113, 0, 0, 0, 114, 0, 0, 0, 115, 0, 0, 0, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -80, 90, 20, 8, 80, 127, 0, 0, -128, 0, 0, 8, 80, 127, 0, 0, 115, 0, 0, 0, 115, 0, 0, 0, 115, 0, 0, 0, 114, 0, 0, 0, 113, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 111, 0, 0, 0, 110, 0, 0, 0, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, -64, 36, 20, 8, 80, 127, 0, 0, 96, 107, 19, 8, 80, 127, 0, 0, -116, 0, 0, 0, -116, 0, 0, 0, -115, 0, 0, 0, -115, 0, 0, 0, -115, 0, 0, 0, -116, 0, 0, 0, -116, 0, 0, 0, -116, 0, 0, 0, -116, 0, 0, 0, -116, 0, 0, 0, -116, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 0, 91, 20, 8, 80, 127, 0, 0, 116, 0, 0, 0, 116, 0, 0, 0, 116, 0, 0, 0, 115, 0, 0, 0, 115, 0, 0, 0, 115, 0, 0, 0, 115, 0, 0, 0, 115, 0, 0, 0, 116, 0, 0, 0, 116, 0, 0, 0, 116, 0, 0, 0, 116, 0, 0, 0, 116, 0, 0, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 112, 101, 19, 8, 80, 127, 0, 0, 63, 1, 0, 0, 117, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 82, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -128, 97, 19, 8, 80, 127, 0, 0, 16, 58, 18, 8, -97, 0, 0, 0, 123, 0, 0, 0, 124, 0, 0, 0, -66, 0, 0, 0, -118, 0, 0, 0, 70, 0, 0, 0, 113, 0, 0, 0, -87, 0, 0, 0, 10, 0, 0, 0, -116, 0, 0, 0, -71, 0, 0, 0, -104, 0, 0, 0, 48, 0, 0, 0, -79, 0, 0, 0, -20, 0, 0, 0, 81, 0, 0, 0, -55, 0, 0, 0, 44, 0, 0, 0, 120, 0, 0, 0, 54, 0, 0, 0, -49, 0, 0, 0, 37, 0, 0, 0, -42, 0, 0, 0, -56, 0, 0, 0, -110, 0, 0, 0, 70, 0, 0, 0, 125, 0, 0, 0, -49, 0, 0, 0, -73, 0, 0, 0, 98, 0, 0, 0, 36, 0, 0, 0, -87, 0, 0, 0, 118, 0, 0, 0, -100, 0, 0, 0, 124, 0, 0, 0, 40, 0, 0, 0, 103, 0, 0, 0, 126, 0, 0, 0, 122, 0, 0, 0, -102, 0, 0, 0, -109, 0, 0, 0, -115, 0, 0, 0, -115, 0, 0, 0, -113, 0, 0, 0, -113, 0, 0, 0, -114, 0, 0, 0, -114, 0, 0, 0, -115, 0, 0, 0, -116, 0, 0, 0, -117, 0, 0, 0, -118, 0, 0, 0, -118, 0, 0, 0, 0, 0, 0, 0, 16, 4, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 48, 45, 20, 8, 80, 127, 0, 0, 113, 1, 0, 0, 119, 0, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 106, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 104, -100, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -96, -115, 19, 8, 80, 127, 0, 0, -80, 37, 20, 8, 80, 127, 0, 0, 121, 0, 0, 0, 122, 0, 0, 0, 122, 0, 0, 0, 122, 0, 0, 0, 122, 0, 0, 0, 121, 0, 0, 0, 121, 0, 0, 0, 121, 0, 0, 0, 121, 0, 0, 0, 121, 0, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, -99, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, 48, 61, 18, 8, 80, 127, 0, 0, -16, 59, 18, 8, 80, 127, 0, 0, -114, 0, 0, 0, -115, 0, 0, 0, -116, 0, 0, 0, -118, 0, 0, 0, -118, 0, 0, 0, -116, 0, 0, 0, -116, 0, 0, 0, -116, 0, 0, 0, -116, 0, 0, 0, -117, 0, 0, 0, -118, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -32, 65, 18, 8, 80, 127, 0, 0, -16, 65, 19, 8, 80, 127, 0, 0, -111, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, -110, 0, 0, 0, -109, 0, 0, 0, -109, 0, 0, 0, -109, 0, 0, 0, -109, 0, 0, 0, 0, 0, 0, 0, 80, 5, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 112, 104, 20, 8, 80, 127, 0, 0, 113, 1, 0, 0, 122, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 56, -87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 104, -87, 0, 0, 49, 2, 0, 0, 0, 0, 0, 0, -96, -110, 19, 8, 80, 127, 0, 0, 48, 67, 19, 8, 80, 127, 0, 0, 100, 0, 0, 0, 100, 0, 0, 0, 101, 0, 0, 0, 101, 0, 0, 0, 99, 0, 0, 0, 99, 0, 0, 0, 99, 0, 0, 0, 99, 0, 0, 0, 100, 0, 0, 0, 101, 0, 0, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -16, 91, 20, 8, 80, 127, 0, 0, 0, 48, 20, 8, 80, 127, 0, 0, 126, 0, 0, 0, 126, 0, 0, 0, 125, 0, 0, 0, 124, 0, 0, 0, 123, 0, 0, 0, 122, 0, 0, 0, 122, 0, 0, 0, 121, 0, 0, 0, 121, 0, 0, 0, 120, 0, 0, 0, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, -75, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, 64, 82, 20, 8, 80, 127, 0, 0, -16, 91, 20, 8, -124, -1, -1, -1, -90, -1, -1, -1, 98, -1, -1, -1, 34, 0, 0, 0, -99, 0, 0, 0, -76, 0, 0, 0, -110, 0, 0, 0, -11, -1, -1, -1, 11, 0, 0, 0, -99, 0, 0, 0, -121, 0, 0, 0, -34, -1, -1, -1, -99, 0, 0, 0, -121, 0, 0, 0, -121, 0, 0, 0, -99, 0, 0, 0, -121, 0, 0, 0, -11, -1, -1, -1, -99, 0, 0, 0, -87, 0, 0, 0, -23, -1, -1, -1, -87, 0, 0, 0, -56, -1, -1, -1, 11, 0, 0, 0, 87, -1, -1, -1, 11, 0, 0, 0, 98, -1, -1, -1, -113, -1, -1, -1, 121, -1, -1, -1, 87, -1, -1, -1, 11, 0, 0, 0, 110, -1, -1, -1, 22, 0, 0, 0, 98, -1, -1, -1, 98, -1, -1, -1, 22, 0, 0, 0, 98, -1, -1, -1, 22, 0, 0, 0, 34, 0, 0, 0, 67, 0, 0, 0, -11, -1, -1, -1, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 86, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 48, 61, 18, 8, 80, 127, 0, 0, 80, 101, 20, 8, 80, 127, 0, 0, -119, 0, 0, 0, -120, 0, 0, 0, -120, 0, 0, 0, -121, 0, 0, 0, -122, 0, 0, 0, -123, 0, 0, 0, -124, 0, 0, 0, -124, 0, 0, 0, -124, 0, 0, 0, -124, 0, 0, 0, -125, 0, 0, 0, 0, 0, 0, 0, -32, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -128, 67, 19, 8, 80, 127, 0, 0, 87, 1, 0, 0, 15, 0, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, -82, 71, -31, 122, 20, -82, -17, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 24, -51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -48, 7, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 112, 69, 18, 8, 80, 127, 0, 0, 98, 1, 0, 0, -123, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 56, -40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 104, -40, 0, 0, -31, 1, 0, 0, 0, 0, 0, 0, -48, 103, 20, 8, 80, 127, 0, 0, -112, 11, 19, 8, 80, 127, 0, 0, 123, 0, 0, 0, 124, 0, 0, 0, 124, 0, 0, 0, 124, 0, 0, 0, 124, 0, 0, 0, 124, 0, 0, 0, 122, 0, 0, 0, 121, 0, 0, 0, 121, 0, 0, 0, 121, 0, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 48, 50, 20, 8, 80, 127, 0, 0, -80, 59, 19, 8, 80, 127, 0, 0, -125, 0, 0, 0, -123, 0, 0, 0, -123, 0, 0, 0, -123, 0, 0, 0, -123, 0, 0, 0, -123, 0, 0, 0, -125, 0, 0, 0, -125, 0, 0, 0, -125, 0, 0, 0, -125, 0, 0, 0, -125, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -64, 58, 19, 8, 80, 127, 0, 0, 88, 1, 0, 0, -126, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -104, -28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -56, -28, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -128, 56, 18, 8, 80, 127, 0, 0, 96, -3, 17, 8, 80, 127, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 56, -27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 104, -27, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, -80, 84, 19, 8, 80, 127, 0, 0, -32, 39, 20, 8, 100, 9, 0, 0, 65, 0, 0, 0, 11, 0, 0, 0, 31, 0, 0, 0, 45, 0, 0, 0, 46, 0, 0, 0, 47, 0, 0, 0, 50, 0, 0, 0, 55, 0, 0, 0, 59, 0, 0, 0, 59, 0, 0, 0, 68, 0, 0, 0, 68, 0, 0, 0, 75, 0, 0, 0, 75, 0, 0, 0, 78, 0, 0, 0, 81, 0, 0, 0, 84, 0, 0, 0, 88, 0, 0, 0, 90, 0, 0, 0, 100, 0, 0, 0, 103, 0, 0, 0, -113, 0, 0, 0, -103, 0, 0, 0, -100, 0, 0, 0, -98, 0, 0, 0, -71, 0, 0, 0, -70, 0, 0, 0, -69, 0, 0, 0, -63, 0, 0, 0, -60, 0, 0, 0, -54, 0, 0, 0, -53, 0, 0, 0, -46, 0, 0, 0, -45, 0, 0, 0, -39, 0, 0, 0, -33, 0, 0, 0, -30, 0, 0, 0, -25, 0, 0, 0, -24, 0, 0, 0, -21, 0, 0, 0, -14, 0, 0, 0, -11, 0, 0, 0, -9, 0, 0, 0, -5, 0, 0, 0, 1, 1, 0, 0, 2, 1, 0, 0, 8, 1, 0, 0, 10, 1, 0, 0, 18, 1, 0, 0, 18, 1, 0, 0, 19, 1, 0, 0, 20, 1, 0, 0, 21, 1, 0, 0, 28, 1, 0, 0, 33, 1, 0, 0, 39, 1, 0, 0, 40, 1, 0, 0, 40, 1, 0, 0, 43, 1, 0, 0, 46, 1, 0, 0, 70, 1, 0, 0, 81, 1, 0, 0, 87, 1, 0, 0, 93, 1, 0, 0, 97, 1, 0, 0, 97, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 80, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 127, 0, 0, 65, 1, 0, 0, 0, 0, 0, 0, 16, 118, 20, 8, 80, 127, 0, 0, 96, 69, 20, 8, 80, 127, 0, 0, 107, 0, 0, 0, 106, 0, 0, 0, 106, 0, 0, 0, 105, 0, 0, 0, 105, 0, 0, 0, 107, 0, 0, 0, 107, 0, 0, 0, 107, 0, 0, 0, 108, 0, 0, 0, 108, 0, 0, 0, 108, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 96, 89, 19, 8, 80, 127, 0, 0, -32, 49, 20, 8, 80, 127, 0, 0, -117, 0, 0, 0, -116, 0, 0, 0, -115, 0, 0, 0, -114, 0, 0, 0, -113, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 127, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, 80, 75, 19, 8, 80, 127, 0, 0, 96, 89, 19, 8, 80, 127, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, -111, 0, 0, 0, -111, 0, 0, 0, -111, 0, 0, 0, -113, 0, 0, 0, -114, 0, 0, 0, -115, 0, 0, 0, -115, 0, 0, 0, -115, 0, 0, 0, -115, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 96, 89, 19, 8, 80, 127, 0, 0, -114, 0, 0, 0, -114, 0, 0, 0, -115, 0, 0, 0, -115, 0, 0, 0, -114, 0, 0, 0, -113, 0, 0, 0, -112, 0, 0, 0, -111, 0, 0, 0, -110, 0, 0, 0, -109, 0, 0, 0, -108, 0, 0, 0, -107, 0, 0, 0, -106, 0, 0, 0, 0, 0, 0, 0, -128, 2, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -32, -1, 18, 8, 80, 127, 0, 0, 77, 0, 0, 0, -94, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 80, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 127, 0, 0, -47, 2, 0, 0, 0, 0, 0, 0, 64, 103, 20, 8, 80, 127, 0, 0, 48, 97, 20, 8, 80, 127, 0, 0, -108, 0, 0, 0, -109, 0, 0, 0, -111, 0, 0, 0, -111, 0, 0, 0, -111, 0, 0, 0, -111, 0, 0, 0, -109, 0, 0, 0, -108, 0, 0, 0, -108, 0, 0, 0, -108, 0, 0, 0, -107, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 112, 98, 20, 8, 80, 127, 0, 0, -122, 0, 0, 0, 11, 1, 0, 0, -106, 0, 0, 0, -88, 0, 0, 0, -67, 0, 0, 0, -107, 0, 0, 0, -113, 0, 0, 0, -82, 0, 0, 0, 114, 0, 0, 0, -97, 0, 0, 0, 57, 0, 0, 0, -112, 0, 0, 0, 12, 0, 0, 0, -91, 0, 0, 0, 84, 0, 0, 0, -84, 0, 0, 0, 72, 0, 0, 0, 13, 0, 0, 0, 115, 0, 0, 0, -71, 0, 0, 0, 16, 0, 0, 0, -18, 0, 0, 0, -14, 0, 0, 0, -70, 0, 0, 0, 57, 0, 0, 0, -53, 0, 0, 0, 94, 0, 0, 0, -17, 0, 0, 0, -123, 0, 0, 0, -1, 0, 0, 0, 57, 0, 0, 0, -42, 0, 0, 0, 68, 0, 0, 0, -53, 0, 0, 0, -45, 0, 0, 0, -36, 0, 0, 0, -66, 0, 0, 0, -62, 0, 0, 0, -84, 0, 0, 0, 51, 0, 0, 0, -48, 0, 0, 0, -61, 0, 0, 0, -96, 0, 0, 0, -71, 0, 0, 0, -42, 0, 0, 0, -80, 0, 0, 0, -39, 0, 0, 0, -81, 0, 0, 0, 85, 0, 0, 0, -82, 0, 0, 0, -21, 0, 0, 0, -51, 0, 0, 0, -42, 0, 0, 0, -90, 0, 0, 0, -92, 0, 0, 0, -91, 0, 0, 0, -105, 0, 0, 0, -71, 0, 0, 0, -54, 0, 0, 0, 112, 0, 0, 0, 49, 0, 0, 0, -95, 0, 0, 0, 106, 0, 0, 0, 123, 0, 0, 0, 17, 0, 0, 0, -124, 0, 0, 0, -113, 0, 0, 0, -113, 0, 0, 0, -113, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 127, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -32, 76, 19, 8, 80, 127, 0, 0, 48, 109, 20, 8, 80, 127, 0, 0, -102, 0, 0, 0, -102, 0, 0, 0, -102, 0, 0, 0, -102, 0, 0, 0, -102, 0, 0, 0, -102, 0, 0, 0, -102, 0, 0, 0, -102, 0, 0, 0, -102, 0, 0, 0, -102, 0, 0, 0, -102, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 127, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -16, 25, 19, 8, 80, 127, 0, 0, -64, 23, 19, 8, 80, 127, 0, 0, -55, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -57, 0, 0, 0, -57, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 48, 109, 20, 8, 80, 127, 0, 0, -114, 0, 0, 0, -112, 0, 0, 0, -112, 0, 0, 0, -111, 0, 0, 0, -110, 0, 0, 0, -109, 0, 0, 0, -108, 0, 0, 0, -110, 0, 0, 0, -111, 0, 0, 0, -111, 0, 0, 0, -111, 0, 0, 0, -111, 0, 0, 0, -110, 0, 0, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -112, 39, 20, 8, 80, 127, 0, 0, 49, 0, 0, 0, -52, 0, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 80, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -96, 5, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 32, 33, 19, 8, 80, 127, 0, 0, 69, 0, 0, 0, -39, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 80, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 127, 0, 0, -79, 4, 0, 0, 0, 0, 0, 0, -64, 88, 20, 8, 80, 127, 0, 0, -48, 52, 20, 8, 80, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -107, 0, 0, 0, -109, 0, 0, 0, -109, 0, 0, 0, -109, 0, 0, 0, -109, 0, 0, 0, -109, 0, 0, 0, -108, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 48, 77, 19, 8, 80, 127, 0, 0, -111, 0, 0, 0, -124, -1, -1, -1, -121, 0, 0, 0, -110, 0, 0, 0, -110, 0, 0, 0, -34, -1, -1, -1, -110, 0, 0, 0, -34, -1, -1, -1, -110, 0, 0, 0, -34, -1, -1, -1, -99, 0, 0, 0, -34, -1, -1, -1, -23, -1, -1, -1, -34, -1, -1, -1, -23, -1, -1, -1, -110, 0, 0, 0, -23, -1, -1, -1, -23, -1, -1, -1, -34, -1, -1, -1, -110, 0, 0, 0, -99, 0, 0, 0, 124, 0, 0, 0, -110, 0, 0, 0, -110, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -45, -1, -1, -1, -99, 0, 0, 0, -121, 0, 0, 0, -11, -1, -1, -1, -34, -1, -1, -1, -87, 0, 0, 0, -23, -1, -1, -1, -34, -1, -1, -1, -45, -1, -1, -1, -11, -1, -1, -1, -23, -1, -1, -1, 0, 0, 0, 0, -87, 0, 0, 0, -110, 0, 0, 0, -23, -1, -1, -1, -76, 0, 0, 0, 0, 0, 0, 0, -45, -1, -1, -1, 0, 0, 0, 0, -121, 0, 0, 0, 0, 0, 0, 0, 11, 0, 0, 0, -11, -1, -1, -1, -45, -1, -1, -1, -45, -1, -1, -1, -121, 0, 0, 0, -11, -1, -1, -1, -11, -1, -1, -1, -11, -1, -1, -1, 0, 0, 0, 0, -45, -1, -1, -1, -45, -1, -1, -1, 87, -1, -1, -1, 11, 0, 0, 0, -11, -1, -1, -1, 22, 0, 0, 0, 45, 0, 0, 0, 34, 0, 0, 0, 45, 0, 0, 0, -108, 0, 0, 0, -108, 0, 0, 0, -106, 0, 0, 0, -105, 0, 0, 0, -105, 0, 0, 0, -105, 0, 0, 0, -105, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 127, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -128, 114, 20, 8, 80, 127, 0, 0, -128, 0, 0, 8, 80, 127, 0, 0, -106, 0, 0, 0, -106, 0, 0, 0, -106, 0, 0, 0, -106, 0, 0, 0, -106, 0, 0, 0, -106, 0, 0, 0, -106, 0, 0, 0, -106, 0, 0, 0, -106, 0, 0, 0, -106, 0, 0, 0, -106, 0, 0, 0, 0, 0, 0, 0, -32, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 80, 8, 19, 8, 80, 127, 0, 0, 64, 1, 0, 0, 82, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 80, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 127, 0, 0, 65, 1, 0, 0, 0, 0, 0, 0, -80, 76, 18, 8, 80, 127, 0, 0, 64, 73, 18, 8, 80, 127, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 80, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 96, 116, 20, 8, 80, 127, 0, 0, -105, 0, 0, 0, -105, 0, 0, 0, -105, 0, 0, 0, -105, 0, 0, 0, -104, 0, 0, 0, -104, 0, 0, 0, -105, 0, 0, 0, -105, 0, 0, 0, -105, 0, 0, 0, -105, 0, 0, 0, -104, 0, 0, 0, -103, 0, 0, 0, -104, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -32, 113, 20, 8, 80, 127, 0, 0, -101, 0, 0, 0, -102, 0, 0, 0, -103, 0, 0, 0, -104, 0, 0, 0, -105, 0, 0, 0, -106, 0, 0, 0, -107, 0, 0, 0, -108, 0, 0, 0, -109, 0, 0, 0, -110, 0, 0, 0, -111, 0, 0, 0, -112, 0, 0, 0, -113, 0, 0, 0, 0, 0, 0, 0, -16, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 32, 75, 18, 8, 80, 127, 0, 0, 46, 1, 0, 0, 62, 0, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 80, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 112, 3, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -64, 53, 20, 8, 80, 127, 0, 0, 52, 1, 0, 0]\"},{\"usuarioId\":67,\"nome\":\"MARILIA\",\"perfil\":\"ROLE_FIN\"},{\"usuarioId\":47,\"nome\":\"MARINEIDE\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":85,\"nome\":\"MATHEUS\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":23,\"nome\":\"MATIAS\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":61,\"nome\":\"MAURICIO\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":22,\"nome\":\"MEIRE\",\"perfil\":\"ROLE_ADM\"},{\"usuarioId\":46,\"nome\":\"MELO\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":79,\"nome\":\"MIGUEL\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":58,\"nome\":\"ROBERTO\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":1,\"nome\":\"RODRIGO\",\"perfil\":\"ROLE_ADM\"},{\"usuarioId\":74,\"nome\":\"RUTTYSON\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":15,\"nome\":\"SILVANA\",\"perfil\":\"ROLE_ADM\"},{\"usuarioId\":69,\"nome\":\"SIMPLETEC\",\"perfil\":\"ROLE_FIN\"},{\"usuarioId\":64,\"nome\":\"THIBERIO\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":76,\"nome\":\"USERTESTE\",\"foto\":\"123\",\"perfil\":\"ROLE_ADM\",\"digital\":\"[70, 80, 50, 2, 0, 0, 0, 0, 0, 1, 100, 9, 0, 0, 42, 0, 0, 0, 12, 0, 0, 0, 16, 0, 0, 0, 21, 0, 0, 0, 24, 0, 0, 0, 24, 0, 0, 0, 26, 0, 0, 0, 44, 0, 0, 0, 49, 0, 0, 0, 56, 0, 0, 0, 78, 0, 0, 0, 110, 0, 0, 0, 116, 0, 0, 0, 118, 0, 0, 0, -125, 0, 0, 0, -123, 0, 0, 0, -100, 0, 0, 0, -99, 0, 0, 0, -95, 0, 0, 0, -95, 0, 0, 0, -89, 0, 0, 0, -88, 0, 0, 0, -78, 0, 0, 0, -73, 0, 0, 0, -60, 0, 0, 0, -55, 0, 0, 0, -44, 0, 0, 0, -39, 0, 0, 0, -31, 0, 0, 0, -24, 0, 0, 0, -5, 0, 0, 0, -1, 0, 0, 0, 1, 1, 0, 0, 18, 1, 0, 0, 43, 1, 0, 0, 61, 1, 0, 0, 75, 1, 0, 0, 85, 1, 0, 0, 87, 1, 0, 0, 96, 1, 0, 0, 108, 1, 0, 0, 109, 1, 0, 0, 113, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 126, 0, 0, 0, 105, 0, 0, 0, -3, 0, 0, 0, 79, 0, 0, 0, -89, 0, 0, 0, 21, 0, 0, 0, 78, 0, 0, 0, 52, 0, 0, 0, -40, 0, 0, 0, -24, 0, 0, 0, 122, 0, 0, 0, -102, 0, 0, 0, -11, 0, 0, 0, 76, 0, 0, 0, -95, 0, 0, 0, 25, 0, 0, 0, -61, 0, 0, 0, 107, 0, 0, 0, -83, 0, 0, 0, 19, 1, 0, 0, 10, 1, 0, 0, -19, 0, 0, 0, 11, 1, 0, 0, -64, 0, 0, 0, 120, 0, 0, 0, -38, 0, 0, 0, -9, 0, 0, 0, -80, 0, 0, 0, 37, 0, 0, 0, -20, 0, 0, 0, -39, 0, 0, 0, 12, 1, 0, 0, 51, 0, 0, 0, 79, 0, 0, 0, 110, 0, 0, 0, -33, 0, 0, 0, -65, 0, 0, 0, 14, 1, 0, 0, -1, 0, 0, 0, 9, 0, 0, 0, -27, 0, 0, 0, -54, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 124, 0, 0, 0, -110, 0, 0, 0, -76, 0, 0, 0, -110, 0, 0, 0, -23, -1, -1, -1, 124, 0, 0, 0, 67, 0, 0, 0, -87, 0, 0, 0, -34, -1, -1, -1, -45, -1, -1, -1, -23, -1, -1, -1, -23, -1, -1, -1, -56, -1, -1, -1, -99, 0, 0, 0, -99, 0, 0, 0, -110, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -11, -1, -1, -1, -99, 0, 0, 0, -99, 0, 0, 0, -87, 0, 0, 0, -34, -1, -1, -1, -11, -1, -1, -1, -87, 0, 0, 0, -76, 0, 0, 0, 110, -1, -1, -1, -76, 0, 0, 0, -99, 0, 0, 0, 98, -1, -1, -1, 87, -1, -1, -1, -124, -1, -1, -1, -99, 0, 0, 0, -87, 0, 0, 0, -76, 0, 0, 0, 98, -1, -1, -1, 87, -1, -1, -1, 56, 0, 0, 0, 34, 0, 0, 0, -11, -1, -1, -1, 56, 0, 0, 0, 56, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 3, 0, 0, 0, 3, 0, 0, 0, 2, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 3, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 4, 0, 0, 0, 100, 9, 0, 0, 56, 0, 0, 0, 12, 0, 0, 0, 12, 0, 0, 0, 16, 0, 0, 0, 17, 0, 0, 0, 25, 0, 0, 0, 27, 0, 0, 0, 30, 0, 0, 0, 39, 0, 0, 0, 64, 0, 0, 0, 81, 0, 0, 0, 88, 0, 0, 0, 102, 0, 0, 0, 114, 0, 0, 0, 121, 0, 0, 0, 122, 0, 0, 0, -123, 0, 0, 0, -114, 0, 0, 0, -109, 0, 0, 0, -106, 0, 0, 0, -96, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -84, 0, 0, 0, -84, 0, 0, 0, -79, 0, 0, 0, -79, 0, 0, 0, -69, 0, 0, 0, -60, 0, 0, 0, -60, 0, 0, 0, -56, 0, 0, 0, -47, 0, 0, 0, -43, 0, 0, 0, -37, 0, 0, 0, -36, 0, 0, 0, -34, 0, 0, 0, -31, 0, 0, 0, -31, 0, 0, 0, -17, 0, 0, 0, -15, 0, 0, 0, -8, 0, 0, 0, 7, 1, 0, 0, 10, 1, 0, 0, 12, 1, 0, 0, 20, 1, 0, 0, 24, 1, 0, 0, 30, 1, 0, 0, 62, 1, 0, 0, 74, 1, 0, 0, 86, 1, 0, 0, 89, 1, 0, 0, 90, 1, 0, 0, 95, 1, 0, 0, 113, 1, 0, 0, 113, 1, 0, 0, 113, 1, 0, 0, 113, 1, 0, 0, 0, 0, 0, 0, -128, 81, 73, 4, -55, 127, 0, 0, -105, 0, 0, 0, -104, 0, 0, 0, -104, 0, 0, 0, -104, 0, 0, 0, -104, 0, 0, 0, -106, 0, 0, 0, -106, 0, 0, 0, -105, 0, 0, 0, -105, 0, 0, 0, -105, 0, 0, 0, -104, 0, 0, 0, -104, 0, 0, 0, -103, 0, 0, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -96, -64, 73, 4, -55, 127, 0, 0, 105, 1, 0, 0, -104, 0, 0, 0, 4, 0, 0, 0, 20, 1, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 21, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 48, 44, 79, 4, -55, 127, 0, 0, -32, -95, 73, 4, -55, 127, 0, 0, 17, 0, 0, 0, 90, 0, 0, 0, -82, 71, -31, 122, 20, -82, -17, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 112, -79, 52, 4, -55, 127, 0, 0, -116, 0, 0, 0, 17, 1, 0, 0, 10, 0, 0, 0, -55, 127, 0, 0, -82, 71, -31, 122, 20, -82, -17, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -127, 7, 0, 0, 0, 0, 0, 0, -96, 47, 79, 4, -55, 127, 0, 0, 32, 31, 81, 4, -55, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -61, 0, 0, 0, -61, 0, 0, 0, -61, 0, 0, 0, -62, 0, 0, 0, -62, 0, 0, 0, -62, 0, 0, 0, -62, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -64, -82, 81, 4, -55, 127, 0, 0, -62, 0, 0, 0, -63, 0, 0, 0, -63, 0, 0, 0, -63, 0, 0, 0, -63, 0, 0, 0, -63, 0, 0, 0, -63, 0, 0, 0, -63, 0, 0, 0, -63, 0, 0, 0, -63, 0, 0, 0, -63, 0, 0, 0, -62, 0, 0, 0, -62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -32, -93, 52, 4, -55, 127, 0, 0, -32, 40, 79, 4, -55, 127, 0, 0, -3, 0, 0, 0, -3, 0, 0, 0, -2, 0, 0, 0, -2, 0, 0, 0, -3, 0, 0, 0, -3, 0, 0, 0, -3, 0, 0, 0, -2, 0, 0, 0, -2, 0, 0, 0, -1, 0, 0, 0, -1, 0, 0, 0, 0, 0, 0, 0, -16, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 64, -8, 73, 4, -55, 127, 0, 0, 42, 0, 0, 0, -113, 0, 0, 0, -66, 0, 0, 0, -6, 0, 0, 0, -96, 0, 0, 0, 7, 1, 0, 0, 98, 0, 0, 0, -60, 0, 0, 0, -13, 0, 0, 0, -104, 0, 0, 0, -82, 0, 0, 0, -13, 0, 0, 0, -14, 0, 0, 0, -26, 0, 0, 0, 50, 0, 0, 0, -66, 0, 0, 0, 85, 0, 0, 0, 96, 0, 0, 0, 9, 1, 0, 0, -27, 0, 0, 0, 7, 1, 0, 0, -121, 0, 0, 0, -3, 0, 0, 0, 110, 0, 0, 0, -32, 0, 0, 0, 35, 0, 0, 0, -41, 0, 0, 0, -74, 0, 0, 0, -45, 0, 0, 0, -38, 0, 0, 0, -15, 0, 0, 0, -121, 0, 0, 0, -6, 0, 0, 0, 52, 0, 0, 0, -36, 0, 0, 0, -94, 0, 0, 0, -64, 0, 0, 0, 6, 1, 0, 0, 114, 0, 0, 0, -26, 0, 0, 0, -35, 0, 0, 0, -74, 0, 0, 0, -38, 0, 0, 0, -94, 0, 0, 0, -14, 0, 0, 0, 18, 1, 0, 0, -32, 0, 0, 0, -16, 0, 0, 0, 10, 0, 0, 0, -21, 0, 0, 0, -81, 0, 0, 0, 49, 0, 0, 0, 8, 1, 0, 0, -98, 0, 0, 0, -85, 0, 0, 0, -72, 0, 0, 0, -57, 0, 0, 0, 0, 0, 0, 0, 48, 49, 79, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -31, 1, 0, 0, 0, 0, 0, 0, 48, 49, 79, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -57, 0, 0, 0, -57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 48, 49, 79, 4, -55, 127, 0, 0, -48, 86, 73, 4, -55, 127, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -112, 85, 73, 4, -55, 127, 0, 0, -57, 0, 0, 0, -57, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -57, 0, 0, 0, -57, 0, 0, 0, -57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 48, 49, 79, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, -57, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -57, 0, 0, 0, -57, 0, 0, 0, -57, 0, 0, 0, -57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, -16, 89, 73, 4, -55, 127, 0, 0, 16, 88, 73, 4, -55, 127, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 96, 88, 73, 4, -55, 127, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, 0, 0, 0, 0, -128, 2, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -96, -85, 81, 4, -55, 127, 0, 0, 62, 0, 0, 0, 124, 0, 0, 0, 124, 0, 0, 0, 90, 0, 0, 0, 112, 0, 0, 0, 90, 0, 0, 0, -34, -1, -1, -1, -56, -1, -1, -1, -90, -1, -1, -1, -45, -1, -1, -1, -45, -1, -1, -1, -101, -1, -1, -1, -101, -1, -1, -1, -101, -1, -1, -1, -23, -1, -1, -1, -56, -1, -1, -1, -23, -1, -1, -1, -99, 0, 0, 0, 110, -1, -1, -1, -113, -1, -1, -1, 87, -1, -1, -1, -99, 0, 0, 0, 87, -1, -1, -1, -11, -1, -1, -1, -87, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -87, 0, 0, 0, -11, -1, -1, -1, -45, -1, -1, -1, -45, -1, -1, -1, 0, 0, 0, 0, -45, -1, -1, -1, -87, 0, 0, 0, -90, -1, -1, -1, -76, 0, 0, 0, 110, -1, -1, -1, -56, -1, -1, -1, -76, 0, 0, 0, -90, -1, -1, -1, -113, -1, -1, -1, 98, -1, -1, -1, -124, -1, -1, -1, 87, -1, -1, -1, -101, -1, -1, -1, 112, 0, 0, 0, -124, -1, -1, -1, -101, -1, -1, -1, -76, 0, 0, 0, -113, -1, -1, -1, 110, -1, -1, -1, -76, 0, 0, 0, 101, 0, 0, 0, 67, 0, 0, 0, 67, 0, 0, 0, 67, 0, 0, 0, 67, 0, 0, 0, 0, 0, 0, 0, 32, 31, 81, 4, -55, 127, 0, 0, -16, 89, 73, 4, -55, 127, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, 32, 31, 81, 4, -55, 127, 0, 0, 64, 90, 73, 4, -55, 127, 0, 0, -39, 0, 0, 0, -39, 0, 0, 0, -39, 0, 0, 0, -39, 0, 0, 0, -39, 0, 0, 0, -39, 0, 0, 0, -39, 0, 0, 0, -39, 0, 0, 0, -40, 0, 0, 0, -40, 0, 0, 0, -40, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 64, 90, 73, 4, -55, 127, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, 0, 0, 0, 0, -112, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -128, 93, 79, 4, -55, 127, 0, 0, 67, 0, 0, 0, 68, 0, 0, 0, 69, 0, 0, 0, 70, 0, 0, 0, 71, 0, 0, 0, 72, 0, 0, 0, 73, 0, 0, 0, 74, 0, 0, 0, 75, 0, 0, 0, 76, 0, 0, 0, 77, 0, 0, 0, 78, 0, 0, 0, 79, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, 32, 31, 81, 4, -55, 127, 0, 0, 64, 85, 73, 4, -55, 127, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 32, 31, 81, 4, -55, 127, 0, 0, -80, 88, 73, 4, -55, 127, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -51, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, 0, 0, 0, 0, 80, 5, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 16, 32, 81, 4, -55, 127, 0, 0, 49, 0, 0, 0, 50, 0, 0, 0, 51, 0, 0, 0, 52, 0, 0, 0, 53, 0, 0, 0, 54, 0, 0, 0, 55, 0, 0, 0, 56, 0, 0, 0, 57, 0, 0, 0, 58, 0, 0, 0, 59, 0, 0, 0, 60, 0, 0, 0, 61, 0, 0, 0, 0, 0, 0, 0, -96, 5, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -32, -46, 73, 4, -55, 127, 0, 0, 20, 0, 0, 0, 100, 9, 0, 0, 56, 0, 0, 0, 12, 0, 0, 0, 12, 0, 0, 0, 12, 0, 0, 0, 12, 0, 0, 0, 17, 0, 0, 0, 25, 0, 0, 0, 42, 0, 0, 0, 64, 0, 0, 0, 65, 0, 0, 0, 68, 0, 0, 0, 73, 0, 0, 0, 76, 0, 0, 0, 99, 0, 0, 0, 114, 0, 0, 0, 118, 0, 0, 0, -126, 0, 0, 0, -111, 0, 0, 0, -99, 0, 0, 0, -84, 0, 0, 0, -81, 0, 0, 0, -79, 0, 0, 0, -71, 0, 0, 0, -66, 0, 0, 0, -65, 0, 0, 0, -64, 0, 0, 0, -57, 0, 0, 0, -53, 0, 0, 0, -52, 0, 0, 0, -48, 0, 0, 0, -47, 0, 0, 0, -38, 0, 0, 0, -34, 0, 0, 0, -33, 0, 0, 0, -29, 0, 0, 0, -28, 0, 0, 0, -23, 0, 0, 0, -16, 0, 0, 0, -6, 0, 0, 0, -6, 0, 0, 0, -4, 0, 0, 0, 2, 1, 0, 0, 4, 1, 0, 0, 11, 1, 0, 0, 15, 1, 0, 0, 23, 1, 0, 0, 28, 1, 0, 0, 40, 1, 0, 0, 40, 1, 0, 0, 47, 1, 0, 0, 49, 1, 0, 0, 58, 1, 0, 0, 90, 1, 0, 0, 100, 1, 0, 0, 113, 1, 0, 0, 113, 1, 0, 0, 114, 1, 0, 0, 0, 0, 0, 0, -96, -109, 81, 4, -55, 127, 0, 0, 89, 1, 0, 0, -72, 0, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, -82, 71, -31, 122, 20, -82, -17, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 29, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 75, 11, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -128, -83, 81, 4, -55, 127, 0, 0, 32, 54, 81, 4, -55, 127, 0, 0, 34, 0, 0, 0, 33, 0, 0, 0, 33, 0, 0, 0, 33, 0, 0, 0, 33, 0, 0, 0, 33, 0, 0, 0, 33, 0, 0, 0, 34, 0, 0, 0, 34, 0, 0, 0, 34, 0, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -29, 4, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, -112, 95, 73, 4, -55, 127, 0, 0, 48, 81, 73, 4, -55, 127, 0, 0, 42, 0, 0, 0, 41, 0, 0, 0, 40, 0, 0, 0, 39, 0, 0, 0, 38, 0, 0, 0, 37, 0, 0, 0, 36, 0, 0, 0, 35, 0, 0, 0, 34, 0, 0, 0, 33, 0, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 77, 35, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -128, -124, 73, 4, -55, 127, 0, 0, 80, -90, 81, 4, -55, 127, 0, 0, -4, 0, 0, 0, -3, 0, 0, 0, -3, 0, 0, 0, -3, 0, 0, 0, -3, 0, 0, 0, -3, 0, 0, 0, -4, 0, 0, 0, -4, 0, 0, 0, -4, 0, 0, 0, -4, 0, 0, 0, -3, 0, 0, 0, 0, 0, 0, 0, 48, 2, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -32, 100, 73, 4, -55, 127, 0, 0, 82, 1, 0, 0, 11, 1, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, -82, 71, -31, 122, 20, -82, -17, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -30, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 125, 22, 0, 0, 1, 10, 0, 0, 0, 0, 0, 0, 112, 31, 81, 4, -55, 127, 0, 0, -128, -83, 81, 4, -55, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 0, 0, 122, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -96, -9, 73, 4, -55, 127, 0, 0, -75, 0, 0, 0, 9, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, -82, 71, -31, 122, 20, -82, -17, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 126, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -84, 9, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -48, -102, 81, 4, -55, 127, 0, 0, 112, -84, 73, 4, -128, 0, 0, 0, -117, 0, 0, 0, -105, 0, 0, 0, -84, 0, 0, 0, -7, 0, 0, 0, -81, 0, 0, 0, -20, 0, 0, 0, -58, 0, 0, 0, -92, 0, 0, 0, -13, 0, 0, 0, 12, 1, 0, 0, 103, 0, 0, 0, -96, 0, 0, 0, -12, 0, 0, 0, -77, 0, 0, 0, -8, 0, 0, 0, -29, 0, 0, 0, -60, 0, 0, 0, 14, 1, 0, 0, 59, 0, 0, 0, 94, 0, 0, 0, 13, 1, 0, 0, 3, 1, 0, 0, 103, 0, 0, 0, -26, 0, 0, 0, -30, 0, 0, 0, -22, 0, 0, 0, 9, 0, 0, 0, -33, 0, 0, 0, -113, 0, 0, 0, 118, 0, 0, 0, -65, 0, 0, 0, -34, 0, 0, 0, -29, 0, 0, 0, -4, 0, 0, 0, 45, 0, 0, 0, 2, 1, 0, 0, -26, 0, 0, 0, 15, 1, 0, 0, -113, 0, 0, 0, -54, 0, 0, 0, -84, 0, 0, 0, 64, 0, 0, 0, -16, 0, 0, 0, -27, 0, 0, 0, 126, 0, 0, 0, -62, 0, 0, 0, -28, 0, 0, 0, -3, 0, 0, 0, -83, 0, 0, 0, -26, 0, 0, 0, -5, 0, 0, 0, 12, 1, 0, 0, -49, 0, 0, 0, -27, 0, 0, 0, -82, 0, 0, 0, 0, 0, 0, 0, 32, -39, 73, 4, -55, 127, 0, 0, -59, 0, 0, 0, -59, 0, 0, 0, -59, 0, 0, 0, -59, 0, 0, 0, -60, 0, 0, 0, -60, 0, 0, 0, -60, 0, 0, 0, -59, 0, 0, 0, -59, 0, 0, 0, -59, 0, 0, 0, -59, 0, 0, 0, -59, 0, 0, 0, -59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -34, 20, 0, 0, -47, 7, 0, 0, 0, 0, 0, 0, -16, -109, 81, 4, -55, 127, 0, 0, -64, -66, 39, 4, -55, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -57, 0, 0, 0, -57, 0, 0, 0, -57, 0, 0, 0, -57, 0, 0, 0, -57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 117, 14, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -16, -109, 81, 4, -55, 127, 0, 0, -64, -79, 73, 4, -55, 127, 0, 0, -48, 0, 0, 0, -47, 0, 0, 0, -47, 0, 0, 0, -47, 0, 0, 0, -47, 0, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -47, 0, 0, 0, -47, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -64, -66, 39, 4, -55, 127, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -59, 0, 0, 0, -59, 0, 0, 0, -59, 0, 0, 0, -59, 0, 0, 0, -59, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, -58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -91, 1, 0, 0, -31, 6, 0, 0, 0, 0, 0, 0, -16, -109, 81, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -54, 0, 0, 0, -54, 0, 0, 0, -55, 0, 0, 0, -55, 0, 0, 0, -55, 0, 0, 0, -55, 0, 0, 0, -55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 32, 0, 0, -111, 6, 0, 0, 0, 0, 0, 0, 32, -79, 73, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, -52, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -64, -79, 73, 4, -55, 127, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, 19, 0, 0, -15, 5, 0, 0, 0, 0, 0, 0, 32, -79, 73, 4, -55, 127, 0, 0, -128, 0, 0, 4, 124, 0, 0, 0, 124, 0, 0, 0, 124, 0, 0, 0, 124, 0, 0, 0, 112, 0, 0, 0, -34, -1, -1, -1, -56, -1, -1, -1, -45, -1, -1, -1, -34, -1, -1, -1, -79, -1, -1, -1, 79, 0, 0, 0, -23, -1, -1, -1, -34, -1, -1, -1, -90, -1, -1, -1, -45, -1, -1, -1, -90, -1, -1, -1, -90, -1, -1, -1, -56, -1, -1, -1, 121, -1, -1, -1, -11, -1, -1, -1, -23, -1, -1, -1, 98, -1, -1, -1, 110, -1, -1, -1, -87, 0, 0, 0, -68, -1, -1, -1, -121, 0, 0, 0, -110, 0, 0, 0, -87, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -11, -1, -1, -1, -87, 0, 0, 0, -34, -1, -1, -1, -34, -1, -1, -1, -34, -1, -1, -1, -87, 0, 0, 0, -45, -1, -1, -1, -90, -1, -1, -1, -56, -1, -1, -1, 0, 0, 0, 0, 110, -1, -1, -1, 87, -1, -1, -1, -87, 0, 0, 0, -90, -1, -1, -1, -101, -1, -1, -1, -76, 0, 0, 0, 98, -1, -1, -1, -113, -1, -1, -1, -90, -1, -1, -1, 87, -1, -1, -1, -113, -1, -1, -1, -90, -1, -1, -1, 112, 0, 0, 0, 67, 0, 0, 0, 67, 0, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 32, -106, 81, 4, -55, 127, 0, 0, 48, -80, 73, 4, -55, 127, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -43, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 64, -76, 73, 4, -55, 127, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, 0, 0, 0, 0, -16, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 32, -79, 73, 4, -55, 127, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -49, 0, 0, 0, -49, 0, 0, 0, -49, 0, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -96, 17, 0, 0, 17, 4, 0, 0, 0, 0, 0, 0, -16, -109, 81, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -45, 0, 0, 0, -45, 0, 0, 0, -45, 0, 0, 0, -45, 0, 0, 0, -45, 0, 0, 0, -45, 0, 0, 0, -45, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -16, -77, 73, 4, -55, 127, 0, 0, -47, 0, 0, 0, -47, 0, 0, 0, -47, 0, 0, 0, -47, 0, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -48, 0, 0, 0, -47, 0, 0, 0, -47, 0, 0, 0, -47, 0, 0, 0, -47, 0, 0, 0, -47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -49, 4, 0, 0, 113, 3, 0, 0, 0, 0, 0, 0, -16, -109, 81, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, -45, 0, 0, 0, -45, 0, 0, 0, -45, 0, 0, 0, -45, 0, 0, 0, -45, 0, 0, 0, -45, 0, 0, 0, -45, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -47, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -112, -76, 73, 4, -55, 127, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -47, 0, 0, 0, -47, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, -46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59, 27, 0, 0, -47, 2, 0, 0, 0, 0, 0, 0, -16, -109, 81, 4, -55, 127, 0, 0, -128, 0, 0, 4, 100, 9, 0, 0, 35, 0, 0, 0, 12, 0, 0, 0, 15, 0, 0, 0, 15, 0, 0, 0, 16, 0, 0, 0, 23, 0, 0, 0, 28, 0, 0, 0, 30, 0, 0, 0, 33, 0, 0, 0, 36, 0, 0, 0, 46, 0, 0, 0, 53, 0, 0, 0, 63, 0, 0, 0, 70, 0, 0, 0, 71, 0, 0, 0, 72, 0, 0, 0, 88, 0, 0, 0, 92, 0, 0, 0, 95, 0, 0, 0, 98, 0, 0, 0, 121, 0, 0, 0, 123, 0, 0, 0, -126, 0, 0, 0, -123, 0, 0, 0, -118, 0, 0, 0, -99, 0, 0, 0, -80, 0, 0, 0, -54, 0, 0, 0, -46, 0, 0, 0, -37, 0, 0, 0, -32, 0, 0, 0, -26, 0, 0, 0, -16, 0, 0, 0, 1, 1, 0, 0, 10, 1, 0, 0, 105, 1, 0, 0, 65, 1, 0, 0, 0, 0, 0, 0, -16, 110, 73, 4, -55, 127, 0, 0, -16, -64, 39, 4, -55, 127, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -25, 20, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -48, -77, 73, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, -99, 0, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, -98, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 64, -108, 73, 4, -55, 127, 0, 0, -100, 0, 0, 0, -100, 0, 0, 0, -100, 0, 0, 0, -100, 0, 0, 0, -100, 0, 0, 0, -100, 0, 0, 0, -100, 0, 0, 0, -100, 0, 0, 0, -100, 0, 0, 0, -100, 0, 0, 0, -100, 0, 0, 0, -100, 0, 0, 0, -100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 8, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -16, 120, 73, 4, -55, 127, 0, 0, -32, 116, 73, 4, -55, 127, 0, 0, -55, 0, 0, 0, -55, 0, 0, 0, -55, 0, 0, 0, -55, 0, 0, 0, -55, 0, 0, 0, -55, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, -56, 0, 0, 0, 0, 0, 0, 0, -32, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 0, -85, 81, 4, -55, 127, 0, 0, 95, 1, 0, 0, -96, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 126, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 2, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 32, -106, 73, 4, -55, 127, 0, 0, 104, 1, 0, 0, -96, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 124, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -128, 2, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -32, -104, 81, 4, -55, 127, 0, 0, 111, 1, 0, 0, -96, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 21, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -81, 17, 0, 0, -127, 7, 0, 0, 0, 0, 0, 0, 96, -80, 73, 4, -55, 127, 0, 0, -48, -82, 73, 4, -55, 127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -96, 0, 0, 0, -96, 0, 0, 0, -95, 0, 0, 0, -95, 0, 0, 0, -94, 0, 0, 0, -94, 0, 0, 0, -94, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 71, 11, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 96, -105, 73, 4, -55, 127, 0, 0, -128, 0, 0, 4, 115, 0, 0, 0, -89, 0, 0, 0, -82, 0, 0, 0, -74, 0, 0, 0, -12, 0, 0, 0, -74, 0, 0, 0, 88, 0, 0, 0, -98, 0, 0, 0, 10, 1, 0, 0, 12, 1, 0, 0, -29, 0, 0, 0, 0, 1, 0, 0, 103, 0, 0, 0, -74, 0, 0, 0, 1, 1, 0, 0, -49, 0, 0, 0, 8, 1, 0, 0, -20, 0, 0, 0, -95, 0, 0, 0, 6, 1, 0, 0, 21, 0, 0, 0, -31, 0, 0, 0, -50, 0, 0, 0, 3, 1, 0, 0, 41, 0, 0, 0, 67, 0, 0, 0, 103, 0, 0, 0, -40, 0, 0, 0, -71, 0, 0, 0, 10, 0, 0, 0, 14, 0, 0, 0, 27, 0, 0, 0, -125, 0, 0, 0, 68, 0, 0, 0, 6, 1, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -96, -104, 73, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -93, 0, 0, 0, -93, 0, 0, 0, -93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 69, 27, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, -64, -101, 73, 4, -55, 127, 0, 0, -96, -104, 73, 4, -55, 127, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -50, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, -51, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -96, -104, 73, 4, -55, 127, 0, 0, -93, 0, 0, 0, -93, 0, 0, 0, -93, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -93, 0, 0, 0, -93, 0, 0, 0, -93, 0, 0, 0, -93, 0, 0, 0, -93, 0, 0, 0, 0, 0, 0, 0, -32, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 112, -92, 81, 4, -55, 127, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 112, 0, 0, 0, 111, 0, 0, 0, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 8, 0, 0, -127, 2, 0, 0, 0, 0, 0, 0, -64, -101, 73, 4, -55, 127, 0, 0, 64, -108, 73, 4, -55, 127, 0, 0, -94, 0, 0, 0, -94, 0, 0, 0, -94, 0, 0, 0, -94, 0, 0, 0, -94, 0, 0, 0, -94, 0, 0, 0, -94, 0, 0, 0, -94, 0, 0, 0, -94, 0, 0, 0, -94, 0, 0, 0, -94, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -89, 1, 0, 0, 49, 2, 0, 0, 0, 0, 0, 0, -64, -101, 73, 4, -55, 127, 0, 0, -80, -105, 73, 4, -55, 127, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -93, 0, 0, 0, -93, 0, 0, 0, -93, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -92, 0, 0, 0, -91, 0, 0, 0, -91, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 116, 30, 0, 0, -31, 1, 0, 0, 0, 0, 0, 0, -64, -101, 73, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, -90, 0, 0, 0, -90, 0, 0, 0, -90, 0, 0, 0, -90, 0, 0, 0, -90, 0, 0, 0, -90, 0, 0, 0, -90, 0, 0, 0, -90, 0, 0, 0, -90, 0, 0, 0, -90, 0, 0, 0, -90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 24, 0, 0, -111, 1, 0, 0, 0, 0, 0, 0, -64, -101, 73, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -91, 17, 0, 0, 65, 1, 0, 0, 0, 0, 0, 0, -64, -101, 73, 4, -55, 127, 0, 0, -128, 0, 0, 4, -121, 0, 0, 0, -121, 0, 0, 0, -121, 0, 0, 0, -121, 0, 0, 0, 67, 0, 0, 0, -99, 0, 0, 0, -110, 0, 0, 0, -11, -1, -1, -1, -87, 0, 0, 0, -87, 0, 0, 0, -99, 0, 0, 0, -34, -1, -1, -1, -87, 0, 0, 0, -11, -1, -1, -1, -68, -1, -1, -1, -76, 0, 0, 0, -101, -1, -1, -1, 110, -1, -1, -1, -76, 0, 0, 0, -124, -1, -1, -1, -87, 0, 0, 0, 98, -1, -1, -1, 87, -1, -1, -1, 121, -1, -1, -1, -87, 0, 0, 0, -76, 0, 0, 0, 87, -1, -1, -1, 98, -1, -1, -1, 87, -1, -1, -1, -76, 0, 0, 0, 0, 0, 0, 0, -76, 0, 0, 0, 87, -1, -1, -1, 87, -1, -1, -1, -90, -1, -1, -1, -95, 0, 0, 0, 0, 0, 0, 0, -64, -101, 73, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, -87, 0, 0, 0, -87, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -88, 0, 0, 0, -87, 0, 0, 0, -87, 0, 0, 0, -86, 0, 0, 0, -86, 0, 0, 0, -86, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 111, -2, -1, -1, 81, 0, 0, 0, 0, 0, 0, 0, -64, -101, 73, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, -86, 0, 0, 0, -85, 0, 0, 0, -85, 0, 0, 0, -85, 0, 0, 0, -85, 0, 0, 0, -85, 0, 0, 0, -86, 0, 0, 0, -86, 0, 0, 0, -86, 0, 0, 0, -86, 0, 0, 0, -86, 0, 0, 0, 0, 0, 0, 0, -80, 4, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -112, 116, 73, 4, -55, 127, 0, 0, 47, 1, 0, 0, -121, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -44, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -44, 20, 0, 0, -127, 2, 0, 0, 0, 0, 0, 0, 96, -65, 39, 4, -55, 127, 0, 0, -96, -109, 73, 4, -55, 127, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 108, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 108, 14, 0, 0, 49, 2, 0, 0, 0, 0, 0, 0, 96, -112, 73, 4, -55, 127, 0, 0, -16, -121, 73, 4, -55, 127, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 8, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, -64, -81, 73, 4, -55, 127, 0, 0, 0, -99, 73, 4, -55, 127, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -99, 1, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -64, -81, 73, 4, -55, 127, 0, 0, 80, -99, 73, 4, -55, 127, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, -82, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, -83, 0, 0, 0, 0, 0, 0, 0, -16, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 48, -119, 73, 4, -55, 127, 0, 0, 40, 1, 0, 0, -21, 0, 0, 0, 25, 0, 0, 0, -51, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -16, 110, 73, 4, -55, 127, 0, 0, 64, -108, 73, 4, 100, 9, 0, 0, 51, 0, 0, 0, 13, 0, 0, 0, 15, 0, 0, 0, 16, 0, 0, 0, 16, 0, 0, 0, 16, 0, 0, 0, 17, 0, 0, 0, 22, 0, 0, 0, 23, 0, 0, 0, 24, 0, 0, 0, 25, 0, 0, 0, 37, 0, 0, 0, 47, 0, 0, 0, 47, 0, 0, 0, 58, 0, 0, 0, 59, 0, 0, 0, 60, 0, 0, 0, 66, 0, 0, 0, 78, 0, 0, 0, 89, 0, 0, 0, 90, 0, 0, 0, 94, 0, 0, 0, 114, 0, 0, 0, -127, 0, 0, 0, -126, 0, 0, 0, -123, 0, 0, 0, -117, 0, 0, 0, -110, 0, 0, 0, -105, 0, 0, 0, -102, 0, 0, 0, -98, 0, 0, 0, -96, 0, 0, 0, -67, 0, 0, 0, -65, 0, 0, 0, -51, 0, 0, 0, -31, 0, 0, 0, -2, 0, 0, 0, 5, 1, 0, 0, 12, 1, 0, 0, 12, 1, 0, 0, 19, 1, 0, 0, 33, 1, 0, 0, 36, 1, 0, 0, 39, 1, 0, 0, 39, 1, 0, 0, 39, 1, 0, 0, 39, 1, 0, 0, 43, 1, 0, 0, 47, 1, 0, 0, 50, 1, 0, 0, 66, 1, 0, 0, 106, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -92, 1, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 32, -91, 73, 4, -55, 127, 0, 0, -112, 21, 102, 4, -55, 127, 0, 0, -32, 0, 0, 0, -31, 0, 0, 0, -31, 0, 0, 0, -31, 0, 0, 0, -31, 0, 0, 0, -31, 0, 0, 0, -32, 0, 0, 0, -32, 0, 0, 0, -32, 0, 0, 0, -32, 0, 0, 0, -32, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -112, -63, 73, 4, -55, 127, 0, 0, -33, 0, 0, 0, -33, 0, 0, 0, -34, 0, 0, 0, -34, 0, 0, 0, -34, 0, 0, 0, -34, 0, 0, 0, -34, 0, 0, 0, -34, 0, 0, 0, -34, 0, 0, 0, -34, 0, 0, 0, -33, 0, 0, 0, -33, 0, 0, 0, -33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 70, 3, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 0, 20, 102, 4, -55, 127, 0, 0, 32, -91, 73, 4, -55, 127, 0, 0, -31, 0, 0, 0, -31, 0, 0, 0, -31, 0, 0, 0, -32, 0, 0, 0, -33, 0, 0, 0, -34, 0, 0, 0, -35, 0, 0, 0, -35, 0, 0, 0, -35, 0, 0, 0, -35, 0, 0, 0, -36, 0, 0, 0, 0, 0, 0, 0, -32, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 96, -90, 73, 4, -55, 127, 0, 0, 63, 1, 0, 0, -32, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 112, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -99, 25, 0, 0, 65, 1, 0, 0, 0, 0, 0, 0, 0, 20, 102, 4, -55, 127, 0, 0, -112, 21, 102, 4, -55, 127, 0, 0, -32, 0, 0, 0, -33, 0, 0, 0, -33, 0, 0, 0, -33, 0, 0, 0, -33, 0, 0, 0, -33, 0, 0, 0, -32, 0, 0, 0, -32, 0, 0, 0, -32, 0, 0, 0, -32, 0, 0, 0, -32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59, 27, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, -80, -90, 73, 4, -55, 127, 0, 0, 16, 14, 102, 4, -55, 127, 0, 0, 14, 1, 0, 0, 14, 1, 0, 0, 13, 1, 0, 0, 12, 1, 0, 0, 12, 1, 0, 0, 13, 1, 0, 0, 13, 1, 0, 0, 15, 1, 0, 0, 15, 1, 0, 0, 15, 1, 0, 0, 15, 1, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 16, 14, 102, 4, -55, 127, 0, 0, -34, 0, 0, 0, -33, 0, 0, 0, -32, 0, 0, 0, -31, 0, 0, 0, -29, 0, 0, 0, -29, 0, 0, 0, -31, 0, 0, 0, -31, 0, 0, 0, -31, 0, 0, 0, -31, 0, 0, 0, -30, 0, 0, 0, -29, 0, 0, 0, -29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 17, 32, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, -80, -90, 73, 4, -55, 127, 0, 0, -128, 0, 0, 4, 69, 0, 0, 0, 110, 0, 0, 0, 99, 0, 0, 0, -118, 0, 0, 0, -15, 0, 0, 0, 25, 0, 0, 0, 125, 0, 0, 0, 72, 0, 0, 0, -104, 0, 0, 0, 116, 0, 0, 0, -116, 0, 0, 0, 23, 0, 0, 0, -85, 0, 0, 0, 8, 1, 0, 0, 22, 1, 0, 0, 93, 0, 0, 0, -79, 0, 0, 0, 42, 0, 0, 0, -44, 0, 0, 0, 123, 0, 0, 0, -67, 0, 0, 0, -2, 0, 0, 0, -44, 0, 0, 0, -119, 0, 0, 0, 19, 1, 0, 0, 11, 0, 0, 0, -21, 0, 0, 0, 15, 1, 0, 0, 7, 1, 0, 0, -64, 0, 0, 0, 57, 0, 0, 0, -5, 0, 0, 0, -24, 0, 0, 0, 71, 0, 0, 0, 95, 0, 0, 0, -127, 0, 0, 0, 25, 0, 0, 0, 40, 0, 0, 0, -20, 0, 0, 0, -49, 0, 0, 0, 40, 0, 0, 0, 126, 0, 0, 0, 81, 0, 0, 0, 101, 0, 0, 0, -125, 0, 0, 0, -119, 0, 0, 0, 70, 0, 0, 0, -83, 0, 0, 0, -57, 0, 0, 0, -122, 0, 0, 0, -121, 0, 0, 0, -26, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 16, -90, 73, 4, -55, 127, 0, 0, -28, 0, 0, 0, -28, 0, 0, 0, -29, 0, 0, 0, -29, 0, 0, 0, -29, 0, 0, 0, -29, 0, 0, 0, -29, 0, 0, 0, -29, 0, 0, 0, -29, 0, 0, 0, -29, 0, 0, 0, -28, 0, 0, 0, -28, 0, 0, 0, -28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -42, 20, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, -32, -88, 73, 4, -55, 127, 0, 0, -128, 0, 0, 4, -55, 127, 0, 0, -24, 0, 0, 0, -24, 0, 0, 0, -24, 0, 0, 0, -24, 0, 0, 0, -24, 0, 0, 0, -24, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -24, 0, 0, 0, -23, 0, 0, 0, 0, 0, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -80, -90, 73, 4, -55, 127, 0, 0, -26, 0, 0, 0, -26, 0, 0, 0, -27, 0, 0, 0, -27, 0, 0, 0, -28, 0, 0, 0, -28, 0, 0, 0, -28, 0, 0, 0, -27, 0, 0, 0, -27, 0, 0, 0, -27, 0, 0, 0, -27, 0, 0, 0, -27, 0, 0, 0, -28, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 96, 41, 73, 4, -55, 127, 0, 0, 50, 1, 0, 0, -27, 0, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -48, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 74, 27, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 80, -84, 73, 4, -55, 127, 0, 0, 0, 20, 102, 4, -55, 127, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -24, 0, 0, 0, -24, 0, 0, 0, -24, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -24, 0, 0, 0, 0, 0, 0, 0, -96, 5, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 0, -84, 73, 4, -55, 127, 0, 0, 35, 0, 0, 0, -10, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, -82, 71, -31, 122, 20, -82, -17, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -90, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 57, 11, 0, 0, 33, 3, 0, 0, 0, 0, 0, 0, 96, 41, 73, 4, -55, 127, 0, 0, 96, -75, 73, 4, -55, 127, 0, 0, -25, 0, 0, 0, -24, 0, 0, 0, -24, 0, 0, 0, -24, 0, 0, 0, -24, 0, 0, 0, -24, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 113, 14, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 80, -84, 73, 4, -55, 127, 0, 0, -64, -86, 73, 4, 101, 0, 0, 0, -101, -1, -1, -1, 90, 0, 0, 0, 121, -1, -1, -1, 124, 0, 0, 0, 112, 0, 0, 0, 34, 0, 0, 0, 112, 0, 0, 0, -110, 0, 0, 0, -110, 0, 0, 0, -23, -1, -1, -1, -99, 0, 0, 0, -23, -1, -1, -1, -45, -1, -1, -1, -121, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -99, 0, 0, 0, -11, -1, -1, -1, -87, 0, 0, 0, 0, 0, 0, 0, -87, 0, 0, 0, 0, 0, 0, 0, -23, -1, -1, -1, -76, 0, 0, 0, 11, 0, 0, 0, 98, -1, -1, -1, -76, 0, 0, 0, -99, 0, 0, 0, 87, -1, -1, -1, 87, -1, -1, -1, -87, 0, 0, 0, -76, 0, 0, 0, -76, 0, 0, 0, -11, -1, -1, -1, -11, -1, -1, -1, 98, -1, -1, -1, 87, -1, -1, -1, 98, -1, -1, -1, 56, 0, 0, 0, 34, 0, 0, 0, 34, 0, 0, 0, 45, 0, 0, 0, 45, 0, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, -76, 0, 0, 0, -90, -1, -1, -1, -90, -1, -1, -1, -32, 0, 0, 0, 0, 0, 0, 0, 64, 1, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 80, -89, 73, 4, -55, 127, 0, 0, -20, 0, 0, 0, -22, 0, 0, 0, -23, 0, 0, 0, -24, 0, 0, 0, -24, 0, 0, 0, -26, 0, 0, 0, -26, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -126, 22, 0, 0, -15, 0, 0, 0, 0, 0, 0, 0, 80, -84, 73, 4, -55, 127, 0, 0, 64, -88, 73, 4, -55, 127, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -26, 0, 0, 0, -26, 0, 0, 0, -26, 0, 0, 0, -26, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -44, 4, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, 80, -84, 73, 4, -55, 127, 0, 0, -32, -88, 73, 4, -55, 127, 0, 0, -26, 0, 0, 0, -26, 0, 0, 0, -26, 0, 0, 0, -26, 0, 0, 0, -26, 0, 0, 0, -26, 0, 0, 0, -26, 0, 0, 0, -26, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, -25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 8, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 80, -84, 73, 4, -55, 127, 0, 0, 48, -87, 73, 4, -55, 127, 0, 0, -23, 0, 0, 0, -23, 0, 0, 0, -23, 0, 0, 0, -23, 0, 0, 0, -23, 0, 0, 0, -23, 0, 0, 0, -24, 0, 0, 0, -26, 0, 0, 0, -27, 0, 0, 0, -27, 0, 0, 0, -27, 0, 0, 0, 0, 0, 0, 0, -128, 2, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -32, 26, 102, 4, -55, 127, 0, 0, 55, 1, 0, 0, -24, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -32, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, -76, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -48, 2, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, -112, -73, 73, 4, -55, 127, 0, 0, 36, 0, 0, 0, -23, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, -82, 71, -31, 122, 20, -82, -17, 63, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 9, 0, 0, 0, 0, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 48, -72, 73, 4, -55, 127, 0, 0, -87, 0, 0, 0, -3, 0, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, -82, 71, -31, 122, 20, -82, -17, 63, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, -93, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 29, 16, 0, 0, -95, 0, 0, 0, 0, 0, 0, 0, 96, -75, 73, 4, -55, 127, 0, 0, 0, 20, 102, 4]\"},{\"usuarioId\":50,\"nome\":\"VALDENISIA\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":68,\"nome\":\"VIKTOR\",\"perfil\":\"ROLE_ADM\"},{\"usuarioId\":93,\"nome\":\"VITORSILVA\",\"perfil\":\"ROLE_SUP\"},{\"usuarioId\":82,\"nome\":\"WENDERSON\",\"perfil\":\"ROLE_SUP\"}]";

    //free this later on
    char *json = get_user_list();
    //printf("%s", json);

    int number_of_users;
    get_number_of_users(json, &number_of_users);
    //printf("%d", number_of_users);

    //Allocating structs to fill with user data
    //PS: Free this later on
    struct user_list *list_of_users = malloc(number_of_users * sizeof(struct user_list));
    num_digitais = deal_with_json(json, list_of_users);

    //criando lista de digitais
    digitais = malloc(sizeof(char*) * num_digitais * 12050);


    for (int i = 0; i < number_of_users; i++){

        if (strcmp((list_of_users)[i].fingerprint, "null") != 0){
            //printf("Id: %d\n", (list_of_users)[i].user_id);
            printf("Name: %s\n", (list_of_users)[i].name);
            //printf("Role: %s\n", (list_of_users)[i].role);
            //printf("Digital: %s\n", (list_of_users)[i].fingerprint);
            ret = read_digital((list_of_users)[i].fingerprint);


        }

    }


    for (int i = 0; *(digitais + i); i++)
    {
           for (int i = 0; i < 12050; i++){
               printf("%d ", (*(digitais + i))[i]);
           }
    }

    do_point(*digitais, num_digitais);

    free(digitais);

    return 0;
}
