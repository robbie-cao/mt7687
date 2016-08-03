/* Copyright (C) 2012 mbed.org, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#include <stdint.h>
#include <stdbool.h>
#include "lwip/sockets.h"

#ifdef MTK_HTTPCLIENT_SSL_ENABLE
#include "mbedtls/compat-1.3.h"
#include "mbedtls/net.h"
#include "mbedtls/ssl.h"
#include "mbedtls/certs.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @addtogroup HttpClient
 * @{
 * HttpClient API implements the client-side of HTTP/1.1. It provides base interfaces to execute an HTTP request on a given URL. It also supports HTTPS (HTTP over TLS) to provide secure communication.\n
 * @section HttpClient_Usage_Chapter How to use this module
 * In this release, MediaTek provides two types of APIs: high level APIs and low level APIs.\n
 * - \b The \b high \b level \b APIs
 *  - Enables to execute a single HTTP request on a given URL.
 *  - Call #httpclient_get(), #httpclient_post(), #httpclient_put() or #httpclient_delete() to get, post, put or delete and HTTP request.\n
 * - \b The \b low \b level \b APIs
 *  - Enables to execute more than one HTTP requests during a Keep-Alive connection. Keep-alive is the idea of using a single TCP connection to send and receive multiple HTTP requests/responses, as opposed to opening a new connection for every single request/response pair.
 *  - Step1: Call #httpclient_connect() to connect to a remote server.
 *  - Step2: Call #httpclient_send_request() to send an HTTP request to the server.
 *  - Step3: Call #httpclient_recv_response() to receive an HTTP response from the server.
 *  - Step4: Repeat Steps 2 and 3 to execute more requests.
 *  - Step5: Call #httpclient_close() to close the connection.
 *  - Sample code: Please refer to the example under <sdk_root>/project/mt7687_hdk/apps/http_client/http_client_keepalive folder.
 */

/** @defgroup httpclient_define Define
  * @{
  */
/** @brief   This macro defines the HTTP port.  */
#define HTTP_PORT   80

/** @brief   This macro defines the HTTPS port.  */
#define HTTPS_PORT 443
/**
 * @}
 */

/** @defgroup httpclient_enum Enum
 *  @{
 */
/** @brief   This enumeration defines the HTTP request type.  */
typedef enum {
    HTTPCLIENT_GET,
    HTTPCLIENT_POST,
    HTTPCLIENT_PUT,
    HTTPCLIENT_DELETE,
    HTTPCLIENT_HEAD
} HTTPCLIENT_REQUEST_TYPE;

/** @brief   This enumeration defines the API return type.  */
typedef enum {
    HTTPCLIENT_ERROR_PARSE = -6,           /**< A URL parse error occurred. */
    HTTPCLIENT_UNRESOLVED_DNS = -5,        /**< Could not resolve the hostname. */
    HTTPCLIENT_ERROR_PRTCL = -4,           /**< A protocol error occurred. */
    HTTPCLIENT_ERROR = -3,                 /**< An unknown error occurred. */
    HTTPCLIENT_CLOSED = -2,                /**< Connection was closed by a remote host. */
    HTTPCLIENT_ERROR_CONN = -1,            /**< Connection failed. */
    HTTPCLIENT_OK = 0,                     /**< The operation was successful. */
    HTTPCLIENT_RETRIEVE_MORE_DATA = 1      /**< More data needs to be retrieved. */
} HTTPCLIENT_RESULT;
/**
 * @}
 */

/** @defgroup httpclient_struct Struct
  * @{
  */
/** @brief   This structure defines the httpclient_t structure.  */
typedef struct {
    int socket;                    /**< Socket ID. */
    int remote_port;               /**< HTTP or HTTPS port. */
    int response_code;             /**< Response code. */
    char *header;                  /**< Custom header. */
    char *auth_user;               /**< Username for basic authentication. */
    char *auth_password;           /**< Password for basic authentication. */
#ifdef MTK_HTTPCLIENT_SSL_ENABLE
    mbedtls_ssl_context ssl;       /**< SSL context. */
    mbedtls_net_context fd;        /**< SSL network context. */
    mbedtls_ssl_config conf;       /**< SSL configuration. */
#endif
} httpclient_t;

/** @brief   This structure defines the HTTP data structure.  */
typedef struct {
    bool is_more;                /**< Indicates if more data needs to be retrieved. */
    bool is_chunked;             /**< Response data is encoded in portions/chunks.*/
    int retrieve_len;            /**< Content length to be retrieved. */
    int response_content_len;    /**< Response content length. */
    int post_buf_len;            /**< Post data length. */
    int response_buf_len;        /**< Response buffer length. */
    char *post_content_type;     /**< Content type of the post data. */
    char *post_buf;              /**< User data to be posted. */
    char *response_buf;          /**< Buffer to store the response data. */
} httpclient_data_t;
/**
 * @}
 */

/**
 * @brief            This function executes a GET request on a given URL. It blocks until completion.
 * @param[in]        client is a pointer to the #httpclient_t.
 * @param[in]        url is the URL to run the request.
 * @param[in]        port is #HTTP_PORT or #HTTPS_PORT.
 * @param[in, out]   client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request.
 * @return           Please refer to #HTTPCLIENT_RESULT.
 * @par              HttpClient Get Example
 * @code
 *                   char *url = "https://www.baidu.com/";
 *                   httpclient_t client = {0};
 *                   httpclient_data_t client_data = {0};
 *                   char *buf = NULL;
 *                   buf = pvPortMalloc(BUF_SIZE);
 *                   if (buf == NULL) {
 *                       printf("Malloc failed.\r\n");
 *                       return;
 *                   }
 *                   memset(buf, 0, sizeof(buf));
 *                   client_data.response_buf = buf;  //Sets a buffer to store the result.
 *                   client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
 *                   httpclient_get(&client, url, HTTPS_PORT, &client_data);
 *                   printf("Data received: %s\r\n", client_data.response_buf);
 * @endcode
 */
HTTPCLIENT_RESULT httpclient_get(httpclient_t *client, char *url, int port, httpclient_data_t *client_data);


/**
 * @brief            This function executes a POST request on a given URL. It blocks until completion.
 * @param[in]        client is a pointer to the #httpclient_t.
 * @param[in]        url is the URL to run the request.
 * @param[in]        port is #HTTP_PORT or #HTTPS_PORT.
 * @param[in, out]   client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request. It also contains the data to be posted.
 * @return           Please refer to #HTTPCLIENT_RESULT.
 * @par              HttpClient Post Example
 * @code
 *                   char *url = "https://api.mediatek.com/mcs/v2/devices/D0n2yhrl/datapoints.csv";
 *                   char *header = "deviceKey:FZoo0S07CpwUHcrt\r\n";
 *                   char *content_type = "text/csv";
 *                   char *post_data = "1,,I am string!";
 *                   httpclient_t client = {0};
 *                   httpclient_data_t client_data = {0};
 *                   char *buf = NULL;
 *                   buf = pvPortMalloc(BUF_SIZE);
 *                   if (buf == NULL) {
 *                       printf("Malloc failed.\r\n");
 *                       return;
 *                   }
 *                   memset(buf, 0, sizeof(buf));
 *                   client_data.response_buf = buf;  //Sets a buffer to store the result.
 *                   client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
 *                   httpclient_set_custom_header(&client, header);  //Sets the custom header if needed.
 *                   client_data.post_buf = post_data;  //Sets the user data to be posted.
 *                   client_data.post_buf_len = strlen(post_data);  //Sets the post data length.
 *                   client_data.post_content_type = content_type;  //Sets the content type.
 *                   httpclient_post(&client, url, HTTPS_PORT, &client_data);
 *                   printf("Data received: %s\r\n", client_data.response_buf);
 * @endcode
 */
HTTPCLIENT_RESULT httpclient_post(httpclient_t *client, char *url, int port, httpclient_data_t *client_data);

/**
 * @brief            This function executes a PUT request on a given URL. It blocks until completion.
 * @param[in]        client is a pointer to the #httpclient_t.
 * @param[in]        url is the URL to run the request.
 * @param[in]        port is #HTTP_PORT or #HTTPS_PORT.
 * @param[in, out]   client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request. It also contains the data to be put.
 * @return           Please refer to #HTTPCLIENT_RESULT.
 * @par              HttpClient Put Example
 * @code
 *                   char *url = "https://ec2-52-76-74-57.ap-southeast-1.compute.amazonaws.com/mcs/test/ok/200";
 *                   char *content_type = "text/csv";
 *                   char *put_data = "1,,I am string!";
 *                   httpclient_t client = {0};
 *                   httpclient_data_t client_data = {0};
 *                   char *buf = NULL;
 *                   buf = pvPortMalloc(BUF_SIZE);
 *                   if (buf == NULL) {
 *                       printf("Malloc failed.\r\n");
 *                       return;
 *                   }
 *                   memset(buf, 0, sizeof(buf));
 *                   client_data.response_buf = buf;  //Sets a buffer to store the result.
 *                   client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
 *                   client_data.post_buf = put_data;  //Sets the user data to be put.
 *                   client_data.post_buf_len = strlen(put_data);  //Sets the put data length.
 *                   client_data.post_content_type = content_type;  //Sets the content type.
 *                   httpclient_put(&client, url, HTTPS_PORT, &client_data);
 *                   printf("Data received: %s\r\n", client_data.response_buf);
 * @endcode
 */
HTTPCLIENT_RESULT httpclient_put(httpclient_t *client, char *url, int port, httpclient_data_t *client_data);

/**
 * @brief            This function executes a DELETE request on a given URL. It blocks until completion.
 * @param[in]        client is a pointer to the #httpclient_t.
 * @param[in]        url is the URL to run the request.
 * @param[in]        port is #HTTP_PORT or #HTTPS_PORT.
 * @param[in, out]   client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request.
 * @return           Please refer to #HTTPCLIENT_RESULT.
 * @par              HttpClient Delete Example
 * @code
 *                   char *url = "https://ec2-52-76-74-57.ap-southeast-1.compute.amazonaws.com/mcs/test/ok/200";
 *                   httpclient_t client = {0};
 *                   httpclient_data_t client_data = {0};
 *                   char *buf = NULL;
 *                   buf = pvPortMalloc(BUF_SIZE);
 *                   if (buf == NULL) {
 *                       printf("Malloc failed.\r\n");
 *                       return;
 *                   }
 *                   memset(buf, 0, sizeof(buf));
 *                   client_data.response_buf = buf;  //Sets a buffer to store the result.
 *                   client_data.response_buf_len = BUF_SIZE;  //Sets the buffer size.
 *                   httpclient_delete(&client, url, HTTPS_PORT, &client_data);
 *                   printf("Data received: %s\r\n", client_data.response_buf);
 * @endcode
 */
HTTPCLIENT_RESULT httpclient_delete(httpclient_t *client, char *url, int port, httpclient_data_t *client_data);

/**
 * @brief            This function establishes an HTTP connection with the remote server by given URL and port ID.
 * @param[in]        client is a pointer to the #httpclient_t.
 * @param[in]        url is the URL to run the request.
 * @param[in]        port is #HTTP_PORT or #HTTPS_PORT.
 * @return           Please refer to #HTTPCLIENT_RESULT.
 */
HTTPCLIENT_RESULT httpclient_connect(httpclient_t *client, char *url, int port);

/**
 * @brief            This function sends an HTTP(GET or POST) request to the given URL.
 * @param[in]        client is a pointer to the #httpclient_t.
 * @param[in]        url is the URL to run the request.
 * @param[in]        method is #HTTPCLIENT_REQUEST_TYPE.
 * @param[in]        client_data is a pointer to the #httpclient_data_t instance to collect the data to be posted.
 * @return           Please refer to #HTTPCLIENT_RESULT.
 */
HTTPCLIENT_RESULT httpclient_send_request(httpclient_t *client, char *url, int method, httpclient_data_t *client_data);

/**
 * @brief            This function receives the response from a server for the last request.
 * @param[in]        client is a pointer to the #httpclient_t.
 * @param[out]       client_data is a pointer to the #httpclient_data_t instance to collect the data returned by the request.
 * @return           Please refer to #HTTPCLIENT_RESULT.
 */
HTTPCLIENT_RESULT httpclient_recv_response(httpclient_t *client, httpclient_data_t *client_data);

/**
 * @brief            This function closes the HTTP connection.
 * @param[in]        client is a pointer to the #httpclient_t.
 * @param[in]        port is #HTTP_PORT or #HTTPS_PORT.
 */
void httpclient_close(httpclient_t *client, int port);

/**
 * @brief            This function gets the HTTP response code assigned to the last request.
 * @param[in]        client is a pointer to the #httpclient_t.
 * @return           The HTTP response code of the last request.
 */
int httpclient_get_response_code(httpclient_t *client);

/**
 * @brief            This function sets a custom header.
 * @param[in]        client is a pointer to the #httpclient_t.
 * @param[in]        header is a custom header string.
 */
void httpclient_set_custom_header(httpclient_t *client, char *header);

/**
* @}
*/

/* The following content is used in the HttpClient module. */
#ifndef MTK_DEBUG_LEVEL_NONE
#define HTTPCLIENT_DEBUG 1
#else
#define HTTPCLIENT_DEBUG 0
#endif

#if HTTPCLIENT_DEBUG
#define DBG(x, ...) printf("[HTTPClient : DBG]"x"\r\n", ##__VA_ARGS__);
#define WARN(x, ...) printf("[HTTPClient : WARN]"x"\r\n", ##__VA_ARGS__);
#define ERR(x, ...) printf("[HTTPClient : ERR]"x"\r\n", ##__VA_ARGS__);
#else
#define DBG(x, ...)
#define WARN(x, ...)
#define ERR(x, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* __HTTPCLIENT_H__ */

