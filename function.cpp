#include "function.h"

void compute_message(char *message, const char *line) {
    strcat(message, line);
    strcat(message, "\r\n");
}

void error(const char *msg)
{
    perror(msg);
    exit(0);
}


char *compute_get_request(char *host, char *url, char *query_params, std::vector<std::string> cookies, int cookies_count) {
    char *message = new char[BUFLEN];
    char *line = new char[LINELEN];

    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (!cookies.empty()) {
        for (int i = 0; i < cookies_count; i++)
       {
            compute_message(message , cookies[i].c_str());
       }
    }

    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type, std::string body_data, int body_data_fields_count, std::vector<std::string> cookies, int cookies_count)
{
    char *message = new char[BUFLEN];
    char *line = new char[LINELEN];
    char *body_data_buffer = new char[LINELEN];

    int body_data_len = body_data.length();

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %d", body_data_len);
    compute_message(message, line);

    if (!cookies.empty()) {
       for (int i = 0; i < cookies_count; i++)
       {
            compute_message(message , cookies[i].c_str());
       }
    }
    compute_message(message , "");

    memset(line, 0, LINELEN);
    compute_message(message , body_data.c_str());

    free(line);

    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params, std::vector<std::string> cookies, int cookies_count) {
    char *message = new char[BUFLEN];
    char *line = new char[LINELEN];

    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (!cookies.empty()) {
        for (int i = 0; i < cookies_count; i++)
       {
            compute_message(message , cookies[i].c_str());
       }
    }

    compute_message(message, "");
    return message;
}

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag)
{
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);
    if (sockfd < 0)
        error("ERROR opening socket");

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(portno);
    inet_aton(host_ip, &serv_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

void close_connection(int sockfd)
{
    close(sockfd);
}

void send_to_server(int sockfd, char *message)
{
    int bytes, sent = 0;
    int total = strlen(message);

    do
    {
        bytes = write(sockfd, message + sent, total - sent);
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;
    } while (sent < total);
}

char *receive_from_server(int sockfd)
{
    char response[BUFLEN];
    buffer buffer = buffer_init();
    int header_end = 0;
    int content_length = 0;

    do {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0){
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
        
        header_end = buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;
            
            int content_length_start = buffer_find_insensitive(&buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);
            
            if (content_length_start < 0) {
                continue;           
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length = strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);
    size_t total = content_length + (size_t) header_end;
    
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t) bytes);
    }
    buffer_add(&buffer, "", 1);
    return buffer.data;
}

char *extract_cookies(std::string text) {
    const char* start = strstr(text.c_str(), "Cookie: ");
    const char* end = strstr(text.c_str(), "; Path");
    if (start != nullptr && end != nullptr) {
        const size_t length = end - start;
        char* substring = new char[length + 1];
        strncpy(substring, start, length);
        substring[length] = '\0';
        return substring;
    }
}

std::string extract_JTW(std::string text) {
    const char* start = strstr(text.c_str(), "token") + strlen("token") + 3;
    const char* end = strstr(text.c_str(), "}") - 1;
    if (start != nullptr && end != nullptr) {
        const size_t length = end - start;
        char* substring = new char[length + 1];
        strncpy(substring, start, length);
        substring[length] = '\0';
        return substring;
    }
}

std::string extract_json_list(std::string text) {
    const char* start = strstr(text.c_str(), "[");
    const char* end = strstr(text.c_str(), "]") + 1;
    if (start != nullptr && end != nullptr) {
        const size_t length = end - start;
        char* substring = new char[length + 1];
        strncpy(substring, start, length);
        substring[length] = '\0';
        return substring;
    }
}

std::string extract_json(std::string text) {
    const char* start = strstr(text.c_str(), "{");
    const char* end = strstr(text.c_str(), "}") + 1;
    if (start != nullptr && end != nullptr) {
        const size_t length = end - start;
        char* substring = new char[length + 1];
        strncpy(substring, start, length);
        substring[length] = '\0';
        return substring;
    }
}

bool is_number(const std::string& input) {
    for (char c : input) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

bool containsWhitespace(const std::string& str) {
    for (char c : str) {
        if (std::isspace(c)) {
            return true;
        }
    }
    return false;
}