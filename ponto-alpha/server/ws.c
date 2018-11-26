//
// Created by leticia on 28/09/18.
//

#include <stdio.h>
#include <stdlib.h>
#include <libwebsockets.h>
#include "cadastro.c"
#include "ponto.c"
#include "ws_utils.c"
#include "utils.c"
#include "user.h"

int * wsi_local;

void send_message_via_ws(char message[]){
    write_back(wsi_local, message);
}

static int callback_http(struct lws *wsi,
                         enum lws_callback_reasons reason, void *user,
                         void *in, size_t len){
    switch( reason )
    {
        case LWS_CALLBACK_HTTP:
            lws_serve_http_file( wsi, "./app/index.html", "text/html", NULL, 0 );
            break;
        default:
            break;
    }

    return 0;
}

int decider(struct lws *wsi, void *in, size_t len){
    char user_id_str[4];
    printf("\nActions: %c\n", ((char *) in)[0]);
    wsi_local = wsi;
    switch (((char *) in)[0])
    {
        case '0':
            printf("\n0: %s\n", "Requesting user list");
            char * users_json = get_user_list_mini();

            int list_size = strlen(users_json);
            printf("\n\nSize: %d\n\n", list_size);
            if (list_size > 0){
                printf("\n\n %s \n\n", users_json);
                compose_json_answer("DATA_RESPONSE", "SUCCESS", "get_user_list", "User list retrieved.", users_json);
            } else {
                compose_json_answer("SCREEN_UPDATE", "ERROR", "get_user_list", "Erro ao obter lista de usuários", "");
            }
            free(users_json);
            return 0;

        case '1':
            //Collect user ID from view
            for (int i = 0; i < 4; i++){
                user_id_str[i] = (((char *) in)[2+i]);
            }
            int user_id = atoi(user_id_str);

            printf("\n1: Record user fingerprint %s\n", user_id_str);
            cadastra_user(user_id);
            printf("\n1: %s\n", "Finished Record user fingerprint");

            return 0;

        case '2':
            printf("\n2: %s\n", "Clock in/out");
            do_point();

            return 0;
        case '3':
            printf("\n3: %s\n", "Verify Adm");

            int trying = 1;
            int result_verify_adm = -22; //iniciando resultado com erro
            while(result_verify_adm == -22 && trying < 5){
                result_verify_adm = verify_adm();
                printf("\n trying: %d, result_verify_adm: %d\n", trying, result_verify_adm);
                trying +=1;
            }
            if(result_verify_adm == -1){
                compose_json_answer("SCREEN_UPDATE", "ERROR", "do_point", "Verificação falhou. Você é um Gerente/RH ?.", "-1");
            }
            return 0;
        default:
            printf("\nError: %s\n", "Don't know how to answer to this. :|");
            compose_json_answer("CONSOLE_LOG", "ERROR", "decider", "Não é possível responder a essa requisição.", "");
            return 1;
    }
}

static int callback_dumb_increment(struct lws *wsi,
                                   enum lws_callback_reasons reason,
                                   void *user, void *in, size_t len)
{
    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: // just log message that someone is connecting
            printf("connection established\n");
            break;
        case LWS_CALLBACK_RECEIVE: { // the funny part
            // pointer to `void *in` holds the incomming request
            // we're just going to put it in reverse order and put it in `buf` with
            // correct offset. `len` holds length of the request.
            int answer = decider(wsi, in, len);
            break;
        }
        default:
            break;
    }

    return 0;
}



static struct lws_protocols protocols[] = {
        /* first protocol must always be HTTP handler */
        {
                "http-only",   // name
                      callback_http, // callback
                            0              // per_session_data_size
        },
        {
                "dumb-increment-protocol", // protocol name - very important!
                      callback_dumb_increment,   // callback
                            0                          // we don't use any per session data
        },
        {
                NULL, NULL, 0   /* End of list */
        }
};


void run_ws(){
    // server url will be http://localhost:9000
    int port = 8000;
    struct lws_context *context;
    struct lws_context_creation_info context_info =
            {
                    .port = port, .iface = NULL, .protocols = protocols, .extensions = NULL,
                    .ssl_cert_filepath = NULL, .ssl_private_key_filepath = NULL, .ssl_ca_filepath = NULL,
                    .gid = -1, .uid = -1, .options = 0, NULL, .ka_time = 0, .ka_probes = 0, .ka_interval = 0
            };
    // create lws context representing this server
    context = lws_create_context(&context_info);

    if (context == NULL) {
        fprintf(stderr, "lws init failed\n");
        return -1;
    }

    printf("starting server...\n");

    // infinite loop, to end this server send SIGTERM. (CTRL+C)
    while (1) {
        lws_service(context, 100);
        // lws_service will process all waiting events with their
        // callback functions and then wait 50 ms.
        // (this is a single threaded webserver and this will keep our server
        // from generating load while there are not requests to process)
    }

    lws_context_destroy(context);
}
