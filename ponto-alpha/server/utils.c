//
// Created by leticia on 14/09/18.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "b64.h"

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

    printf("%s\n", encoded_data); // YnJpYW4gdGhlIG1vbmtleSBhbmQgYnJhZGxleSB0aGUga2lua2Fqb3UgYXJlIGZyaWVuZHM=

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