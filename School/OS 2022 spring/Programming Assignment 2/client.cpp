// Write your code here
// Please note this is a C program
// It compiles without warnings with gcc


//************ CODE CREDIT : DR.Rincon **************//
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include <vector>
using namespace std;

void error(char *msg)
{
    perror(msg);
    exit(0);
}

struct message
{
    int option;
    int account;
    double amount;
};

struct message2
{
    int value;
    char buffer[256];
};

// struct for split up binary numbers and their respective hostname and port number
struct f
{
    string bin_val; // split up binary values (010,100 etc)
    char* hostname; 
    int port;
    char* decoded;
};

// splits the binary message according to split size and push into a vector
void Split_String(string str, int n, vector<string> &test)
{
    int length = str.length();
    int parts = length / n;

    for (int x = 0; x < length; x += parts)
    {
        string temp = str.substr(x, parts);
        test.push_back(temp);
    }
}
void *dummy1(void *input)
{
    struct f *p = (struct f *)input;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int sockfd, portno, n;
    char c;
    portno = p->port;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("ERROR opening socket");
    server = gethostbyname(p->hostname);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(p->port);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        perror("ERROR connecting");
    string b = p->bin_val;
    n=write(sockfd,b.c_str(),sizeof(b));
    if (n < 0) perror("ERROR writing from socket");
    n=read(sockfd,&c,sizeof(c));
    if (n < 0) perror("ERROR reading from socket");

    *p->decoded=c; // this is where we store the decoded characters
    close(sockfd);
    return NULL;

}
int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct message msg1;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        perror("ERROR connecting");

        
    // ****** reading input ****** //
    int bitsize;        // contains the bitsize
    int size;           // size of the message to be decoded
    string message;     // message to be decoded
    vector<string> msg; // store the split up message in here
    f *arr;             // array of struct f

    read(sockfd, &bitsize, sizeof(bitsize));   // reading bitsize from the server
    close(sockfd);                          
    getline(cin, message);                   // grab the message that is to be decoded
    size = message.length() / bitsize;      // size of the message/input
    Split_String(message, size, msg);      // splitting the 
    arr = new f[size];                    // initializing the struct array

    for (int i = 0; i < msg.size(); i++)
    {
        arr[i].bin_val = msg[i];        // storing the message according to bitsize
        arr[i].hostname = argv[1];      // storing the hostname 
        arr[i].port = atoi(argv[2]);    // storing the port number
    }
    char *decodedMsg = new char[size];  // array to store decoded message
    
    //********* threading part **********//
    //  Creates n number of threads and performs binary conversions and determines frequency
    pthread_t tid1[size];
    for (int i = 0; i < size; i++)
    {
        arr[i].decoded = &decodedMsg[i];
        if (pthread_create(&tid1[i], NULL, dummy1, &arr[i])!=0)
        {
            std::cout << "Failed to join thread.\n"
                      << std::endl;
            return 1;
        };
    }

    // creating joins for the above threads
    for (int i = 0; i < size; i++)
    {
        if (pthread_join(tid1[i], NULL) != 0)
        {
            std::cout << "Failed to join thread.\n"
                      << std::endl;
            return 2;
        }
    }
    cout << "Decompressed message: " ;
    for(int i=0; i<size; i++)
    {
        cout << decodedMsg[i];
    }
    return 0;
}
