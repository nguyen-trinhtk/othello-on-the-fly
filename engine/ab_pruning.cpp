#include <iostream>
#include <vector>

struct TreeNode
{
    int val;
    std::vector<TreeNode *> children; // Can hold any number of children
    bool visited = false;             // Mark if node was visited by ab_pruning
    TreeNode(int v) : val(v) {}
};

// Util and main

int is_terminal_node(TreeNode *node)
{
    return node->children.empty();
}

int ab_pruning(TreeNode *node, int depth, int alpha, int beta, int maximizing_player)
{
    if (node == nullptr || is_terminal_node(node) || depth == 0)
    {
        if (node)
            node->visited = true;
        return node ? node->val : 0;
    }
    node->visited = true;
    if (maximizing_player)
    {
        int max_eval = -1000;
        for (auto child : node->children)
        {
            if (child)
            {
                int eval = ab_pruning(child, depth - 1, alpha, beta, 0);
                max_eval = std::max(max_eval, eval);
                alpha = std::max(alpha, eval);
                if (beta <= alpha)
                    break; // Pruning
            }
        }
        return max_eval;
    }
    else
    {
        int min_eval = 1000;
        for (auto child : node->children)
        {
            if (child)
            {
                int eval = ab_pruning(child, depth - 1, alpha, beta, 1);
                min_eval = std::min(min_eval, eval);
                beta = std::min(beta, eval);
                if (beta <= alpha)
                    break; // Pruning
            }
        }
        return min_eval;
    }
}

TreeNode *random_tree(int depth)
{
    if (depth == 0)
        return nullptr;
    TreeNode *node = new TreeNode(rand() % 100);
    int num_children = rand() % (depth + 1); // up to 'depth' children
    for (int i = 0; i < num_children; ++i)
    {
        TreeNode *child = random_tree(depth - 1);
        if (child)
            node->children.push_back(child);
    }
    return node;
}

void print_tree(TreeNode *node, std::string prefix = "", bool isLeft = true)
{
    if (node == nullptr)
        return;
    if (!prefix.empty())
    {
        std::cout << prefix;
        std::cout << (isLeft ? "├── " : "└── ");
    }
    if (node->visited)
        std::cout << node->val << std::endl;
    else
        std::cout << "X" << std::endl; // Mark pruned nodes as 'X'
    std::string childPrefix = prefix + (isLeft ? "│   " : "    ");
    for (auto child : node->children)
    {
        if (child)
            print_tree(child, childPrefix, true);
    }
}

int main()
{
    srand(time(0));
    int height = 5;
    TreeNode *tree = random_tree(height);
    int result = ab_pruning(tree, height, -1000, 1000, 1);
    print_tree(tree);
    std::cout << "Alpha-Beta pruning result: " << result << std::endl;
    return 0;
}