#include<iostream>
#include<Windows.h>
#include<cstdlib>
#include<string>
#include<ctime>
#include <unordered_map>
using namespace std;
#define loadfactor 1.75
#define k 2.5

string books[10] = { "Біблія", "Гобсек", "Капітал", "1984", "Дюна", "Протистояння", "Воно", "Секрети", "Загублена", "Правда" };

struct Data
{
	string name;
	int price;
	int pages;
	Data()
	{
		name = books[rand() % 10];
		price = rand();
		pages = rand();
	}
};

struct HashNode
{
	long long key;
	Data value;
	HashNode() {}
	HashNode(long long newkey, Data newvalue)
	{
		key = newkey;
		value = newvalue;
	}
};

struct Node
{
	HashNode hashnode;
	Node* next = nullptr;
	Node* previous = nullptr;
	Node() {}
	Node(HashNode& newhashnode, Node* preprevious = nullptr, Node* nextnode = nullptr)
	{
		hashnode = newhashnode;
		previous = preprevious;
	}
};

struct LinkedList
{
	int N = 0;
	Node* head;

	int size()
	{
		return N;
	}



	void push(HashNode& book)
	{
		if (N == 0)
		{
			head = new Node(book);
		}
		else
		{
			Node* tmp = head;
			Node* tmp2 = tmp;
			while (tmp != nullptr)
			{
				if (tmp->hashnode.key == book.key)
				{
					tmp->hashnode.value = book.value;
					return;
				}
				tmp2 = tmp;
				tmp = tmp->next;
			}
			tmp2->next = new Node(book, tmp2);
		}
		N++;
	}


	Data* pop(long long key)
	{
		if (N != 0)
		{
			Node* tmp = head;
			while (tmp != nullptr)
			{
				if (key == tmp->hashnode.key)
				{
					if (tmp->previous == nullptr && tmp->next != nullptr)
					{
						head = tmp->next;
						head->previous = nullptr;
					}
					else
					{
						if (tmp->previous != nullptr && tmp->next == nullptr)
						{
							tmp->previous->next = nullptr;
						}
						else
						{
							if (tmp->previous != nullptr)
							{
								tmp->previous->next = tmp->next;
								tmp->next->previous = tmp->previous;
							}
						}
					}
					Data deleted = tmp->hashnode.value;
					delete tmp;
					N--;
					return &deleted;
				}
				tmp = tmp->next;
			}
		}
		return NULL;
	}
};



struct HashTable
{
	int m = 8;
	LinkedList* bucketsArray = new LinkedList[m];
	HashNode node;
	int Size = 0;

	int size()
	{
		return Size;
	}

	int hash(long long key)
	{
		return key % m;
	}

	void increase()
	{


		m *= k;
		LinkedList* bucketsArray2 = new LinkedList[m];
		for (int i = 0; i < m / k; i++)
		{
			if (bucketsArray[i].size() > 0)
			{
				Node* tmp = bucketsArray[i].head;
				while (tmp->next != nullptr)
				{
					bucketsArray2[hash(tmp->hashnode.key)].push(tmp->hashnode);
					tmp = tmp->next;
					delete tmp->previous;
				}
				if (tmp->next == nullptr)
				{
					bucketsArray2[hash(tmp->hashnode.key)].push(tmp->hashnode);
				}
				delete tmp;
			}
		}
		delete[] bucketsArray;
		bucketsArray = bucketsArray2;
	}
	void insert(long long key, Data value)
	{
		if ((double)Size / m > loadfactor)
		{
			increase();
		}
		node = HashNode(key, value);
		int size = bucketsArray[hash(key)].size();
		bucketsArray[hash(key)].push(node);
		if (size != bucketsArray[hash(key)].size())
			Size++;
	}

	Data* find(long long key)
	{
		if (bucketsArray[hash(key)].size() > 0)
		{
			Node* tmp = bucketsArray[hash(key)].head;
			while (tmp != nullptr)
			{
				if (key == tmp->hashnode.key)
				{
					return &tmp->hashnode.value;
				}
				tmp = tmp->next;
			}
		}
		return NULL;
	}

	Data* erase(long long key)
	{
		Data* deleted = bucketsArray[hash(key)].pop(key);
		if (deleted != NULL)
		{
			Size--;
		}
		return deleted;
	}
};

long long generateRandLong()
{
	long long key = 0;
	for (int i = 0; i < 10; i++)
	{
		key += pow(10, i) * (rand() % 10);
	}
	return key;
}



bool testHashTable()
{
	const int iters = 500000;
	const int keysAmount = iters * 1;
	// generate random keys:
	long long* keys = new long long[keysAmount];
	long long* keysToInsert = new long long[iters];
	long long* keysToErase = new long long[iters];
	long long* keysToFind = new long long[iters];
	for (int i = 0; i < keysAmount; i++)
	{
		keys[i] = generateRandLong();
	}
	for (int i = 0; i < iters; i++)
	{
		keysToInsert[i] = keys[generateRandLong() % keysAmount];
		keysToErase[i] = keys[generateRandLong() % keysAmount];
		keysToFind[i] = keys[generateRandLong() % keysAmount];
	}
	// test my HashTable:
	HashTable hashTable;
	clock_t myStart = clock();
	for (int i = 0; i < iters; i++)
	{
		hashTable.insert(keysToInsert[i], Data());
	}
	int myInsertSize = hashTable.size();
	for (int i = 0; i < iters; i++)
	{
		hashTable.erase(keysToErase[i]);
	}
	int myEraseSize = hashTable.size();
	int myFoundAmount = 0;
	for (int i = 0; i < iters; i++) {
		if (hashTable.find(keysToFind[i]) != NULL)
		{
			myFoundAmount++;
		}
	}
	clock_t myEnd = clock();
	float myTime = (float(myEnd - myStart)) / CLOCKS_PER_SEC;
	// test STL hash table:
	unordered_map<long long, Data> unorderedMap;
	clock_t stlStart = clock();
	for (int i = 0; i < iters; i++)
	{
		unorderedMap.insert({ keysToInsert[i], Data() });
	}
	int stlInsertSize = unorderedMap.size();
	for (int i = 0; i < iters; i++)
	{
		unorderedMap.erase(keysToErase[i]);
	}
	int stlEraseSize = unorderedMap.size();
	int stlFoundAmount = 0;
	for (int i = 0; i < iters; i++)
	{
		if (unorderedMap.find(keysToFind[i]) != unorderedMap.end())
		{
			stlFoundAmount++;
		}
	}
	clock_t stlEnd = clock();
	float stlTime = (float(stlEnd - stlStart)) / CLOCKS_PER_SEC;
	cout << "My HashTable:" << endl;
	cout << "Time: " << myTime << ", size: " << myInsertSize << " - " << myEraseSize <<
		", found amount: " << myFoundAmount << endl;
	cout << "STL unordered_map:" << endl;
	cout << "Time: " << stlTime << ", size: " << stlInsertSize << " - " << stlEraseSize
		<< ", found amount: " << stlFoundAmount << endl << endl;
	delete keys;
	delete keysToInsert;
	delete keysToErase;
	delete keysToFind;
	if (myInsertSize == stlInsertSize && myEraseSize == stlEraseSize && myFoundAmount ==
		stlFoundAmount)
	{
		cout << "The lab is completed" << endl;
		return true;
	}
	cerr << ":(" << endl;
	return false;
}

int main()
{
	srand(time(0));
	SetConsoleOutputCP(1251);
	testHashTable();
}

