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

// ****** struct for threads part1 ****** //
struct temp1
{
    string binarys;      // variable that represents binary conversions
    int alpha_frequency; // variable that represents the frequency
};

struct temp2
{
    int Dec;                  // Decimal form of the numbers
    int size;                 // bitsize of the binary number
    string message_to_decode; // message to be decoded
    string *str;              // address of binary from temp1 struct
    int *freq;                // address of the frequency from temp1
};

// ****** struct for the threads par2 ****** //
struct temp3
{
    string binary_num;    // holds encode character
    vector<string> alpha; // holds alphabets
    vector<string> beta;  // holds binary
    string *output;       // address of the decoded message
};

// ****** returns the bitsize for dec-freq conversion ****** //
int find_bitsize(vector<int> v) // int vector and integer(for looping)
{
    int max = *max_element(v.begin(), v.end());
    int bitsize = ceil(log2(max));
    return bitsize;
}

// ****** finds a frequency of a word within a string ******//
int find_frequency(string str, int b_size, string str2)
{
    int counter = 0;
    for (int j = 0; j < str2.size(); j += b_size)
    {
        if (str2.substr(j, b_size) == str)
        {
            counter += 1;
        }
    }
    return counter;
}
// ****** string  splitter function ******
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
// ****** helper function for threads ****** //
// task 1 -> performs the decimal to binary conversions and push it into the struct array //
// task 2 -> Determines the frequency of the symbol in the compressed message //
void *dummy1(void *input)
{
    struct temp2 *p = (struct temp2 *)input;
    int num_zeros;
    string dec = "";
    while (p->Dec != 0)
    {
        dec += (p->Dec % 2 == 0 ? "0" : "1");
        p->Dec /= 2;
    }
    reverse(dec.begin(), dec.end()); // reverse the string

    if (p->size != dec.size())
    {

        num_zeros = p->size - dec.size();

        dec = add_zeros(dec, num_zeros, p->size);
    }

    *p->str = dec;                                                 // dec to freq conversion
    *p->freq = find_frequency(dec, p->size, p->message_to_decode); // pushing the calculated frequencies
}

// ****** helper function for threads ****** //
// task->to determine each character of the decompressed message
void *dummy2(void *input)
{
    struct temp3 *s = (struct temp3 *)input;

    int index;

    for (int i = 0; i < s->beta.size(); i++)
    {

        if (s->binary_num == s->beta[i])
        {
            index = i;
            break;
        }
    }

    *s->output = s->alpha[index];
}
int main()
{
    string temp;
    string num;
    int n;
    int number;
    int bitsize; // represents the bitsize for binary numbers
    string alphabet;
    string decimal;
    string message;

    vector<int> decimals; // contains the decimals in int value
    vector<string> msg;   // split up the message acc to bitsize and store it
    vector<string> alpha; // contains the alphabets
    vector<string> dec;   // contains the decimals in string format
    temp1 *arr;  // here we will store the binary conversions
    temp2 *arr2; // here we will store the decimal numbers and its index

    getline(cin, num);
    n = stoi(num);

    arr = new temp1[n];  // assigning the size
    arr2 = new temp2[n]; // assigning the size

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

    // assigning the decimal values and index (kind of making like a pair)
    for (int i = 0; i < decimals.size(); i++)
    {
        arr2[i].Dec = decimals[i];
    }
    getline(cin, message); // grab the message that is to be decoded
    for (int i = 0; i < n; i++)
    {
        arr2[i].size = bitsize;              // we asssign bitsize to the array
        arr2[i].message_to_decode = message; //
    }

    int size = message.length() / bitsize;

    Split_String(message, size, msg);    // split the msg acc to bitsize and store it in a string vector
    temp3 *arr3 = new temp3[msg.size()]; // here we will store the split up binary numbers and its index
    for (int i = 0; i < msg.size(); i++)
    {
        arr3[i].binary_num = msg[i];
    }

    //********************************************************************* thread part *********************************************************************//

    // thread part1
    //  Creates n number of threads and performs binary conversions and determines frequency
    pthread_t tid1[n];
    for (int i = 0; i < n; i++)
    {
        arr2[i].str = &arr[i].binarys;
        arr2[i].freq = &arr[i].alpha_frequency;
        if (pthread_create(&tid1[i], NULL, dummy1, &arr2[i]))
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

    int n2 = msg.size(); // storing the size of message to intialize the array that stores the decoded msg
    for (int j = 0; j < n2; j++)
    {
        for (int i = 0; i < n; i++)
        {
            arr3[j].alpha.push_back(alpha[i]);
            arr3[j].beta.push_back(arr[i].binarys);
        }
    }

    string *finalOutput = new string[n2];

    // thread part2
    // Creates n number of threads and decodes the message
    pthread_t tid2[n2];
    for (int i = 0; i < n2; i++)
    {
        arr3[i].output = &finalOutput[i];
        if (pthread_create(&tid2[i], NULL, dummy2, &arr3[i]))
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
    // ********************* printing the result ********************* //
    cout << "Alphabet: " << endl;
    for (int j = 0; j < n; j++)
    {
        cout << "Character: " << alpha[j] << ", Code: " << arr[j].binarys << ", Frequency: " << arr[j].alpha_frequency << endl;
    }
    cout << endl;
    cout << "Decompressed message: ";
    for (int i = 0; i < n2; i++)
    {
        cout << finalOutput[i];
    }

    return 0;
}
