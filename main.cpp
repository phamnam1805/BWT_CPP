#include <bits/stdc++.h>
#include "SACA-K.h"
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

using namespace std;
int fd = dup(fileno(stdout));
void redirectStdout()
{
    FILE *fp = freopen("redirect.txt", "w", stdout);
    fflush(fp);
    dup2(fd, fileno(fp));
}

void printArray(unsigned int a[], int n)
{
    for (int i = 0; i < n; i++)
    {
        cout << a[i] << endl;
    }
}

void printString(unsigned char s[], int n){
    for(int i = 0; i < n; i++){
        cout<<s[i];
    }
}
int bwtTransform(unsigned char *s, unsigned char *bwt, unsigned int *SA, int n)
{
    int index, first, prev;
    for (int i = 0; i < n; i++)
    {
        index = SA[i];
        if (index == 0)
        {
            first = i;
        }
        prev = index - 1;
        if (prev < 0)
        {
            prev += n;
        }
        bwt[i] = s[prev];
    }
    return first;
}
void bwtInverse(unsigned char *s, unsigned char *bwt, int first, int n)
{
    int R = 256;
    int *cnt = new int[R + 1];
    for (int i = 0; i <= R; i++)
    {
        cnt[i] = 0;
    }
    for (int i = 0; i < n; i++)
    {
        cnt[bwt[i] + 1]++;
    }

    for (int r = 0; r < R; r++)
    {
        cnt[r + 1] += cnt[r];
    }

    unsigned char *aux = new unsigned char[n];
    int *next = new int[n];
    for (int i = 0; i < n; i++)
    {
        unsigned char c = bwt[i];
        aux[cnt[c]] = c;
        next[cnt[c]] = i;
        cnt[c]++;
    }
    // for(int i = 0; i <= R; i++){
    //     cout<<cnt[i]<<endl;
    // }
    // cnt[0] = 1;
    int ptr = first;
    for (int i = 0; i < n; i++)
    {
        s[i] = aux[ptr];
        ptr = next[ptr];
    }
}

int search(unsigned char *list, unsigned char value, int length)
{
    for (int i = 0; i < length; i++)
    {
        if (list[i] == value)
        {
            return i;
            break;
        }
    }
    return -1;
}
void moveToFront(unsigned char *list, int c_it)
{
    unsigned char temp = list[c_it];
    for (int z = c_it - 1; z >= 0; z--)
    {
        list[z + 1] = list[z];
    }
    list[0] = temp;
}
void mtfEncode(unsigned char *s, int n)
{
    unsigned char *list = new unsigned char[256];
    for (int i = 0; i < 256; i++)
    {
        list[i] = i;
    }
    for (int i = 0; i < n; i++)
    {
        unsigned char c = s[i];
        int c_it = search(list, c, 256);
        s[i] = c_it;
        moveToFront(list, c_it);
    }
}

void mtfDecode(unsigned char *s, int n)
{
    unsigned char *list = new unsigned char[256];
    for (int i = 0; i < 256; i++)
    {
        list[i] = i;
    }
    for (int i = 0; i < n; i++)
    {
        unsigned char c = s[i];
        s[i] = list[c];
        moveToFront(list, c);
    }
}

void encode(char *inputFilePath, char *outputFilePath, char *firstIndexFilePath)
{
    FILE *input = freopen(inputFilePath, "rb", stdin);
    fseek(input, 0, SEEK_END);
    unsigned long n = ftell(input);
    n++;
    unsigned char *s = new unsigned char[n];
    unsigned int *SA = new unsigned int[n];
    unsigned char *bwt = new unsigned char[n];
    int firstIndex;
    fseek(input, 0, SEEK_SET);
    fread((unsigned char *)s, 1, n - 1, input);
    fclose(input);
    s[n - 1] = 0;
    // compute SA
    SACA_K(s, SA, n, 256, n, 0);
    //bwt transform
    firstIndex = bwtTransform(s, bwt, SA, n);
    //mtf encode
    mtfEncode(bwt, n);
    //write file
    FILE *output = freopen(outputFilePath, "wb", stdout);
    fwrite((unsigned char *)bwt, 1, n, output);
    fclose(output);
    redirectStdout();
    ofstream fileOutput(firstIndexFilePath);
    fileOutput << firstIndex;
    fileOutput.close();

    delete[] s;
    delete[] SA;
    delete[] bwt;

}

void decode(char *inputFilePath, char *outputFilePath, char *firstIndexFilePath)
{
    FILE *input = freopen(inputFilePath, "rb", stdin);
    fseek(input, 0, SEEK_END);
    unsigned long n = ftell(input);
    unsigned char *s = new unsigned char[n];
    fseek(input, 0, SEEK_SET);
    fread((unsigned char *)s, 1, n, input);
    fclose(input);

    int firstIndex;
    ifstream fileInput(firstIndexFilePath);
    fileInput >> firstIndex;
    fileInput.close();
    //MTF decode
    mtfDecode(s, n);
    // BWT inverse
    bwtInverse(s, s, firstIndex, n);
    n = n - 1;
    FILE *output = freopen(outputFilePath, "wb", stdout);
    fwrite((unsigned char *)s, 1, n, output);
    fclose(output);
    redirectStdout();
    delete[] s;
}

int main()
{
#ifdef _WIN32
    setmode(fileno(stdout), O_BINARY);
    setmode(fileno(stdin), O_BINARY);
#endif
    char *input = (char *)"test/test1.txt";
    char *output = (char *)"output.txt";
    char *first = (char *)"first_index.txt";
    char *result = (char *)"result.txt";
    long start = clock();
    encode(input, output, first);
    long mid = clock();
    cout << "encode time: " << (mid - start) << "ms" << endl;

    decode(output, result, first);
    long end = clock();
    cout << "decode time: " << (end - mid) << "ms" << endl;
    return 0;
}