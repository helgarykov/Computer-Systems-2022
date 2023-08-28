#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include<unistd.h>

#ifdef __APPLE__
#include "./endian.h"
#else
#include <endian.h>
#endif

#include "./networking.h"
#include "./sha256.h"

char server_ip[IP_LEN];
char server_port[PORT_LEN];
char my_ip[IP_LEN];
char my_port[PORT_LEN];

int c;

/*
 * Gets a sha256 hash of specified data, sourcedata. The hash itself is
 * placed into the given variable 'hash'. Any size can be created, but a
 * a normal size for the hash would be given by the global variable
 * 'SHA256_HASH_SIZE', that has been defined in sha256.h
 */
void get_data_sha(const char* sourcedata, hashdata_t hash, uint32_t data_size, 
    int hash_size)
{
  SHA256_CTX shactx;
  unsigned char shabuffer[hash_size];
  sha256_init(&shactx);
  sha256_update(&shactx, sourcedata, data_size);
  sha256_final(&shactx, shabuffer);

  for (int i=0; i<hash_size; i++)
  {
    hash[i] = shabuffer[i];
  }
}

/*
 * Gets a sha256 hash of specified data file, sourcefile. The hash itself is
 * placed into the given variable 'hash'. Any size can be created, but a
 * a normal size for the hash would be given by the global variable
 * 'SHA256_HASH_SIZE', that has been defined in sha256.h
 */
void get_file_sha(const char* sourcefile, hashdata_t hash, int size)
{
    int casc_file_size;

    FILE* fp = Fopen(sourcefile, "rb");
    if (fp == 0)
    {
        printf("Failed to open source: %s\n", sourcefile);
        return;
    }

    fseek(fp, 0L, SEEK_END);
    casc_file_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char local_buffer[casc_file_size];
    Fread(local_buffer, casc_file_size, 1, fp);
    Fclose(fp);

    get_data_sha(local_buffer, hash, casc_file_size, size);
}

/*
 * Combine a password and salt together and hash the result to form the 
 * 'signature'. The result should be written to the 'hash' variable. Note that 
 * as handed out, this function is never called. You will need to decide where 
 * it is sensible to do so.
 */
void get_signature(char* password, char* salt, hashdata_t* hash)
{
    char password_salt[PASSWORD_LEN + SALT_LEN];
    strcpy(password_salt, password);
    strncat(password_salt, salt, SALT_LEN);

    get_data_sha(password_salt, (uint8_t*)hash, strlen(password) + strlen(salt), SHA256_HASH_SIZE);
}

/// <summary> Builds the request header. Expected sizes: 16 + 32 + 4 = 52 bytes. </summary>
/// <param name="request"> Where the request header is built. </param>
void build_request_header(const char* username, char* signature, uint32_t request_length, char* request) {
    // put username in request
    for (int i = 0; i < USERNAME_LEN; i++) request[i] = username[i];

    // put signature in request
    for (int i = 0; i < SHA256_HASH_SIZE; i++) request[USERNAME_LEN + i] = signature[i];

    // put request_length in request (this is black magic [big endian] and I hate this with a passion!)
    // very imperative, but useful for this one purpose
    for (int i = 0; i < 4; i++) request[USERNAME_LEN + SHA256_HASH_SIZE + i] = '\0';
    request[USERNAME_LEN + SHA256_HASH_SIZE + 3] = request_length;
}

/*
 * Register a new user with a server by sending the username and signature to 
 * the server
 */
void register_user(char* username, char* password, char* salt)
{
    hashdata_t signature[SHA256_HASH_SIZE];
    char buffer[MAX_MSG_LEN];
    int clientfd;
    rio_t rio;

    clientfd = Open_clientfd(server_ip, server_port);
    Rio_readinitb(&rio, clientfd);

    get_signature(password, salt, signature);
    
    build_request_header(username, (char*)signature, 0, buffer);

    Rio_writen(clientfd, buffer, REQUEST_HEADER_LEN);
    Rio_readlineb(&rio, buffer, MAX_MSG_LEN);

    printf("(Server response) %s\n", &buffer[RESPONSE_HEADER_LEN]);
    Close(clientfd);
}

uint32_t create_uint32_t_from_uint8arr(char* buffer) {
    return ((uint8_t)buffer[3] | (uint8_t)buffer[2] << 8 | (uint8_t)buffer[1] << 16 | (uint8_t)buffer[0] << 24);
}

/*
 * Get a file from the server by sending the username and signature, along with
 * a file path. Note that this function should be able to deal with both small 
 * and large files. 
 */
void get_file(char* username, char* password, char* salt, char* to_get)
{
    FILE *f;
    hashdata_t signature[SHA256_HASH_SIZE];
    size_t request_body_length = strlen(to_get);
    uint32_t payload_length;
    int clientfd;
    char buffer[MAX_MSG_LEN]; // client buffer
    rio_t rio;

    clientfd = Open_clientfd(server_ip, server_port);
    Rio_readinitb(&rio, clientfd);
    f = fopen(to_get, "w");

    // Create signature
    get_signature(password, salt, signature);

    // Build the request header
    build_request_header(username, (char*)signature, request_body_length, buffer);

    // Build the request body
    for (int i = 0; i < (int)request_body_length; i++) {
        buffer[REQUEST_HEADER_LEN + i] = to_get[i];
    }
    
    // Send the file request to the server
    Rio_writen(clientfd, buffer, REQUEST_HEADER_LEN + (int)request_body_length);

    // Handle response
    while (Rio_readnb(&rio, buffer, MAX_MSG_LEN) != 0) {
        payload_length = create_uint32_t_from_uint8arr(buffer);
        if (buffer[7] != 1) {
            printf("Server threw an error (Server response): %.*s\n", payload_length, &buffer[RESPONSE_HEADER_LEN]);
            remove(to_get);
            break;
        }
        fwrite(&buffer[RESPONSE_HEADER_LEN], 1, payload_length, f);
    }

    Close(clientfd);
}

int main(int argc, char **argv)
{
    // Users should call this script with a single argument describing what 
    // config to use
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <config file>\n", argv[0]);
        exit(EXIT_FAILURE);
    } 

    // Read in configuration options. Should include a client_directory, 
    // client_ip, client_port, server_ip, and server_port
    char buffer[128];
    fprintf(stderr, "Got config path at: %s\n", argv[1]);
    FILE* fp = Fopen(argv[1], "r");
    while (fgets(buffer, 128, fp)) {
        if (starts_with(buffer, CLIENT_IP)) {
            memcpy(my_ip, &buffer[strlen(CLIENT_IP)], 
                strcspn(buffer, "\r\n")-strlen(CLIENT_IP));
            if (!is_valid_ip(my_ip)) {
                fprintf(stderr, ">> Invalid client IP: %s\n", my_ip);
                exit(EXIT_FAILURE);
            }
        }else if (starts_with(buffer, CLIENT_PORT)) {
            memcpy(my_port, &buffer[strlen(CLIENT_PORT)], 
                strcspn(buffer, "\r\n")-strlen(CLIENT_PORT));
            if (!is_valid_port(my_port)) {
                fprintf(stderr, ">> Invalid client port: %s\n", my_port);
                exit(EXIT_FAILURE);
            }
        }else if (starts_with(buffer, SERVER_IP)) {
            memcpy(server_ip, &buffer[strlen(SERVER_IP)], 
                strcspn(buffer, "\r\n")-strlen(SERVER_IP));
            if (!is_valid_ip(server_ip)) {
                fprintf(stderr, ">> Invalid server IP: %s\n", server_ip);
                exit(EXIT_FAILURE);
            }
        }else if (starts_with(buffer, SERVER_PORT)) {
            memcpy(server_port, &buffer[strlen(SERVER_PORT)], 
                strcspn(buffer, "\r\n")-strlen(SERVER_PORT));
            if (!is_valid_port(server_port)) {
                fprintf(stderr, ">> Invalid server port: %s\n", server_port);
                exit(EXIT_FAILURE);
            }
        }        
    }
    fclose(fp);

    fprintf(stdout, "Client at: %s:%s\n", my_ip, my_port);
    fprintf(stdout, "Server at: %s:%s\n", server_ip, server_port);

    char username[USERNAME_LEN];
    char password[PASSWORD_LEN];
    char user_salt[SALT_LEN+1];
    
    fprintf(stdout, "Enter a username to proceed: ");
    scanf("%16s", username);
    while ((c = getchar()) != '\n' && c != EOF);
    // Clean up username string as otherwise some extra chars can sneak in.
    for (int i=strlen(username); i<USERNAME_LEN; i++)
    {
        username[i] = '\0';
    }
 
    fprintf(stdout, "Enter your password to proceed: ");
    scanf("%16s", password);
    while ((c = getchar()) != '\n' && c != EOF);
    // Clean up password string as otherwise some extra chars can sneak in.
    for (int i=strlen(password); i<PASSWORD_LEN; i++)
    {
        password[i] = '\0';
    }

    // Note that a random salt should be used, but you may find it easier to
    // repeatedly test the same user credentials by using the hard coded value
    // below instead, and commenting out this randomly generating section.
    for (int i=0; i<SALT_LEN; i++)
    {
        user_salt[i] = 'a' + (random() % 26);
    }
    user_salt[SALT_LEN] = '\0';
    //strncpy(user_salt, 
    //    "0123456789012345678901234567890123456789012345678901234567890123\0", 
    //    SALT_LEN+1);

    fprintf(stdout, "Using salt: %s\n", user_salt);

    // Try to get file before registration
    get_file(username, password, user_salt, "tiny_test.txt");

    // Register user with empty username
    register_user("", password, user_salt);

    // Register the given user
    register_user(username, password, user_salt);

    // Try to register same user
    register_user(username, password, user_salt);

    // Retrieve the smaller file, that doesn't not require support for blocks
    get_file(username, password, user_salt, "tiny.txt");

    // Retrieve the larger file, that requires support for blocked messages
    get_file(username, password, user_salt, "hamlet.txt");

    // Try to get a non-existing file
    get_file(username, password, user_salt, "m4g1c.txt");

    // Try to get an empty file
    get_file(username, password, user_salt, "empty.txt");

    exit(EXIT_SUCCESS);
}