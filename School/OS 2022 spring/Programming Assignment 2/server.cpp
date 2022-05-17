// A simple server in the internet domain using TCP
// The port nu1mber is passed as an argument

// Please note this is a C program
// It compiles without warnings with gcc
//************ CODE CREDIT : DR.Rincon **************//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <map>

using namespace std;
void error(char *msg)
{
    perror(msg);
    exit(1);
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

// ****** returns the bitsize for dec-freq conversion ****** //
int find_bitsize(vector<int> v) // int vector and integer(for looping)
{
    int max = *max_element(v.begin(), v.end());
    int bitsize = ceil(log2(max));
    return bitsize;
}

// ****** this function adds zero padding to the binary numbers ****** //
string add_zeros(string result, int numzeros, int x)
{

    int size = x - result.size();
    if (x != result.size())
    {
        for (int i = 0; i < size; i++)
        {
            string temp1 = result;
            result = '0';
            result += temp1;
        }
    }
    return result;
}
// ****** this function does the decimal to binary conversion ****** //
string dec_to_bin(int num, int bitsize)
{
    int numzeros;
    string dec = "";
    while (num != 0)
    {
        dec += (num % 2 == 0 ? "0" : "1");
        num /= 2;
    }
    reverse(dec.begin(), dec.end());
    if (dec.size() != bitsize)
    {
        numzeros = bitsize - dec.size();
        dec = add_zeros(dec, numzeros, bitsize);
    }
    return dec;
}
//***** fireman function *****//
void fireman(int)
{
    waitpid(-1, NULL, WNOHANG);
}

int main(int argc, char *argv[])
{
    int sockfd, newsockfd, portno, clilen;
    char buffer[256];
    struct message msg1;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        perror("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        perror("ERROR on binding");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
    if (newsockfd < 0)
        perror("ERROR on accept");

    string temp;              // temp variable for reading input
    string num;               // number of symbols in string form
    int N;                    // number of symbols in int form
    char alphabet;          // alphabets in input
    string decimal;           // decimals in input
    int bitsize;              // bitsize for the binary numbers
    string binary_to_compare; //

    vector<char> alpha;                // contians alphabets
    vector<string> dec;                  // contains decimals in string form
    vector<int> decimals;                // contains decimals in int form
    vector<string> binarys;              // converted binary values
    map<string,char> mp;               // map of alphabets and binarys
    
    // ****** reading the input ****** //
    getline(cin, num);
    N = stoi(num);
    for (int i = 0; i < N; i++)
    {
        getline(cin, temp);
        alphabet = temp[0];
        alpha.push_back(alphabet);

        for (int j = 2; j < temp.size(); j++)
        {
            decimal += temp[j];
        }
        dec.push_back(decimal);

        decimal = " ";
    }
    // convert decimals to integer form and push into seperate vector<int> //
    for (int i = 0; i < N; i++)
    {
        decimals.push_back(stoi(dec[i]));
    }
    bitsize = find_bitsize(decimals);        // assign bitize to the variable


    if(fork()==0)
    {
        write(newsockfd, &bitsize, sizeof(int)); // write the bitsize so we can read it on client side
        close(newsockfd);
        _exit(0);
    }

    for (int i = 0; i < decimals.size(); i++)
    {
        binarys.push_back(dec_to_bin(decimals[i], bitsize));
    }
    // inserting values in the map
    for (int i = 0; i < N; i++)
    {
        mp.insert({binarys[i],alpha[i]});
    }

    while (true)
    {
        
        int a = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
        if (a < 0)
            perror("ERROR IN ACCEPT!");
        if (fork() == 0)
        {
            char buffer[256];
            n = read(a, &buffer, sizeof(buffer)); // read from the client side
            if (n < 0) perror("ERROR writing from socket");
            for (int i = 0; i < bitsize; i++)    
            {
                binary_to_compare += buffer[i]; // we push the binary into the buffer for comparison
            } 
            char decode_character;
            for(auto i = mp.begin();i!=mp.end();i++)
            {
                
                if ( i->first== binary_to_compare) // comparing the binary number in map to binary number in the message
                {
                    decode_character = i->second;
                    //cout << binary_to_compare << " " <<i->second << endl;
                    if (n < 0) 
                        perror("ERROR writing from socket");
                    break;
                }
                
            }
            n=write(a, &decode_character, sizeof(char));
            close(a);
            _exit(0);
        }
    }
    return 0;
}
