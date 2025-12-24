#pragma once
#include "rb_node.hpp"
#include "buffer.hpp"
#include <memory>  
#include <functional>
#include <fstream>

template <typename T>
class RedBlackTree {
public:
    RedBlackTree() : root(nullptr) {}
    ~RedBlackTree() = default;

    int blackHeight(RBNode<T>* node) {
        if (!node) return 1;

        int leftBH  = blackHeight(node->left.get());
        int rightBH = blackHeight(node->right.get());

        if (leftBH != rightBH) {
            return -1;
        }
        return leftBH + (node->color == Color::Black ? 1 : 0);
    }

    int checkRedRule(RBNode<T>* node) {
        if (!node) return 0;

        if (node->color == Color::Red) {
            if ((node->left  && node->left->color  == Color::Red) ||
                (node->right && node->right->color == Color::Red)) {
                return -1;
            }
        }
        int left = checkRedRule(node->left.get());
        if (left == -1) return -1;

        int right = checkRedRule(node->right.get());
        if (right == -1) return -1;

        return 0; 
    }

    // int checkParents(RBNode<T>* node) {
    //     if (!node) return 0;

    //     if (node->left && node->left->parent != node) {
    //         return -1;
    //     }
    //     if (node->right && node->right->parent != node) {
    //         return -1;
    //     }
    //     int left = checkParents(node->left.get());
    //     if (left == -1) return -1;

    //     int right = checkParents(node->right.get());
    //     if (right == -1) return -1;

    //     return 0; 
    // }

    int validate() {
        if (root && root->color != Color::Black) return -1;

        if (blackHeight(root.get()) == -1 ||
            checkRedRule(root.get()) == -1) {
            return -1;
        }
        return 0;
    }

    void insert(const T& value) {
        insertImp(value);
    }

    void remove(const T& value) {
        removeImp(value);
    }

    RBNode<T>* getRoot() {
        return root.get();
    }

    void inorder(const std::function<void(const T&)>& visit) const {
        inorder(root.get(), visit);
    }

    void saveToFile(const std::string& filename) const {
        std::ofstream out(filename, std::ios::binary);
        if (!out) return;

        CircularBuffer buffer(4096);

        inorder([&](const T& value) {
            const char* ptr = reinterpret_cast<const char*>(&value);
            for (size_t i = 0; i < sizeof(T); ++i)
                buffer.push(ptr[i]);

            if (buffer.full()) {
                char c;
                while (buffer.pop(c)) out.write(&c, 1);
            }
        });

        char c;
        while (buffer.pop(c)) out.write(&c, 1);
    }

    void loadFromFile(const std::string& filename) {
        std::ifstream in(filename, std::ios::binary);
        if (!in) return;

        root.reset();

        CircularBuffer buffer(4096);
        char byte;

        while (in.read(&byte, 1)) {
            buffer.push(byte);

            T value;
            while (readValue(buffer, value)) {
                insert(value);
            }
        }
    }

private:
    std::unique_ptr<RBNode<T>> root;  

    void inorder(RBNode<T>* node, const std::function<void(const T&)>& visit) const {
        if (!node) return;
        inorder(node->left.get(), visit);
        visit(node->data);
        inorder(node->right.get(), visit);
    }

    bool readValue(CircularBuffer& buffer, T& value) {
        if (buffer.size() < sizeof(T)) return false;
        char* ptr = reinterpret_cast<char*>(&value);
        for (size_t i = 0; i < sizeof(T); ++i) buffer.pop(ptr[i]);
        return true;
    }

    void removeImp(const T& value) {
        RBNode<T>* node = search(root.get(), value);
        Color origColor = node->color;
        if (!node) {
            return;
        }
        RBNode<T>* pivot = nullptr;
        RBNode<T>* parent = nullptr;
        if (!node->left) {
            pivot = node->right.get();
            parent = node->parent;
            transplant(node, std::move(node->right));
        } else if (!node->right) {
            pivot = node->left.get();
            parent = node->parent;
            transplant(node, std::move(node->left));
        } else {
            RBNode<T>* minNode = minimum(node->right.get());
            node->data = minNode->data;
            origColor = minNode->color;
            pivot = minNode->right.get();
            parent = minNode->parent;
            transplant(minNode, std::move(minNode->right));
        }
        if (origColor == Color::Black) {
            fixDelete(pivot, parent);
        }
    }

    void insertImp(const T& value) {
        std::unique_ptr<RBNode<T>> newNode = std::make_unique<RBNode<T>>(value);

        if (!root) {
            newNode->color = Color::Black;
            root = std::move(newNode);
            return;
        }

        RBNode<T>* parent = nullptr;
        RBNode<T>* current = root.get();
        while (current) {
            parent = current;           
            if (value < current->data) {
                current = current->left.get();  
            } else {
                current = current->right.get();  
            }
        }

        newNode->parent = parent;
        if (value < parent->data) {
            parent->left = std::move(newNode);
            balanceTree(parent->left.get());
        } else {
            parent->right = std::move(newNode);
            balanceTree(parent->right.get());
        }
    }


    RBNode<T>* search(RBNode<T>* node, const T& data) {
        if (!node || node->data == data) {
            return node;
        }
        if (data < node->data) {
            return search(node->left.get(), data);
        } else {
            return search(node->right.get(), data);
        }
    }

    void balanceTree(RBNode<T>* node) {
        while (node->parent && node->parent->color == Color::Red) {
            if (node->parent == node->parent->parent->left.get()) {
                RBNode<T>* uncle = node->parent->parent->right.get();
                if (uncle && uncle->color == Color::Red) {
                    node->parent->color = Color::Black;
                    uncle->color = Color::Black;
                    node->parent->parent->color = Color::Red;
                    node = node->parent->parent;
                } else {
                    if (node == node->parent->right.get()) {
                        node = node->parent;
                        rotateLeft(node);
                    }
                    node->parent->color = Color::Black;
                    node->parent->parent->color = Color::Red;
                    rotateRight(node->parent->parent);
                }

            } else {
                RBNode<T>* uncle = node->parent->parent->left.get();
                if (uncle && uncle->color == Color::Red) {
                    node->parent->color = Color::Black;
                    uncle->color = Color::Black;
                    node->parent->parent->color = Color::Red;
                    node = node->parent->parent;
                } else {
                    if (node == node->parent->left.get()) {
                        node = node->parent;
                        rotateRight(node);
                    }
                    node->parent->color = Color::Black;
                    node->parent->parent->color = Color::Red;
                    rotateLeft(node->parent->parent);
                }
            }
        }
        root->color = Color::Black;
    }

    void rotateLeft(RBNode<T>* parent) {
        std::unique_ptr<RBNode<T>> pivot = std::move(parent->right);
        parent->right = std::move(pivot->left);
        if (parent->right) {
            parent->right->parent = parent;
        }
        pivot->parent = parent->parent;
        RBNode<T>* oldroot = parent->parent;
        if (!pivot->parent) {
            RBNode<T>* oldroot = root.release();
            root = std::move(pivot);
            root->left = std::unique_ptr<RBNode<T>>(oldroot);
            root->left->parent = root.get();
        } else if (parent == pivot->parent->left.get()) {
            RBNode<T>* oldparent = oldroot->left.release();
            oldroot->left = std::move(pivot);
            oldroot->left->left = std::unique_ptr<RBNode<T>>(oldparent);
            oldroot->left->left->parent = oldroot->left.get();
        } else {
            RBNode<T>* oldparent = oldroot->right.release();
            oldroot->right = std::move(pivot);
            oldroot->right->left = std::unique_ptr<RBNode<T>>(oldparent);
            oldroot->right->left->parent = oldroot->left.get();
        }
    }
    
    void rotateRight(RBNode<T>* parent) {
        std::unique_ptr<RBNode<T>> pivot = std::move(parent->left);
        parent->left = std::move(pivot->right);
        if (parent->left) {
            parent->left->parent = parent;
        }
        pivot->parent = parent->parent;
        RBNode<T>* oldroot = parent->parent;
        if (!pivot->parent) {
            RBNode<T>* oldroot = root.release();
            root = std::move(pivot);
            root->right = std::unique_ptr<RBNode<T>>(oldroot);
            root->right->parent = root.get();
        } else if (parent == pivot->parent->left.get()) {
            RBNode<T>* oldparent = oldroot->left.release();
            oldroot->left = std::move(pivot);
            oldroot->left->right = std::unique_ptr<RBNode<T>>(oldparent);
            oldroot->left->right->parent = oldroot->left.get();
        } else {
            RBNode<T>* oldparent = oldroot->right.release();
            oldroot->right = std::move(pivot);
            oldroot->right->right = std::unique_ptr<RBNode<T>>(oldparent);
            oldroot->right->right->parent = oldroot->right.get();
        }
    }

    RBNode<T>* minimum(RBNode<T>* node) {
        while (node->left)
            node = node->left.get();
        return node;
    }

    std::unique_ptr<RBNode<T>>& ownerLink(RBNode<T>* node) {
        if (!node->parent)
            return root;
        if (node == node->parent->left.get())
            return node->parent->left;
        return node->parent->right;
    }   

    Color getColor(RBNode<T>* node) {
        return node ? node->color : Color::Black;
    }

    void transplant(RBNode<T>* parent, std::unique_ptr<RBNode<T>> pivot) {
        std::unique_ptr<RBNode<T>>& link = ownerLink(parent);
        link = std::move(pivot);
        if (link) {
            link->parent = parent->parent;
        }
    }

    void fixDelete(RBNode<T>* node, RBNode<T>* parent) {
        while (node != root.get() && (!node || getColor(node) == Color::Black)) {
            RBNode<T>* brother;
            if (parent && node == parent->left.get()) {
                brother = parent->right.get();
                if (getColor(brother) == Color::Red) {
                    brother->color = Color::Black;
                    parent->color = Color::Red;
                    rotateLeft(parent);
                    brother = parent->right.get();
                }
                if (getColor(brother->left.get()) == Color::Black && getColor(brother->right.get()) == Color::Black) {
                    brother->color = Color::Red;
                    node = parent;
                    parent = parent->parent;
                }
                else {
                    if (getColor(brother->right.get()) == Color::Black) {
                        brother->left->color = Color::Black;
                        brother->color = Color::Red;
                        rotateRight(brother);
                        brother = parent->right.get();
                    }
                    brother->color = parent->color;
                    parent->color = Color::Black;
                    brother->right->color = Color::Black;
                    rotateLeft(parent);
                    node = root.get();
                }
            }
            else {
                // if (!parent) {
                //     break;
                // }
                brother = parent->left.get();
                if (getColor(brother) == Color::Red) {
                    brother->color = Color::Black;
                    parent->color = Color::Red;
                    rotateRight(parent);
                    brother = parent->left.get();
                }
                if (getColor(brother->left.get()) == Color::Black && getColor(brother->right.get()) == Color::Black) {
                    rotateRight(parent);
                    brother = parent->left.get();
                }
                else {
                    if (getColor(brother->left.get()) == Color::Black) {
                        brother->right->color = Color::Black;
                        brother->color = Color::Red;
                        rotateLeft(brother);
                        brother = parent->left.get();
                    }
                    brother->color = parent->color;
                    parent->color = Color::Black;
                    brother->left->color = Color::Black;
                    rotateRight(parent);
                    node = root.get();
                }
            }
        }
        if (node) {
            node->color = Color::Black;
        }
    }
};
