#include "function.h"

int main() {

    char* message;
    int sockfd;
    char* authorization = NULL;
    std::string token_JWT;

    while (1)
    {
        char *cmd = (char*)malloc(20 * sizeof(char));
        fgets(cmd, 20, stdin);
        if (!strncmp(cmd , "exit\n" , 5)) {
            break;
        }

        else if (!strncmp(cmd , "register\n" , 9)) {

            std::vector<std::string> empty_vect;
            std::string username , password , response;
            std::cout << "\nusername : ";
            std::cin >> username;

            if (username.find_first_of(" \t\n") != std::string::npos) {
                std::cout << "Username contains white space.\n\n";
                continue;
            }

            std::cout << "password : ";
            std::cin >> password;

            if (password.find_first_of(" \t\n") != std::string::npos) {
                std::cout << "Password contains white space.\n\n";
                continue;
            }

            nlohmann::json elem = {
                {"username" , username},
                {"password" , password},
            };

            std::cout << elem.dump() << "\n";

            message = compute_post_request(
                "34.254.242.81:8080",
                "/api/v1/tema/auth/register",
                "application/json", elem.dump() , 1, empty_vect, 0);

            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

            send_to_server(sockfd , message);

            response = receive_from_server(sockfd);

            std::string search_str = "error";
            if (response.find(search_str) != std::string::npos) {
                std::cout << "The username " << username << " is taken!" << std::endl;
            } else {
                std::cout << "User added successfully!" << std::endl;
            }

            close_connection(sockfd);

        }

        else if (!strncmp(cmd , "login\n" , 6)) {
            std::vector<std::string> empty_vect;
            std::string username , password , response;
            std::cout << "\nusername : ";
            std::cin >> username;

            if (username.find_first_of(" \t\n") != std::string::npos) {
                std::cout << "Username contains white space.\n\n";
                continue;
            }

            std::cout << "password : ";
            std::cin >> password;

            if (password.find_first_of(" \t\n") != std::string::npos) {
                std::cout << "Password contains white space.\n\n";
                continue;
            }

            nlohmann::json elem = {
                {"username" , username},
                {"password" , password},
            };

            message = compute_post_request(
                "34.254.242.81:8080",
                "/api/v1/tema/auth/login",
                "application/json", elem.dump() , 1, empty_vect , 0);

            sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

            send_to_server(sockfd , message);

            response = receive_from_server(sockfd);

            std::string search_str = "error";
            if (response.find(search_str) != std::string::npos) {
                std::cout << "Credentials are not good!\n\n";
            } else {
                std::cout << "Connect\n\n";
            }

            authorization = extract_cookies(response);

            close_connection(sockfd);

        }
        else if (!strncmp(cmd , "enter_library\n" , 14)) {
            
            std::string response;

            if (authorization == NULL) {
                std::cout << "You are not logged in!\n\n";
            } else {
                std::vector<std::string> cookies;
                cookies.push_back(authorization);

                message = compute_get_request(
                    "34.254.242.81:8080",
                    "/api/v1/tema/library/access",
                    NULL, cookies, 1
                );

                sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd , message);

                response = receive_from_server(sockfd);

                std::string search_str = "error";
                if (response.find(search_str) != std::string::npos) {
                    std::cout << "Error! You are not logged in!\n\n";
                } else {
                    std::cout << "You entered the library\n\n";
                    token_JWT = "Authorization: Bearer " + extract_JTW(response);
                }
            }
            close_connection(sockfd);
        }
        else if (!strncmp(cmd , "get_books\n" , 10)) {

            std::string response;

            if (!token_JWT.empty()) {
                std::vector<std::string> cookies;
                cookies.push_back(token_JWT);

                message = compute_get_request(
                    "34.254.242.81:8080",
                     "/api/v1/tema/library/books" ,
                     NULL , cookies , 1);

                sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd , message);

                response = receive_from_server(sockfd);

                std::string search_str = "error";
                if (response.find(search_str) != std::string::npos) {
                    std::cout << "Error! Something bad happened!" << "\n";
                } else {
                    std::string json_list = extract_json_list(response);
                    nlohmann::json j = nlohmann::json::parse(json_list);
                    if (j.is_array()) {
                        int i = 1;
                        for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
                            std::cout << std::to_string(i) <<")\nTitle: " << (*it)["title"].get<std::string>() << std::endl;
                            std::cout << "ID: " << (*it)["id"].get<int>() << std::endl;
                            std::cout << "\n";
                            i++;
                        }
                    }
                }
            } else {
                std::cout << "Error! You do not have access to the library!\n\n";
            }

            close_connection(sockfd);
        }
        else if (!strncmp(cmd , "get_book\n" , 9)) {
            std::string response;
            std::vector<std::string> cookies;
            cookies.push_back(token_JWT);
            
            int id;
            std::string input;

            std::cout << "\nid : ";
            std::cin >> input;

            if (!is_number(input)) {
                std::cout << "Id must be a number!\n\n";
                continue;
            } else {
                id = std::stoi(input);
            }

            nlohmann::json elem = {
                {"id" , id},
            };

            if (!token_JWT.empty()) {
                message = compute_get_request(
                    "34.254.242.81:8080",
                     (char *)(std::string("/api/v1/tema/library/books/") + std::to_string(id)).c_str(),
                     NULL , cookies , 1);

                sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd , message);

                response = receive_from_server(sockfd);

                std::string search_str = "error";
                if (response.find(search_str) != std::string::npos) {
                    std::cout << "Error! No book was found!\n\n";
                } else {
                    std::string json_list = extract_json(response);
                    nlohmann::json j = nlohmann::json::parse(json_list);
                    std::cout << "\n";

                    std::cout << "Title: " << j["title"].get<std::string>() << std::endl;
                    std::cout << "Author: " << j["author"].get<std::string>() << std::endl;
                    std::cout << "Publisher: " << j["publisher"].get<std::string>() << std::endl;
                    std::cout << "Genre: " << j["genre"].get<std::string>() << std::endl;
                    std::cout << "Page_count: " << j["page_count"].get<int>() << std::endl;
                    std::cout << "\n";
                }

            } else {
                std::cout << "Error! You do not have access to the library!\n\n";
            }
            close_connection(sockfd);
        }
        else if (!strncmp(cmd , "add_book\n" , 9)) {

            std::string response;
            std::vector<std::string> cookies;
            cookies.push_back(token_JWT);
            std::string title , author , genre , publisher ;
            int page_count;
            std::cout << "\ntitle : ";
            std::cin >> title;
            std::cout << "author : ";
            std::cin >> author;   
            std::cout << "genre : ";
            std::cin >> genre;
            std::cout << "publisher : ";
            std::cin >> publisher;

            std::string input;

            std::cout << "page_count : ";
            std::cin >> input;

            if (!is_number(input)) {
                std::cout << "Page count must be a number!\n\n";
                continue;
            } else {
                page_count = std::stoi(input);
            }

            nlohmann::json elem = {
                {"title" , title},
                {"author" , author},
                {"genre" , genre},
                {"publisher" , publisher},
                {"page_count" , page_count},
            };

            if (!token_JWT.empty()) {
                message = compute_post_request(
                "34.254.242.81:8080",
                "/api/v1/tema/library/books",
                "application/json", elem.dump() , 1, cookies , 1);

                sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd , message);

                response = receive_from_server(sockfd);

                std::string search_str = "error";
                if (response.find(search_str) != std::string::npos) {
                    std::cout << "\nError! Something bad happened!" << "\n\n";
                } else {
                    std::cout << "\nBook successfully added!\n\n";
                }

            } else {
                std::cout << "\nError! You do not have access to the library!\n\n";
            }
            close_connection(sockfd);
        }
        else if (!strncmp(cmd , "delete_book\n" , 12)) {
            int id;
            std::string input;

            std::cout << "\nid : ";
            std::cin >> input;

            if (!is_number(input)) {
                std::cout << "Id must be a number!\n\n";
                continue;
            } else {
                id = std::stoi(input);
            }

            nlohmann::json elem = {
                {"id" , id},
            };

            std::string response;
            std::vector<std::string> cookies;
            cookies.push_back(token_JWT);

            if (!token_JWT.empty()) {
                message = compute_delete_request(
                    "34.254.242.81:8080",
                     (char *)(std::string("/api/v1/tema/library/books/") + std::to_string(id)).c_str(),
                     NULL , cookies , 1);

                sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd , message);

                response = receive_from_server(sockfd);         

                std::string search_str = "error";
                if (response.find(search_str) != std::string::npos) {
                    std::cout << "Error! No book was deleted!\n\n";
                } else {
                    std::cout << "The book with ID = " << id << " has been successfully deleted!\n\n";
                }
            
            } else {
                std::cout << "Error! You do not have access to the library!\n\n";
            }
            close_connection(sockfd);

        }
        else if (!strncmp(cmd , "logout\n" , 7)) {
            
            std::string response;

            if (authorization == NULL) {
                std::cout << "You are not logged in!\n\n";
            } else {
                std::vector<std::string> cookies;
                cookies.push_back(authorization);

                message = compute_get_request(
                    "34.254.242.81:8080",
                     "/api/v1/tema/auth/logout" ,
                     NULL , cookies , 1);

                sockfd = open_connection("34.254.242.81", 8080, AF_INET, SOCK_STREAM, 0);

                send_to_server(sockfd , message);

                response = receive_from_server(sockfd);

                std::string search_str = "error";
                if (response.find(search_str) != std::string::npos) {
                    std::cout << "Error! Unsucessful logout!\n\n";
                } else {
                    std::cout << "Logout successful\n\n";
                }

                authorization = NULL;
                token_JWT.clear();
            }
            close_connection(sockfd);
        }
    }
    return 0;
}