#pragma once
#include "Types.h"

template<typename T>
class List
{
public:
	List() {}
	~List() {}

	void push_back(T& pItem)
	{
		auto nextNode = backNode;
		backNode = getNextNode();
		nextNode->item = pItem;
		nextNode->next = backNode;
	}

	void get(u64 pIndex)
	{

	}

private:

	List_Node<T>* getNextNode()
	{
		return new List_Node<T>;
	}

	u64 size;

	List_Node<T>* parentNode;
	List_Node<T>* backNode;
};

template<typename T>
class List_Node
{
	friend class List;
private:
	List_Node() {}
	~List_Node() {}

	T item;
	List_Node<T>* next;
};