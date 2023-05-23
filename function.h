#include <iostream>
#include <string.h>
#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <cctype>

#include "buffer.h"
#include "nlohmann/json.hpp"
#include "nlohmann/json_fwd.hpp"

#define BUFLEN 4096
#define LINELEN 1000

#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)


void compute_message(char *message, const char *line);
void error(const char *msg);


char *compute_get_request(char *host, char *url, char *query_params, std::vector<std::string> cookies, int cookies_count);
char *compute_post_request(char *host, char *url, char* content_type, std::string body_data, int body_data_fields_count, std::vector<std::string> cookies, int cookies_count);
char *compute_delete_request(char *host, char *url, char *query_params, std::vector<std::string> cookies, int cookies_count);

int open_connection(char *host_ip, int portno, int ip_type, int socket_type, int flag);
void close_connection(int sockfd);

void send_to_server(int sockfd, char *message);
char *receive_from_server(int sockfd);

char *extract_cookies(std::string text);
std::string extract_JTW(std::string text);
std::string extract_json_list(std::string text);
std::string extract_json(std::string text);

bool is_number(const std::string& input);
bool containsWhitespace(const std::string& str);