//Credits where credits are due; this class is very much based upon the "ID lookup table" data structure by bitsquid:
//http://bitsquid.blogspot.se/2011/09/managing-decoupling-part-4-id-lookup.html

#pragma once

template<typename T>
class DODContainer
{

/************************************************************************/
/*						Various definitions                             */
/************************************************************************/

private:
	static const unsigned short UShortMax = 65535;

private:
	struct ContainerStruct
	{
		unsigned short id;
		T data;
	};

	struct Index
	{
		unsigned short id;
		unsigned short index;
		unsigned short next;
	};

public:

	/************************************************************************/
	/*                    Constructors and destructors                      */
	/************************************************************************/
	DODContainer(unsigned short size)
	{
		currentActiveObjects = 0;

		maxObjects = (size < UShortMax) ? size : UShortMax-1;

		objectArray = new ContainerStruct[maxObjects];
		internalHandleArray = new Index[maxObjects];

		for(unsigned short i = 0; i < maxObjects; ++i)
		{
			internalHandleArray[i].id = i;
			internalHandleArray[i].next = i+1;
			internalHandleArray[i].index = UShortMax; //Just a dummy value...
		}

		freelist_dequeue = 0;
		freelist_enqueue = maxObjects-1;
	}

	~DODContainer()
	{
		delete[] objectArray;
		delete[] internalHandleArray;
	}

	/************************************************************************/
	/*                              Operators                               */
	/************************************************************************/
	const T&	operator[](int i)	const		{ return objectArray[i].data; }
	T&			operator[](int i)				{ return objectArray[i].data; }

	/************************************************************************/
	/*                          Other functions                             */
	/************************************************************************/
	unsigned short GetActiveObjectCount() 
	{ 
		return currentActiveObjects; 
	}

	T* GetSpecificObject(unsigned short handle) 
	{ 
		return &objectArray[internalHandleArray[handle].index].data; 
	}

	bool AddNewObject(unsigned short& outHandle)
	{
		if(currentActiveObjects < maxObjects)
		{
			Index& idx = internalHandleArray[freelist_dequeue];
			freelist_dequeue = idx.next;

			idx.index = currentActiveObjects++;
			
			ContainerStruct& object = objectArray[idx.index];
			object.id = idx.id;

			outHandle = object.id;

			return true;
		}

		return false;
	}

	//http://bitsquid.blogspot.se/2011/09/managing-decoupling-part-4-id-lookup.html
	void RemoveObject(unsigned short externalHandle)
	{
		Index& idx = internalHandleArray[externalHandle];

		ContainerStruct& object = objectArray[idx.index];
		object = objectArray[--currentActiveObjects];
		internalHandleArray[object.id].index = idx.index;

		idx.index = UShortMax;
		internalHandleArray[freelist_enqueue].next = externalHandle;
		freelist_enqueue = externalHandle;
	}

private:

	/************************************************************************/
	/*							Variables                                   */
	/************************************************************************/

	unsigned short maxObjects;
	unsigned short currentActiveObjects;
	unsigned short freelist_enqueue, freelist_dequeue;

	Index* internalHandleArray;
	ContainerStruct* objectArray;
};