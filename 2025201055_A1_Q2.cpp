#include <iostream>
#include <vector>
#include <algorithm>
#include <cstring>
using namespace std;

class trie
{
public:
    // constructor
    trie() : root(new Node()) {}
    ~trie() { free(root); }

    void insert(const string &s)
    {
        Node *curr = root;
        for (char c : s)
        {
            int index = c - 'a';
            if (index < 0 || index >= 26)
                continue; // skip invalid characters
            if (!curr->children[index])
                curr->children[index] = new Node();
            curr = curr->children[index];
        }
        curr->isterminal = true; // mark the end of the string
    }

    // check the dictionary has the word or not
    bool containsornot(const string &s) const
    {
        const Node *node = walk(s);
        return node && node->isterminal; // check the word ends or not and if ends then its terminal or not
    }

    // now we will collect all the words having same prefix and put into a vector named "out"
    void autocompletefromprefix(const string &prefix, vector<string> &out) const
    {
        const Node *start = walk(prefix);
        if (!start)
            return; // if the prefix is not found, return
        string buffer = prefix;
        collectwithdfs(start, buffer, out); // it will visit all the words with the same prefix as in the buffer and place all in out vector
    }

    void autocorrect(const string &word, int maximumdist, vector<string> &out)
    {
        const int m = static_cast<int>(word.size());
        vector<int> baserow(m + 1);
        for (int i = 0; i <= m; ++i)
            baserow[i] = i; // initialize the base row for Levenshtein distance
        string path;
        // now we wiil explore the child

        for (int c = 0; c < 26; c++)
        {
            if (!root->children[c])
                continue;       // if the child is not present, continue
            char ch = 'a' + c;  // get the character from the index
            path.push_back(ch); // add the character to the path
            dfsdp(root->children[c], ch, word, baserow, path, maximumdist, out);
            // call the dfsdp function to explore the child
            path.pop_back(); // remove the character from the path
        }
    }

private:
    struct Node
    {
        Node() : isterminal(false)
        {
            memset(children, 0, sizeof(children)); // initialize the children array
        }
        bool isterminal;    // flag to check if the node is terminal i.e the end of a word
        Node *children[26]; // array of children nodes for each character a-z
    };
    Node *root; // root node of the trie

    // its only job is to walk that path and tell you where it ends up.
    const Node *walk(const string &s) const
    {
        const Node *curr = root;
        for (char c : s)
        {
            int index = c - 'a';          // get the index of the character
            if (index < 0 || index >= 26) // if the character is not valid or the child is not present in the child
                return nullptr;           // return null
            curr = curr->children[index]; // move to the child node
            if (!curr)                    // if the no child is  present
                return nullptr;           // return null
        }
        return curr;
    }

    // now to find all words with the prefix recursively and put inside out buffer
    void collectwithdfs(const Node *node, string &buffer, vector<string> &out) const
    {
        if (node->isterminal) // if it is terminal add the buffer to the output vector
        {
            out.push_back(buffer);
        }
        for (int i = 0; i < 26; ++i) // iterate through the children
        {
            if (!node->children[i])
                continue; // if the child is not present, continue

            buffer.push_back(static_cast<char>('a' + i)); // add the character to the buffer
            collectwithdfs(node->children[i], buffer, out);
            buffer.pop_back(); // remove the character from the buffer
        }
    }

    void dfsdp(
        const Node *node,
        char ch,
        const string &target,
        const vector<int> &prev,
        string &path,
        int maximumdist,
        vector<string> &out) const
    {
        int m = static_cast<int>(target.size());
        vector<int> curr(m + 1);
        curr[0] = prev[0] + 1;    // initialize the first element of the current row
        int rowminimum = curr[0]; // initialize the row minimum
        for (int j = 1; j <= m; ++j)
        {
            int costinsertion = curr[j - 1] + 1;                                         // cost of insertion
            int costdeletion = prev[j] + 1;                                              // cost of deletion
            int costreplace = prev[j - 1] + (ch == target[j - 1] ? 0 : 1);               // cost of replacement
            int bestvalue = costinsertion < costdeletion ? costinsertion : costdeletion; // find the best value
            if (costreplace < bestvalue)
                bestvalue = costreplace; // update the best value if replacement is better
            curr[j] = bestvalue;         // update the current row
            if (curr[j] < rowminimum)
                rowminimum = curr[j]; // update the row minimum
        }

        // if the edit distance is within the limit
        if (node->isterminal && curr[m] <= maximumdist)
        {
            out.push_back(path); // add the path to the output vector
        }

        // if it exceed maxdist then return
        if (rowminimum > maximumdist)
            return; // if the row minimum exceeds the maximum distance, return

        // now to keep the outputs lexiographically

        for (int c = 0; c < 26; ++c)
        {
            if (!node->children[c])
                continue;                                                           // if the child is not present, continue
            char nextch = 'a' + c;                                                  // get the character from the index
            path.push_back(nextch);                                                 // add the character to the path
            dfsdp(node->children[c], nextch, target, curr, path, maximumdist, out); // call the dfsdp function to explore the child
            path.pop_back();                                                        // remove the character from the path
        }
    }

    // free nodes
    void free(Node *node)
    {
        if (!node)
            return;                  // if the node is null, return
        for (int i = 0; i < 26; ++i) // iterate through the children
        {
            if (node->children[i])
                free(node->children[i]); // recursively free the child nodes
        }
        delete node; // delete the current node
    }
};

int main()
{
    int n, q;      // n is the number of queries and q is the number of queries
    cin >> n >> q; // read the number of queries
    trie t;        // create the trie obj
    string s;

    for (int i = 0; i < n; ++i)
    {
        cin >> s;    // read the string
        t.insert(s); // insert the string into the trie
    }

    for (int i = 0; i < q; i++)
    {
        int type;
        string word;
        cin >> type >> word;

        if (type == 1)
        {
            // check spelling
            cout << (t.containsornot(word) ? 1 : 0) << endl; // check if the word is present in the trie
        }

        else if (type == 2)
        {
            // autocomplete
            vector<string> out;               // vector to store the output
            t.autocompletefromprefix(word, out); // get the words with the same prefix
            cout << out.size() << endl;       // print the size of the output
            for (const auto &word : out)
                cout << word << endl; // print the words with the same prefix
        }
        else if (type == 3)
        {
            // autocorrect where editdist<=3
            vector<string> out;          // vector to store the output
            t.autocorrect(word, 3, out); // get the words with the same prefix
            cout << out.size() << endl;  // print the size of the output
            for (const auto &word : out)
                cout << word << endl; // print the words with the same prefix
        }
    }
    return 0;
}