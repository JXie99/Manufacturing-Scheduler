/*
 * File:          mqueue.cpp
 * Project:       CMSC 341 Project 2 Fall 2021
 * Author:        Johnson Xie 
 *
 * Description:   This file contains the mqueue.cpp
 *
 *
 */

#include "mqueue.h"
using namespace std;
using std::domain_error;

//constructor
MQueue::MQueue(prifn_t priFn)
{
    m_priorFunc = priFn;
    m_heap = nullptr;
    m_size = 0;
}

//destructor
MQueue::~MQueue()
{
    destroyTree(m_heap);
    m_heap = nullptr;
    m_size = 0;
}

//copy constructor 
MQueue::MQueue(const MQueue& rhs)
{
    // if rhs ends up being an empty queue
    if(rhs.m_heap == nullptr)
    {
        m_heap = nullptr;
        m_size = rhs.m_size;
        m_priorFunc = rhs.m_priorFunc;
    }
    else
    {
        // copy rhs variables
        m_size = rhs.m_size;
        m_priorFunc = rhs.m_priorFunc;

        //helper function to loop through rhs tree for copying
        copyTree(rhs.m_heap, m_heap);
    }

}

//assignment operator
MQueue& MQueue::operator=(const MQueue& rhs)
{
    if(this != &rhs)
    {
        // deallocate old memory
        destroyTree(this->m_heap);

        //copies rhs member variables
        m_size = rhs.m_size;
        m_priorFunc = rhs.m_priorFunc;

        // copies nodes over from rhs using helper function.
        copyTree(rhs.m_heap, m_heap);
    }
    return *this;
}

//inserts an order into queue and maintains min heap property
void MQueue::insertOrder(const Order& input) 
{

    //if the heap is empty,
    if(m_heap == nullptr)
    {
        m_heap = new Node(input);
        m_size++;
    }

    else
    {
        //creates new node and merge
        Node* newOrder = new Node(input);
        m_heap = merge(m_heap, newOrder);
        m_size++;
    }

}

//removes node and return highest priority order
Order MQueue::getNextOrder() 
{

    //stops empty queue extractions
    if(m_heap == nullptr)
    {
        throw std::domain_error("queue is empty");
    }

    //get curr order info
    Node* currOrder = m_heap;
    Order currOrderVal = m_heap->m_order;

    //merges curr order left and right children
    m_heap = merge(m_heap->m_left, m_heap->m_right);

    //deletes order
    delete currOrder;
    m_size--;

    return currOrderVal;

}

//merge host queue with rhs
void MQueue::mergeWithQueue(MQueue& rhs) 
{

    //stops merging different priority queues
    if(m_priorFunc != rhs.m_priorFunc)
        throw std::domain_error("priority functions are not the same!!!");

    //stops self assignment
    if(this!= &rhs)
    {
        //merges rhs heap with this heap
        m_heap = merge(m_heap, rhs.m_heap);

        //size of merged heap
        m_size = m_size + rhs.m_size;
    }

    rhs.m_heap = nullptr;
}

//clears queue
void MQueue::clear() 
{
    destroyTree(m_heap);
    m_heap = nullptr;
    m_size = 0;
}

//return curr num of orders in queue
int MQueue::numOrders() const
{
    return m_size;
}

//prints contents of queue using pre order traversal 
void MQueue::printOrderQueue() const 
{
    preOrder(m_heap);
}

//get curr priority function 
prifn_t MQueue::getPriorityFn() const 
{
    return m_priorFunc;
}

//set a new priority function and rebuilds heap
void MQueue::setPriorityFn(prifn_t priFn) 
{
    int tempSize = m_size;

    // creates temp heap with new priority
    MQueue tempHeap(priFn);

    //gives temp heap orders
    while(m_heap)
        tempHeap.insertOrder(getNextOrder());

    //sets heap to temp heap
    m_heap = tempHeap.m_heap;

    tempHeap.m_heap = nullptr;

    m_priorFunc = priFn;
    m_size = tempSize;

}

// overloaded insertion operator for Order class
ostream& operator<<(ostream& sout, const Order& order)
 {
    sout << "Order: " << order.getCustomer() << ", PM priority: " << order.getPMPriority()
         << ", quantity: " << order.getQuantityValue()
         << ", material availability: " << order.getAvailabilityProbablity() << "%"
         << ", arrival time: " << order.getArrivalTime();
    return sout;
}

// overloaded insertion operator for Node class
ostream& operator<<(ostream& sout, const Node& node) 
{
    sout << node.getOrder();
    return sout;
}

// for debugging
void MQueue::dump() const
{
    if (m_size == 0) 
    {
        cout << "Empty skew heap.\n" ;
    } 

    else 
    {
        dump(m_heap);
        cout << endl;
    }
}

// for debugging
void MQueue::dump(Node *pos) const 
{
    if ( pos != nullptr ) 
    {
        cout << "(";
        dump(pos->m_left);
        cout << m_priorFunc(pos->m_order) << ":" << pos->m_order.getCustomer();
        dump(pos->m_right);
        cout << ")";
    }
}

void MQueue::destroyTree(Node *aNode) 
{
    if(aNode == nullptr)
        return;
    else
    {
        //traverse through tree and delete
        destroyTree(aNode->m_left);
        destroyTree(aNode->m_right);
        delete aNode;
    }
}

void MQueue::preOrder(Node *aNode) const 
{
    if(aNode == nullptr)
        return;

    cout << "[" << m_priorFunc(aNode->getOrder()) << "] "<< aNode->getOrder() << endl;
    preOrder(aNode->m_left);
    preOrder(aNode->m_right);
}

void MQueue::copyTree(Node* sourceNode, Node*& copyNode) 
{
    if(sourceNode == nullptr)
        return;

    //copies curr order
    copyNode = new Node(sourceNode->getOrder());

    //traverse to get rest of orders
    copyTree(sourceNode->m_left, copyNode->m_left);
    copyTree(sourceNode->m_right, copyNode->m_right);

}

Node * MQueue::merge(Node*& p1, Node*& p2)
{

    Node* temp = nullptr;

    //place holder values
    int p1Priority = 100;
    int p2Priority = 200;

    //gets priority if the current order node is not a null pointer
    if(p1 != nullptr)
        p1Priority = m_priorFunc(p1->getOrder());

    // gets priority if current order node is not a null pointer
    if(p2 != nullptr)
        p2Priority = m_priorFunc(p2->getOrder());

    if(p1 != nullptr && p2 != nullptr)
    {
        //if p1 heap is less than or equal to p2 heap
        if(p1Priority <= p2Priority)
        {
            // recursively merges right subtree of p1 with p2
            p1->m_right = merge(p1->m_right, p2);
            temp = p1;
        }

        else if(p1Priority >= p2Priority)
        {
            //swaps in order for p1 to have highest priority
            swap(p1, p2);

            // recursively merges right subtree of p1 with p2
            p1->m_right = merge(p1->m_right, p2);
            temp = p1;

        }
    }

    // will return p1 if p2 is null
    else if(p1 != nullptr && p2 == nullptr)
    {
        temp = p1;
    }

    // will return p2 if p1 is null
    else if(p1 == nullptr && p2 != nullptr)
    {
        temp = p2;
    }

    //swaps children
    if(temp != nullptr)
    {
        swap(temp->m_left, temp->m_right);
    }

    return temp;
}


