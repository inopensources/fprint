//
// Created by leticia on 14/09/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "b64.h"
#include "structs.h"
#include <json-c/json.h>

char * compose_json_answer_user_matches(char id_user[], char name_user[]){

    char json_0[] = "{\n";
    char json_1[] = " \"id\" : ";
    char json_2[] = " \"name\" : ";
    char json_3[] = "}\n";

    int json_size = strlen(json_0) +
                    strlen(json_1) + strlen(id_user) +
                    strlen(json_2) + strlen(name_user) +
                    strlen(json_3);

    //+10 to take each \0 into consideration
    char * json = calloc(json_size + 20, sizeof(char));

    sprintf(json, "{\n \"id\" : \"%s\", \"name\" : \"%s\"\n}", id_user, name_user);

    return json;

}

void compose_json_answer(char type[], char status[], char method_name[], char message[], char data[]){
    char json_0[] = "{\n";
    char json_1[] = " \"type\" : ";
    char json_2[] = " \"status\" : ";
    char json_3[] = " \"method_name\" : ";
    char json_4[] = " \"message\" : ";
    char json_5[] = " \"data\" : ";
    char json_6[] = "}\n";

    //Encoding data to base64
    char *encoded_data = b64_encode(data, strlen(data));

    //printf("%s\n", encoded_data); // YnJpYW4gdGhlIG1vbmtleSBhbmQgYnJhZGxleSB0aGUga2lua2Fqb3UgYXJlIGZyaWVuZHM=

    int json_size = strlen(json_0) +
                    strlen(json_1) + strlen(type) +
                    strlen(json_2) + strlen(status) +
                    strlen(json_3) + strlen(method_name) +
                    strlen(json_4) + strlen(message) +
                    strlen(json_5) + strlen(encoded_data) +
                    strlen(json_6);

    //+10 to take each \0 into consideration
    char * json = calloc(json_size + 20, sizeof(char));

    sprintf(json, "{\n \"type\" : \"%s\", \"status\" : \"%s\",\n \"method_name\" : \"%s\",\n \"message\": \"%s\",\n \"data\": \"%s\"\n}", type, status, method_name, message, encoded_data);

    send_message_via_ws(json);


    free(json);
    free(encoded_data);
}

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

void get_number_of_users(char* json_str, int* number_of_users){
    json_object * jobj = json_tokener_parse(json_str);
    int length_array = json_object_array_length(jobj);

    //Writing the number of users in the JSON to number_of_users
    *number_of_users = length_array;

}

void fprint_to_string(char * ret, int length, char digital[]){
    int i;
    int ret_temp;
    int index_digital = 1;

    printf("length: %d", length);

    for(i=0; i<length; i++) {
        if (i == 0){
            digital[0] = '[';
        }
        ret_temp = ret[i];
        char ret_string[12];
        sprintf(ret_string, "%d", ret_temp);
        for (int j = 0; j < strlen(ret_string); j++) {
            digital[index_digital++] = ret_string[j];
        }

        digital[index_digital++] = ',';
        digital[index_digital++] = ' ';
    }

    digital[index_digital-2] = ']';

}

int get_length_digital(char * ret, int length){

    //gambis achar length
    int length_dig=0;
    int i;
    int ret_temp = 0;
    printf("ret_length: %d\n", strlen(ret));
    for(i=0; i<length; i++) {

        ret_temp = ret[i];
        char ret_string[12];
        sprintf(ret_string, "%d", ret_temp);
        for (int j = 0; j < strlen(ret_string); j++) {
            length_dig++;
        }
        length_dig++;
        length_dig++;
    }

    return length_dig;
}

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

