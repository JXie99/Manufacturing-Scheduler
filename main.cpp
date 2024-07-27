#include <iostream>
#include "mqueue.h"
#include <ctime>

const int INSERT_TEST_QUEUE_SIZE = 5;
const int ASSIGN_OP_TESTS_PASSED = 2;
const int COPY_TESTS_PASSED = 2;
const int TEST_SIZE_ONE = 200000;
const int TEST_SIZE_TWO = 400000;
const int TEST_SIZE_THREE = 800000;

// Priority functions compute an integer priority for an order.  Internal
// computations may be floating point, but must return an integer.

int priorityFn1(const Order &order);
int priorityFn2(const Order &order);

class Tester{
public:
    void testInsert();
    void testCopyConstructor();
    void testAssignmentOperator();
    void testGetNextOrderError();
    void measureSimulationTime();
    void testMergeError();
};

int main() {
    try{
        MQueue queue1(priorityFn1);
        MQueue queue2(priorityFn1);

        // Create some test orders and insert them into the queue
        //Order(string customer, int priority, int material, int quantity, int arrival)
        Order order1("Ford",1,1,1,20);
        Order order2("Honda",5,1,2,21);
        Order order3("Toyota",2,0,0,22);
        Order order4("GM",4,3,2,23);

        queue1.insertOrder(order3);
        queue1.insertOrder(order1);
        queue1.insertOrder(order2);
        queue1.insertOrder(order4);

        std::cout << "\nqueue1 dump after inserting 4 orders (Toyota, Honda, Ford, GM):" << std::endl;
        queue1.dump();

        Order order5("Ferrari",6,3,2,1);
        Order order6("Hyundai",3,0,0,10);

        queue2.insertOrder(order5);
        queue2.insertOrder(order6);

        std::cout << "\nqueue2 dump after inserting 2 students (Ferrari, Hyundai):" << std::endl;
        queue2.dump();

        queue1.mergeWithQueue(queue2);

        // Print the queue, change priority function, print again, change
        // again, print again.  Changing the priority function should cause
        // the heap to be rebuilt.

        std::cout << std::endl << "Contents of queue1 after merging with queue2:\n";
        queue1.printOrderQueue();
        std::cout << "\nDump queue1 after merging with queue2:\n";
        queue1.dump();

        queue1.setPriorityFn(priorityFn2);

        std::cout << std::endl << "Contents of queue1 after changing the priority function to priorityFn2:\n";
        queue1.printOrderQueue();
        std::cout << "\nDump queue1 after changing the priority function to priorityFn2:\n";
        queue1.dump();

        queue1.setPriorityFn(priorityFn1);

        std::cout << std::endl << "Contents of queue1 after changing the priority function back to priorityFn1:\n";
        queue1.printOrderQueue();
        std::cout << "\nDump queue1 after changing the priority function back to priorityFn1:\n";
        queue1.dump();

        // Remove and print orders in priority order

        std::cout << "\nFinal queue1 in priority order:\n";
        while (queue1.numOrders() > 0) {
            Order order = queue1.getNextOrder();
            // this will be printed in the ascending order (sorted)
            std::cout << "[" << priorityFn1(order) << "] " << order << std::endl;
        }
    }
    catch(std::out_of_range &e){
        cout << e.what() << endl;
    }

    Tester testing;

    testing.testInsert();
    testing.testCopyConstructor();
    testing.testAssignmentOperator();
    testing.measureSimulationTime();

    //function to see merging queues with different priorities
    try {
        testing.testMergeError();
    }
    catch (std::domain_error const& error ) {
        cout << "\ndomain error exception for merging different priority queues passed" << endl;
        cout << error.what() << endl;
    }

    //function attempts to dequeue an empty queue
    try
    {
        testing.testGetNextOrderError();
    }
    catch (std::domain_error const& error){
        cout << "\ndomain error exception for attempting to dequeue empty queue passed" << endl;
        cout << error.what() << endl;
    }

    return 0;
}

int priorityFn1(const Order & order) {
    //priority value is determined based on some criteria
    //priority value falls in the range [1-58]
    //the smaller value means the higher priority
    int priority = order.getMaterialAvailability() + order.getQuantity() + order.getArrivalTime();
    return priority;
}

int priorityFn2(const Order & order) {
    //priority is determined by a production manager
    //priority value falls in the range [1-58]
    //the smaller value means the higher priority
    return order.getPMPriority();
}

void Tester::testInsert() {

    cout << "\nThis is a test of the insert function" << endl;

    //creates empty queue
    MQueue queueOne(priorityFn1);

    //orders for queue
    Order order1("Mercedes Benz",1,1,1,20);
    Order order2("Infiniti",5,1,2,21);
    Order order3("Ferrari",2,0,0,22);
    Order order4("Corvette",4,3,2,23);
    Order order5("BMW",4,3,2,23);

    //inserting orders into queue
    queueOne.insertOrder(order1);
    queueOne.insertOrder(order2);
    queueOne.insertOrder(order3);
    queueOne.insertOrder(order4);
    queueOne.insertOrder(order5);


    // checks size of queue to see if orders were inserted properly
    if(queueOne.m_size == INSERT_TEST_QUEUE_SIZE) {
        cout << "Insert seems to be working" << endl;
    }
    else {
        cout << "Insert has failed" << endl;
    }
}

void Tester::testAssignmentOperator() {
    cout << "\nthis is a test of the assignment operator" << endl;

    int testPassed = 0;

    // empty queue
    MQueue queueOne(priorityFn1);

    //orders for queue
    Order order1("Mercedes Benz",1,1,1,20);
    Order order2("Infiniti",5,1,2,21);
    Order order3("Ferrari",2,0,0,22);
    Order order4("Corvette",4,3,2,23);
    Order order5("BMW",4,3,2,23);

    //insert orders for queue
    queueOne.insertOrder(order1);
    queueOne.insertOrder(order2);
    queueOne.insertOrder(order3);
    queueOne.insertOrder(order4);
    queueOne.insertOrder(order5);

    // creating empty queue
    MQueue queueTwo(priorityFn1);

    //assign queue two contents to be same as queue one
    queueTwo = queueOne;

    //first test to check if it is a deep copy
    //if they have same memory add, not deep copy
    if(queueOne.m_heap != queueTwo.m_heap)
    {
        testPassed++;
    }

    //clears queue one
    queueOne.clear();

    //second test to check for deep copy
    //check if queue two contents remain after queue one is cleared
    if(queueOne.m_heap == nullptr)
    {
        if(queueTwo.m_heap != nullptr)
        {
            testPassed++;
        }
    }

    //if both tests passed, assignment op works
    if(testPassed == ASSIGN_OP_TESTS_PASSED)
    {
        cout << "Assignment Operator appears to be working" << endl;
    }
    else
    {
        cout << "Assignment Operator has failed" << endl;
    }

}

void Tester::testCopyConstructor()
{
    cout << "\nThis is to test the copy constructor" << endl;

    int testPassed = 0;

    //create empty queue
    MQueue queueOne(priorityFn1);

    //creates orders for queue
    Order order1("Mercedes Benz",1,1,1,20);
    Order order2("Infiniti",5,1,2,21);
    Order order3("Ferrari",2,0,0,22);
    Order order4("Corvette",4,3,2,23);
    Order order5("BMW",4,3,2,23);

    //inserts orders to queue
    queueOne.insertOrder(order1);
    queueOne.insertOrder(order2);
    queueOne.insertOrder(order3);
    queueOne.insertOrder(order4);
    queueOne.insertOrder(order5);

    //creates queue two using copy constructor
    MQueue queueTwo(queueOne);

    //first test to check for deep copy
    //if mem address are different, it is deep copy
    if(queueOne.m_heap != queueTwo.m_heap)
    {
        testPassed++;
    }

    //clears queue one
    queueOne.clear();

    //second test to see if deep copy
    if(queueOne.m_heap == nullptr)
    {
        //if queue two remains after queue one is cleared, passed
        if(queueTwo.m_heap != nullptr)
        {
            testPassed++;
        }
    }

    //if both test passed, it is deep copy.
    if(testPassed == COPY_TESTS_PASSED)
    {
        cout << "Copy Constructor appears to be working" << endl;
    }
    else
    {
        cout << "Copy Constructor has failed" << endl;
    }
}

void Tester::measureSimulationTime() {

    cout << "\nmeasuring the efficiency of heap insertion\n" << endl;

    double T = 0.0;//to store running times
    clock_t start, stop;//stores the clock ticks while running the program

    //creates queues for testing
    MQueue queueOne(priorityFn1);
    MQueue queueTwo(priorityFn1);
    MQueue queueThree(priorityFn1);

    //order for testing
    Order order1("Mercedes Benz",1,1,1,20);

    start = clock();

    //inserts order into queue 20,000 times
    for (int i = 0; i < TEST_SIZE_ONE ;i++)
    {
        queueOne.insertOrder(order1);
    }

    stop = clock();
    T = stop - start;//number of clock ticks the algorithm took

    cout << "Counting 200000 insertions took " << T << " clock ticks ("<< T/CLOCKS_PER_SEC << " seconds)!" << endl;

    //test for 40,000 insertions
    start = clock();

    //inserts order into queue 40,000 times
    for(int i = 0; i < TEST_SIZE_TWO; i++)
        queueTwo.insertOrder(order1);

    stop = clock();

    T = stop - start;

    cout << "Counting 400000 insertions took " << T << " clock ticks ("<< T/CLOCKS_PER_SEC << " seconds)!" << endl;

    //test for 80,000 insertions
    start = clock();

    //inserts order into queue 80,000 times
    for(int i = 0; i < TEST_SIZE_THREE; i++)
        queueThree.insertOrder(order1);

    stop = clock();

    T = stop - start;

    cout << "Counting 800000 insertions took " << T << " clock ticks ("<< T/CLOCKS_PER_SEC << " seconds)!" << endl;


    cout << "\nNow measuring the efficiency of extraction\n" << endl;

    //tests for 20,000 extractions
    start = clock();

    //extracts 20,000 orders
    for(int i = 0; i < TEST_SIZE_ONE; i++)
        queueOne.getNextOrder();

    stop = clock();

    T = stop - start;

    cout << "Counting 200000 extractions took " << T << " clock ticks ("<< T/CLOCKS_PER_SEC << " seconds)!" << endl;

    //test for 40,000 extractions
    start = clock();

    //extracts 40,000 orders
    for(int i = 0; i < TEST_SIZE_TWO; i++)
        queueTwo.getNextOrder();

    stop = clock();

    T = stop - start;

    cout << "Counting 400000 extractions took " << T << " clock ticks ("<< T/CLOCKS_PER_SEC << " seconds)!" << endl;

    //test for 80,000 extractions
    start = clock();

    //extracts 80,000 orders
    for(int i = 0; i < TEST_SIZE_THREE; i++)
        queueThree.getNextOrder();

    stop = clock();

    T = stop - start;

    cout << "Counting 800000 extractions took " << T << " clock ticks ("<< T/CLOCKS_PER_SEC << " seconds)!" << endl;

}


void Tester::testMergeError() {
    MQueue queue1(priorityFn1);
    MQueue queue2(priorityFn2);

    queue1.mergeWithQueue(queue2);
}

void Tester::testGetNextOrderError() {
    MQueue queue1(priorityFn2);
    queue1.getNextOrder();
}