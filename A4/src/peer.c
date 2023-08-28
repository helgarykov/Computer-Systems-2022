#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#ifdef __APPLE__
#include "./endian.h"
#else
#include <endian.h>
#endif

#include "./peer.h"
#include "./sha256.h"


// Global variables to be used by both the server and client side of the peer.
// Some of these are not currently used but should be considered STRONG hints
PeerAddress_t *my_address;

pthread_mutex_t network_mutex = PTHREAD_MUTEX_INITIALIZER;
PeerAddress_t** network = NULL;
uint32_t peer_count = 0;

pthread_mutex_t retrieving_mutex = PTHREAD_MUTEX_INITIALIZER;
FilePath_t* retrieving_files = NULL;
uint32_t file_count = 0;


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

    char buffer[casc_file_size];
    Fread(buffer, casc_file_size, 1, fp);
    Fclose(fp);

    get_data_sha(buffer, hash, casc_file_size, size);
}

/*
 * A simple min function, which apparently C doesn't have as standard
 */
uint32_t min(int a, int b)
{
    if (a < b) 
    {
        return a;
    }
    return b;
}

/*
 * Select a peer from the network at random, without picking the peer defined
 * in my_address
 */
void get_random_peer(PeerAddress_t* peer_address)
{ 
    PeerAddress_t** potential_peers = Malloc(sizeof(PeerAddress_t*));
    uint32_t potential_count = 0; 
    for (uint32_t i=0; i<peer_count; i++)
    {
        if (strcmp(network[i]->ip, my_address->ip) != 0 
                || strcmp(network[i]->port, my_address->port) != 0 )
        {
            potential_peers = realloc(potential_peers, 
                (potential_count+1) * sizeof(PeerAddress_t*));
            potential_peers[potential_count] = network[i];
            potential_count++;
        }
    }

    if (potential_count == 0)
    {
        printf("No peers to connect to. You probably have not implemented "
            "registering with the network yet.\n");
    }

    uint32_t random_peer_index = rand() % potential_count;

    memcpy(peer_address->ip, potential_peers[random_peer_index]->ip, IP_LEN);
    memcpy(peer_address->port, potential_peers[random_peer_index]->port, 
        PORT_LEN);

    Free(potential_peers);

    printf("Selected random peer: %s:%s\n", 
        peer_address->ip, peer_address->port);
}

/*
 * Send a request message to another peer on the network. Unless this is 
 * specifically an 'inform' message as described in the assignment handout, a 
 * reply will always be expected.
 */
void send_message(PeerAddress_t peer_address, int command, char* request_body)
{
    fprintf(stdout, "Connecting to server at %s:%s to run command %d (%s)\n", 
        peer_address.ip, peer_address.port, command, request_body);

    rio_t rio;
    char msg_buf[MAX_MSG_LEN];
    FILE* fp;

    // Setup the eventual output file path. This is being done early so if 
    // something does go wrong at this stage we can avoid all that pesky 
    // networking
    char output_file_path[strlen(request_body)+1];
    if (command == COMMAND_RETREIVE)
    {     
        strcpy(output_file_path, request_body);

        if (access(output_file_path, F_OK ) != 0 ) 
        {
            fp = Fopen(output_file_path, "a");
            pthread_mutex_lock(&retrieving_mutex);
            memcpy(retrieving_files->path, output_file_path, PATH_LEN);
            pthread_mutex_unlock(&retrieving_mutex);
            Fclose(fp);
        }
    }

    //Check if command is inform
    uint32_t request_body_len = strlen(request_body);
    if (command == COMMAND_INFORM) {
        request_body_len = IP_LEN + 4;
    }

    // Setup connection
    int peer_socket = Open_clientfd(peer_address.ip, peer_address.port);
    Rio_readinitb(&rio, peer_socket);

    // Construct a request message and send it to the peer
    struct RequestHeader request_header;
    strncpy(request_header.ip, my_address->ip, IP_LEN);
    request_header.port = htonl(atoi(my_address->port));
    request_header.command = htonl(command);
    request_header.length = htonl(request_body_len);

    memcpy(msg_buf, &request_header, REQUEST_HEADER_LEN);
    memcpy(msg_buf+REQUEST_HEADER_LEN, request_body, request_body_len);

    Rio_writen(peer_socket, msg_buf, REQUEST_HEADER_LEN+request_body_len);

    // We don't expect replies to inform messages so we're done here
    if (command == COMMAND_INFORM)
    {
        return;
    }

    // Read a reply
    Rio_readnb(&rio, msg_buf, REPLY_HEADER_LEN);

    // Extract the reply header 
    char reply_header[REPLY_HEADER_LEN];
    memcpy(reply_header, msg_buf, REPLY_HEADER_LEN);

    uint32_t reply_length = ntohl(*(uint32_t*)&reply_header[0]);
    uint32_t reply_status = ntohl(*(uint32_t*)&reply_header[4]);
    uint32_t this_block = ntohl(*(uint32_t*)&reply_header[8]);
    uint32_t block_count = ntohl(*(uint32_t*)&reply_header[12]);
    hashdata_t block_hash;
    memcpy(block_hash, &reply_header[16], SHA256_HASH_SIZE);
    hashdata_t total_hash;
    memcpy(total_hash, &reply_header[48], SHA256_HASH_SIZE);

    // Determine how many blocks we are about to recieve
    hashdata_t ref_hash;
    memcpy(ref_hash, &total_hash, SHA256_HASH_SIZE);
    uint32_t ref_count = block_count;

    // Loop until all blocks have been recieved
    for (uint32_t b=0; b<ref_count; b++)
    {
        // Don't need to re-read the first block
        if (b > 0)
        {
            // Read the response
            Rio_readnb(&rio, msg_buf, REPLY_HEADER_LEN);

            // Read header
            memcpy(reply_header, msg_buf, REPLY_HEADER_LEN);

            // Parse the attributes
            reply_length = ntohl(*(uint32_t*)&reply_header[0]);
            reply_status = ntohl(*(uint32_t*)&reply_header[4]);
            this_block = ntohl(*(uint32_t*)&reply_header[8]);
            block_count = ntohl(*(uint32_t*)&reply_header[12]);

            memcpy(block_hash, &reply_header[16], SHA256_HASH_SIZE);
            memcpy(total_hash, &reply_header[48], SHA256_HASH_SIZE);

            // Check we're getting consistent results
            if (ref_count != block_count)
            {
                fprintf(stdout, 
                    "Got inconsistent block counts between blocks\n");
                Close(peer_socket);
                return;
            }

            for (int i=0; i<SHA256_HASH_SIZE; i++)
            {
                if (ref_hash[i] != total_hash[i])
                {
                    fprintf(stdout, 
                        "Got inconsistent total hashes between blocks\n");
                    Close(peer_socket);
                    return;
                }
            }
        }

        // Check response status
        if (reply_status != STATUS_OK)
        {
            if (command == COMMAND_REGISTER && reply_status == STATUS_PEER_EXISTS)
            {
                printf("Peer already exists\n");
            }
            else
            {
                printf("Got unexpected status %d\n", reply_status);
                Close(peer_socket);
                return;
            }
        }

        // Read the payload
        char payload[reply_length+1];
        Rio_readnb(&rio, msg_buf, reply_length);
        memcpy(payload, msg_buf, reply_length);
        payload[reply_length] = '\0';
        
        // Check the hash of the data is as expected
        hashdata_t payload_hash;
        get_data_sha(payload, payload_hash, reply_length, SHA256_HASH_SIZE);

        for (int i=0; i<SHA256_HASH_SIZE; i++)
        {
            if (payload_hash[i] != block_hash[i])
            {
                fprintf(stdout, "Payload hash does not match specified\n");
                Close(peer_socket);
                return;
            }
        }

        // If we're trying to get a file, actually write that file
        if (command == COMMAND_RETREIVE)
        {
            // Check we can access the output file
            fp = Fopen(output_file_path, "r+b");
            if (fp == 0)
            {
                printf("Failed to open destination: %s\n", output_file_path);
                Close(peer_socket);
            }

            uint32_t offset = this_block * (MAX_MSG_LEN-REPLY_HEADER_LEN);
            fprintf(stdout, "Block num: %d/%d (offset: %d)\n", this_block+1, 
                block_count, offset);
            fprintf(stdout, "Writing from %d to %d\n", offset, 
                offset+reply_length);

            // Write data to the output file, at the appropriate place
            fseek(fp, offset, SEEK_SET);
            Fputs(payload, fp);
            Fclose(fp);
            pthread_mutex_lock(&retrieving_mutex);
            memcpy(retrieving_files->path, "", PATH_LEN);
            pthread_mutex_unlock(&retrieving_mutex);
        }
    }

    // Confirm that our file is indeed correct
    if (command == COMMAND_RETREIVE)
    {
        fprintf(stdout, "Got data and wrote to %s\n", output_file_path);

        // Finally, check that the hash of all the data is as expected
        hashdata_t file_hash;
        get_file_sha(output_file_path, file_hash, SHA256_HASH_SIZE);

        for (int i=0; i<SHA256_HASH_SIZE; i++)
        {
            if (file_hash[i] != total_hash[i])
            {
                fprintf(stdout, "File hash does not match specified for %s\n", 
                    output_file_path);
                Close(peer_socket);
                return;
            }
        }
    }

    // If we are registering with the network we should note the complete 
    // network reply
    char* reply_body = Malloc(reply_length + 1);
    memset(reply_body, 0, reply_length + 1);
    memcpy(reply_body, msg_buf, reply_length);

    if (reply_status == STATUS_OK)
    {
        if (command == COMMAND_REGISTER)
        {
            // Your code here. This code has been added as a guide, but feel 
            // free to add more, or work in other parts of the code
            pthread_mutex_lock(&network_mutex);
            for (uint32_t i = 0; i < reply_length; i = i + 20) { 
                
                // IP and Port (20 bytes)
                //Retrieving IP (Bytes 16)
                PeerAddress_t *peer_address = (PeerAddress_t*)Malloc(sizeof(PeerAddress_t));
                memcpy(peer_address->ip, &reply_body[i], IP_LEN);
                
                //Retrieving Port (Bytes 4)
                uint32_t port = ntohl(*(uint32_t*)&reply_body[i+IP_LEN]);
                sprintf(peer_address->port, "%d", port);
                
                //Updating network list
                network[i/20] = peer_address;
            }
            peer_count = reply_length/20;
            pthread_mutex_unlock(&network_mutex);
        }
    } 
    else
    {
        printf("Got response code: %d, %s\n", reply_status, reply_body);
    }
    Free(reply_body);
    Close(peer_socket);
}


/*
 * Function to act as thread for all required client interactions. This thread 
 * will be run concurrently with the server_thread but is finite in nature.
 * 
 * This is just to register with a network, then download two files from a 
 * random peer on that network. As in A3, you are allowed to use a more 
 * user-friendly setup with user interaction for what files to retrieve if 
 * preferred, this is merely presented as a convienient setup for meeting the 
 * assignment tasks
 */ 
void* client_thread(void* thread_args)
{
    struct PeerAddress *peer_address = thread_args;

    // Register the given user
    send_message(*peer_address, COMMAND_REGISTER, "\0");

    // Update peer_address with random peer from network
    get_random_peer(peer_address);

    // Retrieve the smaller file, that doesn't not require support for blocks
    send_message(*peer_address, COMMAND_RETREIVE, "tiny.txt");

    // Update peer_address with random peer from network
    get_random_peer(peer_address);

    // Retrieve the larger file, that requires support for blocked messages
    send_message(*peer_address, COMMAND_RETREIVE, "hamlet.txt");

    // Update peer_address with random peer from network
    //get_random_peer(peer_address);

    // Request a file that does not exist
    //send_message(*peer_address, COMMAND_RETREIVE, "no_file.txt");

    return NULL;
}

/*
 * Handle any 'register' type requests, as defined in the asignment text. This
 * should always generate a response.
 */
void handle_register(int connfd, char* client_ip, int client_port_int)
{   
    char port_string[PORT_LEN];
    sprintf(port_string, "%d", client_port_int);
    uint32_t status_code = STATUS_OK;

    if (!is_valid_ip(client_ip) || !is_valid_port(port_string)) {
        status_code = STATUS_MALFORMED;
    }

    pthread_mutex_lock(&network_mutex);
    for (uint32_t i = 0; i < peer_count; i++) {
        if (strcmp(client_ip, network[i]->ip) == 0 &&
            strcmp(port_string, network[i]->port) == 0) {
                status_code = STATUS_PEER_EXISTS;
        }
    }

    PeerAddress_t *peer_address = (PeerAddress_t*)Malloc(sizeof(PeerAddress_t));
    memcpy(peer_address->ip, client_ip, IP_LEN);
    memcpy(peer_address->port, port_string, IP_LEN);
    
    if (status_code == STATUS_OK) {
        network[peer_count] = peer_address;
        peer_count++;
    }

    uint32_t reply_body_length = 20*peer_count;
    char reply_body[reply_body_length];

    //Locking Network then creating and sending message
    for (uint32_t i = 0; i < peer_count; i++) {
        memcpy(&reply_body[i*20], network[i]->ip, IP_LEN);
        uint32_t n_port = htonl(atoi(network[i]->port));
        memcpy(&reply_body[i*20+IP_LEN], &n_port, 4);
    }

    //Creating header
    ReplyHeader_t reply_header_struct;

    //Header: Length of block (4 bytes)
    reply_header_struct.length = htonl(reply_body_length);

    //Header: Status code (4 bytes)
    reply_header_struct.status = htonl(status_code);

    //Header: block number (4 bytes)
    reply_header_struct.this_block = htonl(0);

    //Header: Block count (4 bytes)
    reply_header_struct.block_count = htonl(1);

    //Header: Block Hashed (32 bytes)
    get_data_sha(reply_body, reply_header_struct.block_hash, reply_body_length, SHA256_HASH_SIZE);
    
    //Header: Total hash (32 bytes)
    get_data_sha(reply_body, reply_header_struct.total_hash, reply_body_length, SHA256_HASH_SIZE);


    printf("Sending reply 1/1 with payload length of %u\n", reply_body_length);

    rio_t rio;
    Rio_readinitb(&rio, connfd);
    Rio_writen(connfd, &reply_header_struct, REPLY_HEADER_LEN);
    Rio_writen(connfd, reply_body, reply_body_length);
    printf("Registered new peer %s:%s\n", client_ip, port_string);
    printf("Register: Network list: ");
    for (uint32_t i = 0; i < peer_count; i++) {
        printf("%s:%s, ", network[i]->ip, network[i]->port);
    }
    printf("\n");


    // Inform rest of network
    for (uint32_t i = 0; i < peer_count; i++) {
        if ((strcmp(network[i]->ip, my_address->ip) == 0 
            && strcmp(network[i]->port, my_address->port) == 0)
            ||
            (strcmp(network[i]->ip, client_ip) == 0
            && strcmp(network[i]->port, port_string) == 0)) {
                continue;
            }
        else {
            char new_peer[IP_LEN + 4];
            memcpy(&new_peer[0], client_ip, IP_LEN);
            uint32_t n_port = htonl(client_port_int);
            memcpy(&new_peer[IP_LEN], &n_port, 4);
            send_message(*network[i], COMMAND_INFORM, new_peer);
        }
    }
    pthread_mutex_unlock(&network_mutex);
}

/*
 * Handle 'inform' type message as defined by the assignment text. These will 
 * never generate a response, even in the case of errors.
 */
void handle_inform(char* request) //Request containing IP and Port
{
    //retrieving ID and port
    PeerAddress_t *peer_address = (PeerAddress_t*)Malloc(sizeof(PeerAddress_t));
    //retrieving ID
    memcpy(peer_address->ip, request, IP_LEN);
    //retrieving port
    uint32_t port = ntohl(*(uint32_t*)&request[IP_LEN]);
    sprintf(peer_address->port, "%d", port);

    //Check if IP and port is valid
    if (!is_valid_ip(peer_address->ip) || !is_valid_port(peer_address->port)) {
        return;
    }

    //Check if IP/port is already in network list
    pthread_mutex_lock(&network_mutex);
    for (uint32_t i = 0; i < peer_count; i++) {
        if (strcmp(network[i]->ip, peer_address->ip) == 0 &&
            strcmp(network[i]->port, peer_address->port) == 0) {
                return;
        }
    }
    
    //Inserting new peer in Network
    network[peer_count] = peer_address;
    peer_count++;
    printf("Informed of new peer %s:%s\n", peer_address->ip, peer_address->port);
    printf("Inform: Network list: ");
    for (uint32_t i = 0; i < peer_count; i++) {
        printf("%s:%s, ", network[i]->ip, network[i]->port);
    }
    printf("\n");

    pthread_mutex_unlock(&network_mutex);

}

/*
 * Handle 'retrieve' type messages as defined by the assignment text. This will
 * always generate a response
 */
void handle_retreive(int connfd, char* request) //Request containing name of file
{
    uint32_t status_code = STATUS_OK;
    pthread_mutex_lock(&retrieving_mutex);
    if (strncmp(request, retrieving_files->path, strlen(request)) == 0) {
        status_code = STATUS_OTHER;
    }
    pthread_mutex_unlock(&retrieving_mutex);

    FILE* fp = Fopen(request, "r"); //r for reading
    fseek(fp, 0, SEEK_END);         //Find end of file
    uint32_t file_size = ftell(fp); //Length of file
    fseek(fp, 0, SEEK_SET);         //Set fp at the beginning

    char file[file_size];
    Fread(file, file_size, 1, fp);
    fseek(fp, 0, SEEK_SET);

    //Hasing the file (32 bytes)
    hashdata_t total_hash;
    get_data_sha(file, total_hash, file_size, SHA256_HASH_SIZE);

    //Keep track of blocks
    uint32_t block_count = (file_size + (MAX_REPLY_LEN - 1)) / MAX_REPLY_LEN;

    //Sending response to requester
    rio_t rio;
    Rio_readinitb(&rio, connfd);
    printf("Sending requested data from %s\n", request);

    //For each block
    for (uint32_t i = 0; i < block_count; i++) {
        char block[MAX_REPLY_LEN+1];
        //Read data into block
        size_t block_size = Fread(block, 1, MAX_REPLY_LEN, fp);
        block[MAX_REPLY_LEN] = 0;

        //Creating header for block (80 bytes)
        ReplyHeader_t reply_header_struct;

        //Header: Length of block (4 bytes)
        reply_header_struct.length = htonl(block_size);

        //Header: Status code (4 bytes)
        reply_header_struct.status = htonl(status_code);

        //Header: block number (4 bytes)
        reply_header_struct.this_block = htonl(i);

        //Header: Block count (4 bytes)
        reply_header_struct.block_count = htonl(block_count);

        //Header: Block Hashed (32 bytes)
        get_data_sha(block, reply_header_struct.block_hash, block_size, SHA256_HASH_SIZE);
        
        //Header: Total hash (32 bytes)
        get_data_sha(file, reply_header_struct.total_hash, file_size, SHA256_HASH_SIZE);

        printf("Sending reply %u/%u with payload length of %lu\n", i+1, block_count, block_size);
        
        //Send Header & body with block
        Rio_writen(connfd, &reply_header_struct, REPLY_HEADER_LEN);
        Rio_writen(connfd, block, block_size);

    }
    Fclose(fp);
}

/*
 * Handler for all server requests. This will call the relevent function based 
 * on the parsed command code
 */
void* handle_server_request(void* arg)
{
    int connfd = *((int*) arg); //Retreive connfd from args
    //Pthread_detach(Pthread_self());
    Free(arg);
    rio_t rio;
    char buffer[MAX_MSG_LEN];
    Rio_readinitb(&rio, connfd);

    // Read request
    Rio_readnb(&rio, buffer, REQUEST_HEADER_LEN);

    // Extracting header
    char request_header[REQUEST_HEADER_LEN];
    memcpy(request_header, buffer, REQUEST_HEADER_LEN);

    char ip[IP_LEN];
    memcpy(ip, request_header, IP_LEN);
    uint32_t port = ntohl(*(uint32_t*)&request_header[IP_LEN]);
    uint32_t command = ntohl(*(uint32_t*)&request_header[IP_LEN+4]);
    uint32_t header_length = ntohl(*(uint32_t*)&request_header[IP_LEN+8]);

    // Extract the request body
    char request_body[header_length+1];
    Rio_readnb(&rio, buffer, header_length);

    memcpy(request_body, buffer, header_length);
    request_body[header_length] = 0;
    char port_string[PORT_LEN];
    sprintf(port_string, "%d", port);

    switch(command) {
        case COMMAND_REGISTER:
            printf("Got registration message from %s:%s\n", ip, port_string);
            handle_register(connfd, ip, port);
            break;
        
        case COMMAND_RETREIVE:
            printf("Got request message from %s:%s\n", ip, port_string);
            handle_retreive(connfd, request_body);
            break;

        case COMMAND_INFORM:
            printf("Got inform from %s:%s\n", ip, port_string);
            handle_inform(request_body);
            break;
        
        case STATUS_OTHER:
            printf("Got other from %s:%s\n", ip, port_string);
            printf("No action is taken\n");
            break;

        case STATUS_MALFORMED:
            printf("Got malformed from %s:%s\n", ip, port_string);
            printf("No action is taken\n");
            break;

        default:
            printf("Unknown commad\n");
            printf("No action is taken\n");
            break;
    }
    return 0;
}

/*
 * Function to act as basis for running the server thread. This thread will be
 * run concurrently with the client thread, but is infinite in nature.
 */
void* server_thread()
{
    // Your code here. This function has been added as a guide, but feel free 
    // to add more, or work in other parts of the code
    int listenfd;
    int *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;
    listenfd = Open_listenfd(my_address->port);
    while (1) {
        clientlen = sizeof(struct sockaddr_storage);
        connfdp = Malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
        Pthread_create(&tid, NULL, handle_server_request, connfdp);
    }
    return 0;
}


int main(int argc, char **argv)
{
    // Initialise with known junk values, so we can test if these were actually
    // present in the config or not
    struct PeerAddress peer_address;
    memset(peer_address.ip, '\0', IP_LEN);
    memset(peer_address.port, '\0', PORT_LEN);
    memcpy(peer_address.ip, "x", 1);
    memcpy(peer_address.port, "x", 1);

    // Users should call this script with a single argument describing what 
    // config to use
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <config file>\n", argv[0]);
        exit(EXIT_FAILURE);
    } 

    my_address = (PeerAddress_t*)Malloc(sizeof(PeerAddress_t));
    memset(my_address->ip, '\0', IP_LEN);
    memset(my_address->port, '\0', PORT_LEN);

    // Read in configuration options. Should include a client_ip, client_port, 
    // server_ip, and server_port
    char buffer[128];
    fprintf(stderr, "Got config path at: %s\n", argv[1]);
    FILE* fp = Fopen(argv[1], "r");
    while (fgets(buffer, 128, fp)) {
        if (starts_with(buffer, MY_IP)) {
            memcpy(&my_address->ip, &buffer[strlen(MY_IP)], 
                strcspn(buffer, "\r\n")-strlen(MY_IP));
            if (!is_valid_ip(my_address->ip)) {
                fprintf(stderr, ">> Invalid client IP: %s\n", my_address->ip);
                exit(EXIT_FAILURE);
            }
        }else if (starts_with(buffer, MY_PORT)) {
            memcpy(&my_address->port, &buffer[strlen(MY_PORT)], 
                strcspn(buffer, "\r\n")-strlen(MY_PORT));
            if (!is_valid_port(my_address->port)) {
                fprintf(stderr, ">> Invalid client port: %s\n", 
                    my_address->port);
                exit(EXIT_FAILURE);
            }
        }else if (starts_with(buffer, PEER_IP)) {
            memcpy(peer_address.ip, &buffer[strlen(PEER_IP)], 
                strcspn(buffer, "\r\n")-strlen(PEER_IP));
            if (!is_valid_ip(peer_address.ip)) {
                fprintf(stderr, ">> Invalid peer IP: %s\n", peer_address.ip);
                exit(EXIT_FAILURE);
            }
        }else if (starts_with(buffer, PEER_PORT)) {
            memcpy(peer_address.port, &buffer[strlen(PEER_PORT)], 
                strcspn(buffer, "\r\n")-strlen(PEER_PORT));
            if (!is_valid_port(peer_address.port)) {
                fprintf(stderr, ">> Invalid peer port: %s\n", 
                    peer_address.port);
                exit(EXIT_FAILURE);
            }
        }
    }
    fclose(fp);

    retrieving_files = Malloc(sizeof(FilePath_t));
    srand(time(0));

    network = Malloc(sizeof(PeerAddress_t*));
    network[0] = my_address;
    peer_count = 1;

    // Setup the client and server threads 
    pthread_t client_thread_id;
    pthread_t server_thread_id;
    
    if (peer_address.ip[0] != 'x' && peer_address.port[0] != 'x')
    {   
        pthread_create(&client_thread_id, NULL, client_thread, &peer_address);
    } 
    pthread_create(&server_thread_id, NULL, server_thread, NULL);

    // Start the threads. Note that the client is only started if a peer is 
    // provided in the config. If none is we will assume this peer is the first
    // on the network and so cannot act as a client.
    if (peer_address.ip[0] != 'x' && peer_address.port[0] != 'x')
    {
        Pthread_join(client_thread_id, NULL);
    }
    printf("Starting server at: %s:%s\n", my_address->ip, my_address->port);
    Pthread_join(server_thread_id, NULL);

    Free(retrieving_files);

    exit(EXIT_SUCCESS);
}
