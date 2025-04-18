/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for
 *license information.
 *--------------------------------------------------------------------------------------------*/

#pragma once

#include <memory>

namespace buffer
{

// // Forward declarations
class Piece;
class PieceTreeBase;

enum class NodeColor
{
    Black = 0,
    Red = 1
};

class TreeNode : public std::enable_shared_from_this<TreeNode>
{
  public:
    std::weak_ptr<TreeNode> parent;
    std::shared_ptr<TreeNode> left;
    std::shared_ptr<TreeNode> right;
    NodeColor color;

    // Piece
    std::shared_ptr<Piece> piece;
    int size_left; // size of the left subtree (not inorder)
    int lf_left;   // line feeds cnt in the left subtree (not in order)

    explicit TreeNode(std::shared_ptr<Piece> piece, NodeColor color) noexcept;
    TreeNode() noexcept;
    std::shared_ptr<TreeNode> next() noexcept;
    std::shared_ptr<TreeNode> prev() noexcept;
    void detach() noexcept;
};

// sentinel node for the tree
extern const std::shared_ptr<TreeNode> SENTINEL;
// Tree traversal functions
std::shared_ptr<TreeNode> leftest(std::shared_ptr<TreeNode> node);
std::shared_ptr<TreeNode> rightest(std::shared_ptr<TreeNode> node);

// Tree size calculation
int calculateSize(std::shared_ptr<TreeNode> node);
int calculateLF(std::shared_ptr<TreeNode> node);

// Reset sentinel node
void resetSentinel();

} // namespace buffer