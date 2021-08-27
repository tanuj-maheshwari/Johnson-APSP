#include <bits/stdc++.h>
using namespace std;

//Structure for a node of binomial heap
struct binomial_heap_node
{
    int value;                                    //distance value
    int key;                                      //number(index) of vertex of graph
    int degree;                                   //degree of the heap node
    binomial_heap_node *child, *parent, *sibling; //pointers maintaining structure
};

//Structure for a node of fibonacci heap
struct fibonacci_heap_node
{
    int value;                                          //distance value
    int key;                                            //number(index) of vertex of graph
    int rank;                                           //rank of heap node
    bool marked;                                        //if the node is marked or not
    fibonacci_heap_node *child, *parent, *left, *right; //pointers maintaining structure
};

//Bellman Ford Algorithm
int BellmanFord(vector<list<pair<int, int>>> &graph, int N, int source, vector<int> &dist_from_source)
{
    vector<pair<pair<int, int>, int>> edge_order; //vector storing the edge order for Bellman Ford

    //Entering edges in lexicographic order in the edge_order vector
    for (int u = 1; u <= N; u++)
    {
        list<pair<int, int>>::iterator it;
        for (it = graph[u].begin(); it != graph[u].end(); it++)
        {
            int v = it->first;
            int e = it->second;
            edge_order.push_back(make_pair(make_pair(u, v), e));
        }
    }

    dist_from_source[source] = 0;
    int flag = 0; //for detecting if there is any change in an iteration

    //Loop N times
    for (int i = 1; i <= N; i++)
    {
        flag = 0;

        //relaxing for all edges in the edge_order list
        vector<pair<pair<int, int>, int>>::iterator it;
        for (it = edge_order.begin(); it != edge_order.end(); it++)
        {
            int u = it->first.first;
            int v = it->first.second;
            int e = it->second;

            //edge will be relaxed only if u is not at infinity distance from source
            if (dist_from_source[u] != 999999)
            {
                if (dist_from_source[v] > dist_from_source[u] + e)
                {
                    dist_from_source[v] = dist_from_source[u] + e;
                    flag = 1;
                }
            }
        }
    }

    //If flag is = 1 in the last iteration, negetive edge cycle exists
    return flag;
}

//Function which finds and returns iterator to the minimum element in the list
list<pair<int, int>>::iterator minEleList(list<pair<int, int>> &List)
{
    list<pair<int, int>>::iterator it, it_min;
    int min = List.begin()->first;
    it_min = List.begin();

    //need to loop for all vertices to find minimum
    for (it = List.begin(); it != List.end(); it++)
    {
        if (it->first < min)
        {
            it_min = it;
            min = it->first;
        }
    }
    return it_min;
}

//Function which finds and returns iterator to element with key as the second element of pair
list<pair<int, int>>::iterator findEleByKeyList(list<pair<int, int>> &List, int key)
{
    list<pair<int, int>>::iterator it;

    //need to loop for all verticies for finding
    for (it = List.begin(); it != List.end(); it++)
    {
        if (it->second == key)
        {
            break;
        }
    }
    return it;
}

//Array (list) based Dijkstra Implementation
void DijkstraList(vector<list<pair<int, int>>> &graph, int N, int source, vector<int> &dist_from_source)
{
    dist_from_source[source] = 0; //distance of source from source is 0

    //list of pair of integers :- first is the distance from source, second is the vertex label
    list<pair<int, int>> dijkstra_heap;

    vector<bool> already_processed(N + 1, false); //boolean vector to keep track of already processed nodes

    //push source into the heap
    dijkstra_heap.push_back(make_pair(0, source));

    //Push all other vertices except source with 999999 as distance
    for (int u = 1; u <= N; u++)
    {
        if (u != source)
        {
            dijkstra_heap.push_back(make_pair(999999, u));
        }
    }

    //Loop until the list isn't empty
    while (!dijkstra_heap.empty())
    {
        int u = minEleList(dijkstra_heap)->second;      //minimum element in list
        dijkstra_heap.erase(minEleList(dijkstra_heap)); //delete minimum element

        already_processed[u] = true; //u's distance from source will not change now

        list<pair<int, int>>::iterator it;
        //For all vertices adjacent to u
        for (it = graph[u].begin(); it != graph[u].end(); it++)
        {
            int v = it->first;
            int e = it->second;

            //If v is not already processed
            if (!already_processed[v])
            {
                //If the edge can be relaxed...
                if (dist_from_source[v] > dist_from_source[u] + e)
                {
                    //...relax the edge...
                    dist_from_source[v] = dist_from_source[u] + e;
                    //...find the corresponding pair in the list...
                    findEleByKeyList(dijkstra_heap, v)->first = dist_from_source[v]; //...and update the value in list
                }
            }
        }
    }
}

//Function to percolate down from index = 0 in binary heap
void heapPercolateDown(vector<pair<int, int>> &heap, int heap_size, vector<int> &index_in_heap)
{
    int i = 0;

    //Loop until last possible parent is reached
    while (i <= (heap_size / 2 - 1))
    {
        int left_index = 2 * i + 1;  //left child
        int right_index = 2 * i + 2; //right child

        int smaller_index; //index for smaller of left or right child
        //right child can be smaller only if it exists
        if (right_index < heap_size)
        {
            smaller_index = (heap[left_index].first < heap[right_index].first) ? left_index : right_index;
        }
        else
        {
            smaller_index = left_index;
        }

        //If heap order property is violated
        if (heap[i].first > heap[smaller_index].first)
        {
            //swap with smaller child
            pair<int, int> temp = heap[i];
            heap[i] = heap[smaller_index];
            heap[smaller_index] = temp;

            //update indexes for both
            index_in_heap[heap[i].second] = i;
            index_in_heap[heap[smaller_index].second] = smaller_index;

            //make i as the smaller index, and repeat
            i = smaller_index;
        }
        else
        {
            break;
        }
    }
}

//Function to percolate up from a given index in binary heap
void heapPercolateUp(vector<pair<int, int>> &heap, int heap_size, int first_index, vector<int> &index_in_heap)
{
    int i = first_index;

    //Loop until topmost node is reached
    while (i > 0)
    {
        int parent_index = (i - 1) / 2; //index of parent node

        //If heap order property is violated
        if (heap[i].first < heap[parent_index].first)
        {
            //swap th node and parent
            pair<int, int> temp = heap[i];
            heap[i] = heap[parent_index];
            heap[parent_index] = temp;

            //update indexes for both
            index_in_heap[heap[i].second] = i;
            index_in_heap[heap[parent_index].second] = parent_index;

            //make i as the parent index and repeat
            i = parent_index;
        }
        else
        {
            break;
        }
    }
}

//Binary Heap based Dijkstra implementation
void DijkstraBinaryHeap(vector<list<pair<int, int>>> &graph, int N, int source, vector<int> &dist_from_source)
{
    dist_from_source[source] = 0; //distance of source from source is 0

    //binary heap of pair of integers :- fist is the distance from source, second is the vertex label
    vector<pair<int, int>> dijkstra_heap;
    int heap_size = 0; //size of heap

    vector<bool> already_processed(N + 1, false); //boolean vector to keep track of already processed nodes
    vector<bool> in_heap(N + 1, false);           //boolean vector to strore if an element is in heap or not
    vector<int> index_in_heap(N + 1, -1);         //vector that stores index of nodes corresponding to each vertex in the heap

    //push source into the heap
    dijkstra_heap.push_back(make_pair(0, source));
    heap_size++;
    in_heap[source] = true;
    index_in_heap[source] = 0;

    //Loop until the heap isn't empty
    while (!dijkstra_heap.empty())
    {
        int u = dijkstra_heap.begin()->second; //minimum element in heap
        //remove u from heap
        in_heap[u] = false;
        index_in_heap[u] = -1;

        //copy last element to 0th position and update heap size
        dijkstra_heap[0] = dijkstra_heap[--heap_size];
        index_in_heap[dijkstra_heap[heap_size].second] = 0; //update it's index
        dijkstra_heap.pop_back();                           //remove last element

        //percolate down to maintain heap order property
        heapPercolateDown(dijkstra_heap, heap_size, index_in_heap);

        already_processed[u] = true; //u's distance from source will not change now

        list<pair<int, int>>::iterator it;
        //For all vertices adjacent to u
        for (it = graph[u].begin(); it != graph[u].end(); it++)
        {
            int v = it->first;
            int e = it->second;

            //If v is not already processed
            if (!already_processed[v])
            {
                //If the edge can be relaxed...
                if (dist_from_source[v] > dist_from_source[u] + e)
                {
                    //...relax the edge...
                    dist_from_source[v] = dist_from_source[u] + e;

                    //If v is not in heap
                    if (!in_heap[v])
                    {
                        //add v in heap and update size
                        dijkstra_heap.push_back(make_pair(dist_from_source[v], v));
                        in_heap[v] = true;
                        index_in_heap[v] = heap_size++;

                        //percolate up to maintain heap order property
                        heapPercolateUp(dijkstra_heap, heap_size, index_in_heap[v], index_in_heap);
                    }

                    //If v is already in heap
                    else
                    {
                        //update its value in heap
                        dijkstra_heap[index_in_heap[v]].first = dist_from_source[v];
                        //percolate up to maintain heap order property
                        heapPercolateUp(dijkstra_heap, heap_size, index_in_heap[v], index_in_heap);
                    }
                }
            }
        }
    }
}

//Function to perform union of two binomial heaps
binomial_heap_node *unionBinomialHeap(binomial_heap_node *heap1, binomial_heap_node *heap2)
{
    //If any of the heaps is NULL, return the other heap
    if (!heap1)
    {
        return heap2;
    }
    if (!heap2)
    {
        return heap1;
    }

    binomial_heap_node *new_heap = NULL;           //the combined heap
    binomial_heap_node *it1 = heap1, *it2 = heap2; //iterators for heap1 and heap2

    //Enter the node with lesser degree in new heap
    if (it1->degree <= it2->degree)
    {
        new_heap = it1;
        it1 = it1->sibling;
    }
    else
    {
        new_heap = it2;
        it2 = it2->sibling;
    }
    binomial_heap_node *new_it = new_heap;

    //use mergeSort type approach to enter nodes in the new heap (on the basis of degree)
    while (it1 != NULL && it2 != NULL)
    {
        if (it1->degree <= it2->degree)
        {
            new_it->sibling = it1;
            new_it = new_it->sibling;
            it1 = it1->sibling;
        }
        else
        {
            new_it->sibling = it2;
            new_it = new_it->sibling;
            it2 = it2->sibling;
        }
    }

    //Enter remaining elements in the new heap
    if (!it1)
    {
        while (it2 != NULL)
        {
            new_it->sibling = it2;
            new_it = new_it->sibling;
            it2 = it2->sibling;
        }
    }
    else if (!it2)
    {
        while (it1 != NULL)
        {
            new_it->sibling = it1;
            new_it = new_it->sibling;
            it1 = it1->sibling;
        }
    }

    binomial_heap_node *prev_x = NULL, *x = new_heap, *next_x = new_heap->sibling; //nodes for prev, x and next

    //Repeat until end of new_heap is not reached
    while (next_x != NULL)
    {
        //If x and next_x don't have same degree, move forward
        if (x->degree != next_x->degree)
        {
            prev_x = x;
            x = next_x;
            next_x = next_x->sibling;
        }

        //Otherwise
        else
        {
            //If next_next_x has the same degree, move forward
            if (next_x->sibling && next_x->degree == next_x->sibling->degree)
            {
                prev_x = x;
                x = next_x;
                next_x = next_x->sibling;
            }

            //If next_x should come below x
            else if (x->value < next_x->value)
            {
                x->sibling = next_x->sibling; //remove next_x from root list
                next_x->parent = x;
                next_x->sibling = x->child;
                x->child = next_x;
                x->degree++;         //increase degree of x
                next_x = x->sibling; //update next_x
            }

            //If x should come below next_x
            else
            {
                //If prev_x exists, remove x from list
                if (prev_x)
                {
                    prev_x->sibling = next_x;
                }
                //Else update new_heap pointer to next_x
                else
                {
                    new_heap = next_x;
                }

                x->parent = next_x;
                x->sibling = next_x->child;
                next_x->child = x;
                next_x->degree++; //increase degree of next_x
                //update x and next_x
                x = next_x;
                next_x = next_x->sibling;
            }
        }
    }

    return new_heap;
}

//Utility function to reverse a binomial heap node list
binomial_heap_node *reverseList(binomial_heap_node *list)
{
    if (!list)
    {
        return NULL;
    }
    if (!list->sibling)
    {
        return list;
    }
    binomial_heap_node *rev = reverseList(list->sibling);
    list->sibling->sibling = list;
    list->sibling = NULL;
    return rev;
}

//Function to extract minimum node from a binomial heap
binomial_heap_node *extractBinomialMin(binomial_heap_node *heap, int &key_of_min)
{
    binomial_heap_node *it = heap, *min_node = heap, *prev_min = NULL;
    binomial_heap_node *new_heap = heap;

    //Find the minimum value node
    while (it->sibling != NULL)
    {
        if (it->sibling->value < min_node->value)
        {
            min_node = it->sibling;
            prev_min = it;
        }
        it = it->sibling;
    }
    key_of_min = min_node->key;

    //If min value node is heap pointer itself
    if (!prev_min)
    {
        //If min_node is the only node in root list
        if (!min_node->sibling)
        {
            binomial_heap_node *min_node_heap = min_node->child;
            it = min_node_heap;

            //make parent of all nodes in child list as NULL
            while (it != NULL)
            {
                it->parent = NULL;
                it = it->sibling;
            }
            delete min_node;

            //return the reversed list (because child list is stored in decreasing order of degree)
            min_node_heap = reverseList(min_node_heap);

            return min_node_heap;
        }
        //Otherwise make new_heap as min_node sibling
        else
        {
            new_heap = min_node->sibling;
        }
    }

    //Otherwise remove min_node from root list
    else
    {
        prev_min->sibling = min_node->sibling;
    }

    binomial_heap_node *min_node_heap = min_node->child; //min node child list
    it = min_node_heap;

    //Make parent pointers as NULL for all nodes in child list
    while (it != NULL)
    {
        it->parent = NULL;
        it = it->sibling;
    }

    delete min_node;

    //If child is NULL, return the original heap (with removed min node)
    if (!min_node_heap)
    {
        return new_heap;
    }

    //reverse child list
    min_node_heap = reverseList(min_node_heap);

    //combine child list with root list
    new_heap = unionBinomialHeap(min_node_heap, new_heap);

    //return new_heap
    return new_heap;
}

//Function to percolate up in Binomial Heap
void binomialHeapPercolateUp(binomial_heap_node *node, vector<binomial_heap_node *> &address_in_heap)
{
    //Loop until root list is reached
    while (node->parent != NULL)
    {
        //If heap order is violated
        if (node->value < node->parent->value)
        {
            //swap value and key for both nodes
            int temp_value = node->value, temp_key = node->key;
            node->value = node->parent->value;
            node->key = node->parent->key;
            node->parent->value = temp_value;
            node->parent->key = temp_key;

            //update address for both keys
            address_in_heap[node->key] = node;
            address_in_heap[node->parent->key] = node->parent;

            //update node as node->parent
            node = node->parent;
        }
        else
        {
            break;
        }
    }
}

//Binomial Heap implementation of Dijkstra Algorithm
void DijkstraBinomialHeap(vector<list<pair<int, int>>> &graph, int N, int source, vector<int> &dist_from_source)
{
    dist_from_source[source] = 0; //distance of source from source is 0

    //heap of pair of integers :- fist is the distance from source, second is the vertex label
    binomial_heap_node *dijkstra_heap = NULL; //binomial heap node

    vector<bool> already_processed(N + 1, false); //boolean vector to keep track of already processed nodes
    vector<bool> in_heap(N + 1, false);           //boolean vector to strore if an element is in heap or not
    vector<binomial_heap_node *> address_in_heap; //vector that stores addresses of nodes corresponding to each vertex in the heap

    //Push NULL for addresses
    for (int i = 0; i <= N; i++)
    {
        address_in_heap.push_back(NULL);
    }

    //push source into the heap
    dijkstra_heap = new binomial_heap_node;
    dijkstra_heap->child = dijkstra_heap->parent = dijkstra_heap->sibling = NULL;
    dijkstra_heap->degree = dijkstra_heap->value = 0;
    dijkstra_heap->key = source;
    in_heap[source] = true;
    address_in_heap[source] = dijkstra_heap;

    //Loop until the heap isn't empty
    while (dijkstra_heap != NULL)
    {
        int u;
        dijkstra_heap = extractBinomialMin(dijkstra_heap, u); //extract minimum and remove from heap
        in_heap[u] = false;
        address_in_heap[u] = NULL;

        already_processed[u] = true; //u's distance from source will not change now

        list<pair<int, int>>::iterator it;
        //For all vertices adjacent to u
        for (it = graph[u].begin(); it != graph[u].end(); it++)
        {
            int v = it->first;
            int e = it->second;

            //If v is not already processed
            if (!already_processed[v])
            {
                //If the edge can be relaxed...
                if (dist_from_source[v] > dist_from_source[u] + e)
                {
                    //...relax the edge...
                    dist_from_source[v] = dist_from_source[u] + e;
                    //If v isn't in heap
                    if (!in_heap[v])
                    {
                        //construct new node for v
                        binomial_heap_node *node_v = new binomial_heap_node;
                        node_v->value = dist_from_source[v];
                        node_v->key = v;
                        node_v->degree = 0;
                        node_v->child = node_v->parent = node_v->sibling = NULL;
                        in_heap[v] = true;
                        address_in_heap[v] = node_v;

                        //If heap is empty, make node as heap
                        if (!dijkstra_heap)
                        {
                            dijkstra_heap = node_v;
                        }
                        //Else do union of node and heap
                        else
                        {
                            dijkstra_heap = unionBinomialHeap(node_v, dijkstra_heap);
                        }
                    }

                    //If v is already in heap
                    else
                    {
                        address_in_heap[v]->value = dist_from_source[v];              //change value of v in heap
                        binomialHeapPercolateUp(address_in_heap[v], address_in_heap); //percolate up to maintain order property
                    }
                }
            }
        }
    }
}

//Function to adjust the Fibonacci heap after decrease key
fibonacci_heap_node *decreaseFibonacci(fibonacci_heap_node *heap, fibonacci_heap_node *node)
{
    //If decrease key operation happened on a node in root list
    if (!node->parent)
    {
        //If node is minimum, return node
        if (node->value < heap->value)
        {
            return node;
        }
        //Else return heap unchanged
        else
        {
            return heap;
        }
    }

    //If order property is not broken, return heap unchanged
    if (node->parent->value <= node->value)
    {
        return heap;
    }

    //Node will be cut down, so its parent's child must not be node itself
    if (node->parent->child == node)
    {
        //If node is only child, parent's child becomes NULL
        if (node->left == node)
        {
            node->parent->child = NULL;
        }
        //Otherwise change child to another node in children list
        else
        {
            node->parent->child = node->left;
        }
    }

    fibonacci_heap_node *parent = node->parent;
    parent->rank--; //reduce parent's rank

    //remove node from children's list
    node->left->right = node->right;
    node->right->left = node->left;
    node->parent = NULL;

    //add node to root list
    node->right = heap->right;
    node->left = heap;
    heap->right->left = node;
    heap->right = node;
    node->marked = false; //make it unmarked

    //Update heap node if necessary
    if (node->value < heap->value)
    {
        heap = node;
    }

    //If parent is not marked, mark it and return the heap
    if (!parent->marked)
    {
        if (parent->parent)
        {
            parent->marked = true;
        }
        return heap;
    }

    //Otherwise
    else
    {
        node = parent;

        //Loop until parent is unmarked
        while (node->marked && node->parent)
        {
            //Adjust parent's child pointer as before
            if (node->parent->child == node)
            {
                if (node->left == node)
                {
                    node->parent->child = NULL;
                }
                else
                {
                    node->parent->child = node->left;
                }
            }

            parent = node->parent;
            parent->rank--; //reduce parent's rank

            //remove node from children's list
            node->left->right = node->right;
            node->right->left = node->left;
            node->parent = NULL;

            //enter node in root list
            node->right = heap->right;
            node->left = heap;
            heap->right->left = node;
            heap->right = node;
            node->marked = false; //make it unmarked
            node = parent;
        }

        //If parent isn't on root list, mark it
        if (node->parent)
        {
            node->marked = true;
        }

        //return heap
        return heap;
    }
}

//Function to extract min value node from Fibonacci Heap
fibonacci_heap_node *extractFibonacciMin(fibonacci_heap_node *heap, int size)
{
    //If heap is one single node, delete the node and return NULL
    if (!heap->child && heap->left == heap)
    {
        delete heap;
        return NULL;
    }

    //Remove heap from root list
    heap->right->left = heap->left;
    heap->left->right = heap->right;

    fibonacci_heap_node *del_node = heap;
    fibonacci_heap_node *child = del_node->child;

    //If root list only contained heap, make it NULL
    if (heap->left == heap)
    {
        heap = NULL;
    }
    //Otherwise change it (thius value isn't the min as of now)
    else
    {
        heap = heap->left;
    }

    delete del_node;

    fibonacci_heap_node *it, *next;

    //If child of deleted node exists
    if (child)
    {
        it = child;

        //make parents of all children as NULL
        do
        {
            it->parent = NULL;
            it = it->right;
        } while (it != child);

        //If heap exists, merge heap and children list
        if (heap)
        {
            fibonacci_heap_node *child_left = child->left, *heap_right = heap->right;
            child->left = heap;
            child_left->right = heap_right;
            heap->right = child;
            heap_right->left = child_left;
        }
        //If heap doesn't exist, make heap as child list
        else
        {
            heap = child;
        }
    }

    //Find min value node from root list and update heap pointer
    fibonacci_heap_node *min_node = heap;
    it = heap->right;
    while (it != heap)
    {
        if (it->value < min_node->value)
        {
            min_node = it;
        }
        it = it->right;
    }
    heap = min_node;

    vector<fibonacci_heap_node *> rank_array; //rank array storing pointers to nodes in root list
    //Fill rank array with NULL pointers
    for (int i = 0; i <= size; i++)
    {
        rank_array.push_back(NULL);
    }

    rank_array[heap->rank] = heap; //add heap pointer in rank array
    it = heap->right;

    //Loop for all nodes in root list
    while (it != heap)
    {
        //If rank position is empty, fill it and move to next
        if (!rank_array[it->rank])
        {
            rank_array[it->rank] = it;
            it = it->right;
        }
        //Otherwise
        else
        {
            fibonacci_heap_node *root_node = rank_array[it->rank];
            //If current node should be below root_node
            if (root_node->value <= it->value)
            {
                //remove current node from root list
                it->left->right = it->right;
                it->right->left = it->left;
                it->left = it->right = it;
                it->parent = root_node; //update its parent

                //If root_node has no child, make current node as its child
                if (!root_node->child)
                {
                    root_node->child = it;
                }
                //Otherwise add current node to child list
                else
                {
                    child = root_node->child;
                    it->right = child->right;
                    it->left = child;
                    child->right->left = it;
                    child->right = it;
                }

                rank_array[root_node->rank] = NULL; //remove from rank_array
                root_node->rank++;                  //increase rank
                it = root_node;                     //update it
            }

            //If root_node should be below current node
            else
            {
                //Same as above with root_node and it swapped
                root_node->left->right = root_node->right;
                root_node->right->left = root_node->left;
                root_node->left = root_node->right = root_node;
                root_node->parent = it;
                if (!it->child)
                {
                    it->child = root_node;
                }
                else
                {
                    child = it->child;
                    root_node->right = child->right;
                    root_node->left = child;
                    child->right->left = root_node;
                    child->right = root_node;
                }
                rank_array[root_node->rank] = NULL;
                it->rank++;
            }
        }
    }

    return heap;
}

//Fibonacci Heap based implementation of Dijkstra Algorithm
void DijkstraFibonacciHeap(vector<list<pair<int, int>>> &graph, int N, int source, vector<int> &dist_from_source)
{
    dist_from_source[source] = 0; //distance of source from source is 0

    //heap of pair of integers :- first is the distance from source, second is the vertex label
    fibonacci_heap_node *dijkstra_heap = NULL;
    int heap_size = 0; //size of heap

    vector<bool> already_processed(N + 1, false);  //boolean vector to keep track of already processed nodes
    vector<bool> in_heap(N + 1, false);            //boolean vector to store if an element is in heap or not
    vector<fibonacci_heap_node *> address_in_heap; //vector that stores addresses of nodes corresponding to each vertex in the heap

    //Push NULL for addresses
    for (int i = 0; i <= N; i++)
    {
        address_in_heap.push_back(NULL);
    }

    //push source into the heap
    dijkstra_heap = new fibonacci_heap_node;
    dijkstra_heap->child = dijkstra_heap->parent = NULL;
    dijkstra_heap->left = dijkstra_heap->right = dijkstra_heap;
    dijkstra_heap->rank = dijkstra_heap->value = 0;
    dijkstra_heap->key = source;
    dijkstra_heap->marked = false;
    heap_size++;
    in_heap[source] = true;
    address_in_heap[source] = dijkstra_heap;

    //Loop until the heap isn't empty
    while (dijkstra_heap != NULL)
    {
        int u = dijkstra_heap->key;
        dijkstra_heap = extractFibonacciMin(dijkstra_heap, heap_size); //extract and delete min node
        heap_size--;                                                   //update heap size
        in_heap[u] = false;
        address_in_heap[u] = NULL;

        already_processed[u] = true; //u's distance from source will not change now

        list<pair<int, int>>::iterator it;
        //For all vertices adjacent to u
        for (it = graph[u].begin(); it != graph[u].end(); it++)
        {
            int v = it->first;
            int e = it->second;

            //If v is not already processed
            if (!already_processed[v])
            {
                //If the edge can be relaxed...
                if (dist_from_source[v] > dist_from_source[u] + e)
                {
                    //...relax the edge...
                    dist_from_source[v] = dist_from_source[u] + e;

                    //If v is not in heap
                    if (!in_heap[v])
                    {
                        //make new node corresponding to v
                        fibonacci_heap_node *node_v = new fibonacci_heap_node;
                        node_v->value = dist_from_source[v];
                        node_v->key = v;
                        node_v->rank = 0;
                        node_v->marked = false;
                        node_v->child = node_v->parent = NULL;
                        node_v->left = node_v->right = node_v;
                        in_heap[v] = true;
                        address_in_heap[v] = node_v;
                        //If heap is empty, make heap as node for v
                        if (!dijkstra_heap)
                        {
                            dijkstra_heap = node_v;
                            heap_size++;
                        }
                        //Otherwise enter v in root list (update heap pointer)
                        else
                        {
                            node_v->right = dijkstra_heap->right;
                            node_v->left = dijkstra_heap;
                            dijkstra_heap->right->left = node_v;
                            dijkstra_heap->right = node_v;
                            if (node_v->value < dijkstra_heap->value)
                            {
                                dijkstra_heap = node_v;
                            }
                            heap_size++;
                        }
                    }

                    //If v is in heap
                    else
                    {
                        address_in_heap[v]->value = dist_from_source[v];                      //update value
                        dijkstra_heap = decreaseFibonacci(dijkstra_heap, address_in_heap[v]); //adjust for decrease key operation
                    }
                }
            }
        }
    }
}

//Main Function
int main(int argc, char **argv)
{
    int testcases;
    cin >> testcases;
    vector<double> execution_times; //vector to store execution times for each test case
    while (testcases--)
    {
        int N, D;
        cin >> N >> D;

        vector<list<pair<int, int>>> graph; //list of pairs because the adjacency list will store adjacent vertex and the edge weight
        list<pair<int, int>> node_list;     //adjacency list for each node

        //enter empty list to avoid 0 indexing
        graph.push_back(node_list);

        int temp; //used for taking in inputs
        for (int i = 1; i <= N; i++)
        {
            for (int j = 1; j <= N; j++)
            {
                cin >> temp;

                //node exists only if weight != inf, or i != j
                if ((temp != 999999) && (i != j))
                {
                    node_list.push_back(make_pair(j, temp));
                }
            }

            graph.push_back(node_list);
            node_list.clear();
        }

        clock_t start, end; //time variables for measuring execution times
        start = clock();

        //make zero node graph
        vector<list<pair<int, int>>> zero_node_graph = graph;

        //zero node has edge to every vertex
        for (int i = 1; i <= N; i++)
        {
            node_list.push_back(make_pair(i, 0));
        }

        //enter the zero node at last (in our case, zero node has an index N + 1 to keep indexing from 1)
        zero_node_graph.push_back(node_list);

        vector<int> dist_from_zero(N + 2, 999999); //distance from zero vector (will be updated by Bellman Ford)

        //Bellman Ford with source as zero vertex
        int neg_cycle = BellmanFord(zero_node_graph, N + 1, N + 1, dist_from_zero);

        //If negetive cycle exists, continue to next iteration
        if (neg_cycle)
        {
            end = clock();
            execution_times.push_back((double)(end - start) / CLOCKS_PER_SEC);
            cout << -1 << endl;
            continue;
        }

        //make updated graph with positive edge weights only
        vector<list<pair<int, int>>> positive_edge_graph = graph;

        //update edge weights according to distances from zero vertex calculated by Bellman Ford
        for (int u = 1; u <= N; u++)
        {
            list<pair<int, int>>::iterator it;
            for (it = positive_edge_graph[u].begin(); it != positive_edge_graph[u].end(); it++)
            {
                int v = it->first;
                it->second = it->second + dist_from_zero[u] - dist_from_zero[v];
            }
        }

        vector<vector<int>> apsp; //2D vector for storing All Pair Shortest Path values
        vector<int> empty_vector;

        apsp.push_back(empty_vector);

        for (int u = 1; u <= N; u++)
        {
            vector<int> dist_from_vertex(N + 1, 999999);

            //If there is a command line argument
            if (argc > 1)
            {
                char heap_choice = argv[1][0];
                switch (heap_choice)
                {
                case '1': //Array
                    DijkstraList(positive_edge_graph, N, u, dist_from_vertex);
                    break;

                case '2': //Binary Heap
                    DijkstraBinaryHeap(positive_edge_graph, N, u, dist_from_vertex);
                    break;

                case '3': //Binomial Heap
                    DijkstraBinomialHeap(positive_edge_graph, N, u, dist_from_vertex);
                    break;

                case '4': //Fibonacci Heap
                    DijkstraFibonacciHeap(positive_edge_graph, N, u, dist_from_vertex);
                    break;

                default: //Fibonacci Heap
                    DijkstraFibonacciHeap(positive_edge_graph, N, u, dist_from_vertex);
                    break;
                }
            }
            //If there are no arguments, use Fibonacci Heap
            else
            {
                DijkstraFibonacciHeap(positive_edge_graph, N, u, dist_from_vertex);
            }

            //Adjust values back by removing the "positive edge only" effect
            for (int v = 1; v <= N; v++)
            {
                if (dist_from_vertex[v] < 999999)
                {
                    dist_from_vertex[v] = dist_from_vertex[v] + dist_from_zero[v] - dist_from_zero[u];
                }
            }

            apsp.push_back(dist_from_vertex);
        }

        //Push execution time in list
        end = clock();
        execution_times.push_back((double)(end - start) / CLOCKS_PER_SEC);

        //Print All Pair Shortest Path
        for (int u = 1; u <= N; u++)
        {
            for (int v = 1; v <= N; v++)
            {
                cout << apsp[u][v] << " ";
            }
            cout << endl;
        }
    }

    //Print execution times
    vector<double>::iterator it;
    for (it = execution_times.begin(); it != execution_times.end(); it++)
    {
        cout << *it << setprecision(5) << " ";
    }

    return 0;
}