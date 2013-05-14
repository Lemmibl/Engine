#ifndef LEMMILINKEDLIST_H
#define LEMMILINKEDLIST_H

template<class T> class LemmiListNode;
template<class T> class LemmiLinkedList;
template<class T> class LemmiListIterator;

template<class T>
class LemmiListNode
{
public:
	LemmiListNode<T>* next;
	LemmiListNode<T>* previous;
	T data;

	void InsertBefore( T p_data )
	{
		LemmiListNode<T>* newnode = new LemmiListNode<T>;
		newnode->data = p_data;

		newnode->next     = this;
		newnode->previous = previous;

		if( previous != 0 )
		{
			previous->next = newnode;
		}

		previous = newnode;
	}

	void Delink()
	{
		if( previous != 0 )
		{
			previous->next = next;
		}

		if( next != 0 )
			next->previous = previous;
	}

	void InsertAfter(T p_data )
	{
		LemmiListNode<T>* newnode = new LemmiListNode<T>;
		newnode->data = p_data;

		newnode->next     = next;
		newnode->previous = this; 

		if( next != 0 )
		{
			next->previous = newnode;
		}

		next = newnode;
	}
};


template<class T>
class LemmiLinkedList
{
public:
	LemmiListNode<T>* head;
	LemmiListNode<T>* tail;
	int count;

	LemmiLinkedList()
	{
		head = 0;
		tail = 0;
		count = 0;
	}

	~LemmiLinkedList()
	{
		LemmiListNode<T>* temp_node = head;
		LemmiListNode<T>* temp_next;

		while( temp_node != 0 )
		{
			temp_next = temp_node->next;

			delete temp_node;

			temp_node = temp_next;
		}
	}

	LemmiListIterator<T> GetIterator()
	{
		return LemmiListIterator<T>( this, head );
	}

	int Count()
	{
		return count;
	}

	void AddTwoLinkedLists(const LemmiListIterator<T>& otherListIterator)
	{
		//I have no fucking idea
		otherListIterator->Start();
		
		while(otherListIterator.Valid())
		{
			Append(otherListIterator->Data());
			otherListIterator->Forth();
		}
	}

	void Append( T p_data ) //Add to the front
	{
		if( head == 0 )
		{
			head = tail = new LemmiListNode<T>;
			head->data = p_data;
			head->next = 0;
			head->previous = 0;
		}
		else
		{
			tail->InsertAfter( p_data );
			tail = tail->next;
		}

		count++;
	}

	void Prepend( T p_data ) //Add to the back
	{
		if( head == 0 )
		{
			head = tail = new LemmiListNode<T>;
			head->data = p_data;
			head->next = 0;
			head->previous = 0;
		}
		else
		{
			head->InsertBefore( p_data );
			head = head->previous;
		}

		count++;
	}

	void Remove( LemmiListIterator<T>& p_iterator )
	{
		LemmiListNode<T>* node;

		if( p_iterator.node == 0 )
		{
			return;
		}

		node = p_iterator.node;

		if( node == head )
		{
			head = head->next;
		}
		else if( node == tail )
		{
			tail = tail->previous;
		}

		p_iterator.Forth();

		node->Delink();
		delete node;

		if( head == 0 )
		{
			tail = 0;		
		}

		count--;
	}

	void RemoveHead()
	{
		LemmiListNode<T>* node = 0;

		if( head != 0 )
		{
			node = head->next;

			delete head;
			head = node;

			if( head == 0 )
				tail = 0;
			else
				head->previous = 0;

			count--;
		}
	}

	void RemoveTail()
	{
		LemmiListNode<T>* node = 0;

		if( tail != 0 )
		{
			node = tail->previous;

			delete tail;
			tail = node;

			if( tail == 0 )
				head = 0;
			else
				tail->next = 0;

			count--;
		}
	}

	void InsertAfter( LemmiListIterator<T>& p_iterator, T p_data )
	{
		if( p_iterator.node != 0 )
		{
			p_iterator.node->InsertAfter( p_data );

			if( p_iterator.node == tail )
				tail = tail->next;
			count++;
		}
		else
		{
			Append( p_data );
		}
	}

	void InsertBefore( LemmiListIterator<T>& p_iterator, T p_data )
	{
		if( p_iterator.node != 0 )
		{
			p_iterator.node->InsertBefore( p_data );

			if( p_iterator.node == head )
				head = head->previous;

			count++;
		}
		else
		{
			Prepend( p_data );
		}
	}
};

template<class T>
class LemmiListIterator
{
private:
	LemmiListNode<T>* node;
	LemmiLinkedList<T>* list;

public:
	LemmiListIterator( LemmiLinkedList<T>* p_list = 0, LemmiListNode<T>* p_node = 0 )
	{
		list = p_list;
		node = p_node;
	}

	void Start()
	{
		if( list != 0 )
		{
			node = list->head;
		}
	}

	void End()
	{
		if( list != 0 )
		{
			node = list->tail;
		}
	}

	void Forth()
	{
		if( node != 0 )
		{
			node = node->next;
		}
	}

	void Back()
	{
		if( node != 0 )
		{
			node = node->previous;
		}
	}

	T& Data()
	{
		return node->data;
	}

	bool Valid()
	{
		return (node != 0);
	}

	bool operator==( LemmiListIterator<T>& p_rightHandSide )
	{
		if( node == p_rightHandSide.node && list == p_rightHandSide.list )
		{
			return true;
		}

		return false;
	}
};

#endif