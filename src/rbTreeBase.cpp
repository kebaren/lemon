#include "rbTreeBase.h"
#include "pieceTreeBase.h"
#include <cstddef>
#include <memory>

namespace buffer
{

// initial sentinel node
const std::shared_ptr<TreeNode> SENTINEL = [] {
    auto sentinel = std::make_shared<TreeNode>(nullptr, NodeColor::Black); // 传入 nullptr 作为 Piece
    sentinel->size_left = 0;
    sentinel->lf_left = 0;
    sentinel->parent = sentinel;
    sentinel->left = sentinel;
    sentinel->right = sentinel;
    return sentinel;
}();

TreeNode::TreeNode() noexcept
{
    this->piece = std::make_shared<Piece>();
    this->color = NodeColor::Black;
    this->size_left = 0;
    this->lf_left = 0;
    this->parent = SENTINEL;
    this->left = SENTINEL;
    this->right = SENTINEL;
}

TreeNode::TreeNode(std::shared_ptr<Piece> piece, NodeColor color) noexcept
{
    // construct the tree node
    this->piece = std::move(piece);
    this->color = color;
    this->size_left = 0;
    this->lf_left = 0;
    this->parent = SENTINEL;
    this->left = SENTINEL;
    this->right = SENTINEL;
}

std::shared_ptr<TreeNode> TreeNode::next() noexcept
{
    if (right != SENTINEL)
    {
        return leftest(right);
    }

    auto node = shared_from_this();
    auto parentNode = node->parent.lock();

    while (parentNode && parentNode != SENTINEL)
    {
        if (parentNode->left == node)
        {
            break;
        }
        node = parentNode;
        parentNode = node->parent.lock();
    }
    if (parentNode == SENTINEL)
    {
        return SENTINEL;
    }
    else
    {
        return parentNode;
    }
}
std::shared_ptr<TreeNode> TreeNode::prev() noexcept
{
    if (left != SENTINEL)
    {
        return rightest(left);
    }
    auto node = shared_from_this();
    auto parentNode = node->parent.lock();
    while (parentNode && parentNode != SENTINEL)
    {
        if (parentNode->right == node)
        {
            break;
        }
        node = parentNode;
        parentNode = node->parent.lock();
    }
    if (node->parent.lock() == SENTINEL)
    {
        return SENTINEL;
    }
    else
    {
        return node->parent.lock();
    }
}
void TreeNode::detach() noexcept
{
    parent.reset();
    left.reset();
    right.reset();
}

// Tree traversal functions
std::shared_ptr<TreeNode> leftest(std::shared_ptr<TreeNode> node)
{
    while (node->left != SENTINEL)
    {
        node = node->left;
    }
    return node;
}

std::shared_ptr<TreeNode> rightest(std::shared_ptr<TreeNode> node)
{
    while (node->right != SENTINEL)
    {
        node = node->right;
    }
    return node;
}

// Tree size calculation
int calculateSize(std::shared_ptr<TreeNode> node)
{
    if (node == SENTINEL)
    {
        return 0;
    }

    return node->size_left + node->piece->length + calculateSize(node->right);
}

int calculateLF(std::shared_ptr<TreeNode> node)
{

    if (node == SENTINEL)
    {
        return 0;
    }
    return node->lf_left + node->piece->lineFeedCnt + calculateLF(node->right);
}

// Reset sentinel node
void resetSentinel()
{
    SENTINEL->parent = SENTINEL;
}

} // namespace buffer
