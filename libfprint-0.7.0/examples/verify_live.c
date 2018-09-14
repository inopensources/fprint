/*
 * Example fingerprint verification program
 * Copyright (C) 2007 Daniel Drake <dsd@gentoo.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "jsmn/jsmn.h"
#include <libfprint/fprint.h>
#include <assert.h>
#include "cadastro.c"

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

/*int verify(struct fp_dev *dev, struct fp_print_data *data)
{
	int r;

	do {
		sleep(1);
		printf("\nScan your finger now.\n");
		r = fp_verify_finger(dev, data);
		if (r < 0) {
			printf("verification failed with error %d :(\n", r);
			return r;
		}
		switch (r) {
		case FP_VERIFY_NO_MATCH:
			printf("NO MATCH!\n");
			return 0;
		case FP_VERIFY_MATCH:
			printf("MATCH!\n");
			return 0;
		case FP_VERIFY_RETRY:
			printf("Scan didn't quite work. Please try again.\n");
			break;
		case FP_VERIFY_RETRY_TOO_SHORT:
			printf("Swipe was too short, please try again.\n");
			break;
		case FP_VERIFY_RETRY_CENTER_FINGER:
			printf("Please center your finger on the sensor and try again.\n");
			break;
		case FP_VERIFY_RETRY_REMOVE_FINGER:
			printf("Please remove finger from the sensor and try again.\n");
			break;
		}
	} while (1);
}*/

/*void iterOverList(struct node_user * head){

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
}*/




/*
void append(struct node_user * head, int id) {

	struct node_user *current = head;
	while (current->next != NULL) {
		current = current->next;
	}

	*//* now we can add a new variable *//*
	current->next = malloc(sizeof(struct node_user));
	current->next->id = id;
	current->next->next = NULL;

}*/

struct node_user * createListUser(){

	printf("Creating list of users..\n");

	unsigned char *digital;
	struct node_user * head = createList(0, digital);
	/*struct node_user * current = head;


    for(int id = 1; id<10; id++){
        current->next = malloc(sizeof(struct node_user));
        current->next->id = id;
        memcpy(current->next->digital ,(const unsigned char*)&digital,sizeof(digital));
        current->next->next = NULL;
        current= current->next;
    }*/
	return head;
}

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


struct node_user * createList(int id, unsigned char *digital){


	struct node_user * head = NULL;
	head = malloc(sizeof(struct node_user));
	if (head == NULL) {
		return head;
	}

	memcpy(head->digital,(const unsigned char*)&digital,sizeof(digital));
	//strcpy(head->digital, digital);
	//int lengthDigital = sizeof(head->digital)/sizeof(char);

	head->id = id;
	head->next = NULL;
	return head;
}

struct node_user * connect_postman(void){

	printf("Connnecting to mock database\n");
	struct node_user * head = createListUser();

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

char * read_digital(){

    ///*get a string and return the array of char*///
    char digital[42682];
    unsigned char *ret_returned = alloca(12050); // todo: generalizar length
	//unsigned char *ret_returned = (unsigned char *)malloc(7088*sizeof(unsigned char));
    int pos = 0;
    FILE *fp = fopen("digital.txt", "r");
    fgets(digital, 42682, fp);
    fclose(fp);
    char** tokens;
    //printf("digital lida = %s\n\n", digital);
    //tokens é uma array de char
    tokens = str_split(digital, ',');
    if (tokens)
    {
        int  i;
        int num;
        for (i = 0; *(tokens + i); i++)
        {
        	//num é um int com valor inteiro equivalente ao token
            num = atoi(*(tokens + i));
            //printf("%d ",num);
			//printf("%c ", num);
           *(ret_returned + i) = num;
			//printf("%c ", *(ret_returned + i));
            free(*(tokens + i));
        }

        printf("\nlength digital from file: %d\n", i);
        free(tokens);
    }

	return ret_returned;

    //unsigned char *digital = (unsigned char *)malloc(41048*sizeof(unsigned char));
    //char *digital = (unsigned char *)malloc(20000*sizeof(char));
    //int indice = 1;
    //FILE *fp = fopen("digital.txt", "r");
    //char c;
    //char temp;

    /*do
	{

		//faz a leitura do caracter no arquivo apontado por pont_arq
		c = getc(fp);
		if(c != '[' && c != ']' && c != ' '){

			if (c == ','){
				digital[indice] = temp;
				temp = ' ';
				indice++;
			}else{
				temp = temp + c;
				printf("%c\n" , temp);
			}


		}

	}while (c != EOF);*/
}


void get_users(){
	struct node_user * head = connect_postman();
	//iterOverList(head);
}

void ponto(){

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


	///*pegando digital de txt*///
	unsigned char *ret_from_file = read_digital();
	int length = 12050;

	//data = enroll(dev);
	int result = compare_digital(dev, ret_from_file, length); //chamada em data.c

	out_close:
	fp_dev_close(dev);
	out:
	fp_exit();
	return r;


}

void cadastrar(){
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
	cadastrar_digital(76, fprint_to_string(ret, length), length);

	///*Encerrando device*///
	out_close:
	fp_dev_close(dev);
	out:
	fp_exit();
	return r;

}

int main(void)
{

    //cadastrar();
	//get_users();

    ponto();

	/*do {
		char buffer[20];

		verify(dev, data_user);
		printf("Verify again? [Y/n]? ");
		fgets(buffer, sizeof(buffer), stdin);
		if (buffer[0] != '\n' && buffer[0] != 'y' && buffer[0] != 'Y')
			break;
	} while (1);*/

	//fp_print_data_free(data_user);



}


