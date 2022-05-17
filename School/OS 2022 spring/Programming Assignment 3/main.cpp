#include <iostream>
#include <cmath>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <pthread.h>
#include <unistd.h>
using namespace std;

struct temp1
{
    int *bitsize;                // bitsize
    int decimals;                // decimals
    char alpha;                  // alphabet values
    int threadId;                // index of the thread
    int *turn;                   // thread turn
    string *encodedmsg;          // storing the binary message
    pthread_mutex_t *dec_to_bin; // semaphore variable
    pthread_cond_t *cond;        // condition variable
    string *str;                 // address of the binary nums
};

struct temp2
{
    vector<char> *alpha;     // holds the alphabets
    vector<string> *bins;    // holds the binary numbers
    int threadId;            // index of the thread
    int *turn;               //  thread turn
    string binaryNum;        // encoded binary num for a character
    string *output;          // address of the decoded message
    pthread_mutex_t *decode; // semaphore variable
    pthread_cond_t *cond2;   // condition variable
};

// ****** returns the bitsize for dec-freq conversion ****** //
int find_bitsize(vector<int> v) // int vector and integer(for looping)
{
    int max = *max_element(v.begin(), v.end());
    int bitsize = ceil(log2(max));
    return bitsize;
}
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
// function for adding the zero padding to the converter binary number
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
// function to convert decimals to a binary using a fixed bitsize
string Dec_to_Bin(int bitsize, int decimal)
{
    int num_zeros;
    string dec = "";
    while (decimal != 0)
    {

        dec += (decimal % 2 == 0 ? "0" : "1");
        decimal /= 2;
    }
    reverse(dec.begin(), dec.end()); // reverse the string

    if (bitsize != dec.size())
    {

        num_zeros = bitsize - dec.size();

        dec = add_zeros(dec, num_zeros, bitsize);
    }
    return dec;
}
// function to find frequency of a binary number in the message
int find_frequency(string str, int *b_size, string *str2)
{
    int counter = 0;
    for (int i = 0; i < str2->size(); i += *b_size)
    {
        if (str2->substr(i, *b_size) == str)
        {
            counter += 1;
        }
    }
    return counter;
}

// function for thread part 1
void *dummy(void *input)
{
    struct temp1 *t = (struct temp1 *)input;
    int threadId = t->threadId;
    char alpha = t->alpha;
    int decimals = t->decimals;
    string *ptr = t->str;
    pthread_mutex_unlock(t->dec_to_bin);

    string abc = Dec_to_Bin(*t->bitsize, decimals); // decimal to binary conversion
    *ptr = abc;
    int frequency = find_frequency(abc, t->bitsize, t->encodedmsg);
    pthread_mutex_lock(t->dec_to_bin);
    while (*t->turn != threadId)
    {
        pthread_cond_wait(t->cond, t->dec_to_bin);
    }
    pthread_mutex_unlock(t->dec_to_bin);
    // printing the output
    if (threadId == 0)
    {
        cout << "Alphabet:" << endl;
    }
    cout << "Character: " << alpha << ", Code: " << abc << ", Frequency: " << frequency << endl;
    pthread_mutex_lock(t->dec_to_bin);
    *t->turn = *t->turn + 1;
    pthread_cond_broadcast(t->cond);
    pthread_mutex_unlock(t->dec_to_bin);

    return NULL;
}
// function for thread part 2
void *dummy2(void *input)
{
    struct temp2 *t2 = (struct temp2 *)input;
    int threadID = t2->threadId;
    string *res = t2->output;
    string biNum = t2->binaryNum;
    pthread_mutex_unlock(t2->decode);
    //int index; // index of the matched value
    char c;
    for (int i = 0; i < t2->bins->size(); i++)
    {
        if (biNum == (*t2->bins)[i])
        {
            c = (*t2->alpha)[i];
            break;
        }
    }
    pthread_mutex_lock(t2->decode);
    while (*t2->turn != threadID)
    {
        pthread_cond_wait(t2->cond2, t2->decode);
    }
    pthread_mutex_unlock(t2->decode);
    *res = c;
    pthread_mutex_lock(t2->decode);
    *t2->turn = *t2->turn + 1;
    pthread_cond_broadcast(t2->cond2);
    pthread_mutex_unlock(t2->decode);
    return NULL;
}

int main()
{
    string temp;
    string num;
    int n; // number of characters in the input
    int bitsize; // represents the bitsize for binary numbers
    // input variables
    char alphabet;
    string decimal;
    string message;

    vector<int> decimals; // contains the decimals in int value
    vector<string> msg;   // split up the message acc to bitsize and store it
    vector<char> alpha;   // contains the alphabets
    vector<string> dec;   // contains the decimals in string format

    pthread_mutex_t dec_to_bin;
    pthread_mutex_t decode;
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
    temp1 s; // struct variable for struct1
    temp2 t; // struct variable for struct2
    pthread_mutex_init(&dec_to_bin, NULL);
    pthread_mutex_init(&decode, NULL);
    getline(cin, num);
    n = stoi(num);
    for (int i = 0; i < n; i++)
    {
        getline(cin, temp);
        alphabet = temp[0];
        alpha.push_back(alphabet);
        for (int j = 2; j < temp.size(); j++)
        {
            decimal += temp[j];
        }
        dec.push_back(decimal);

        decimal = ""; // reset the decimal for next loop
    }

    // convert decimals to integer form and push into seperate vector<int>
    for (int i = 0; i < n; i++)
    {
        decimals.push_back(stoi(dec[i]));
    }

    bitsize = find_bitsize(decimals); // this will assign the bitsize to the variable
    getline(cin, message);            // grab the message that is to be decoded

    int size = message.length() / bitsize;
    Split_String(message, size, msg); // split the msg acc to bitsize and store it in a string vector
    int turn = 0;
    //************************************************************ thread part 1 *********************************************************************//
    //Creating n child threads to determine the binary code and their frequency in the message
    string converted_bins[n]; // store the converted bin values from threads
    s.dec_to_bin = &dec_to_bin;
    s.cond = &cond;
    s.turn = &turn;
    s.encodedmsg = &message;
    s.bitsize = &bitsize;
    pthread_t tid1[n];

    for (int i = 0; i < n; i++)
    {
        pthread_mutex_lock(&dec_to_bin);
        s.decimals = decimals[i];
        s.alpha = alpha[i];
        s.threadId = i;
        s.str = &converted_bins[i];
        if (pthread_create(&tid1[i], NULL, dummy, &s))
        {
            std::cout << "Failed to join thread.\n"
                      << std::endl;
            return 1;
        };
    }

    // creating joins for the above threads

    for (int i = 0; i < n; i++)
    {
        if (pthread_join(tid1[i], NULL) != 0)
        {
            std::cout << "Failed to join thread.\n"
                      << std::endl;
            return 2;
        }
    }
    pthread_mutex_destroy(&dec_to_bin);

    int n2 = msg.size(); // storing the size of message to intialize the array that stores the decoded msg

    vector<char> v;
    vector<string> v2;
    // storing the alphabets and

    for (int i = 0; i < n; i++)
    {
        v.push_back(alpha[i]);
        v2.push_back(converted_bins[i]);
    }

     //for(int i = 0; i<v.size(); i ++) { cout << "From main " << v[i] << endl;}
    // for(int i = 0; i<v2.size(); i ++) { cout << "From main " << v2[i] << endl;}
    turn = 0;
    //************************************************************ thread part 2 *********************************************************************//
    //Create m child processes or threads to determine each character of the decompressed message
    string *finalOutput = new string[n2]; // storing the decoded message from threads
    t.cond2 = &cond2;
    t.turn = &turn;
    t.decode = &decode;
    t.alpha = &v;
    t.bins = &v2;
    pthread_t tid2[n2];
    for (int i = 0; i < n2; i++)
    {
        pthread_mutex_lock(&decode);
        t.output = &finalOutput[i];
        t.threadId = i;
        t.binaryNum = msg[i]; // here we will store the split up binary numbers
        if (pthread_create(&tid2[i], NULL, dummy2, &t))
        {
            std::cout << "Failed to join thread.\n"
                      << std::endl;         
            return 3;
        };
    }

    // creating joins for the above threads

    for (int i = 0; i < n2; i++)
    {
        if (pthread_join(tid2[i], NULL) != 0)
        {
            std::cout << "Failed to join thread.\n"
                      << std::endl;
            return 4;
        }
    }
   // printing final decompressed message
    cout << endl;
    cout << "Decompressed message: ";
    for (int i = 0; i < n2; i++)
    {
        cout << finalOutput[i];
    }
    pthread_mutex_destroy(&decode);

    return 0;
}
