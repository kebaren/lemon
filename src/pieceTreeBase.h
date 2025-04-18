/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for
 *license information.
 *--------------------------------------------------------------------------------------------*/

#pragma once

#include "common/position.h"
#include "common/range.h"
#include "rbTreeBase.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <regex>
#include <string>
#include <tuple>
#include <vector>

#ifdef NDEBUG
// Release模式
#define DEBUG_LOG(MSG, X)
#else
// Debug模式
#include <iostream>
#define DEBUG_LOG(MSG, X) std::cout << MSG << X << std::endl
#endif

namespace buffer
{

// Average buffer size for the pieces
constexpr int AverageBufferSize = 65535;

/**
 * Creates an appropriate typed array (Uint16Array or Uint32Array) based on the
 * contents.
 */
std::vector<int> createUintArray(const std::vector<int> &arr);

/**
 * Interface for a text snapshot, allowing to read chunks of the content as
 * needed.
 */
class ITextSnapshot
{
  public:
    virtual ~ITextSnapshot() = default;
    virtual std::string read() = 0;
};

/**
 * Represents line starts information with performance metadata
 */
class LineStarts
{
  public:
    std::vector<int> lineStarts;
    int cr;
    int lf;
    int crlf;
    bool isBasicASCII;

    LineStarts(const std::vector<int> &lineStarts, int cr, int lf, int crlf, bool isBasicASCII);
};

/**
 * Fast line starts computation without collecting metadata
 */
std::vector<int> createLineStartsFast(const std::string &str, bool readonly = true);

/**
 * Compute line starts with metadata about line break types and ASCII status
 */
LineStarts createLineStarts(const std::string &str);

/**
 * Cursor position within a buffer (line and column)
 */
struct BufferCursor
{
    int line;   // Line number in current buffer
    int column; // Column number in current buffer

    BufferCursor() : line(0), column(0)
    {
    }
    BufferCursor(int line, int column) : line(line), column(column)
    {
    }
};

/**
 * A piece within the piece table, referencing part of the original document or
 * the change buffer
 */
class Piece
{
  public:
    int bufferIndex;
    BufferCursor start;
    BufferCursor end;
    int length;
    int lineFeedCnt;

    Piece(int bufferIndex, const BufferCursor &start, const BufferCursor &end, int lineFeedCnt, int length);
    Piece();
};

/**
 * A buffer holding content with line starts information for efficient
 * navigation
 */
class StringBuffer
{
  public:
    std::string buffer;
    std::vector<int> lineStarts;

    StringBuffer(const std::string &buffer, const std::vector<int> &lineStarts);
};

/**
 * Position within the piece tree (node, remainder, and offset)
 */
struct NodePosition
{
    std::shared_ptr<TreeNode> node; // Piece Index
    int remainder;                  // Remainder in current piece
    int nodeStartOffset;            // Node start offset in document

    NodePosition() : node(nullptr), remainder(0), nodeStartOffset(0)
    {
    }
    NodePosition(std::shared_ptr<TreeNode> node, int remainder, int nodeStartOffset)
        : node(node), remainder(remainder), nodeStartOffset(nodeStartOffset)
    {
    }
};

/**
 * Cache entry for piece tree search
 */
struct CacheEntry
{
    std::shared_ptr<TreeNode> node;
    int nodeStartOffset;
    int nodeStartLineNumber;

    CacheEntry() : node(nullptr), nodeStartOffset(0), nodeStartLineNumber(0)
    {
    }
    CacheEntry(std::shared_ptr<TreeNode> node, int nodeStartOffset, int nodeStartLineNumber = 0)
        : node(node), nodeStartOffset(nodeStartOffset), nodeStartLineNumber(nodeStartLineNumber)
    {
    }
};

/**
 * Search cache for the piece tree to improve performance
 */
class PieceTreeSearchCache
{
  private:
    int _limit;
    std::vector<CacheEntry> _cache;

  public:
    PieceTreeSearchCache(int limit);
    PieceTreeSearchCache() = default;

    std::shared_ptr<CacheEntry> get(int offset);
    std::shared_ptr<CacheEntry> get2(int lineNumber) noexcept;
    void set(const CacheEntry &nodePosition);
    void validate(int offset);
};

/**
 * Readonly snapshot for piece tree
 */
class PieceTreeBase;
class PieceTreeSnapshot : public ITextSnapshot
{
  private:
    std::vector<std::shared_ptr<Piece>> _pieces;
    int _index;
    std::shared_ptr<PieceTreeBase> _tree;
    std::string _BOM;

  public:
    PieceTreeSnapshot(std::shared_ptr<PieceTreeBase> tree, const std::string &BOM);
    std::string read() override;
};

/**
 * The main piece tree buffer implementation
 */
class PieceTreeBase : public std::enable_shared_from_this<PieceTreeBase>
{
  public:
    std::shared_ptr<TreeNode> root;

  protected:
    std::vector<StringBuffer> _buffers; // 0 is change buffer, others are readonly original buffer
    int _lineCnt;
    int _length;
    std::string _EOL;
    int _EOLLength;
    bool _EOLNormalized;

  private:
    BufferCursor _lastChangeBufferPos;
    PieceTreeSearchCache _searchCache;
    struct
    {
        int lineNumber;
        std::string value;
    } _lastVisitedLine;

  public:
    explicit PieceTreeBase() = default;
    PieceTreeBase(std::vector<StringBuffer> &chunks, const std::string &eol, bool eolNormalized) noexcept;
    void create(std::vector<StringBuffer> &chunks, const std::string &eol, bool eolNormalized) noexcept;
    void normalizeEOL(const std::string &eol);

    // Line feed and EOL functions
    std::string getEOL() const;
    void setEOL(const std::string &newEOL);

    // Snapshot and comparison
    std::unique_ptr<ITextSnapshot> createSnapshot(const std::string &BOM);
    bool equal(const PieceTreeBase &other);

    // Position and offset conversion
    int getOffsetAt(int lineNumber, int column);
    Position getPositionAt(int offset);

    // Content retrieval
    std::string getValueInRange(const Range &range, const std::string &eol = "\n");
    std::string getValueInRange2(const NodePosition &startPosition, const NodePosition &endPosition);
    std::vector<std::string> getLinesContent();
    int getLength() const;
    int getLineCount() const;
    std::string getLineContent(int lineNumber);
    int getLineCharCode(int lineNumber, int index);
    int getLineLength(int lineNumber);

    // Editing operations
    void insert(int offset, std::string &value, bool eolNormalized = false);
    void delete_(int offset, int cnt);

    // Node operations
    void insertContentToNodeLeft(std::string &value, std::shared_ptr<TreeNode> node);
    void insertContentToNodeRight(std::string &value, std::shared_ptr<TreeNode> node);
    BufferCursor positionInBuffer(std::shared_ptr<TreeNode> node, int remainder,
                                  std::shared_ptr<BufferCursor> ret = nullptr);
    int getLineFeedCnt(int bufferIndex, const BufferCursor &start, const BufferCursor &end) const;
    int offsetInBuffer(int bufferIndex, const BufferCursor &cursor) const;
    void deleteNodes(const std::vector<std::shared_ptr<TreeNode>> nodes);
    std::vector<Piece> createNewPieces(const std::string &text);

    // Content retrieval (raw)
    std::string getLinesRawContent();
    std::string getLineRawContent(int lineNumber, int endOffset = 0);

    // Metadata computation
    void computeBufferMetadata();

    std::tuple<int, int> getIndexOf(std::shared_ptr<TreeNode> node, int accumulatedValue);
    int getAccumulatedValue(std::shared_ptr<TreeNode> node, int index);

    // Node modification
    void deleteNodeTail(std::shared_ptr<TreeNode> node, const BufferCursor &pos);
    void deleteNodeHead(std::shared_ptr<TreeNode> node, const BufferCursor &pos);
    void shrinkNode(std::shared_ptr<TreeNode> node, const BufferCursor &start, const BufferCursor &end);
    void appendToNode(std::shared_ptr<TreeNode> node, std::string &value);

    // Node navigation
    NodePosition nodeAt(int offset);
    NodePosition nodeAt2(int lineNumber, int column);
    int nodeCharCodeAt(std::shared_ptr<TreeNode> node, int offset);
    int offsetOfNode(std::shared_ptr<TreeNode> node);

    // CRLF handling
    bool shouldCheckCRLF();
    bool startWithLF(const std::string &val);
    bool startWithLF(std::shared_ptr<TreeNode> val);
    bool endWithCR(const std::string &val);
    bool endWithCR(std::shared_ptr<TreeNode> val);
    void validateCRLFWithPrevNode(std::shared_ptr<TreeNode> nextNode);
    void validateCRLFWithNextNode(std::shared_ptr<TreeNode> node);
    void fixCRLF(std::shared_ptr<TreeNode> prev, std::shared_ptr<TreeNode> next);
    bool adjustCarriageReturnFromNext(std::string &value, std::shared_ptr<TreeNode> node);

    // Tree operations
    bool iterate(std::shared_ptr<TreeNode> node, std::function<bool(std::shared_ptr<TreeNode>)> callback);
    std::string getNodeContent(std::shared_ptr<TreeNode> node);
    std::string getPieceContent(const Piece &piece);
    std::shared_ptr<TreeNode> rbInsertRight(std::shared_ptr<TreeNode> node, const Piece &p);
    std::shared_ptr<TreeNode> rbInsertLeft(std::shared_ptr<TreeNode> node, const Piece &p);
    std::string getContentOfSubTree(std::shared_ptr<TreeNode> node);

  private:
    // Tree rotation functions
    void leftRotate(std::shared_ptr<TreeNode> x);
    void rightRotate(std::shared_ptr<TreeNode> y);

    // Node delete
    void rbDelete(std::shared_ptr<TreeNode> z);

    // Fix tree after insert
    void fixInsert(std::shared_ptr<TreeNode> x);

    // Update tree metadata
    void updateTreeMetadata(std::shared_ptr<TreeNode> x, int delta, int lineFeedCntDelta);

    // Recompute tree metadatg
    void recomputeTreeMetadata(std::shared_ptr<TreeNode> x);
};

std::vector<std::string> regex_split(const std::string &str, const std::regex &regex);

} // namespace buffer