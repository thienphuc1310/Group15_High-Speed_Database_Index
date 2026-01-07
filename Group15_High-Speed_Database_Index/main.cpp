#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>

#include "cuckoohash.h"
#include "BTree.h"
#include "count.h"

using namespace std;
using namespace chrono;

//random data
vector<int> generateRandomArray(int n)
{
    vector<int> a(n);
    for (int i = 0; i < n; i++) a[i] = i;

    random_device rd;
    mt19937 rng(rd());
    shuffle(a.begin(), a.end(), rng);

    return a;
}

//menu
void printMenu()
{
    cout << "\n========== DATABASE INDEX TEST ==========\n";
    cout << "1. Build index (Cuckoo Hash primary + B-Tree secondary)\n";
    cout << "2. Test point lookup (find/search)\n";
    cout << "3. Test range query (B-Tree only)\n";
    cout << "4. Test ordered traversal (B-Tree only)\n";
    cout << "5. Delete ONE key (Cuckoo vs B-Tree)\n";
    cout << "6. Delete RANGE (B-Tree only)\n";
    cout << "0. Exit\n";
    cout << "Choose: ";
}

int main()
{
    const int N = 100000;

    cout << "Generating random dataset...\n";
    vector<int> data = generateRandomArray(N);
    cout << "Dataset size: " << N << "\n";

    CuckooHash* cuckoo = nullptr;
    BTree* btree = nullptr;

    int choice;

    do {
        printMenu();
        cin >> choice;

        //insert key vào từng thuật toán
        if (choice == 1)
        {
            delete cuckoo;
            delete btree;

            cuckoo = new CuckooHash(N * 2);
            btree = new BTree(50);

            cout << "\n[BUILD INDEX]\n";

            //cukoo hash
            gCount.reset();
            auto start = high_resolution_clock::now();
            for (int x : data) cuckoo->insert(x);
            auto end = high_resolution_clock::now();

            cout << "\nCuckoo Hash (PRIMARY INDEX):\n";
            cout << "  Time     : " << duration_cast<milliseconds>(end - start).count() << " ms\n";
            cout << "  Hash     : " << gCount.hashCount << "\n";
            cout << "  Kicks    : " << gCount.kickCount << "\n";
            cout << "  Rehash   : " << gCount.rehashCount << "\n";

            //b-tree
            gCount.reset();
            start = high_resolution_clock::now();
            for (int x : data) btree->insert(x);
            end = high_resolution_clock::now();

            cout << "\nB-Tree (SECONDARY INDEX):\n";
            cout << "  Time     : " << duration_cast<milliseconds>(end - start).count() << " ms\n";
            cout << "  Splits   : " << gCount.btreeSplitCount << "\n";
        }

        //tìm kiếm tất cả các key của từng thuật toán
        else if (choice == 2)
        {
            if (!cuckoo || !btree)
            {
                cout << "Please build index first (option 1)\n";
                continue;
            }

            cout << "\n[POINT LOOKUP TEST]\n";

            //cuckoo hash
            gCount.reset();
            auto start = high_resolution_clock::now();
            for (int x : data) cuckoo->find(x);
            auto end = high_resolution_clock::now();

            cout << "\nCuckoo Hash (PRIMARY):\n";
            cout << "  Time      : " << duration_cast<milliseconds>(end - start).count() << " ms\n";
            cout << "  Hash call : " << gCount.hashCount << "\n";

            //b-tree
            gCount.reset();
            start = high_resolution_clock::now();
            for (int x : data) btree->search(x);
            end = high_resolution_clock::now();

            cout << "\nB-Tree (SECONDARY):\n";
            cout << "  Time      : " << duration_cast<milliseconds>(end - start).count() << " ms\n";
            cout << "  Node acc  : " << gCount.btreeNodeAccess
                << " (~" << (double)gCount.btreeNodeAccess / N << " / search)\n";
        }

        //lấy key thuộc đoạn L, R
        else if (choice == 3)
        {
            if (!btree)
            {
                cout << "Please build index first (option 1)\n";
                continue;
            }

            int L, R;
            cout << "Enter L R: ";
            cin >> L >> R;
            if (L > R) swap(L, R);

            cout << "\n[RANGE QUERY]\n";
            cout << "Cuckoo Hash: NOT SUPPORTED\n";

            vector<int> result;
            gCount.reset();

            auto start = high_resolution_clock::now();
            btree->rangeQuery(L, R, result);
            auto end = high_resolution_clock::now();

            cout << "\nB-Tree:\n";
            cout << "  Time      : " << duration_cast<milliseconds>(end - start).count() << " ms\n";
            cout << "  Found     : " << result.size() << " keys\n";
            cout << "  Node acc  : " << gCount.btreeNodeAccess << "\n";
        }

        //duyệt tuần tự theo theo max K, ví dụ: K = 3, sẽ cho ra 0, 1, 2, 3
        else if (choice == 4)
        {
            if (!btree)
            {
                cout << "Please build index first (option 1)\n";
                continue;
            }

            int K;
            cout << "Enter K: ";
            cin >> K;

            cout << "\n[ORDERED TRAVERSAL]\n";
            cout << "Cuckoo Hash: NOT SUPPORTED\n";

            vector<int> out;
            gCount.reset();
            btree->collectFirstK(K, out);

            cout << "B-Tree first " << K << " keys:\n";
            for (int x : out) cout << x << " ";
            cout << "\n";
        }

        //xóa 1 key chỉ định
        else if (choice == 5)
        {
            if (!cuckoo || !btree)
            {
                cout << "Please build index first (option 1)\n";
                continue;
            }

            int key;
            cout << "Enter key to delete: ";
            cin >> key;

            cout << "\n[DELETE ONE KEY]\n";

            //cuckoo hash
            gCount.reset();
            auto start = high_resolution_clock::now();
            bool ok = cuckoo->erase(key);
            auto end = high_resolution_clock::now();

            cout << "\nCuckoo Hash:\n";
            cout << "  Result : " << (ok ? "DELETED" : "NOT FOUND") << "\n";
            cout << "  Time   : " << duration_cast<microseconds>(end - start).count() << " us\n";

            //b-tree
            bool existed = (btree->search(key) != nullptr);

            gCount.reset();
            start = high_resolution_clock::now();
            btree->remove(key);
            end = high_resolution_clock::now();

            bool stillThere = (btree->search(key) != nullptr);

            cout << "\nB-Tree:\n";
            cout << "  Result   : " << (!existed ? "NOT FOUND" : (stillThere ? "FAILED" : "DELETED")) << "\n";
            cout << "  Time     : " << duration_cast<microseconds>(end - start).count() << " us\n";
            cout << "  Node acc : " << gCount.btreeNodeAccess << "\n";
        }

        //xóa theo khoảng
        else if (choice == 6)
        {
            if (!btree)
            {
                cout << "Please build index first (option 1)\n";
                continue;
            }

            int L, R;
            cout << "Enter L R: ";
            cin >> L >> R;
            if (L > R) swap(L, R);

            cout << "\n[DELETE RANGE]\n";
            cout << "Cuckoo Hash: NOT SUPPORTED\n";

            vector<int> willDelete;
            btree->rangeQuery(L, R, willDelete);

            gCount.reset();
            auto start = high_resolution_clock::now();
            btree->removeRange(L, R);
            auto end = high_resolution_clock::now();

            cout << "\nB-Tree:\n";
            cout << "  Deleted   : " << willDelete.size() << " keys\n";
            cout << "  Time      : " << duration_cast<milliseconds>(end - start).count() << " ms\n";
            cout << "  Node acc  : " << gCount.btreeNodeAccess << "\n";
        }

    } while (choice != 0);

    delete cuckoo;
    delete btree;

    cout << "Exit program.\n";
    return 0;
}
