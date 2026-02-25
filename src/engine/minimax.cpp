#include <iostream>

struct TreeNode
{
    int val;
    TreeNode *left = nullptr, *right = nullptr;
    TreeNode(int v) : val(v) {}
};

// Util and main

int is_terminal_node(TreeNode *node)
{
    return node->left == nullptr && node->right == nullptr;
}

int minimax(int depth, TreeNode *node, int maximizing_player)
{
    if (is_terminal_node(node) || depth == 0)
    {
        return node->val;
    }
    if (maximizing_player)
    {
        int max_eval = -1000;
        max_eval = std::max(max_eval, minimax(depth - 1, node->left, 0));
        max_eval = std::max(max_eval, minimax(depth - 1, node->right, 0));
        return max_eval;
    }
    else
    {
        int min_eval = 1000;
        min_eval = std::min(min_eval, minimax(depth - 1, node->left, 1));
        min_eval = std::min(min_eval, minimax(depth - 1, node->right, 1));
        return min_eval;
    }
}

TreeNode *random_tree(int depth)
{
    if (depth == 0)
        return nullptr;
    TreeNode *node = new TreeNode(rand() % 100);
    node->left = random_tree(depth - 1);
    node->right = random_tree(depth - 1);
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
    std::cout << node->val << std::endl;
    std::string childPrefix = prefix + (isLeft ? "│   " : "    ");
    if (node->left || node->right)
    {
        if (node->right)
            print_tree(node->right, childPrefix, false);
        if (node->left)
            print_tree(node->left, childPrefix, true);
    }
}

int main()
{
    srand(time(0));
    int height = 3;
    TreeNode *tree = random_tree(height);
    int result = minimax(height, tree, 1);
    print_tree(tree);
    std::cout << "Minimax result: " << result << std::endl;
    return 0;
}