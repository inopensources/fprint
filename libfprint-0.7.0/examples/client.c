#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static struct lws *web_socket = NULL;

#define EXAMPLE_RX_BUFFER_BYTES (10)

int status;

int get_status(){
    return status;
}

void set_status(int new_status){
    status = new_status;
}

static int callback_example_client( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
    switch( reason )
    {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            lws_callback_on_writable( wsi );
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            /* Handle incomming messages here. */
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
        {
            unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];

            unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];

            // size_t n = sprintf( (char *)p, "%u", rand());
            size_t n = sprintf( (char *)p, "%d", get_status());
            printf("\nStatus on Protocol: %d\n", get_status());


            lws_write( wsi, p, n, LWS_WRITE_TEXT );

            free(p);

            break;
        }

        case LWS_CALLBACK_CLOSED:
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            web_socket = NULL;
            break;

        default:
            break;
    }

    return 0;
}



static int callback_ponto_client( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len )
{
    switch( reason )
    {
        case LWS_CALLBACK_CLIENT_ESTABLISHED:
            lws_callback_on_writable( wsi );
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            /* Handle incomming messages here. */
            lwsl_notice("Client RX: %s", (char *)in);
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
        {
            unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];

            unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];

            // size_t n = sprintf( (char *)p, "%u", rand());
            size_t n = sprintf( (char *)p, "%d", get_status());
            lws_write( wsi, p, n, LWS_WRITE_TEXT );

            break;
        }

        case LWS_CALLBACK_CLOSED:
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            web_socket = NULL;
            break;

        default:
            break;
    }

    return 0;
}


enum protocols_client
{
    PROTOCOL_EXAMPLE_CLIENT = 0,
    PROTOCOL_PONTO_CLIENT = 0,
    PROTOCOL_COUNT_CLIENT
};

static struct lws_protocols protocols_client[] =
        {
                {
                        "example-protocol",
                        callback_example_client,
                              0,
                        EXAMPLE_RX_BUFFER_BYTES,
                },
                {
                        "ponto-protocol",
                        callback_example_client,
                              0,
                        EXAMPLE_RX_BUFFER_BYTES,
                },
                { NULL, NULL, 0, 0 } /* terminator */
        };

struct lws_context * init_context_client(){


    struct lws_context_creation_info info;
    memset( &info, 0, sizeof(info) );

    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = protocols_client;
    info.gid = -1;
    info.uid = -1;
    struct lws_context *context = lws_create_context( &info );


    return context;
}

void destroy_context_client(struct lws_context *context){

    lws_context_destroy( context );
}

void serving(struct lws_context * context){
    while(1){
        lws_service( context, /* timeout_ms = */ 250 );
    }
}

//chamando em verify_live para teste
void client(int status, struct lws_context * context){

    set_status(status);

    printf("\nStatus on Client: %d\n", get_status());

    /* Connect if we are not connected to the server. */
    if(!web_socket){
        struct lws_client_connect_info ccinfo = {0};
        ccinfo.context = context;
        ccinfo.address = "localhost";
        ccinfo.port = 8000;
        ccinfo.path = "/";
        ccinfo.host = lws_canonical_hostname( context );
        ccinfo.origin = "origin";
        ccinfo.protocol = protocols_client[PROTOCOL_EXAMPLE_CLIENT].name;
        web_socket = lws_client_connect_via_info(&ccinfo);
    }

    lws_callback_on_writable(web_socket);
    //lws_service( context, /* timeout_ms = */ 250 );

}