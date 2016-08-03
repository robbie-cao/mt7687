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

#include "httpclient.h"
#include "lwip/sockets.h"
#include "stdio.h"
#include "lwip/netdb.h"
#include "lwip/tcp.h"
#include "lwip/err.h"
#include "mbedtls/debug.h"

#define MIN(x,y) (((x)<(y))?(x):(y))
#define MAX(x,y) (((x)>(y))?(x):(y))

#define HTTPCLIENT_AUTHB_SIZE     128

#define HTTPCLIENT_CHUNK_SIZE     512
#define HTTPCLIENT_SEND_BUF_SIZE  512

#define HTTPCLIENT_MAX_HOST_LEN   64
#define HTTPCLIENT_MAX_URL_LEN    256

#if defined(MBEDTLS_DEBUG_C)
#define DEBUG_LEVEL 2
#endif

static int httpclient_parse_host(char *url, char *host, size_t maxhost_len);
static int httpclient_parse_url(const char *url, char *scheme, size_t max_scheme_len, char *host, size_t maxhost_len, int *port, char *path, size_t max_path_len);
static int httpclient_tcp_send_all(int sock_fd, char *data, int length);
static int httpclient_conn(httpclient_t *client, char *host, int port);
static int httpclient_recv(httpclient_t *client, char *buf, int min_len, int max_len, int *p_read_len);
static int httpclient_retrieve_content(httpclient_t *client, char *data, int len, httpclient_data_t *client_data);
static int httpclient_response_parse(httpclient_t *client, char *data, int len, httpclient_data_t *client_data);
#ifdef MTK_HTTPCLIENT_SSL_ENABLE
static int httpclient_ssl_conn(httpclient_t *client, char *url, int port);
static int httpclient_ssl_start(mbedtls_ssl_context *pssl,
                                mbedtls_net_context *socket,
                                mbedtls_ssl_config *conf,
                                char *host);
static int httpclient_ssl_send_all(mbedtls_ssl_context *ssl, const char *data, size_t length);
static int httpclient_ssl_client_init(mbedtls_ssl_context *ssl,
                                      mbedtls_net_context *tcp_fd,
                                      mbedtls_ssl_config *conf);
static int httpclient_ssl_nonblock_recv(void *ctx, unsigned char *buf, size_t len);
#endif



static void httpclient_base64enc(char *out, const char *in)
{
    const char code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=" ;
    int i = 0, x = 0, l = 0;

    for (; *in; in++) {
        x = x << 8 | *in;
        for (l += 8; l >= 6; l -= 6) {
            out[i++] = code[(x >> (l - 6)) & 0x3f];
        }
    }
    if (l > 0) {
        x <<= 6 - l;
        out[i++] = code[x & 0x3f];
    }
    for (; i % 4;) {
        out[i++] = '=';
    }
    out[i] = '\0' ;
}

int httpclient_conn(httpclient_t *client, char *url, int port)
{
    struct addrinfo hints, *addr_list, *cur;
    char host[HTTPCLIENT_MAX_HOST_LEN];
    int ret = 0;

    httpclient_parse_host(url, host, sizeof(host));

    DBG("%s, host:%s\n", __func__, host);

    memset( &hints, 0, sizeof( hints ) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if ( getaddrinfo( host, "80" , &hints, &addr_list ) != 0 ) {
        DBG("getaddrinfo != 0, return HTTPCLIENT_UNRESOLVED_DNS");
        return HTTPCLIENT_UNRESOLVED_DNS;
    }

    /* Try the sockaddrs until a connection succeeds */
    ret = HTTPCLIENT_UNRESOLVED_DNS;
    for ( cur = addr_list; cur != NULL; cur = cur->ai_next ) {
        client->socket = (int) socket( cur->ai_family, cur->ai_socktype,
                                        cur->ai_protocol );
        if ( client->socket < 0 ) {
            ret = HTTPCLIENT_ERROR_CONN;
            continue;
        }

        if ( connect( client->socket, cur->ai_addr, (int)cur->ai_addrlen ) == 0 ) {
            ret = 0;
            break;
        }

        close( client->socket );
        ret = HTTPCLIENT_ERROR_CONN;
    }

    freeaddrinfo( addr_list );

    return ret;
}


int httpclient_parse_url(const char *url, char *scheme, size_t max_scheme_len, char *host, size_t maxhost_len, int *port, char *path, size_t max_path_len)
{
    char *scheme_ptr = (char *) url;
    char *host_ptr = (char *) strstr(url, "://");
    size_t host_len = 0;
    size_t path_len;
    char *port_ptr;
    char *path_ptr;
    char *fragment_ptr;

    if (host_ptr == NULL) {
        WARN("Could not find host");
        return HTTPCLIENT_ERROR_PARSE; /* URL is invalid */
    }

    if ( max_scheme_len < host_ptr - scheme_ptr + 1 ) { /* including NULL-terminating char */
        WARN("Scheme str is too small (%d >= %d)", max_scheme_len, host_ptr - scheme_ptr + 1);
        return HTTPCLIENT_ERROR_PARSE;
    }
    memcpy(scheme, scheme_ptr, host_ptr - scheme_ptr);
    scheme[host_ptr - scheme_ptr] = '\0';

    host_ptr += 3;

    port_ptr = strchr(host_ptr, ':');
    if ( port_ptr != NULL ) {
        uint16_t tport;
        host_len = port_ptr - host_ptr;
        port_ptr++;
        if ( sscanf(port_ptr, "%hu", &tport) != 1) {
            WARN("Could not find port");
            return HTTPCLIENT_ERROR_PARSE;
        }
        *port = (int)tport;
    } else {
        *port = 0;
    }
    path_ptr = strchr(host_ptr, '/');
    if ( host_len == 0 ) {
        host_len = path_ptr - host_ptr;
    }

    if ( maxhost_len < host_len + 1 ) { /* including NULL-terminating char */
        WARN("Host str is too small (%d >= %d)", maxhost_len, host_len + 1);
        return HTTPCLIENT_ERROR_PARSE;
    }
    memcpy(host, host_ptr, host_len);
    host[host_len] = '\0';

    fragment_ptr = strchr(host_ptr, '#');
    if (fragment_ptr != NULL) {
        path_len = fragment_ptr - path_ptr;
    } else {
        path_len = strlen(path_ptr);
    }

    if ( max_path_len < path_len + 1 ) { /* including NULL-terminating char */
        WARN("Path str is too small (%d >= %d)", max_path_len, path_len + 1);
        return HTTPCLIENT_ERROR_PARSE;
    }
    memcpy(path, path_ptr, path_len);
    path[path_len] = '\0';

    return HTTPCLIENT_OK;
}

int httpclient_parse_host(char *url, char *host, size_t maxhost_len)
{
    char *host_ptr = (char *) strstr(url, "://");
    size_t host_len = 0;
    char *port_ptr;
    char *path_ptr;

    if (host_ptr == NULL) {
        WARN("Could not find host");
        return HTTPCLIENT_ERROR_PARSE; /* URL is invalid */
    }
    host_ptr += 3;

    port_ptr = strchr(host_ptr, ':');
    if ( port_ptr != NULL ) {
        uint16_t tport;
        host_len = port_ptr - host_ptr;
        port_ptr++;
        if ( sscanf(port_ptr, "%hu", &tport) != 1) {
            WARN("Could not find port");
            return HTTPCLIENT_ERROR_PARSE;
        }
    }

    path_ptr = strchr(host_ptr, '/');
    if ( host_len == 0 ) {
        host_len = path_ptr - host_ptr;
    }

    if ( maxhost_len < host_len + 1 ) { /* including NULL-terminating char */
        WARN("Host str is too small (%d >= %d)", maxhost_len, host_len + 1);
        return HTTPCLIENT_ERROR_PARSE;
    }
    memcpy(host, host_ptr, host_len);
    host[host_len] = '\0';

    return HTTPCLIENT_OK;
}


int httpclient_get_info(httpclient_t *client, char *send_buf, int *send_idx, char *buf, size_t len)   /* 0 on success, err code on failure */
{
    int ret ;
    int cp_len ;
    int idx = *send_idx;

    if (len == 0) {
        len = strlen(buf);
    }

    do {
        if ((HTTPCLIENT_SEND_BUF_SIZE - idx) >= len) {
            cp_len = len ;
        } else {
            cp_len = HTTPCLIENT_SEND_BUF_SIZE - idx ;
        }

        memcpy(send_buf + idx, buf, cp_len) ;
        idx += cp_len ;
        len -= cp_len ;

        if (idx == HTTPCLIENT_SEND_BUF_SIZE) {
            if (client->remote_port == HTTPS_PORT) {
                ERR("send buffer overflow");
                return HTTPCLIENT_ERROR ;
            }
            ret = httpclient_tcp_send_all(client->socket, send_buf, HTTPCLIENT_SEND_BUF_SIZE) ;
            if (ret) {
                return (ret) ;
            }
        }
    } while (len) ;

    *send_idx = idx;
    return HTTPCLIENT_OK ;
}

void httpclient_set_custom_header(httpclient_t *client, char *header)
{
    client->header = header ;
}

int httpclient_basic_auth(httpclient_t *client, char *user, char *password)
{
    if ((strlen(user) + strlen(password)) >= HTTPCLIENT_AUTHB_SIZE) {
        return HTTPCLIENT_ERROR ;
    }
    client->auth_user = user;
    client->auth_password = password;
    return HTTPCLIENT_OK ;
}

int httpclient_send_auth(httpclient_t *client, char *send_buf, int *send_idx)
{
    char b_auth[(int)((HTTPCLIENT_AUTHB_SIZE + 3) * 4 / 3 + 1)] ;
    char base64buff[HTTPCLIENT_AUTHB_SIZE + 3] ;

    httpclient_get_info(client, send_buf, send_idx, "Authorization: Basic ", 0) ;
    sprintf(base64buff, "%s:%s", client->auth_user, client->auth_password) ;
    DBG("bAuth: %s", base64buff) ;
    httpclient_base64enc(b_auth, base64buff) ;
    b_auth[strlen(b_auth) + 1] = '\0' ;
    b_auth[strlen(b_auth)] = '\n' ;
    DBG("b_auth:%s", b_auth) ;
    httpclient_get_info(client, send_buf, send_idx, b_auth, 0) ;
    return HTTPCLIENT_OK ;
}


int httpclient_tcp_send_all(int sock_fd, char *data, int length)
{
    int written_len = 0;

    while (written_len < length) {
        int ret = send(sock_fd, data + written_len, length - written_len, 0);
        if (ret > 0) {
            written_len += ret;
            continue;
        } else if (ret == 0) {
            return written_len;
        } else {
            return -1; /* Connnection error */
        }
    }

    return written_len;
}


int httpclient_send_header(httpclient_t *client, char *url, int method, httpclient_data_t *client_data)
{
    char scheme[8] = {0};
    char host[HTTPCLIENT_MAX_HOST_LEN] = {0};
    char path[HTTPCLIENT_MAX_URL_LEN] = {0};
    int len;
    char send_buf[HTTPCLIENT_SEND_BUF_SIZE] = {0};
    char buf[HTTPCLIENT_SEND_BUF_SIZE] = {0};
    char *meth = (method == HTTPCLIENT_GET) ? "GET" : (method == HTTPCLIENT_POST) ? "POST" : (method == HTTPCLIENT_PUT) ? "PUT" : (method == HTTPCLIENT_DELETE) ? "DELETE" : (method == HTTPCLIENT_HEAD) ? "HEAD" : "";
    int ret;

    /* First we need to parse the url (http[s]://host[:port][/[path]]) */
    int res = httpclient_parse_url(url, scheme, sizeof(scheme), host, sizeof(host), &(client->remote_port), path, sizeof(path));
    if (res != HTTPCLIENT_OK) {
        ERR("httpclient_parse_url returned %d", res);
        return res;
    }

    if (client->remote_port == 0) {
        if (strcmp(scheme, "http") == 0) {
            client->remote_port = HTTP_PORT ;
        } else if (strcmp(scheme, "https") == 0) {
            client->remote_port = HTTPS_PORT ;
        }
    }

    /* Send request */
    memset(send_buf, 0, HTTPCLIENT_SEND_BUF_SIZE);
    len = 0 ; /* Reset send buffer */

    snprintf(buf, sizeof(buf), "%s %s HTTP/1.1\r\nHost: %s\r\n", meth, path, host); /* Write request */
    ret = httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
    if (ret) {
        ERR("Could not write request");
        return HTTPCLIENT_ERROR_CONN;
    }

    /* Send all headers */
    if (client->auth_user) {
        httpclient_send_auth(client, send_buf, &len) ; /* send out Basic Auth header */
    }

    /* Add user header information */
    if (client->header) {
        httpclient_get_info(client, send_buf, &len, (char *)client->header, strlen(client->header));
    }

    if ( client_data->post_buf != NULL ) {
        snprintf(buf, sizeof(buf), "Content-Length: %d\r\n", client_data->post_buf_len);
        httpclient_get_info(client, send_buf, &len, buf, strlen(buf));

        if (client_data->post_content_type != NULL)  {
            snprintf(buf, sizeof(buf), "Content-Type: %s\r\n", client_data->post_content_type);
            httpclient_get_info(client, send_buf, &len, buf, strlen(buf));
        }
    }

    /* Close headers */
    httpclient_get_info(client, send_buf, &len, "\r\n", 0);

    DBG("Trying to write %d bytes http header:%s\n", len, send_buf);

#ifdef MTK_HTTPCLIENT_SSL_ENABLE
    if (client->remote_port == HTTPS_PORT) {
        DBG("Enter PolarSSL_write");
        if (httpclient_ssl_send_all(&client->ssl, send_buf, len) != len) {
            ERR("SSL_write failed");
            return HTTPCLIENT_ERROR;
        }
        return HTTPCLIENT_OK;
    }
#endif

    ret = httpclient_tcp_send_all(client->socket, send_buf, len);
    if (ret > 0) {
        DBG("Written %d bytes, socket = %d", ret, client->socket);
    } else if ( ret == 0 ) {
        WARN("ret == 0,Connection was closed by server");
        return HTTPCLIENT_CLOSED; /* Connection was closed by server */
    } else {
        ERR("Connection error (send returned %d)", ret);
        return HTTPCLIENT_ERROR_CONN;
    }

    return HTTPCLIENT_OK;
}

int httpclient_send_userdata(httpclient_t *client, httpclient_data_t *client_data)
{
    int ret = 0;

    if (client_data->post_buf && client_data->post_buf_len) {
        DBG("client_data->post_buf:%s", client_data->post_buf);
#ifdef MTK_HTTPCLIENT_SSL_ENABLE
        if (client->remote_port == HTTPS_PORT) {
            if (httpclient_ssl_send_all(&client->ssl, client_data->post_buf, client_data->post_buf_len) != client_data->post_buf_len) {
                ERR("SSL_write failed");
                return HTTPCLIENT_ERROR;
            }
        } else
#endif
        {
            ret = httpclient_tcp_send_all(client->socket, client_data->post_buf, client_data->post_buf_len);
            if (ret > 0) {
                DBG("Written %d bytes", ret);
            } else if ( ret == 0 ) {
                WARN("ret == 0,Connection was closed by server");
                return HTTPCLIENT_CLOSED; /* Connection was closed by server */
            } else {
                ERR("Connection error (send returned %d)", ret);
                return HTTPCLIENT_ERROR_CONN;
            }
        }
    }

    return HTTPCLIENT_OK;
}

int httpclient_recv(httpclient_t *client, char *buf, int min_len, int max_len, int *p_read_len)   /* 0 on success, err code on failure */
{
    int ret = 0;
    size_t readLen = 0;

    while (readLen < max_len) {
        buf[readLen] = '\0';
        if (client->remote_port != HTTPS_PORT) {
            if (readLen < min_len) {
                ret = recv(client->socket, buf + readLen, min_len - readLen, 0);
                DBG("%s, recv [blocking] return:%d", __func__, ret);
            } else {
                ret = recv(client->socket, buf + readLen, max_len - readLen, MSG_DONTWAIT);
                DBG("%s, recv [not blocking] return:%d", __func__, ret);
                if (ret == -1 && errno == EWOULDBLOCK) {
                    DBG("%s, recv [not blocking] EWOULDBLOCK", __func__);
                    break;
                }
            }
        }
#ifdef MTK_HTTPCLIENT_SSL_ENABLE
        else {
            if (readLen < min_len) {
                mbedtls_ssl_set_bio(&client->ssl, &client->fd, mbedtls_net_send, mbedtls_net_recv, NULL);
                ret = mbedtls_ssl_read(&client->ssl, (unsigned char *)buf + readLen, min_len - readLen);
                DBG("%s, mbedtls_ssl_read [blocking] return:%d", __func__, ret);
            } else {
                mbedtls_ssl_set_bio(&client->ssl, &client->fd, mbedtls_net_send, httpclient_ssl_nonblock_recv, NULL);
                ret = mbedtls_ssl_read(&client->ssl, (unsigned char *)buf + readLen, max_len - readLen);
                DBG("%s, mbedtls_ssl_read [not blocking] return:%d", __func__, ret);
                if (ret == -1 && errno == EWOULDBLOCK) {
                    DBG("%s, mbedtls_ssl_read [not blocking] EWOULDBLOCK", __func__);
                    break;
                }
            }
            if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {/* read already complete(if call mbedtls_ssl_read again, it will return 0(eof)) */
                break;
            }
        }
#endif

        if (ret > 0) {
            readLen += ret;
        } else if (ret == 0) {
            break;
        } else {
            ERR("Connection error (recv returned %d)", ret);
            *p_read_len = readLen;
            return HTTPCLIENT_ERROR_CONN;
        }
    }

    DBG("Read %d bytes", readLen);
    *p_read_len = readLen;
    buf[readLen] = '\0';

    return HTTPCLIENT_OK;
}

int httpclient_retrieve_content(httpclient_t *client, char *data, int len, httpclient_data_t *client_data)
{
    int count = 0;
    int templen = 0;
    int crlf_pos;
    /* Receive data */
    DBG("Receiving data:%s", data);
    client_data->is_more = true;

    if (client_data->response_content_len == -1 && client_data->is_chunked == false) {
        while(true)
        {
            int ret, max_len;
            if (count + len < client_data->response_buf_len - 1) {
                memcpy(client_data->response_buf + count, data, len);
                count += len;
                client_data->response_buf[count] = '\0';
            } else {
                memcpy(client_data->response_buf + count, data, client_data->response_buf_len - 1 - count);
                client_data->response_buf[client_data->response_buf_len - 1] = '\0';
                return HTTPCLIENT_RETRIEVE_MORE_DATA;
            }

            max_len = MIN(HTTPCLIENT_CHUNK_SIZE - 1, client_data->response_buf_len - 1 - count);
            ret = httpclient_recv(client, data, 1, max_len, &len);

            /* Receive data */
            DBG("data len: %d %d", len, count);

            if (ret == HTTPCLIENT_ERROR_CONN) {
                DBG("ret == HTTPCLIENT_ERROR_CONN");
                return ret;
            }

            if (len == 0) {/* read no more data */
                DBG("no more len == 0");
                client_data->is_more = false;
                return HTTPCLIENT_OK;
            }
        }
    }

    while (true) {
        size_t readLen = 0;

        if ( client_data->is_chunked && client_data->retrieve_len <= 0) {
            /* Read chunk header */
            bool foundCrlf;
            int n;
            do {
                foundCrlf = false;
                crlf_pos = 0;
                data[len] = 0;
                if (len >= 2) {
                    for (; crlf_pos < len - 2; crlf_pos++) {
                        if ( data[crlf_pos] == '\r' && data[crlf_pos + 1] == '\n' ) {
                            foundCrlf = true;
                            break;
                        }
                    }
                }
                if (!foundCrlf) { /* Try to read more */
                    if ( len < HTTPCLIENT_CHUNK_SIZE ) {
                        int new_trf_len, ret;
                        ret = httpclient_recv(client, data + len, 0, HTTPCLIENT_CHUNK_SIZE - len - 1, &new_trf_len);
                        len += new_trf_len;
                        if (ret == HTTPCLIENT_ERROR_CONN) {
                            return ret;
                        } else {
                            continue;
                        }
                    } else {
                        return HTTPCLIENT_ERROR;
                    }
                }
            } while (!foundCrlf);
            data[crlf_pos] = '\0';
            n = sscanf(data, "%x", &readLen);/* chunk length */
            client_data->retrieve_len = readLen;
            client_data->response_content_len += client_data->retrieve_len;
            if (n != 1) {
                ERR("Could not read chunk length");
                return HTTPCLIENT_ERROR_PRTCL;
            }

            memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2)); /* Not need to move NULL-terminating char any more */
            len -= (crlf_pos + 2);

            if ( readLen == 0 ) {
                /* Last chunk */
                client_data->is_more = false;
                DBG("no more (last chunk)");
                break;
            }
        } else {
            readLen = client_data->retrieve_len;
        }

        DBG("Retrieving %d bytes, len:%d", readLen, len);

        do {
            templen = MIN(len, readLen);
            if (count + templen < client_data->response_buf_len - 1) {
                memcpy(client_data->response_buf + count, data, templen);
                count += templen;
                client_data->response_buf[count] = '\0';
                client_data->retrieve_len -= templen;
            } else {
                memcpy(client_data->response_buf + count, data, client_data->response_buf_len - 1 - count);
                client_data->response_buf[client_data->response_buf_len - 1] = '\0';
                client_data->retrieve_len -= (client_data->response_buf_len - 1 - count);
                return HTTPCLIENT_RETRIEVE_MORE_DATA;
            }

            if ( len > readLen ) {
                DBG("memmove %d %d %d\n", readLen, len, client_data->retrieve_len);
                memmove(data, &data[readLen], len - readLen); /* chunk case, read between two chunks */
                len -= readLen;
                readLen = 0;
                client_data->retrieve_len = 0;
            } else {
                readLen -= len;
            }

            if (readLen) {
                int ret;
                int max_len = MIN(HTTPCLIENT_CHUNK_SIZE - 1, client_data->response_buf_len - 1 - count);
                ret = httpclient_recv(client, data, 1, max_len, &len);
                if (ret == HTTPCLIENT_ERROR_CONN) {
                    return ret;
                }
            }
        } while (readLen);

        if ( client_data->is_chunked ) {
            if (len < 2) {
                int new_trf_len, ret;
                /* Read missing chars to find end of chunk */
                ret = httpclient_recv(client, data + len, 2 - len, HTTPCLIENT_CHUNK_SIZE - len - 1, &new_trf_len);
                if (ret == HTTPCLIENT_ERROR_CONN) {
                    return ret;
                }
                len += new_trf_len;
            }
            if ( (data[0] != '\r') || (data[1] != '\n') ) {
                ERR("Format error, %s", data); /* after memmove, the beginning of next chunk */
                return HTTPCLIENT_ERROR_PRTCL;
            }
            memmove(data, &data[2], len - 2); /* remove the \r\n */
            len -= 2;
        } else {
            DBG("no more(content-length)\n");
            client_data->is_more = false;
            break;
        }

    }

    return HTTPCLIENT_OK;
}

int httpclient_response_parse(httpclient_t *client, char *data, int len, httpclient_data_t *client_data)
{
    int crlf_pos;

    client_data->response_content_len = -1;

    char *crlf_ptr = strstr(data, "\r\n");
    if (crlf_ptr == NULL) {
        ERR("\r\n not found");
        return HTTPCLIENT_ERROR_PRTCL;
    }

    crlf_pos = crlf_ptr - data;
    data[crlf_pos] = '\0';

    /* Parse HTTP response */
    if ( sscanf(data, "HTTP/%*d.%*d %d %*[^\r\n]", &(client->response_code)) != 1 ) {
        /* Cannot match string, error */
        ERR("Not a correct HTTP answer : %s\n", data);
        return HTTPCLIENT_ERROR_PRTCL;
    }

    if ( (client->response_code < 200) || (client->response_code >= 400) ) {
        /* Did not return a 2xx code; TODO fetch headers/(&data?) anyway and implement a mean of writing/reading headers */
        WARN("Response code %d", client->response_code);
    }

    DBG("Reading headers%s", data);

    memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2) + 1); /* Be sure to move NULL-terminating char as well */
    len -= (crlf_pos + 2);

    client_data->is_chunked = false;

    /* Now get headers */
    while ( true ) {
        char key[32];
        char value[32];
        int n;

        key[31] = '\0';
        value[31] = '\0';

        crlf_ptr = strstr(data, "\r\n");
        if (crlf_ptr == NULL) {
            if ( len < HTTPCLIENT_CHUNK_SIZE - 1 ) {
                int new_trf_len, ret;
                ret = httpclient_recv(client, data + len, 1, HTTPCLIENT_CHUNK_SIZE - len - 1, &new_trf_len);
                len += new_trf_len;
                data[len] = '\0';
                DBG("Read %d chars; In buf: [%s]", new_trf_len, data);
                if (ret == HTTPCLIENT_ERROR_CONN) {
                    return ret;
                } else {
                    continue;
                }
            } else {
                DBG("header len > chunksize");
                return HTTPCLIENT_ERROR;
            }
        }

        crlf_pos = crlf_ptr - data;
        if (crlf_pos == 0) { /* End of headers */
            memmove(data, &data[2], len - 2 + 1); /* Be sure to move NULL-terminating char as well */
            len -= 2;
            break;
        }

        data[crlf_pos] = '\0';

        n = sscanf(data, "%31[^:]: %31[^\r\n]", key, value);
        if ( n == 2 ) {
            DBG("Read header : %s: %s\n", key, value);
            if ( !strcmp(key, "Content-Length") ) {
                sscanf(value, "%d", &(client_data->response_content_len));
                client_data->retrieve_len = client_data->response_content_len;
            } else if ( !strcmp(key, "Transfer-Encoding") ) {
                if ( !strcmp(value, "Chunked") || !strcmp(value, "chunked") ) {
                    client_data->is_chunked = true;
                    client_data->response_content_len = 0;
                    client_data->retrieve_len = 0;
                }
            }
            memmove(data, &data[crlf_pos + 2], len - (crlf_pos + 2) + 1); /* Be sure to move NULL-terminating char as well */
            len -= (crlf_pos + 2);

        } else {
            ERR("Could not parse header");
            return HTTPCLIENT_ERROR;
        }
    }

    return httpclient_retrieve_content(client, data, len, client_data);
}


#ifdef MTK_HTTPCLIENT_SSL_ENABLE
u32_t httpclient_random(void)
{
    return (((u32_t)rand() << 16) + rand());
}

static int httpclient_ssl_random(void *p_rng, unsigned char *output, size_t output_len)
{
    uint32_t rnglen = output_len;
    uint8_t   rngoffset = 0;

    while (rnglen > 0) {
        *(output + rngoffset) = (unsigned char)httpclient_random() ;
        rngoffset++;
        rnglen--;
    }
    return 0;
}

static void httpclient_debug( void *ctx, int level, const char *file, int line, const char *str )
{
    printf("%s\n", str);
}

int httpclient_ssl_conn(httpclient_t *client, char *url, int port)
{
    char host[HTTPCLIENT_MAX_HOST_LEN];

    /* URL type: http[s]://host/ */
    if (HTTPCLIENT_ERROR_PARSE == httpclient_parse_host(url, host, HTTPCLIENT_MAX_HOST_LEN)) {
        WARN("Parse URL failed.");
        return -1;
    }

    return httpclient_ssl_start(&client->ssl, &client->fd, &client->conf, host);
}
#endif

HTTPCLIENT_RESULT httpclient_connect(httpclient_t *client, char *url, int port)
{
    int ret = HTTPCLIENT_ERROR_CONN;

    client->socket = -1;
    if (port != HTTPS_PORT) {
        ret = httpclient_conn(client, url, port);
        if (0 == ret) {
            client->remote_port = HTTP_PORT;
        }
    }
#ifdef MTK_HTTPCLIENT_SSL_ENABLE
    else {
        client->fd.fd = -1;
        ret = httpclient_ssl_conn(client, url, port);
        if (0 == ret) {
            client->remote_port = HTTPS_PORT;
            client->socket = client->fd.fd;
        }
    }
#endif

    return (HTTPCLIENT_RESULT)ret;
}

HTTPCLIENT_RESULT httpclient_send_request(httpclient_t *client, char *url, int method, httpclient_data_t *client_data)
{
    int ret = HTTPCLIENT_ERROR_CONN;

    if (client->socket < 0) {
        return (HTTPCLIENT_RESULT)ret;
    }

    ret = httpclient_send_header(client, url, method, client_data);
    if (ret != 0) {
        return (HTTPCLIENT_RESULT)ret;
    }

    if (method == HTTPCLIENT_POST || method == HTTPCLIENT_PUT) {
        ret = httpclient_send_userdata(client, client_data);
    }

    return (HTTPCLIENT_RESULT)ret;
}

HTTPCLIENT_RESULT httpclient_recv_response(httpclient_t *client, httpclient_data_t *client_data)
{
    int reclen = 0;
    int ret = HTTPCLIENT_ERROR_CONN;
    char buf[HTTPCLIENT_CHUNK_SIZE] = {0};

    if (client->socket < 0) {
        return (HTTPCLIENT_RESULT)ret;
    }

    if (client_data->is_more) {
        client_data->response_buf[0] = '\0';
        ret = httpclient_retrieve_content(client, buf, reclen, client_data);
    } else {
        ret = httpclient_recv(client, buf, 1, HTTPCLIENT_CHUNK_SIZE - 1, &reclen);
        if (ret != 0) {
            return (HTTPCLIENT_RESULT)ret;
        }

        buf[reclen] = '\0';

        if (reclen) {
            DBG("%s, reclen:%d, buf:", __func__, reclen);
            DBG("%s", buf);
            ret = httpclient_response_parse(client, buf, reclen, client_data);
        }
    }

    return (HTTPCLIENT_RESULT)ret;
}

void httpclient_close(httpclient_t *client, int port)
{
    if (port != HTTPS_PORT) {
        if (client->socket >= 0) {
            close(client->socket);
        }
        client->socket = -1;
    }
#ifdef MTK_HTTPCLIENT_SSL_ENABLE
    else {
        mbedtls_ssl_close_notify( &(client->ssl) );
        mbedtls_net_free( &client->fd );
        mbedtls_ssl_free( &(client->ssl) );
        mbedtls_ssl_config_free( &(client->conf) );
        client->socket = -1;
    }
#endif
}

int httpclient_get_response_code(httpclient_t *client)
{
    return client->response_code;
}

static HTTPCLIENT_RESULT httpclient_common(httpclient_t *client, char *url, int port, int method, httpclient_data_t *client_data)
{
    HTTPCLIENT_RESULT ret = HTTPCLIENT_ERROR_CONN;

    ret = httpclient_connect(client, url, port);

    if (!ret) {
        ret = httpclient_send_request(client, url, method, client_data);

        if (!ret) {
            ret = httpclient_recv_response(client, client_data);
        }
    }

    httpclient_close(client, port);

    return ret;
}

HTTPCLIENT_RESULT httpclient_get(httpclient_t *client, char *url, int port, httpclient_data_t *client_data)
{
    return httpclient_common(client, url, port, HTTPCLIENT_GET, client_data);
}

HTTPCLIENT_RESULT httpclient_post(httpclient_t *client, char *url, int port, httpclient_data_t *client_data)
{
    return httpclient_common(client, url, port, HTTPCLIENT_POST, client_data);
}

HTTPCLIENT_RESULT httpclient_put(httpclient_t *client, char *url, int port, httpclient_data_t *client_data)
{
    return httpclient_common(client, url, port, HTTPCLIENT_PUT, client_data);
}

HTTPCLIENT_RESULT httpclient_delete(httpclient_t *client, char *url, int port, httpclient_data_t *client_data)
{
    return httpclient_common(client, url, port, HTTPCLIENT_DELETE, client_data);
}

#ifdef MTK_HTTPCLIENT_SSL_ENABLE
int httpclient_ssl_nonblock_recv( void *ctx, unsigned char *buf, size_t len )
{
    int ret;
    int fd = ((mbedtls_net_context *) ctx)->fd;
    if ( fd < 0 ) {
        return ( MBEDTLS_ERR_NET_INVALID_CONTEXT );
    }

    ret = (int) recv( fd, buf, len, MSG_DONTWAIT );

    if ( ret < 0 ) {
#if ( defined(_WIN32) || defined(_WIN32_WCE) ) && !defined(EFIX64) && \
    !defined(EFI32)
        if ( WSAGetLastError() == WSAECONNRESET ) {
            return ( MBEDTLS_ERR_NET_CONN_RESET );
        }
#else
        if ( errno == EPIPE || errno == ECONNRESET ) {
            return ( MBEDTLS_ERR_NET_CONN_RESET );
        }

        if ( errno == EINTR ) {
            return ( MBEDTLS_ERR_SSL_WANT_READ );
        }
#endif
        if (ret == -1 && errno == EWOULDBLOCK) {
            return ret;
        }
        return ( MBEDTLS_ERR_NET_RECV_FAILED );
    }

    return ( ret );
}

int httpclient_ssl_send_all(mbedtls_ssl_context *ssl, const char *data, size_t length)
{
    size_t written_len = 0;

    while (written_len < length) {
        int ret = mbedtls_ssl_write(ssl, (unsigned char *)(data + written_len), (length - written_len));
        if (ret > 0) {
            written_len += ret;
            continue;
        } else if (ret == 0) {
            return written_len;
        } else {
            return -1; /* Connnection error */
        }
    }

    return written_len;
}

int httpclient_ssl_client_init(mbedtls_ssl_context *ssl,
                               mbedtls_net_context *tcp_fd,
                               mbedtls_ssl_config *conf)
{
    /*
     * 0. Initialize the RNG and the session data
     */
#if defined(MBEDTLS_DEBUG_C)
    mbedtls_debug_set_threshold(DEBUG_LEVEL);
#endif
    mbedtls_net_init( tcp_fd );
    mbedtls_ssl_init( ssl );
    mbedtls_ssl_config_init( conf );

    return 0;
}

int httpclient_ssl_start(mbedtls_ssl_context *pssl,
                         mbedtls_net_context *psocket,
                         mbedtls_ssl_config *conf,
                         char *host)
{
    int ret = -1;

    /*
     * 0. Init
     */
    if (0 != (ret = httpclient_ssl_client_init(pssl, psocket, conf))) {
        DBG( " failed ! httpclient_ssl_client_init returned -0x%04x", -ret );
        return ret;
    }
    DBG("  . Connecting to tcp/%s/%4d...", host, HTTPS_PORT);

    /*
     * 1. Start the connection
     */
    if (0 != (ret = mbedtls_net_connect(psocket, host, "443", MBEDTLS_NET_PROTO_TCP))) {
        DBG(" failed ! net_connect returned -0x%04x", -ret);
        return ret;
    }
    DBG( " ok\n" );

    /*
     * 2. Setup stuff
     */
    DBG( "  . Setting up the SSL/TLS structure..." );
    if ( ( ret = mbedtls_ssl_config_defaults( conf,
                 MBEDTLS_SSL_IS_CLIENT,
                 MBEDTLS_SSL_TRANSPORT_STREAM,
                 MBEDTLS_SSL_PRESET_DEFAULT ) ) != 0 ) {
        DBG( " failed\n  ! mbedtls_ssl_config_defaults returned %d\n\n", ret );
        return ret;
    }
    DBG( " ok\n" );

    /* OPTIONAL is not optimal for security,
      * but makes interop easier in this simplified example */
    mbedtls_ssl_conf_authmode( conf, MBEDTLS_SSL_VERIFY_NONE );
    mbedtls_ssl_conf_rng( conf, httpclient_ssl_random, NULL );
    mbedtls_ssl_conf_dbg( conf, httpclient_debug, NULL );

    if ( ( ret = mbedtls_ssl_setup( pssl, conf ) ) != 0 ) {
        DBG( " failed\n  ! mbedtls_ssl_setup returned %d\n\n", ret );
        return ret;
    }
    mbedtls_ssl_set_hostname(pssl, host);
    mbedtls_ssl_set_bio( pssl, psocket, mbedtls_net_send, mbedtls_net_recv, NULL );

    /*
     * 4. Handshake
     */
    DBG("  . Performing the SSL/TLS handshake...");

    while ((ret = mbedtls_ssl_handshake(pssl)) != 0) {
        if ((ret != MBEDTLS_ERR_SSL_WANT_READ) && (ret != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            DBG( " failed  ! mbedtls_ssl_handshake returned -0x%04x", -ret);
            return ret;
        }
    }
    DBG( " ok" );

    return 0;
}
#endif




