#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>

using namespace std;

int gcd(int a, int b)
{
    int temp;

    if (b == 0)
        return a;

    if (a == b)
        return a;

    if(a < b)
    {
        temp = a;
        a = b;
        b = temp;
    }

    return gcd(b, a % b);
}

bool isPrime(int n)
{
    int firstPrime = 0;
    int secPrime;

    for (int i = 1; i <= sqrt(n); i++)
        {
            if(n % i == 0)
            {
                firstPrime = i;
                secPrime = n / i;
            }
        }

    if (firstPrime != 1)
        return false;

    return true;
}

bool isValid(int e, int p, int q)
{
    int n = p * q;

    // Check that p and q are distinct primes.
    if(p == q)
    {
        cout << "Error, p and q must be distinct primes!\n";
        return false;
    }

    if(!isPrime(p) || !isPrime(q))
    {
        cout << "Error! Either p or q is not prime!\n";
        return false;
    }

    // n is valid. Check if e is valid.
    else
    {
        // Check that e is within [1,n).
        if(e < 1 || e > n )
        {
            cout << "Error! e must be between 1 and n!\n";
            return false;
        }

//        cout << "In isValid. e: " << e << ", p: " << p << ", q: " << q << endl;
//        cout << "gcd(e, (p-1)(q-1)) = " <<
        // Check that gcd(e, (p-1)(q-1)) = 1;
        if (gcd(e, (p-1)*(q-1)) != 1)
        {
            cout << "Error! gcd(e, (p-1)(q-1) != 1.\n";
            return false;
        }
    }
    return true;
}



pair<int, int> findPrimePair(int n)
{
    int firstPrime;
    int secPrime;

    // Divide n by primes up to 7 to find two prime pairs for p and q whos
    // product is n.
    for (int i = 1; i <= sqrt(n); i++)
    {
        if(n % i == 0)
        {
            firstPrime = i;
            secPrime = n / i;
        }
    }

    return make_pair(firstPrime, secPrime);
}

int computeD(int eulers, int e)
{
    bool found = false;
    int inverse = 1;
    int count = 1;
    int tempE = e;
    int tempEulers = eulers;

    while (!found)
    {
        if(tempE == tempEulers)
        {
            found = true;
            return inverse;
        }

        if(tempE < tempEulers)
        {
            tempE += e;
            inverse++;
        }

        if (tempE > tempEulers)
        {
            count++;
            tempEulers = count*eulers + 1;
        }

    }

    return -1;
}

vector<double> computeSum(int d)
{
    int tempD = d;
    vector<double> exponents;
    double result = log2(d);

    // Result is a whole number, meaning that d is a power of 2.
    // Compute mod through exponentiation.
    if (result == floor(result))
    {
        exponents.push_back(pow(2,result));
    }

    else
    {
        while(log2(tempD) != floor(log2(tempD)))
        {
            int powOfTwo = pow(2, floor(log2(tempD)));
            exponents.push_back(powOfTwo);
            tempD = tempD - powOfTwo;
        }
        exponents.push_back(1);
    }
    return exponents;
}

int decipherChar(int c, int d, int n)
{
    vector<double> exponents = computeSum(d);    // Contains powers of 2
                                                // that sum up to d.
    double greatestExponent = exponents.front();
    int dividend = c;
    int result = 1;

    // If one of the exponents is a 1, then add c to the result. Otherwise,
    // the exponent is even and 1 doesn't need to be included.
    if(exponents.back() == 1)
    {
        result *= c;
        exponents.pop_back();
    }

    // Computes modular exponentiation of powers of 2.
    for (int i = 2; i <= greatestExponent; i *= 2)
    {
        dividend = (dividend*dividend) % n;

        // If the current i matches and exponent, multiply it to the result.
        if (i == exponents.back())
        {
            result *= dividend;
            exponents.pop_back();
        }
    }

    return result % n;
}

void decryptMessage(int d, int n, string fileName)
{
    ifstream fin;
    ofstream fout;
    int num;
    int decryptedChar;

    fin.open(fileName);
    fout.open("decrypted. txt");

    if(fin.is_open())
    {
        while(!fin.eof())
        {
            fin >> num;
            decryptedChar = decipherChar(num, d, n);

            if (decryptedChar == 28)
                fout << " ";

            else
                fout << (char)(decryptedChar + 95);
        }
    }

    else
        cout << "File could not be opened.\n";
}


void encryptMessage(int e, int n, string fileName)
{
    ifstream fin;
    ofstream fout;
    int num;
    char letter;
    int encryptedChar;

    fin.open(fileName);
    fout.open("incrypted.txt");

    if(fin.is_open())
    {
        while(!fin.eof())
        {
            letter = fin.get();

            if (letter == ' ')
            {
                encryptedChar = decipherChar(28, e, n);
                fout << encryptedChar;
            }

            else
            {
                encryptedChar = decipherChar(letter - 95, e, n);
                fout << encryptedChar;
            }

            if (fin.peek() != EOF)
                fout << " ";
        }
    }

    else
        cout << "File could not be opened.\n";
}

int main()
{
    int e;
    int n;
    char choice;
    pair<int, int> primePair;
    int p;
    int q;
    string fileName;

    do
    {
        cout << "Enter public key (e, n):" << endl;
        cin >> e;
        cin >> n;

        primePair = findPrimePair(n);
        p = primePair.first;
        q = primePair.second;
     } while(!isValid(e, p, q));

    cout << "Enter 'e' to encrypt or 'd' to decrypt: " << endl;
    cin >> choice;

    if (choice == 'e')
    {
        cout << "Enter name of file to be encrypted: ";
        cin >> fileName;

        encryptMessage(e, n, fileName);
        cout << "\"incrypted.txt\"\n";
    }

    if (choice == 'd')
    {
        int eulers = (p-1)*(q-1);
        int d = computeD(eulers, e);

        cout << "Enter name of file to decrypt: ";
        cin >> fileName;
        decryptMessage(d, n, fileName);

        cout << "p = " << p << ", q = " << q << ", \"decrypted.txt\"\n";
    }
    return 0;
}


