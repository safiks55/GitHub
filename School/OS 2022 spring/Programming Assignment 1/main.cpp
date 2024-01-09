#include <iostream>
#include <cmath>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <pthread.h>
#include <mutex>

using namespace std;

struct ThreadData {
    string binarys;
    int alpha_frequency;
};

struct DecodingData {
    string binary_num;
    vector<string> alpha;
    vector<string> beta;
    string* output;
};

struct ConversionData {
    int Dec;
    int size;
    string message_to_decode;
    string* str;
    int* freq;
};

mutex mtx;  // mutex for thread safety

int findBitsize(const vector<int>& v) {
    int max = *max_element(v.begin(), v.end());
    return ceil(log2(max));
}

int findFrequency(const string& str, int b_size, const string& str2) {
    int counter = 0;
    for (int j = 0; j < str2.size(); j += b_size) {
        if (str2.substr(j, b_size) == str) {
            counter += 1;
        }
    }
    return counter;
}

void SplitString(const string& str, int n, vector<string>& test) {
    int length = str.length();
    int parts = length / n;

    for (int x = 0; x < length; x += parts) {
        string temp = str.substr(x, parts);
        test.push_back(temp);
    }
}

string addZeros(const string& result, int numZeros, int x) {
    string paddedResult = result;
    int size = x - result.size();
    if (size > 0) {
        paddedResult = string(size, '0') + paddedResult;
    }
    return paddedResult;
}

void* DecimalToBinary(void* input) {
    ConversionData* p = static_cast<ConversionData*>(input);
    int numZeros;
    string dec = "";
    while (p->Dec != 0) {
        dec += (p->Dec % 2 == 0 ? "0" : "1");
        p->Dec /= 2;
    }
    reverse(dec.begin(), dec.end());

    if (p->size != dec.size()) {
        numZeros = p->size - dec.size();
        dec = addZeros(dec, numZeros, p->size);
    }

    {
        lock_guard<mutex> lock(mtx);
        *p->str = dec;
        *p->freq = findFrequency(dec, p->size, p->message_to_decode);
    }

    return nullptr;
}

void* DecodeMessage(void* input) {
    DecodingData* s = static_cast<DecodingData*>(input);
    int index;

    for (int i = 0; i < s->beta.size(); i++) {
        if (s->binary_num == s->beta[i]) {
            index = i;
            break;
        }
    }

    {
        lock_guard<mutex> lock(mtx);
        *s->output = s->alpha[index];
    }

    return nullptr;
}

int main() {
    // ... (input handling)

    vector<int> decimals; 
    vector<string> msg;   
    vector<string> alpha; 
    vector<string> dec;   

    // ... (input reading and processing)

    vector<ConversionData> arr2(n);
    vector<ThreadData> arr(n);
    vector<DecodingData> arr3(msg.size());
    string* finalOutput = new string[msg.size()];

    // ... (input population)

    int bitsize = findBitsize(decimals);

    for (int i = 0; i < n; i++) {
        arr2[i].Dec = decimals[i];
        arr2[i].size = bitsize;
        arr2[i].message_to_decode = message;
        arr2[i].str = &arr[i].binarys;
        arr2[i].freq = &arr[i].alpha_frequency;
    }

    int size = message.length() / bitsize;
    SplitString(message, size, msg);

    for (int i = 0; i < msg.size(); i++) {
        arr3[i].binary_num = msg[i];
    }

    // ... (thread creation and execution)

    // ********************* printing the result ********************* //
    cout << "Alphabet: " << endl;
    for (int j = 0; j < n; j++) {
        cout << "Character: " << alpha[j] << ", Code: " << arr[j].binarys << ", Frequency: " << arr[j].alpha_frequency << endl;
    }
    cout << endl;
    cout << "Decompressed message: ";
    for (int i = 0; i < msg.size(); i++) {
        cout << finalOutput[i];
    }

    delete[] finalOutput;

    return 0;
}
