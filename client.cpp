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

// =====================================================================================================================        

        else if (!strncmp(cmd , "register\n" , 9)) {

            if (authorization != NULL) {
                std::cout << "  !!! You are already logged in!\n\n";
                continue;
            }

            std::vector<std::string> empty_vect;
            std::string username , password , response;
            
            int cont = 1;

            while (cont) {
                std::cout << "username : ";
                getline(std::cin , username);

                if (containsWhitespace(username)) {
                    std::cout << "  !!! Username contains white space.\n";
                } else {
                    cont = 0;
                }
            }

            cont = 1;

            while (cont) {
                std::cout << "password : ";
                getline(std::cin , password);


                if (containsWhitespace(password)) {
                    std::cout << "  !!! Password contains white space.\n";
                } else { 
                    cont = 0;
                }
            }

            nlohmann::json elem = {
                {"username" , username},
                {"password" , password},
            };

            message = compute_post_request(
                (char *)("34.254.242.81:8080"),
                (char *)("/api/v1/tema/auth/register"),
                (char *)("application/json"), elem.dump() , 1, empty_vect, 0);

            sockfd = open_connection((char *)("34.254.242.81"), 8080, AF_INET, SOCK_STREAM, 0);

            send_to_server(sockfd , message);

            response = receive_from_server(sockfd);

            std::string search_str = "error";
            if (response.find(search_str) != std::string::npos) {
                std::cout << "  !!! The username " << username << " is taken!\n\n";
            } else {
                std::cout << "  User added successfully!\n\n";
            }

            close_connection(sockfd);

        }

// =====================================================================================================================          

        else if (!strncmp(cmd , "login\n" , 6)) {
            std::vector<std::string> empty_vect;
            std::string username , password , response;
            int cont = 1;

            if (authorization != NULL) {
                std::cout << "  !!! You are already logged in!\n";
                continue;
            }

            while (cont) {
                std::cout << "username : ";
                getline(std::cin , username);

                if (containsWhitespace(username)) {
                    std::cout << "  !!! Username contains white space.\n";
                } else {
                    cont = 0;
                }
            }

            cont = 1;

            while (cont) {
                std::cout << "password : ";
                getline(std::cin , password);


                if (containsWhitespace(password)) {
                    std::cout << "  !!! Password contains white space.\n";
                } else { 
                    cont = 0;
                }
            }

            nlohmann::json elem = {
                {"username" , username},
                {"password" , password},
            };

            message = compute_post_request(
                (char *)("34.254.242.81:8080"),
                (char *)("/api/v1/tema/auth/login"),
                (char *)("application/json"), elem.dump() , 1, empty_vect , 0);

            sockfd = open_connection((char *)("34.254.242.81"), 8080, AF_INET, SOCK_STREAM, 0);

            send_to_server(sockfd , message);

            response = receive_from_server(sockfd);

            std::string search_str = "error";
            if (response.find(search_str) != std::string::npos) {
                std::cout << "  !!! Credentials are not good!\n\n";
            } else {
                std::cout << "  Connect\n\n";
            }

            authorization = extract_cookies(response);

            close_connection(sockfd);

        }

// =====================================================================================================================  

        else if (!strncmp(cmd , "enter_library\n" , 14)) {
            
            std::string response;

            if (authorization == NULL) {
                std::cout << "  !!! You are not logged in!\n\n";
                continue;
            } 

            if (!token_JWT.empty()) {
                std::cout << "  !!! You are already in the library!\n\n";
                continue;
            }

            std::vector<std::string> cookies;
            cookies.push_back(authorization);

            message = compute_get_request(
                (char *)("34.254.242.81:8080"),
                (char *)("/api/v1/tema/library/access"),
                NULL, cookies, 1
            );

            sockfd = open_connection((char *)("34.254.242.81"), 8080, AF_INET, SOCK_STREAM, 0);

            send_to_server(sockfd , message);

            response = receive_from_server(sockfd);

            std::string search_str = "error";
            if (response.find(search_str) != std::string::npos) {
                std::cout << "  !!! Something bad happened!\n\n";
            } else {
                std::cout << "  You entered the library\n\n";
                token_JWT = "Authorization: Bearer " + extract_JTW(response);
            }
            
            close_connection(sockfd);
        }

// =====================================================================================================================  

        else if (!strncmp(cmd , "get_books\n" , 10)) {

            std::string response;

            if (authorization == NULL) {
                std::cout << "  !!! You are not logged in!\n\n";
                continue;
            }

            if (token_JWT.empty()) {
                std::cout << "  !!! You do not have access to the library!\n\n";
                continue;
            }


            std::vector<std::string> cookies;
            cookies.push_back(token_JWT);

            message = compute_get_request(
                (char *)("34.254.242.81:8080"),
                (char *)("/api/v1/tema/library/books") ,
                    NULL , cookies , 1);

            sockfd = open_connection((char *)("34.254.242.81"), 8080, AF_INET, SOCK_STREAM, 0);

            send_to_server(sockfd , message);

            response = receive_from_server(sockfd);

            std::string search_str = "error";
            if (response.find(search_str) != std::string::npos) {
                std::cout << "  !!! Something bad happened!" << "\n";
            } else {
                std::string json_list = extract_json_list(response);
                nlohmann::json j = nlohmann::json::parse(json_list);
                if (j.is_array()) {
                    for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) {
                        std::cout <<"{\n\tTitle: " << (*it)["title"].get<std::string>() << std::endl;
                        std::cout << "\tID: " << (*it)["id"].get<int>() << std::endl;
                        std::cout << "}\n";
                    }
                }
                std::cout << "\n";
            }

            close_connection(sockfd);
        }

// =====================================================================================================================  

        else if (!strncmp(cmd , "get_book\n" , 9)) {

            if (authorization == NULL) {
                std::cout << "  !!! You are not logged in!\n\n";
                continue;
            }

            if (token_JWT.empty()) {
                std::cout << "  !!! You do not have access to the library!\n\n";
                continue;
            }

            std::string response;
            std::vector<std::string> cookies;
            cookies.push_back(token_JWT);
            
            int id;
            std::string input;
            int cout = 1;

            while (cout) {
                std::cout << "id : ";
                std::cin >> input;

                if (is_number(input)) {
                    cout = 0;
                } else {
                    std::cout << "  !!! Id must be a number!\n";
                }
            }

            id = std::stoi(input);

            nlohmann::json elem = {
                {"id" , id},
            };

            if (token_JWT.empty()) {

                std::cout << "Error! You do not have access to the library!\n\n";
                continue;

            }

            message = compute_get_request(
                (char *)("34.254.242.81:8080"),
                (char *)(std::string("/api/v1/tema/library/books/") + std::to_string(id)).c_str(),
                NULL , cookies , 1);

            sockfd = open_connection((char *)("34.254.242.81"), 8080, AF_INET, SOCK_STREAM, 0);

            send_to_server(sockfd , message);

            response = receive_from_server(sockfd);

            std::string search_str = "error";
            if (response.find(search_str) != std::string::npos) {
                std::cout << "Error! No book was found!\n\n";
                continue;
            } 

            std::string json_list = extract_json(response);
            nlohmann::json j = nlohmann::json::parse(json_list);

            std::cout << "{\n\tTitle: " << j["title"].get<std::string>() << std::endl;
            std::cout << "\tAuthor: " << j["author"].get<std::string>() << std::endl;
            std::cout << "\tPublisher: " << j["publisher"].get<std::string>() << std::endl;
            std::cout << "\tGenre: " << j["genre"].get<std::string>() << std::endl;
            std::cout << "\tPage_count: " << j["page_count"].get<int>() << std::endl;
            std::cout << "}\n\n";

            close_connection(sockfd);
        }

// =====================================================================================================================  

        else if (!strncmp(cmd , "add_book\n" , 9)) {

            if (authorization == NULL) {
                std::cout << "  !!! You are not logged in!\n\n";
                continue;
            } 

            if (token_JWT.empty()) {
                std::cout << "  !!! You do not have access to the library!\n\n";
                continue;
            }

            std::string response;
            std::vector<std::string> cookies;
            cookies.push_back(token_JWT);
            std::string title , author , genre , publisher ;
            int page_count;
            std::cout << "\ntitle : ";
            getline(std::cin , title);
            std::cout << "author : ";
            getline(std::cin , author);
            std::cout << "genre : ";
            getline(std::cin , genre);
            std::cout << "publisher : ";
            getline(std::cin , publisher);

            std::string input;

            int cout = 1;

            while (cout) {
                std::cout << "page_count : ";
                std::cin >> input;
                if (is_number(input)) {
                    cout = 0;
                } else {
                    std::cout << "  !!! Page count must be a number!\n";
                }
            }

            page_count = std::stoi(input);

            nlohmann::json elem = {
                {"title" , title},
                {"author" , author},
                {"genre" , genre},
                {"publisher" , publisher},
                {"page_count" , page_count},
            };

            message = compute_post_request(
            (char *)("34.254.242.81:8080"),
            (char *)("/api/v1/tema/library/books"),
            (char *)("application/json"), elem.dump() , 1, cookies , 1);

            sockfd = open_connection((char *)("34.254.242.81"), 8080, AF_INET, SOCK_STREAM, 0);

            send_to_server(sockfd , message);

            response = receive_from_server(sockfd);

            std::string search_str = "error";
            if (response.find(search_str) != std::string::npos) {
                std::cout << "  !!! Something bad happened!\n\n";
            } else {
                std::cout << "  Book successfully added!\n\n";
            }

            close_connection(sockfd);
        }

// =====================================================================================================================  

        else if (!strncmp(cmd , "delete_book\n" , 12)) {

            if (authorization == NULL) {
                std::cout << "  !!! You are not logged in!\n\n";
                continue;
            }

            if (token_JWT.empty()) {
                std::cout << "  !!! You do not have access to the library!\n\n";
                continue;
            }

            int id;
            std::string input;

            int cout = 1;

            while (cout) {
                std::cout << "id : ";
                std::cin >> input;

                if (is_number(input)) {
                    cout = 0;
                } else {
                    std::cout << "  !!! Id must be a number!\n";
                }
            }

            id = std::stoi(input);

            nlohmann::json elem = {
                {"id" , id},
            };

            std::string response;
            std::vector<std::string> cookies;
            cookies.push_back(token_JWT);

            message = compute_delete_request(
                (char *)("34.254.242.81:8080"),
                (char *)(std::string("/api/v1/tema/library/books/") + std::to_string(id)).c_str(),
                NULL , cookies , 1);

            sockfd = open_connection((char *)("34.254.242.81"), 8080, AF_INET, SOCK_STREAM, 0);

            send_to_server(sockfd , message);

            response = receive_from_server(sockfd);         

            std::string search_str = "error";
            if (response.find(search_str) != std::string::npos) {
                std::cout << "  !!! No book was deleted!\n\n";
            } else {
                std::cout << "  The book with ID = " << id << " has been successfully deleted!\n\n";
            }

            close_connection(sockfd);

        }

// =====================================================================================================================  

        else if (!strncmp(cmd , "logout\n" , 7)) {
            
            if (authorization == NULL) {
                std::cout << "  !!! You are not logged in!\n\n";
                continue;
            }

            std::string response;

            std::vector<std::string> cookies;
            cookies.push_back(authorization);

            message = compute_get_request(
                (char *)("34.254.242.81:8080"),
                (char *)("/api/v1/tema/auth/logout") ,
                NULL , cookies , 1);

            sockfd = open_connection((char *)("34.254.242.81"), 8080, AF_INET, SOCK_STREAM, 0);

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

            close_connection(sockfd);
        }
    }
    return 0;
}