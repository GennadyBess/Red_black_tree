#include <memory>

enum class Color { Red, Black };

template <typename T>
struct RBNode {
    T data;
    Color color;
    std::unique_ptr<RBNode<T>> left;
    std::unique_ptr<RBNode<T>> right;
    RBNode<T>* parent;

    RBNode(const T& data) : data{data}, color{Color::Red}, parent{nullptr} {}
};