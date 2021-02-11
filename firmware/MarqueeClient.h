#ifndef __MARQUEE_CLIENT_H_
#define __MARQUEE_CLIENT_H_

#include "application.h"
#include "spark_wiring_string.h"
#include "spark_wiring_tcpclient.h"
//#include "spark_wiring_usbserial.h"

/**
 * Defines for the HTTP methods.
 */
static const char* HTTP_METHOD_GET    = "GET";
static const char* HTTP_METHOD_POST   = "POST";
static const char* HTTP_METHOD_PUT    = "PUT";
static const char* HTTP_METHOD_DELETE = "DELETE";
static const char* HTTP_METHOD_PATCH = "PATCH";

/**
 * This struct is used to pass additional HTTP headers such as API-keys.
 * Normally you pass this as an array. The last entry must have NULL as key.
 */
//typedef struct
//{
//  const char* header;
//  const char* value;
//} marquee_header_t;

/**
 * HTTP Request struct.
 * hostname request host
 * path	 request path
 * port     request port
 * body	 request body
 */
typedef struct
{
  String hostname;
  //IPAddress ip;
  String path;
  // TODO: Look at setting the port by default.
  //int port = 80;
  //int port;
  //String body;
} marquee_request_t;



enum MARQUEE_RESPONSE_STATE 
{
    RESPONSE_STATE_UNKNOWN = 0,
    //RESPONSE_STATE_OPEN, 
    RESPONSE_STATE_HEADERS,
    RESPONSE_STATE_DATA,
    RESPONSE_STATE_SUCCESS_HANDLER,
    RESPONSE_STATE_TIMEOUT_HANDLER,
    RESPONSE_STATE_ERROR_HANDLER,
    RESPONSE_STATE_COMPLETE,
    //RESPONSE_STATE_TIMEOUT,
    //RESPONSE_STATE_OVERFLOW
}; 

class MarqueeClient;

typedef void (*REQUEST_BODY_CALLBACK)(MarqueeClient* client, marquee_request_t* request, char* body);

class MarqueeClient {
public:

    /**
    * Constructor.
    */
    MarqueeClient();
    void SetReceiveBuffer( char* buffer, int buffer_size );
    bool StartRequest( const char* hostname, const char* path );
    MARQUEE_RESPONSE_STATE CheckRequest();
    int GetElapsedTicks();
    void ResetState();
    //static char* GetContent();
    
    void SetRequestHandlers( REQUEST_BODY_CALLBACK success, REQUEST_BODY_CALLBACK timeout, REQUEST_BODY_CALLBACK error, REQUEST_BODY_CALLBACK complete );
    
    //void SetRequestCompleteHandler( REQUEST_BODY_CALLBACK callback );
    //void SetRequestTimeoutHandler( REQUEST_BODY_CALLBACK callback );
private:

    //static char* buffer;
    //static int buffer_size;
    //static MARQUEE_RESPONSE_STATE response_state;
    
    REQUEST_BODY_CALLBACK callback_success = NULL;
    REQUEST_BODY_CALLBACK callback_timeout = NULL;
    REQUEST_BODY_CALLBACK callback_error = NULL;
    
    REQUEST_BODY_CALLBACK callback_complete = NULL;
    
    TCPClient client;
    
    unsigned int buffer_position, buffer_size;
    unsigned int lastRead;
    unsigned int request_started;
    char *buffer;
    bool processingHeaders;
    
    MARQUEE_RESPONSE_STATE response_state;


};

#endif /* __HTTP_CLIENT_H_ */
