#include <gtest/gtest.h>
#include "rb_tree.hpp"
#include <string>
#include <fstream>

TEST(CircularBufferTest, PushPop) {
    CircularBuffer buffer(3);
    buffer.push('a');
    buffer.push('b');
    buffer.push('c');

    char val;
    EXPECT_TRUE(buffer.pop(val));
    EXPECT_EQ(val, 'a');
    EXPECT_TRUE(buffer.pop(val));
    EXPECT_EQ(val, 'b');
    EXPECT_TRUE(buffer.pop(val));
    EXPECT_EQ(val, 'c');
    EXPECT_FALSE(buffer.pop(val)); 
}

TEST(RBTreeTest, InsertAndInorder) {
    RedBlackTree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(20);

    std::vector<int> result;
    tree.inorder([&](const int& val) { result.push_back(val); });

    std::vector<int> expected = {5, 10, 20};
    EXPECT_EQ(result, expected);
}

TEST(RBTreeTest, SaveLoadFile) {
    RedBlackTree<int> tree;
    tree.insert(10);
    tree.insert(5);
    tree.insert(20);

    const std::string filename = "tree_test.bin";
    tree.saveToFile(filename);

    RedBlackTree<int> tree2;
    tree2.loadFromFile(filename);

    std::vector<int> result;
    tree2.inorder([&](const int& val) { result.push_back(val); });

    std::vector<int> expected = {5, 10, 20};
    EXPECT_EQ(result, expected);

    std::remove(filename.c_str()); 
}

TEST(RBTreeTest, Insert) {
    RedBlackTree<int> tree;
    tree.insert(15);
    tree.insert(17);
    tree.insert(13);
    tree.insert(14);
    tree.insert(9);
    tree.insert(12);
    tree.insert(10);
    tree.insert(16);


    std::vector<int> result;
    tree.inorder([&](const int& val) { result.push_back(val); });

    std::vector<int> expected = {9, 10, 12, 13, 14, 15, 16, 17};
    EXPECT_EQ(result, expected);

    EXPECT_EQ(tree.validate(), 0);
}

TEST(RBTreeTest, Remove) {
    RedBlackTree<int> tree;
    tree.insert(15);
    tree.insert(17);
    tree.insert(13);
    tree.insert(14);
    tree.insert(9);
    tree.insert(12);
    tree.insert(10);
    tree.insert(16);

    tree.remove(13);

    std::vector<int> result;
    tree.inorder([&](const int& val) { result.push_back(val); });

    std::vector<int> expected = {9, 10, 12, 14, 15, 16, 17};
    EXPECT_EQ(result, expected);

    EXPECT_EQ(tree.validate(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
