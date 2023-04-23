#include <bits/stdc++.h>

int k = 3;

struct Lesson{
    int classroom;
    std::string surname;
    std::string name;
    std::string lastname;
    int hour;
    int minutes;
    std::string subject;
};

struct AhoTrie {
    // An one-use trie for the Aho-Corasick algorithm. Need to reset count to zero for all nodes to reuse
    struct Node {
        Node* link = nullptr;
        std::vector<int> terminals;
        std::map<char, Node*> arcs;

        int count = 0;  // How many times reached in string
    };

    std::deque<Node> nodes;  // We use deque to prevent pointer invalidation when capacity is exceeded
    Node* root;
    int num_terminals;
    std::vector<Node*> terminal_nodes;

    AhoTrie() {
        nodes.resize(1);
        root = &nodes[0];
    }

    void init(std::vector<std::string>& strs) {
        // First initialize the Trie
        num_terminals = strs.size();
        for(int i=0; i<(int)strs.size(); i++) {
            std::string &str = strs[i];
            Node* cur_node = root;
            for(char c : str) {  // Add the string to the trie
                if(cur_node->arcs.count(c) == 0) {
                    nodes.push_back(Node());
                    cur_node->arcs[c] = &nodes.back();
                }
                cur_node = cur_node->arcs[c];
            }
            cur_node->terminals.push_back(i);
            terminal_nodes.push_back(cur_node);  // Mark the node reached by this string as terminal
        }

        // Next initialize the links with BFS
        std::vector<Node*> front;
        for(std::pair<char, Node*> cp : root->arcs) {  // Process root separately
            cp.second->link = root;
            front.push_back(cp.second); // Initialize the queue
        }
        for(int i=0; i<(int)front.size(); i++) {
            Node* cur_node = front[i];
            for(std::pair<char, Node*> cp : cur_node->arcs) {  // Iterate over all arcs
                char c = cp.first;
                Node* next = cp.second;
                Node* link = cur_node->link;

                while(link != 0 && link->arcs.count(c) == 0) {  // While we can't add c to suffix
                    link = link->link;  // Move to smaller suffix
                }
                if(link == 0) link = root;  // Character not matched
                else link = link->arcs[c];

                next->link = link;
                front.push_back(next);
            }
        }
    }

    std::vector<int> process_string(std::string s) {
        Node* cur_node = root;
        for(char c : s) {
            while(cur_node != 0 && cur_node->arcs.count(c) == 0) {  // If no arc allows for this character
                cur_node = cur_node->link;  // Move to smaller suffix
            }
            if(cur_node == 0) cur_node = root;  // Character not matched
            else cur_node = cur_node->arcs[c];

            cur_node->count++;
        }

        std::vector<Node*> front(1, root);  // First do BFS to find a depth-based ordering of nodes
        for(int i=0; i<(int)front.size(); i++) {
            for(std::pair<char, Node*> cp : front[i]->arcs) {
                front.push_back(cp.second);
            }
        }
        for(int i=(int)front.size()-1; i>0; i--) {  // Traverse the trie in reverse order of depth
            if(front[i]->link != 0) {
                // If a string is matched, then so are all its suffixes
                front[i]->link->count += front[i]->count;  // Send the counts up the links
            }
        }

        std::vector<int> result(num_terminals, 0);  // Go through the 'dictionary', count occurrences
        for(int i=0; i<num_terminals; i++) {
            result[i] = terminal_nodes[i]->count;
            terminal_nodes[i]->count = 0;
        }

        return result;
    }
};

int main() {

    std::vector<std::string> dict = {
            "ab",
            "ba",
            "bb"
    };
    AhoTrie trie;
    trie.init(dict);

//    std::vector<int> result = trie.process_string("abbba");
//    for (auto i : result)std::cout << i << ' ';

    std::ifstream input("lessons.txt", std::ios::in);

    if (!input.is_open()) {
        std::cout << "File in couldn't be opened.";
        return 0;
    }

    while (!input.eof()){
        int classroom, minute, hour;
        std::string surname, name, lastname, subject;

        input >> classroom >> surname >> name >>
        lastname >> hour >> minute >> subject;

        auto resulting = {surname, name, lastname, subject};
        for (auto i: resulting){
            int pattern_number = 0;
            AhoTrie trie1 = trie;
            std::vector<int>result = trie1.process_string(i);
            for (auto j: result){
                if (j >  0) pattern_number++;
            }

            if (pattern_number >= k){
                std::cout << classroom << ' ' << surname << ' '  << name  << ' ' << lastname << ' ' <<
                          hour << ' ' << minute << ' ' << subject << std::endl;
                break;
            }

        }
    }
    input.close();
}
