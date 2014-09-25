/*
* Class Declaration
*/
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

#ifndef HEAP_H
#define HEAP_H


class BinaryHeap
{
private:
	vector <int> heap;
	int left(int parent);
	int right(int parent);
	int parent(int child);
	void heapifyup(int index);
	void heapifydown(int index);
public:
	BinaryHeap()
	{}
	void Insert(int element);
	void DeleteMin();
	int ExtractMin();
	void DisplayHeap();
	int Size();
};



#endif