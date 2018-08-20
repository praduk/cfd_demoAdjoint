/*
* mempool.hh
* 
* Copyright (c) 2017-2018 Pradu Kannan. All rights reserved.
*/

#ifndef ___MEMPOOL_HH
#define ___MEMPOOL_HH

template<typename T>
struct MemPoolNode
{
    T data;
    MemPoolNode* next;
};

template<typename T, unsigned long N>
class MemPool
{
    MemPoolNode<T> node[N];
    MemPoolNode<T>* head;
public:
    MemPool()
    {
        reset();
    }

    void reset()
    {
        for(unsigned long i=0; i<(N-1); i++)
            node[i].next = node+i+1;
        node[N-1].next = 0;
        head = node;
    }

    T* alloc()
    {
        if(head)
        {
            T* ret = &(head->data);
            head = head->next;
            return ret;
        }
        return 0;
    }

    void free(T* element)
    {
        MemPoolNode<T>* p = reinterpret_cast<MemPoolNode<T>*>(element);
        p->next = head;
        head = p;
    }

};

#endif  // ___MEMPOOL_HH

