#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <cstdlib>

using namespace std;

int main(int argc, char* argv[])
{
    if (argc < 3) 
    {
        cout << "Usage: gen_data <output_file> <N>\n";
        return 0;
    }

    string filename = argv[1];
    int N = atoi(argv[2]);

    ofstream out(filename);
    if (!out.is_open()) 
    {
        cerr << "Cannot open output file\n";
        return 1;
    }

    // Random generator (reproducible)
    mt19937 rng(123456);
    uniform_int_distribution<int> keyDist(1, 1000000000);
    uniform_int_distribution<int> opDist(1, 100);

    for (int i = 0; i < N; i++) 
    {
        int op = opDist(rng);
        int key = keyDist(rng);

        if (op <= 60) 
        {
            // 60% INSERT
            out << "insert " << key << " " << i << "\n";
        }
        else if (op <= 90) 
        {
            // 30% FIND
            out << "find " << key << "\n";
        }
        else 
        {
            // 10% ERASE
            out << "erase " << key << "\n";
        }
    }

    out.close();
    cout << "Generated " << N << " operations into " << filename << "\n";
    return 0;
}
