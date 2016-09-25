/*
* C++ Program to Implement Binary Heap
*/
#include <iostream>
#include <cstdlib>
#include <iterator>
#include "heap.h"


int main()
{
	BinaryHeap h;
	while (1)
	{
		cout << "------------------" << endl;
		cout << "Operations on Heap" << endl;
		cout << "------------------" << endl;
		cout << "1.Insert Element" << endl;
		cout << "2.Delete Minimum Element" << endl;
		cout << "3.Extract Minimum Element" << endl;
		cout << "4.Print Heap" << endl;
		cout << "5.Exit" << endl;
		int choice, element;
		cout << "Enter your choice: ";
		cin >> choice;
		switch (choice)
		{
		case 1:
			cout << "Enter the element to be inserted: ";
			cin >> element;
			h.Insert(element);
			break;
		case 2:
			h.DeleteMin();
			break;
		case 3:
			cout << "Minimum Element: ";
			if (h.ExtractMin() == -1)
			{
				cout << "Heap is Empty" << endl;
			}
			else
				cout << "Minimum Element:  " << h.ExtractMin() << endl;
			break;
		case 4:
			cout << "Displaying elements of Hwap:  ";
			h.DisplayHeap();
			break;
		case 5:
			exit(1);
		default:
			cout << "Enter Correct Choice" << endl;
		}
	}
	return 0;
}