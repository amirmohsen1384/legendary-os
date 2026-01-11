#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <vector>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include "models/task.h"

class TaskQueue
{
protected:
    int64_t parent(int64_t nodeIndex)
    {
        return (nodeIndex - 1) / 2;
    }

    int64_t left(int64_t nodeIndex)
    {
        return nodeIndex * 2 + 1;
    }

    int64_t right(int64_t nodeIndex)
    {
        return nodeIndex * 2 + 2;
    }

    bool hasLeft(int64_t nodeIndex)
    {
        return left(nodeIndex) < container.size();
    }

    bool hasRight(int64_t nodeIndex)
    {
        return right(nodeIndex) < container.size();
    }

    void swap(int64_t firstNode, int64_t secondNode)
    {
        if (firstNode < 0 || firstNode >= container.size())
        {
            return;
        }
        else if (secondNode < 0 || secondNode >= container.size())
        {
            return;
        }
        std::swap(container[firstNode], container[secondNode]);
    }

    void upheap(int64_t nodeIndex)
    {
        int64_t currentIndex = nodeIndex;
        while (currentIndex > 0)
        {
            int64_t parentIndex = parent(currentIndex);
            if (container[currentIndex]->getPriority() < container[parentIndex]->getPriority())
            {
                break;
            }
            swap(currentIndex, parent(currentIndex));
            currentIndex = parentIndex;
        }
    }

    void downheap(int64_t nodeIndex)
    {
        while (hasLeft(nodeIndex))
        {
            int64_t leftIndex = left(nodeIndex);
            int64_t smallChildIndex = leftIndex;
            if (hasRight(nodeIndex))
            {
                int64_t rightIndex = right(nodeIndex);
                if (container[leftIndex]->getPriority() <= container[rightIndex]->getPriority())
                {
                    smallChildIndex = rightIndex;
                }
            }
            if (container[smallChildIndex]->getPriority() <= container[nodeIndex]->getPriority())
            {
                break;
            }
            swap(nodeIndex, smallChildIndex);
            nodeIndex = smallChildIndex;
        }
    }

public:
    TaskQueue() : maximumSize(0)
    {}

    TaskQueue(size_t value) : maximumSize(value)
    {}

    bool isEmpty() const
    {
        return size() == 0;
    }

    size_t size() const
    {
        return container.size();
    }

    void insert(ProcessInfo *info)
    {
        if (maximumSize > 0 && size() > maximumSize)
        {
            throw std::out_of_range("The queue is full of tasks.");
        }
        if (info == nullptr)
        {
            throw std::invalid_argument("Cannot insert null process");
        }
        container.push_back(info);
        upheap(size() - 1);
    }
    ProcessInfo* mostCritical()
    {
        if (isEmpty())
        {
            throw std::out_of_range("The queue is empty");
        }
        return container.front();
    }

    ProcessInfo* removeMostCritical()
    {
        if (isEmpty())
        {
            throw std::out_of_range("The queue is empty");
        }
        auto result = container.front();
        swap(0, container.size() - 1);
        container.pop_back();
        if (!isEmpty())
        {
            downheap(0);
        }
        return result;
    }

    void setMaximumSize(size_t value)
    {
        maximumSize = value;
        while (size() > maximumSize && maximumSize > 0)
        {
            removeMostCritical();
        }
    }

    size_t getMaximumSize() const
    {
        return maximumSize;
    }

private:
    size_t maximumSize = 0;
    std::vector<ProcessInfo*> container;
};


#endif // TASKQUEUE_H
