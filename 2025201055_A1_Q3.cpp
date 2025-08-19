#include <iostream>
#include <cstdio> // for printf and scanf

using namespace std;
struct coordinates
{
    int type;                      // 0 = update, 1 = print
    long long left, right, height; // the coordinates of the banner
};
// now we will declare the global constants
const int maxq = 100000;          // as there can be at most these much of queries
static coordinates queries[maxq]; // this will store the queries
long long coord[2 * maxq];        // this will store the coordinates of the banners before sorting and reemoving duplicates
long long uniqcoord[2 * maxq];    // this will store the unique coordinates after sorting and removing duplicates
int q;                            // num of queries from the input
int coordcount;                   // keep the track of how many coordinates we have added to coord so far
// arrays for the segment tree
static long long minheight[8 * maxq]; // this will store min height in the strip
static long long maxheight[8 * maxq]; // this will store max height in the strip
static long long lazytag[8 * maxq];   // this is the lazy tag in segment tree updation
static int M;                         // it stores num of unique coordinates

// the mergesort function to sort the coordinates
void mergesort(long long *arr, int left, int right, long long *temp)
{

    if (left >= right)
        return;                           // base case
    int mid = (left + right) / 2;         // find the mid point
    mergesort(arr, left, mid, temp);      // sort the left half
    mergesort(arr, mid + 1, right, temp); // sort the right half
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right)
    {
        if (arr[i] <= arr[j])
            temp[k++] = arr[i++];
        else
            temp[k++] = arr[j++];
    }
    while (i <= mid)
        temp[k++] = arr[i++];
    while (j <= right)
        temp[k++] = arr[j++];
    for (i = left; i <= right; i++)
        arr[i] = temp[i]; // copy to the orginal array after arr is sorted
}
// to map large indexes to small index & help to find the index of x in the unique sorted coordinates

int tofindthecompressedcoord(long long x)
{
    int left = 0, right = M - 1;
    while (left <= right)
    {
        int mid = (left + right) / 2;
        if (uniqcoord[mid] == x)
            return mid; // found the index
        else if (uniqcoord[mid] < x)
            left = mid + 1; // search in the right half
        else
            right = mid - 1; // search in the left half
    }
    return -1;
}

// segment tree  functions

inline void applylazytag(int node, long long value)
{
    minheight[node] = maxheight[node] = value; // add the lazy tag to the min  , max height
    lazytag[node] = value;                     // update the lazy tag
}

// to push down the lazy tag to the children nodes
inline void pushdown(int node)
{
    if (lazytag[node] != -1) // if there is a lazy
    {
        applylazytag(2 * node, lazytag[node]);
        applylazytag(2 * node + 1, lazytag[node]);
        lazytag[node] = -1; // reset the lazy tag
    }
}
// update the parents min and max height after updating the children nodes
inline void pullup(int node)
{
    minheight[node] = minheight[2 * node] < minheight[2 * node + 1] ? minheight[2 * node] : minheight[2 * node + 1];
    maxheight[node] = maxheight[2 * node] > maxheight[2 * node + 1] ? maxheight[2 * node] : maxheight[2 * node + 1];
}

void build(int node, int start, int end)
{
    minheight[node] = 0; // initialize the min height to 0
    maxheight[node] = 0; // initialize the max height to 0
    lazytag[node] = -1;  // initialize the lazy tag to -1
    if (start == end)
        return;                        // base case
    int mid = (start + end) / 2;       // find the mid point
    build(2 * node, start, mid);       // build the left child
    build(2 * node + 1, mid + 1, end); // build the right child
}

void rangechangeheight(int node, int start, int end, int queryleft, int queryright, long long height)
{
    if (queryleft > end || queryright < start)
        return;                                  // no overlap i.e  if the query range is outside the range of the node return
    if (queryleft <= start && queryright >= end) // if the query range is completely inside the range of the node
    {
        applylazytag(node, height); // apply the lazy tag
        return;
    }
    if (maxheight[node] >= height)
        return; // already high , no need to update
    if (start == end)
    {
        if (maxheight[node] < height) // if the max height is less than the height
        {
            applylazytag(node, height); // apply the lazy tag
        }
    }

    pushdown(node);                                                               // push down the lazy tag to the children nodes
    int mid = (start + end) / 2;                                                  // find the mid point
    rangechangeheight(2 * node, start, mid, queryleft, queryright, height);       // update the left child
    rangechangeheight(2 * node + 1, mid + 1, end, queryleft, queryright, height); // update the right child
    pullup(node);                                                                 // update the parents min and max height
}

void collect(int node, int left, int right, long long HEIGHT[])
{
    if (lazytag[node] != -1) // if there is a lazy tag
    {
        for (int i = left; i <= right; i++)
        {
            HEIGHT[i] = lazytag[node]; // update the height
        }
        return;
    }
    if (left == right) // if the left and right are same
    {
        HEIGHT[left] = maxheight[node]; // update the height
        return;
    }
    int mid = (left + right) / 2;                  // find the mid point
    collect(2 * node, left, mid, HEIGHT);          // collect the left child
    collect(2 * node + 1, mid + 1, right, HEIGHT); // collect the right child
}

int main()
{

    cin >> q;       // read the number of queries
    coordcount = 0; // initialize the coordcount

    for (int i = 0; i < q; i++)
    {
        int type;
        cin >> type;            // read the type of query
        queries[i].type = type; // store the type of query
        if (type == 0)          // if the query is update
        {
            long long left, right, height;
            cin >> left >> right >> height; // read the coordinates and height
            queries[i].left = left;         // store the left coordinate
            queries[i].right = right;       // store the right coordinate
            queries[i].height = height;     // store the height
            coord[coordcount++] = left;     // add the left coordinate to the coord array
            coord[coordcount++] = right;    // add the right coordinate to the coord array
        }
    }
    // cout << "Finished reading input" << endl;
    // cout << "Total queries = " << q << endl;
    // for (int i = 0; i < q; i++)
    // {
    //     cout << "Query " << i << ": left=" << queries[i].left
    //          << " right=" << queries[i].right
    //          << " height=" << queries[i].height << endl;
    // }

    // now we will do the coordinate compression
    static long long temp[2 * maxq];           // temporary array for mergesort sorting
    mergesort(coord, 0, coordcount - 1, temp); // sort the coordinates
    M = 0;                                     // initialize the M
    for (int i = 0; i < coordcount; i++)
    {
        if (i == 0 || coord[i] != coord[i - 1]) // check to remove duplicates
        {
            uniqcoord[M++] = coord[i]; // add the  unique coordinate
        }
    }

    if (M < 2)
    {
        // that means there is no interval
        return 0; // no need to do anything
    }

    // build the segment tree
    build(1, 0, M - 2);
    // now we will process the queries
    for (int i = 0; i < q; i++)
    {
        if (queries[i].type == 0) // if the query is update
        {
            int left = tofindthecompressedcoord(queries[i].left);   // find the index of the left coordinate
            int right = tofindthecompressedcoord(queries[i].right); // find the index of the right coordinate
            if (left < right)                                       // if the left coordinate is less than the right coordinate
            {
                rangechangeheight(1, 0, M - 2, left, right - 1, queries[i].height); // update the segment tree
            }
        }

        else // query for the print when type is 1
        {
            static long long HEIGHT[2 * maxq]; // temporary array to store the heights
            collect(1, 0, M - 2, HEIGHT);      // collect the heights
            long long previousheight = 0;      // initialize the previous height
            for (int j = 0; j < M - 1; j++)
            {
                if (j == 0 && HEIGHT[j] > 0)
                {
                    cout << uniqcoord[0] << " " << HEIGHT[j] << endl; // print the first coordinate and height
                }
                else if (HEIGHT[j] != previousheight)
                {
                    cout << uniqcoord[j] << " " << HEIGHT[j] << endl; // print the coordinate and height
                }
                previousheight = HEIGHT[j]; // update the previous  height
            }
            if (previousheight > 0) // if the previous height is greater than 0
            {
                // cout << uniqcoord[M - 1] << " " << previousheight << endl; // print the last coordinate and height
                cout << uniqcoord[M - 1] << " " << 0 << endl; // print the last coordinate with height 0
            }
        }
    }

    return 0;
}
