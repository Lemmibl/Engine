#pragma once
#include <queue>
#include <climits>

template<typename T>
class DODContainer
{
public:

	DODContainer(unsigned short size)
	: currentActiveObjects(0), maxObjects(size)
	{
		objectArray = new T[maxObjects];
		internalHandles = new unsigned short[maxObjects];

		for(unsigned short i = 0; i < maxObjects; i++)
		{
			prioQueue.push(i);
			internalHandles[i] = i;
		}
	}

	~DODContainer()
	{
		delete [] objectArray;
		delete [] internalHandles;

		objectArray = 0;
		internalHandles = 0;
	}

	const T& operator[](int i) const { return internalHandles[i]; }
	T& operator[](int i)			{ return internalHandles[i]; }

	bool AddNewObject(unsigned short& outHandle)
	{
		if(currentActiveObjects < maxObjects)
		{
			//Get next free externalHandle
			unsigned short nextSlot = GetNextOpenSlot();

			internalHandles[nextSlot] = currentActiveObjects++;
			outHandle = internalHandles[nextSlot];

			return true;
		}

		return false;
	}

	void RemoveObject(unsigned short externalHandle)
	{
		//We remove a sentence. This is how we do that:
		// 1) Swap furthest out sentence with the one we should delete. 
		// 2) Update indices to reflect this.
		// 3) Reduce activeSentences by one. 
		// 4) Re-add the ID to the prio queue so that it can be reused.

		unsigned short idx = internalHandles[externalHandle];

		//Get reference to the sentence we're going to remove.
		T& container = objectArray[idx];

		//Swap sentence with furthest out active sentence.
		container = objectArray[currentActiveObjects-1];

		internalHandles[currentActiveObjects-1] = idx;

		//Decrement active sentences, essentially invalidating the swapped sentence. It's still technically there, just that we're not going to read or access it.
		currentActiveObjects--;

		//Add the ID back to the priority queue to let the allocator know that this ID slot is available.
		prioQueue.push(externalHandle);
	}

	unsigned short GetActiveObjectCount() { return currentActiveObjects; }

	T* GetSpecificObject(unsigned short handle) { return &objectArray[handle]; }
	T* GetObjectArray() { return objectArray; }

private:
	unsigned short GetNextOpenSlot() { unsigned short result = prioQueue.top(); prioQueue.pop(); return result; }

	std::priority_queue<unsigned short, std::vector<unsigned short>, std::greater<unsigned short>> prioQueue;
	unsigned short* internalHandles;

	unsigned short maxObjects;
	unsigned short currentActiveObjects;

	T* objectArray;
};