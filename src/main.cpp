#include "rb_tree.hpp"
#include <iostream>
#include <memory>
#include <string>
#include "buffer.hpp"

template <typename T>
void printTree(const RBNode<T>* node, const std::string& prefix = "", bool isLeft = true) {
    if (!node) return;

    printTree(node->right.get(), prefix + (isLeft ? "│   " : "    "), false);

    std::cout << prefix;
    std::cout << (isLeft ? "└── " : "┌── ");
    std::cout << node->data << " (" << (node->color == Color::Red ? "R" : "B") << ")" << std::endl;

    printTree(node->left.get(), prefix + (isLeft ? "    " : "│   "), true);
}



int main() {
    RedBlackTree<int> tree;
    tree.insert(15);
    tree.insert(17);
    tree.insert(13);
    tree.insert(14);    
    tree.insert(9); 
    tree.insert(12);
    tree.insert(10); 
    tree.insert(16);

    tree.remove(14);

    std::cout << "Исходное дерево: " << std::endl;
    printTree(tree.getRoot());
    tree.inorder([](const int& x){ std::cout << x << " "; });
    std::cout << std::endl;
    std::cout << "Проверка дерева: " << (tree.validate() == 0 ? "Успешно" : "Несбалансированое") << std::endl << std::endl;
    

    // tree.saveToFile("tree.bin");

    // RedBlackTree<int> newTree;
    // newTree.loadFromFile("tree.bin");

    // std::cout << "Дерево после loadFromFile: ";
    // std::cout << std::endl;
    // printTree(newTree.getRoot());
    // newTree.inorder([](const int& x){ std::cout << x << " "; });
    // std::cout << std::endl;
    // std::cout << "Проверка дерева: " << (newTree.validate() == 0 ? "Успешно" : "Несбалансированое") << std::endl << std::endl;
}
