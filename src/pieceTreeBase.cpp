#include "pieceTreeBase.h"
#include "common/charCode.h"
#include "rbTreeBase.h"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <regex>

namespace buffer
{
// LineStarts implementation
LineStarts::LineStarts(const std::vector<int> &lineStarts, int cr, int lf, int crlf, bool isBasicASCII)
    : lineStarts(lineStarts), cr(cr), lf(lf), crlf(crlf), isBasicASCII(isBasicASCII)
{
}

std::vector<int> createLineStartsFast(const std::string &str, bool readonly)
{
    std::vector<int> result = {0};

    for (int i = 0, len = str.length(); i < len; i++)
    {
        const char chr = str[i];

        if (chr == CharCode::CarriageReturn)
        {
            if (i + 1 < len && str[i + 1] == CharCode::LineFeed)
            {
                // \r\n... case
                result.push_back(i + 2);
                i++; // skip \n
            }
            else
            {
                // \r... case
                result.push_back(i + 1);
            }
        }
        else if (chr == CharCode::LineFeed)
        {
            result.push_back(i + 1);
        }
    }

    return std::move(result);
}

LineStarts createLineStarts(const std::string &str)
{
    std::vector<int> r = {0};
    // int rLength = 1;
    int cr = 0, lf = 0, crlf = 0;
    bool isBasicASCII = true;

    for (int i = 0, len = str.length(); i < len; i++)
    {
        const char chr = str[i];

        if (chr == CharCode::CarriageReturn)
        {
            if (i + 1 < len && str[i + 1] == CharCode::LineFeed)
            {
                // \r\n... case
                crlf++;
                r.push_back(i + 2);
                i++; // skip \n
            }
            else
            {
                cr++;
                // \r... case
                r.push_back(i + 1);
            }
        }
        else if (chr == CharCode::LineFeed)
        {
            lf++;
            r.push_back(i + 1);
        }
        else if (isBasicASCII)
        {
            if (chr != CharCode::Tab && (chr < 32 || chr > 126))
            {
                isBasicASCII = false;
            }
        }
    }

    return std::move(LineStarts(r, cr, lf, crlf, isBasicASCII));
}

// Piece implementation
Piece::Piece(int bufferIndex, const BufferCursor &start, const BufferCursor &end, int lineFeedCnt, int length)
    : bufferIndex(bufferIndex), start(start), end(end), lineFeedCnt(lineFeedCnt), length(length)
{
}

Piece::Piece() : bufferIndex(0), start(), end(), lineFeedCnt(0), length(0)
{
}

// StringBuffer implementation
StringBuffer::StringBuffer(const std::string &buffer, const std::vector<int> &lineStarts)
    : buffer(buffer), lineStarts(lineStarts)
{
}

// PieceTreeSearchCache implementation
PieceTreeSearchCache::PieceTreeSearchCache(int limit) : _limit(limit), _cache(std::vector<CacheEntry>())
{
}

std::shared_ptr<CacheEntry> PieceTreeSearchCache::get(int offset)
{
    if (_cache.size() <= 0)
    {
        return nullptr;
    }
    for (int i = _cache.size() - 1; i >= 0; i--)
    {
        auto nodePos = _cache[i];
        if (nodePos.nodeStartOffset <= offset && nodePos.nodeStartOffset + nodePos.node->piece->length >= offset)
        {
            return std::make_shared<CacheEntry>(nodePos);
        }
    }
    return nullptr;
}

std::shared_ptr<CacheEntry> PieceTreeSearchCache::get2(int lineNumber) noexcept
{

    if (_cache.size() <= 0)
    {
        return nullptr;
    }

    for (int i = _cache.size() - 1; i >= 0; i--)
    {
        const auto &nodePos = _cache[i];
        if (nodePos.nodeStartLineNumber && nodePos.nodeStartLineNumber < lineNumber &&
            nodePos.nodeStartLineNumber + nodePos.node->piece->lineFeedCnt >= lineNumber)
        {
            return std::make_shared<CacheEntry>(nodePos);
        }
    }
    return nullptr;
}

void PieceTreeSearchCache::set(const CacheEntry &nodeposition)
{
    if (_cache.size() >= _limit)
    {
        _cache.erase(_cache.begin());
    }
    _cache.push_back(nodeposition);
}

void PieceTreeSearchCache::validate(int offset)
{
    auto hasInvalidVal = false;
    auto tmp = _cache;
    for (int i = 0; i < tmp.size(); i++)
    {
        auto nodePos = tmp[i];
        if (nodePos.node->parent.lock() == nullptr || nodePos.nodeStartOffset >= offset)
        {
            tmp[i] = CacheEntry();
            hasInvalidVal = true;
            continue;
        }
    }

    if (hasInvalidVal)
    {
        std::vector<CacheEntry> newArr;
        for (const auto &entry : tmp)
        {
            if (entry.node != nullptr)
            {
                newArr.push_back(entry);
            }
        }
        _cache = newArr;
    }
}

// PieceTreeSnapshot implementation
PieceTreeSnapshot::PieceTreeSnapshot(std::shared_ptr<PieceTreeBase> tree, const std::string &BOM)
    : _pieces(std::vector<std::shared_ptr<Piece>>()), _index(0), _tree(tree), _BOM(BOM)
{
    // copy the pieces from the tree
    if (tree->root != SENTINEL)
    {
        tree->iterate(tree->root, [this](std::shared_ptr<TreeNode> node) {
            if (node != SENTINEL)
            {
                this->_pieces.emplace_back(node->piece);
            }
            return true;
        });
    }
}

std::string PieceTreeSnapshot::read()
{
    if (_pieces.size() == 0)
    {
        if (_index == 0)
        {
            _index++;
            return _BOM;
        }
        else
        {
            return nullptr;
        }
    }

    if (_index > _pieces.size() - 1)
    {
        return nullptr;
    }

    if (_index == 0)
    {
        return _BOM + _tree->getPieceContent(*_pieces[_index++]);
    }

    return _tree->getPieceContent(*_pieces[_index++]);
}

// PieceTreeBase implementation
PieceTreeBase::PieceTreeBase(std::vector<StringBuffer> &chunks, const std::string &eol, bool eolNormalized) noexcept
    : std::enable_shared_from_this<PieceTreeBase>()
{
    if (eol != "\r\n" && eol != "\n")
    {
        return;
    }

    create(chunks, eol, eolNormalized);
}

void PieceTreeBase::create(std::vector<StringBuffer> &chunks, const std::string &eol, bool eolNormalized) noexcept
{
    _buffers.reserve(128);
    _buffers.emplace_back(StringBuffer("", {0}));
    _lastChangeBufferPos = {0, 0};
    root = SENTINEL;
    _lineCnt = 1;
    _length = 0;
    _EOL = eol;
    _EOLLength = eol.length();
    _EOLNormalized = eolNormalized;

    std::shared_ptr<TreeNode> lastNode = SENTINEL;

    for (int i = 0, len = chunks.size(); i < len; i++)
    {

        if (chunks[i].buffer.length() > 0)
        {

            if (chunks[i].lineStarts.empty())
            {

                chunks[i].lineStarts = createLineStartsFast(chunks[i].buffer);
            }

            auto piece =
                Piece(i + 1, BufferCursor(0, 0),
                      BufferCursor(chunks[i].lineStarts.size() - 1,
                                   chunks[i].buffer.length() - chunks[i].lineStarts[chunks[i].lineStarts.size() - 1]),
                      chunks[i].lineStarts.size() - 1, chunks[i].buffer.length());

            _buffers.emplace_back(chunks[i]);

            lastNode = rbInsertRight(lastNode, piece);
        }
    }
    _searchCache = PieceTreeSearchCache(1);
    _lastVisitedLine = {0, ""};
    computeBufferMetadata();
}

void PieceTreeBase::normalizeEOL(const std::string &eol)
{
    if (eol != "\r\n" && eol != "\n")
        return;

    auto averageBufferSize = AverageBufferSize;
    auto min = averageBufferSize - std::floor(averageBufferSize / 3);
    auto max = min * 2;

    std::string tempChunk = "";
    int tempChunkLen = 0;
    std::vector<StringBuffer> chunks;

    iterate(root, [&](std::shared_ptr<TreeNode> node) -> bool {
        auto str = getNodeContent(node);
        auto len = str.size();
        if (tempChunkLen <= min || tempChunkLen + len < max)
        {
            tempChunk += str;
            tempChunkLen += len;
            return true;
        }
        auto text = std::regex_replace(tempChunk, std::regex("\r\n|\r|\n", std::regex_constants::icase), eol);
        chunks.emplace_back(StringBuffer(text, createLineStartsFast(text)));
        tempChunk = str;
        tempChunkLen = len;
        return true;
    });

    if (tempChunkLen > 0)
    {
        auto text = std::regex_replace(tempChunk, std::regex("\r\n|\r|\n", std::regex_constants::icase), eol);
        chunks.emplace_back(StringBuffer(text, createLineStartsFast(text)));
    }

    create(chunks, eol, true);
}

std::string PieceTreeBase::getEOL() const
{
    return _EOL;
}

void PieceTreeBase::setEOL(const std::string &newEOL)
{
    if (newEOL != "\r\n" && newEOL != "\n")
        return;
    _EOL = newEOL;
    _EOLLength = static_cast<int>(newEOL.length());
    normalizeEOL(newEOL);
}

std::unique_ptr<ITextSnapshot> PieceTreeBase::createSnapshot(const std::string &BOM)
{
    return std::make_unique<PieceTreeSnapshot>(shared_from_this(), BOM);
}

bool PieceTreeBase::equal(const PieceTreeBase &other)
{
    if (this->_length != other._length)
    {
        return false;
    }

    if (this->getLineCount() != other.getLineCount())
    {
        return false;
    }

    int offset = 0;
    bool ret = iterate(root, [&](std::shared_ptr<TreeNode> node) -> bool {
        if (node == SENTINEL)
            return true;
        auto str = getNodeContent(node);
        auto len = str.length();
        auto startPosition = const_cast<PieceTreeBase &>(other).nodeAt(offset);
        auto endPosition = const_cast<PieceTreeBase &>(other).nodeAt(offset + len);
        auto val = const_cast<PieceTreeBase &>(other).getValueInRange2(startPosition, endPosition);

        return str == val;
    });
    return ret;
}

int PieceTreeBase::getOffsetAt(int lineNumber, int column)
{
    int leftLen = 0;
    auto x = this->root;
    while (x != SENTINEL)
    {
        if (x->left != SENTINEL && x->lf_left + 1 >= lineNumber)
        {
            x = x->left;
        }
        else if (x->lf_left + x->piece->lineFeedCnt + 1 >= lineNumber)
        {
            leftLen += x->size_left;
            // lineNumber >= 2
            auto accumualtedValInCurrentIndex = this->getAccumulatedValue(x, lineNumber - x->lf_left - 2);
            return leftLen += accumualtedValInCurrentIndex + column - 1;
        }
        else
        {
            lineNumber -= x->lf_left + x->piece->lineFeedCnt;
            leftLen += x->size_left + x->piece->length;
            x = x->right;
        }
    }
    return leftLen;
}

Position PieceTreeBase::getPositionAt(int offset)
{
    offset = std::floor(offset);
    offset = std::max((int)0, offset);

    auto x = this->root;
    int lfCnt = 0;
    auto originalOffset = offset;

    while (x != SENTINEL)
    {
        if (x->size_left != 0 && x->size_left >= offset)
        {
            x = x->left;
        }
        else if (x->size_left + x->piece->length >= offset)
        {
            auto [index, remainder] = this->getIndexOf(x, offset - x->size_left);
            lfCnt += x->lf_left + index;

            if (index == 0)
            {
                auto lineStartOffset = this->getOffsetAt(lfCnt + 1, 1);
                auto column = originalOffset - lineStartOffset;
                return Position(lfCnt + 1, column + 1);
            }
            return Position(lfCnt + 1, remainder + 1);
        }
        else
        {
            offset -= x->size_left + x->piece->length;
            lfCnt += x->lf_left + x->piece->lineFeedCnt;

            if (x->right == SENTINEL)
            {
                auto lineStartOffset = this->getOffsetAt(lfCnt + 1, 1);
                auto column = originalOffset - offset - lineStartOffset;
                return Position(lfCnt + 1, column + 1);
            }
            else
            {
                x = x->right;
            }
        }
    }
    return Position(1, 1);
}
std::string PieceTreeBase::getValueInRange(const Range &range, const std::string &eol)
{
    if (range.startLineNumber == range.endLineNumber && range.startColumn == range.endColumn)
    {
        return "";
    }

    auto startPosition = nodeAt2(range.startLineNumber, range.startColumn);
    auto endPosition = nodeAt2(range.endLineNumber, range.endColumn);
    auto value = getValueInRange2(startPosition, endPosition);
    if (!eol.empty())
    {
        if (eol != this->_EOL || !this->_EOLNormalized)
        {
            return std::regex_replace(value, std::regex("\r\n|\r|\n", std::regex_constants::icase), eol);
        }

        if (eol == this->getEOL() && this->_EOLNormalized)
        {
            if (eol == "\r\n")
            {
            }
            return value;
        }
        return std::regex_replace(value, std::regex("\r\n|\r|\n", std::regex_constants::icase), eol);
    }
    return value;
}

std::string PieceTreeBase::getValueInRange2(const NodePosition &startPosition, const NodePosition &endPosition)
{
    if (startPosition.node == endPosition.node)
    {
        auto node = startPosition.node;
        auto buffer = _buffers[node->piece->bufferIndex].buffer;
        auto startOffset = offsetInBuffer(node->piece->bufferIndex, node->piece->start);
        return buffer.substr(startOffset + startPosition.remainder, endPosition.remainder - startPosition.remainder);
    }

    auto x = startPosition.node;
    auto buffer = _buffers[x->piece->bufferIndex].buffer;
    auto startOffset = offsetInBuffer(x->piece->bufferIndex, x->piece->start);
    auto ret = buffer.substr(startOffset + startPosition.remainder, x->piece->length - startPosition.remainder);

    x = x->next();
    while (x != SENTINEL)
    {
        auto buffer = _buffers[x->piece->bufferIndex].buffer;
        auto startOffset = offsetInBuffer(x->piece->bufferIndex, x->piece->start);

        if (x == endPosition.node)
        {
            ret += buffer.substr(startOffset, endPosition.remainder);
            break;
        }
        else
        {
            ret += buffer.substr(startOffset, x->piece->length);
        }
        x = x->next();
    }
    return ret;
}

std::vector<std::string> PieceTreeBase::getLinesContent()
{

    auto strs = getContentOfSubTree(root);

    return regex_split(strs, std::regex("\r\n|\r|\n", std::regex_constants::icase));
}
int PieceTreeBase::getLength() const
{
    return _length;
}

int PieceTreeBase::getLineCount() const
{
    return _lineCnt;
}

std::string PieceTreeBase::getLineContent(int lineNumber)
{
    if (this->_lastVisitedLine.lineNumber == lineNumber)
    {
        return this->_lastVisitedLine.value;
    }

    this->_lastVisitedLine.lineNumber = lineNumber;

    if (lineNumber == this->_lineCnt)
    {
        this->_lastVisitedLine.value = this->getLineRawContent(lineNumber);
    }
    else if (this->_EOLNormalized)
    {
        this->_lastVisitedLine.value = this->getLineRawContent(lineNumber, this->_EOLLength);
    }
    else
    {
        auto str = this->getLineRawContent(lineNumber);
        this->_lastVisitedLine.value =
            std::regex_replace(this->getLineRawContent(lineNumber), std::regex("(\r\n|\r|\n)$"), "");
    }
    return this->_lastVisitedLine.value;
}

int PieceTreeBase::getLineCharCode(int lineNumber, int index)
{
    auto nodePos = this->nodeAt2(lineNumber, index + 1);
    if (nodePos.remainder == nodePos.node->piece->length)
    {
        // the char we want to fetch is at the head of next node.
        auto matchingNode = nodePos.node->next();
        if (!matchingNode)
        {
            return 0;
        }
        auto buffer = _buffers[matchingNode->piece->bufferIndex];
        auto startOffset = this->offsetInBuffer(matchingNode->piece->bufferIndex, matchingNode->piece->start);
        return (int)buffer.buffer.at(startOffset);
    }
    else
    {
        auto buffer = _buffers[nodePos.node->piece->bufferIndex];
        auto startOffset = this->offsetInBuffer(nodePos.node->piece->bufferIndex, nodePos.node->piece->start);
        auto targetOffset = startOffset + nodePos.remainder;

        return (int)buffer.buffer.at(targetOffset);
    }
}

int PieceTreeBase::getLineLength(int lineNumber)
{
    if (lineNumber == getLineCount())
    {
        auto startOffset = this->getOffsetAt(lineNumber, 1);
        return this->getLength() - startOffset;
    }
    return this->getOffsetAt(lineNumber + 1, 1) - this->getOffsetAt(lineNumber, 1) - this->_EOLLength;
}

// region piece table
void PieceTreeBase::insert(int offset, std::string &value, bool eolNormalized)
{
    this->_EOLNormalized = this->_EOLNormalized && eolNormalized;
    this->_lastVisitedLine.lineNumber = 0;
    this->_lastVisitedLine.value = "";
    if (this->root != SENTINEL)
    {
        auto [node, remainder, nodeStartOffset] = this->nodeAt(offset);
        auto piece = node->piece;
        auto bufferIndex = piece->bufferIndex;
        auto insertPosInBuffer = this->positionInBuffer(node, remainder);
        if (node->piece->bufferIndex == 0 && piece->end.line == this->_lastChangeBufferPos.line &&
            piece->end.column == this->_lastChangeBufferPos.column && (nodeStartOffset + piece->length == offset) &&
            value.length() < AverageBufferSize)
        {
            // changed buffer
            this->appendToNode(node, value);
            this->computeBufferMetadata();
            return;
        }

        if (nodeStartOffset == offset)
        {
            this->insertContentToNodeLeft(value, node);
            this->_searchCache.validate(offset);
        }
        else if (nodeStartOffset + node->piece->length > offset)
        {
            // we are inserting into the middle of a node.
            auto nodesToDel = std::vector<std::shared_ptr<TreeNode>>();
            auto newRightPiece =
                Piece(piece->bufferIndex, insertPosInBuffer, piece->end,
                      getLineFeedCnt(piece->bufferIndex, insertPosInBuffer, piece->end),
                      offsetInBuffer(bufferIndex, piece->end) - offsetInBuffer(bufferIndex, insertPosInBuffer));

            if (this->shouldCheckCRLF() && this->endWithCR(value))
            {
                auto headOfRight = this->nodeCharCodeAt(node, remainder);

                if (headOfRight == 10 /** \n */)
                {
                    auto newStart = BufferCursor(newRightPiece.start.line + 1, 0);
                    newRightPiece = Piece(newRightPiece.bufferIndex, newStart, newRightPiece.end,
                                          getLineFeedCnt(newRightPiece.bufferIndex, newStart, newRightPiece.end),
                                          newRightPiece.length - 1);
                    value += "\n";
                }
            }

            // reuse node for content before insertion point
            if (this->shouldCheckCRLF() && this->startWithLF(value))
            {
                auto tailOfLeft = this->nodeCharCodeAt(node, remainder - 1);

                if (tailOfLeft == 13 /** \r */)
                {
                    auto previousPos = this->positionInBuffer(node, remainder - 1);
                    this->deleteNodeTail(node, previousPos);
                    value += "\r";

                    if (node->piece->length == 0)
                    {
                        nodesToDel.emplace_back(node);
                    }
                }
                else
                {
                    this->deleteNodeTail(node, insertPosInBuffer);
                }
            }
            else
            {
                this->deleteNodeTail(node, insertPosInBuffer);
            }

            auto newPieces = this->createNewPieces(value);
            if (newRightPiece.length > 0)
            {
                this->rbInsertRight(node, newRightPiece);
            }

            auto tmpNode = node;
            for (int k = 0; k < newPieces.size(); k++)
            {
                tmpNode = this->rbInsertRight(tmpNode, newPieces[k]);
            }
            this->deleteNodes(nodesToDel);
        }
        else
        {
            this->insertContentToNodeRight(value, node);
        }
    }
    else
    {
        // insert new node
        auto pieces = this->createNewPieces(value);
        auto node = this->rbInsertLeft(nullptr, pieces[0]);

        for (int k = 1; k < pieces.size(); k++)
        {
            node = this->rbInsertRight(node, pieces[k]);
        }
    }

    // todo, this is too brutal. Total line feed count should be updated the same
    // way as lf_left.
    this->computeBufferMetadata();
}

void PieceTreeBase::delete_(int offset, int cnt)
{
    this->_lastVisitedLine.lineNumber = 0;
    this->_lastVisitedLine.value = "";

    if (cnt <= 0 || this->root == SENTINEL)
    {
        return;
    }
    if (offset < 0)
        offset = 0;
    if ((offset + cnt) > getLength())
        cnt = getLength() - offset;

    auto startPosition = this->nodeAt(offset);
    auto endPosition = this->nodeAt(offset + cnt);
    auto startNode = startPosition.node;
    auto endNode = endPosition.node;

    if (startNode == endNode)
    {
        auto startSplitPosInBuffer = this->positionInBuffer(startNode, startPosition.remainder);
        auto endSplitPosInBuffer = this->positionInBuffer(startNode, endPosition.remainder);

        if (startPosition.nodeStartOffset == offset)
        {
            if (cnt == startNode->piece->length)
            {
                // delete node
                auto next = startNode->next();
                rbDelete(startNode);
                this->validateCRLFWithPrevNode(next);
                this->computeBufferMetadata();
                return;
            }
            this->deleteNodeHead(startNode, endSplitPosInBuffer);
            this->_searchCache.validate(offset);
            this->validateCRLFWithPrevNode(startNode);
            this->computeBufferMetadata();
            return;
        }
        if (startPosition.nodeStartOffset + startNode->piece->length == offset + cnt)
        {
            this->deleteNodeTail(startNode, startSplitPosInBuffer);
            this->validateCRLFWithNextNode(startNode);
            this->computeBufferMetadata();
            return;
        }

        // delete content in the middle, this node will be splitted to nodes
        this->shrinkNode(startNode, startSplitPosInBuffer, endSplitPosInBuffer);
        this->computeBufferMetadata();
        return;
    }

    // delete content in multiple nodes
    auto nodesToDel = std::vector<std::shared_ptr<TreeNode>>();
    auto startSplitPosInBuffer = this->positionInBuffer(startNode, startPosition.remainder);
    this->deleteNodeTail(startNode, startSplitPosInBuffer);
    this->_searchCache.validate(offset);
    if (startNode->piece->length == offset)
    {
        nodesToDel.emplace_back(startNode);
    }

    // update last touched node
    auto endSplitPosInBuffer = this->positionInBuffer(endNode, endPosition.remainder);
    this->deleteNodeHead(endNode, endSplitPosInBuffer);
    if (endNode->piece->length == 0)
    {
        nodesToDel.emplace_back(endNode);
    }

    // delete nodes in between
    auto secondNode = startNode->next();
    for (auto node = secondNode; node != SENTINEL && node != endNode; node = node->next())
    {
        nodesToDel.emplace_back(node);
    }

    auto prev = startNode->piece->length == 0 ? startNode->prev() : startNode;
    this->deleteNodes(nodesToDel);
    this->validateCRLFWithNextNode(prev);
    this->computeBufferMetadata();
}

void PieceTreeBase::insertContentToNodeLeft(std::string &value, std::shared_ptr<TreeNode> node)
{
    // we are inserting content to the beginning of node
    auto nodesToDel = std::vector<std::shared_ptr<TreeNode>>();
    if (this->shouldCheckCRLF() && this->endWithCR(value) && this->startWithLF(node))
    {
        // move `\n` to new node.

        auto piece = node->piece;
        auto newStart = BufferCursor(piece->start.line + 1, 0);
        auto nPiece = Piece(piece->bufferIndex, newStart, piece->end,
                            getLineFeedCnt(piece->bufferIndex, newStart, piece->end), piece->length - 1);

        node->piece = std::move(std::make_shared<Piece>(nPiece));

        value += "\n";
        updateTreeMetadata(node, -1, -1);

        if (node->piece->length == 0)
        {
            nodesToDel.emplace_back(node);
        }
    }

    auto newPieces = this->createNewPieces(value);
    if (newPieces.empty())
        return;
    auto newNode = this->rbInsertLeft(node, newPieces[newPieces.size() - 1]);
    for (int k = newPieces.size() - 2; k >= 0; k--)
    {
        newNode = this->rbInsertLeft(newNode, newPieces[k]);
    }
    this->validateCRLFWithPrevNode(newNode);
    this->deleteNodes(nodesToDel);
}

void PieceTreeBase::insertContentToNodeRight(std::string &value, std::shared_ptr<TreeNode> node)
{
    // we are inserting to the right of this node.
    if (this->adjustCarriageReturnFromNext(value, node))
    {
        // move \n to the new node.
        value += "\n";
    }
    auto newPieces = this->createNewPieces(value);
    auto newNode = this->rbInsertRight(node, newPieces[0]);
    auto tmpNode = newNode;

    for (auto k = 1; k < newPieces.size(); k++)
    {
        tmpNode = this->rbInsertRight(tmpNode, newPieces[k]);
    }

    this->validateCRLFWithPrevNode(newNode);
}

BufferCursor PieceTreeBase::positionInBuffer(std::shared_ptr<TreeNode> node, int remainder,
                                             std::shared_ptr<BufferCursor> ret)
{
    auto piece = node->piece;
    auto bufferIndex = piece->bufferIndex;
    auto lineStarts = this->_buffers[bufferIndex].lineStarts;

    auto startOffset = lineStarts[piece->start.line] + piece->start.column;

    auto offset = startOffset + remainder;

    // binary search offset between startOffset and endOffset
    auto low = piece->start.line;
    auto high = piece->end.line;

    int mid = 0;
    int midStop = 0;
    int midStart = 0;

    while (low <= high)
    {
        mid = low + ((high - low) / 2) | 0;
        midStart = lineStarts[mid];
        if (mid == high)
        {
            break;
        }
        midStop = lineStarts[mid + 1];
        if (offset < midStart)
        {
            high = mid - 1;
        }
        else if (offset >= midStop)
        {
            low = mid + 1;
        }
        else
        {
            break;
        }
    }

    if (ret)
    {
        ret->line = mid;
        ret->column = offset - midStart;
        return BufferCursor();
    }
    return BufferCursor(mid, offset - midStart);
}

int PieceTreeBase::getLineFeedCnt(int bufferIndex, const BufferCursor &start, const BufferCursor &end) const
{
    // we don't need to worry about start: abc\r|\n, or abc|\r, or abc|\n, or
    // abc|\r\n doesn't change the fact that, there is one line break after start.
    // now let's take care of end: abc\r|\n, if end is in between \r and \n, we
    // need to add line feed count by 1
    if (end.column == 0)
    {
        return end.line - start.line;
    }
    auto lineStarts = _buffers[bufferIndex].lineStarts;
    if (end.line == lineStarts.size() - 1)
    {
        // it means, there is no \n after end, otherwise, there will be one more
        // lineStart.
        return end.line - start.line;
    }

    auto nextLineStartOffset = lineStarts[end.line + 1];
    auto endOffset = lineStarts[end.line] + end.column;
    if (nextLineStartOffset > endOffset + 1)
    {
        // there are more than 1 character after end, which means it can't be \n
        return end.line - start.line;
    }
    // endOffset + 1 === nextLineStartOffset
    // character at endOffset is \n, so we check the character before first
    // if character at endOffset is \r, end.column is 0 and we can't get here.
    auto previousCharOffset = endOffset - 1; // end.column > 0 so it's okay.
    auto buffer = _buffers[bufferIndex].buffer;

    if (buffer[previousCharOffset] == 13)
    {
        return end.line - start.line + 1;
    }
    else
    {
        return end.line - start.line;
    }
}

int PieceTreeBase::offsetInBuffer(int bufferIndex, const BufferCursor &cursor) const
{
    auto lineStarts = _buffers[bufferIndex].lineStarts;
    return lineStarts[cursor.line] + cursor.column;
}

void PieceTreeBase::deleteNodes(std::vector<std::shared_ptr<TreeNode>> nodes)
{
    for (int i = 0; i < nodes.size(); i++)
    {
        rbDelete(nodes[i]);
    }
}

std::vector<Piece> PieceTreeBase::createNewPieces(const std::string &text)
{
    std::vector<Piece> newPieces;
    std::string currentText = text;
    if (currentText.length() > AverageBufferSize)
    {
        // the content is large, operations like substring, charCode becomes slow
        // so here we split it into smaller chunks, just like what we did for CR/LF
        // normalization

        while (currentText.length() > AverageBufferSize)
        {
            const auto lastChar = currentText[AverageBufferSize - 1];
            std::string splitText;
            if (lastChar == CharCode::CarriageReturn || ((int)lastChar >= 0xD800 && (int)lastChar <= 0xDBFF))
            {
                // last character is \r or a high surrogate => keep it back
                splitText = currentText.substr(0, AverageBufferSize - 1);
                currentText = currentText.substr(AverageBufferSize - 1);
            }
            else
            {
                splitText = currentText.substr(0, AverageBufferSize);
                currentText = currentText.substr(AverageBufferSize);
            }

            auto lineStarts = createLineStartsFast(splitText);
            newPieces.emplace_back(
                Piece(_buffers.size(), BufferCursor(0, 0),
                      BufferCursor(lineStarts.size() - 1, splitText.length() - lineStarts[lineStarts.size() - 1]),
                      lineStarts.size() - 1, splitText.length()));
            _buffers.emplace_back(StringBuffer(splitText, lineStarts));
        }

        auto lineStarts = createLineStartsFast(currentText);
        newPieces.emplace_back(
            Piece(_buffers.size(), BufferCursor(0, 0),
                  BufferCursor(lineStarts.size() - 1, currentText.length() - lineStarts[lineStarts.size() - 1]),
                  lineStarts.size() - 1, currentText.length()));
        _buffers.emplace_back(StringBuffer(currentText, lineStarts));
        return newPieces;
    }

    auto startOffset = _buffers[0].buffer.length();
    auto lineStarts = createLineStartsFast(currentText, false);
    auto start = _lastChangeBufferPos;
    if (_buffers[0].lineStarts[_buffers[0].lineStarts.size() - 1] == startOffset && startOffset != 0 &&
        startWithLF(currentText) && this->endWithCR(_buffers[0].buffer))
    {
        _lastChangeBufferPos = {_lastChangeBufferPos.line, _lastChangeBufferPos.column + 1};
        start = _lastChangeBufferPos;

        for (int i = 0; i < lineStarts.size(); i++)
        {
            lineStarts[i] += startOffset + 1;
        }

        std::vector<int> subVec(lineStarts.begin() + 1, lineStarts.end());
        _buffers[0].lineStarts.insert(_buffers[0].lineStarts.end(), subVec.begin(), subVec.end());
        _buffers[0].buffer += '_' + currentText;
        startOffset += 1;
    }
    else
    {
        if (startOffset != 0)
        {
            for (int i = 0; i < lineStarts.size(); i++)
            {
                lineStarts[i] += startOffset;
            }
        }
        std::vector<int> subVec(lineStarts.begin() + 1, lineStarts.end());
        _buffers[0].lineStarts.insert(_buffers[0].lineStarts.end(), subVec.begin(), subVec.end());
        _buffers[0].buffer += currentText;
    }

    const auto endOffset = _buffers[0].buffer.length();
    auto endIndex = _buffers[0].lineStarts.size() - 1;
    auto endColumn = endOffset - _buffers[0].lineStarts[endIndex];
    auto endPos = BufferCursor(endIndex, endColumn);
    newPieces.emplace_back(Piece(0, /** todo@peng */
                                 start, endPos, getLineFeedCnt(0, start, endPos), endOffset - startOffset));
    _lastChangeBufferPos = endPos;
    return newPieces;
}

std::string PieceTreeBase::getLinesRawContent()
{
    return getContentOfSubTree(root);
}

std::string PieceTreeBase::getLineRawContent(int lineNumber, int endOffset)
{
    auto x = root;

    std::string ret = "";
    auto cache = _searchCache.get2(lineNumber);

    if (cache)
    {
        x = cache->node;

        int prevAccumualtedValue = getAccumulatedValue(x, lineNumber - cache->nodeStartLineNumber - 1);
        auto buffer = _buffers[x->piece->bufferIndex].buffer;
        int startOffset = offsetInBuffer(x->piece->bufferIndex, x->piece->start);
        if (cache->nodeStartLineNumber + x->piece->lineFeedCnt == lineNumber)
        {

            ret = buffer.substr(startOffset + prevAccumualtedValue, x->piece->length - prevAccumualtedValue);
        }
        else
        {
            int accumualtedValue = getAccumulatedValue(x, lineNumber - cache->nodeStartLineNumber);
            return buffer.substr(startOffset + prevAccumualtedValue,
                                 accumualtedValue - endOffset - prevAccumualtedValue);
        }
    }
    else
    {
        int nodeStartOffset = 0;
        const int originalLineNumber = lineNumber;
        while (x != SENTINEL)
        {
            if (x->left != SENTINEL && x->lf_left >= lineNumber - 1)
            {
                x = x->left;
            }
            else if (x->lf_left + x->piece->lineFeedCnt > lineNumber - 1)
            {
                int prevAccumualtedValue = getAccumulatedValue(x, lineNumber - x->lf_left - 2);
                int accumualtedValue = getAccumulatedValue(x, lineNumber - x->lf_left - 1);
                auto buffer = _buffers[x->piece->bufferIndex].buffer;
                int startOffset = offsetInBuffer(x->piece->bufferIndex, x->piece->start);
                nodeStartOffset += x->size_left;
                auto cacheEntry = CacheEntry{x, nodeStartOffset, originalLineNumber - (lineNumber - 1 - x->lf_left)};
                _searchCache.set(cacheEntry);

                return buffer.substr(startOffset + prevAccumualtedValue,
                                     accumualtedValue - endOffset - prevAccumualtedValue);
            }
            else if (x->lf_left + x->piece->lineFeedCnt == lineNumber - 1)
            {
                int prevAccumualtedValue = getAccumulatedValue(x, lineNumber - x->lf_left - 2);
                auto buffer = _buffers[x->piece->bufferIndex].buffer;
                int startOffset = offsetInBuffer(x->piece->bufferIndex, x->piece->start);
                ret = buffer.substr(startOffset + prevAccumualtedValue, x->piece->length - prevAccumualtedValue);
                break;
            }
            else
            {
                lineNumber -= x->lf_left + x->piece->lineFeedCnt;
                nodeStartOffset += x->size_left + x->piece->length;
                x = x->right;
            }
        }
    }

    x = x->next();

    while (x && x != SENTINEL)
    {
        auto buffer = _buffers[x->piece->bufferIndex].buffer;
        if (x->piece->lineFeedCnt > 0)
        {
            auto accumualtedValue = getAccumulatedValue(x, 0);
            auto startOffset = offsetInBuffer(x->piece->bufferIndex, x->piece->start);
            ret += buffer.substr(startOffset, accumualtedValue - endOffset);
            return ret;
        }
        else
        {
            auto startOffset = offsetInBuffer(x->piece->bufferIndex, x->piece->start);
            ret += buffer.substr(startOffset, x->piece->length);
        }
        x = x->next();
    }
    return ret;
}

void PieceTreeBase::computeBufferMetadata()
{
    auto x = root;

    int lfCnt = 1;
    int len = 0;

    while (x != SENTINEL)
    {

        lfCnt += x->lf_left + x->piece->lineFeedCnt;
        len += x->size_left + x->piece->length;

        x = x->right;
    }

    _lineCnt = lfCnt;
    _length = len;
    _searchCache.validate(_length);
}

std::tuple<int, int> PieceTreeBase::getIndexOf(std::shared_ptr<TreeNode> node, int accumulatedValue)
{
    auto piece = node->piece;
    auto pos = positionInBuffer(node, accumulatedValue);
    auto lineCnt = pos.line - piece->start.line;
    if (offsetInBuffer(piece->bufferIndex, piece->end) - offsetInBuffer(piece->bufferIndex, piece->start) ==
        accumulatedValue)
    {
        // we are checking the end of this node, so a CRLF check is necessary.
        auto realLineCnt = getLineFeedCnt(piece->bufferIndex, piece->start, pos);
        if (realLineCnt != lineCnt)
        {
            // aha yes, CRLF
            return {realLineCnt, 0};
        }
    }
    return {lineCnt, pos.column};
}

int PieceTreeBase::getAccumulatedValue(std::shared_ptr<TreeNode> node, int index)
{
    if (index < 0)
    {
        return 0;
    }
    auto piece = node->piece;
    auto lineStarts = _buffers[piece->bufferIndex].lineStarts;
    auto expectedLineStartIndex = piece->start.line + index + 1;
    if (expectedLineStartIndex > piece->end.line)
    {
        return lineStarts[piece->end.line] + piece->end.column - lineStarts[piece->start.line] - piece->start.column;
    }
    else
    {
        return lineStarts[expectedLineStartIndex] - lineStarts[piece->start.line] - piece->start.column;
    }
}

void PieceTreeBase::deleteNodeTail(std::shared_ptr<TreeNode> node, const BufferCursor &pos)
{
    const auto piece = node->piece;
    const int originalLFCnt = piece->lineFeedCnt;
    const int originalEndOffset = offsetInBuffer(piece->bufferIndex, piece->end);

    const auto newEnd = pos;
    const int newEndOffset = offsetInBuffer(piece->bufferIndex, newEnd);
    const int newLineFeedCnt = getLineFeedCnt(piece->bufferIndex, piece->start, newEnd);

    const int lf_delta = newLineFeedCnt - originalLFCnt;
    const int size_delta = newEndOffset - originalEndOffset;
    const int newLength = piece->length + size_delta;

    node->piece =
        std::move(std::make_shared<Piece>(piece->bufferIndex, piece->start, newEnd, newLineFeedCnt, newLength));

    updateTreeMetadata(node, size_delta, lf_delta);
}

void PieceTreeBase::deleteNodeHead(std::shared_ptr<TreeNode> node, const BufferCursor &pos)
{
    const auto piece = node->piece;
    const int originalLFCnt = piece->lineFeedCnt;
    const int originalStartOffset = offsetInBuffer(piece->bufferIndex, piece->start);

    const auto newStart = pos;
    const int newLineFeedCnt = getLineFeedCnt(piece->bufferIndex, newStart, piece->end);
    const int newStartOffset = offsetInBuffer(piece->bufferIndex, newStart);
    const int lf_delta = newLineFeedCnt - originalLFCnt;
    const int size_delta = originalStartOffset - newStartOffset;
    const int newLength = piece->length + size_delta;

    node->piece = std::make_shared<Piece>(piece->bufferIndex, newStart, piece->end, newLineFeedCnt, newLength);

    updateTreeMetadata(node, size_delta, lf_delta);
}

void PieceTreeBase::shrinkNode(std::shared_ptr<TreeNode> node, const BufferCursor &start, const BufferCursor &end)
{
    const auto piece = node->piece;
    const auto originalStartPos = piece->start;
    const auto originalEndPos = piece->end;

    // old piece, originalStartPos, start
    const auto oldLength = piece->length;
    const auto oldLFCnt = piece->lineFeedCnt;
    const auto newEnd = start;
    const auto newLineFeedCnt = getLineFeedCnt(piece->bufferIndex, piece->start, newEnd);
    const auto newLength =
        offsetInBuffer(piece->bufferIndex, start) - offsetInBuffer(piece->bufferIndex, originalStartPos);

    node->piece =
        std::move(std::make_shared<Piece>(piece->bufferIndex, piece->start, newEnd, newLineFeedCnt, newLength));

    updateTreeMetadata(node, newLength - oldLength, newLineFeedCnt - oldLFCnt);

    // new right piece, end, originalEndPos
    auto newPiece =
        Piece(piece->bufferIndex, end, originalEndPos, getLineFeedCnt(piece->bufferIndex, end, originalEndPos),
              offsetInBuffer(piece->bufferIndex, originalEndPos) - offsetInBuffer(piece->bufferIndex, end));
    auto newNode = rbInsertRight(node, newPiece);
    validateCRLFWithPrevNode(newNode);
}

void PieceTreeBase::appendToNode(std::shared_ptr<TreeNode> node, std::string &value)
{
    if (adjustCarriageReturnFromNext(value, node))
    {
        value += '\n';
    }

    const auto hitCRLF = shouldCheckCRLF() && startWithLF(value) && endWithCR(node);
    const int startOffset = _buffers[0].buffer.length();
    _buffers[0].buffer += value;
    auto lineStarts = createLineStartsFast(value, false);
    for (int i = 0; i < lineStarts.size(); i++)
    {
        lineStarts[i] += startOffset;
    }
    if (hitCRLF)
    {
        auto prevStartOffset = _buffers[0].lineStarts[_buffers[0].lineStarts.size() - 2];
        _buffers[0].lineStarts.pop_back();
        // _lastChangeBufferPos is already wrong
        _lastChangeBufferPos = {_lastChangeBufferPos.line - 1, startOffset - prevStartOffset};
    }

    std::vector<int> vecSub(lineStarts.begin() + 1, lineStarts.end());
    _buffers[0].lineStarts.insert(_buffers[0].lineStarts.end(), vecSub.begin(), vecSub.end());
    const auto endIndex = _buffers[0].lineStarts.size() - 1;
    const auto endColumn = _buffers[0].buffer.length() - _buffers[0].lineStarts[endIndex];
    const auto newEnd = BufferCursor(endIndex, endColumn);
    const auto newLength = node->piece->length + value.length();
    const auto oldLineFeedCnt = node->piece->lineFeedCnt;
    const auto newLineFeedCnt = getLineFeedCnt(0, node->piece->start, newEnd);
    const auto lf_delta = newLineFeedCnt - oldLineFeedCnt;

    node->piece = std::move(
        std::make_shared<Piece>(node->piece->bufferIndex, node->piece->start, newEnd, newLineFeedCnt, newLength));

    _lastChangeBufferPos = newEnd;
    updateTreeMetadata(node, value.length(), lf_delta);
}

NodePosition PieceTreeBase::nodeAt(int offset)
{
    auto x = root;
    auto cache = _searchCache.get(offset);
    if (cache)
    {
        return {cache->node, offset - cache->nodeStartOffset, cache->nodeStartOffset};
    }

    int nodeStartOffset = 0;
    while (x != SENTINEL)
    {
        if (x->size_left > offset)
        {
            x = x->left;
        }
        else if (x->size_left + x->piece->length >= offset)
        {
            nodeStartOffset += x->size_left;
            auto ret = NodePosition{x, offset - x->size_left, nodeStartOffset};
            _searchCache.set(CacheEntry{x, nodeStartOffset, 0});
            return ret;
        }
        else
        {
            offset -= x->size_left + x->piece->length;
            nodeStartOffset += x->size_left + x->piece->length;
            x = x->right;
        }
    }
    return NodePosition();
}

NodePosition PieceTreeBase::nodeAt2(int lineNumber, int column)
{
    auto x = root;
    int nodeStartOffset = 0;

    while (x != SENTINEL)
    {
        if (x->left != SENTINEL && x->lf_left >= lineNumber - 1)
        {
            x = x->left;
        }
        else if (x->lf_left + x->piece->lineFeedCnt > lineNumber - 1)
        {
            auto prevAccumualtedValue = getAccumulatedValue(x, lineNumber - x->lf_left - 2);
            auto accumualtedValue = getAccumulatedValue(x, lineNumber - x->lf_left - 1);
            nodeStartOffset += x->size_left;

            return {x, std::min(prevAccumualtedValue + column - 1, (int)accumualtedValue), nodeStartOffset};
        }
        else if (x->lf_left + x->piece->lineFeedCnt == lineNumber - 1)
        {
            auto prevAccumualtedValue = getAccumulatedValue(x, lineNumber - x->lf_left - 2);
            if (prevAccumualtedValue + column - 1 <= x->piece->length)
            {
                return {x, prevAccumualtedValue + column - 1, nodeStartOffset};
            }
            else
            {
                column -= x->piece->length - prevAccumualtedValue;
                break;
            }
        }
        else
        {
            lineNumber -= x->lf_left + x->piece->lineFeedCnt;
            nodeStartOffset += x->size_left + x->piece->length;
            x = x->right;
        }
    }
    // search in order, to find the node contains position.column
    x = x->next();
    while (x != SENTINEL)
    {
        if (x->piece->lineFeedCnt > 0)
        {
            auto accumualtedValue = getAccumulatedValue(x, 0);
            auto nodeStartOffset = offsetOfNode(x);
            return {x, std::min(column - 1, (int)accumualtedValue), nodeStartOffset};
        }
        else
        {
            if (x->piece->length >= column - 1)
            {
                auto nodeStartOffset = offsetOfNode(x);
                return {x, column - 1, nodeStartOffset};
            }
            else
            {
                column -= x->piece->length;
            }
        }
        x = x->next();
    }
    return NodePosition();
}

int PieceTreeBase::nodeCharCodeAt(std::shared_ptr<TreeNode> node, int offset)
{
    if (node->piece->lineFeedCnt < 1)
    {
        return -1;
    }
    auto buffer = _buffers[node->piece->bufferIndex];
    auto newOffset = offsetInBuffer(node->piece->bufferIndex, node->piece->start) + offset;
    return buffer.buffer[newOffset];
}

int PieceTreeBase::offsetOfNode(std::shared_ptr<TreeNode> node)
{
    if (!node)
    {
        return 0;
    }
    auto pos = node->size_left;
    while (node != root)
    {
        if (node->parent.lock()->right == node)
        {
            pos += node->parent.lock()->size_left + node->parent.lock()->piece->length;
        }

        node = node->parent.lock();
    }
    return pos;
}

// #endregion

// #region CRLF
bool PieceTreeBase::shouldCheckCRLF()
{
    return !(_EOLNormalized && _EOL == "\n");
}

bool PieceTreeBase::startWithLF(const std::string &val)
{
    return val[0] == 10;
}

bool PieceTreeBase::startWithLF(std::shared_ptr<TreeNode> val)
{
    if (val == SENTINEL || val->piece->lineFeedCnt == 0)
    {
        return false;
    }

    auto piece = val->piece;
    auto lineStarts = _buffers[piece->bufferIndex].lineStarts;
    auto line = piece->start.line;
    auto startOffset = lineStarts[line] + piece->start.column;
    if (line == lineStarts.size() - 1)
    {
        // last line, so there is no line feed at the end of this line
        return false;
    }
    auto nextLineOffset = lineStarts[line + 1];
    if (nextLineOffset > startOffset + 1)
    {
        return false;
    }
    return _buffers[piece->bufferIndex].buffer[startOffset] == 10;
}

bool PieceTreeBase::endWithCR(const std::string &val)
{
    return val[val.length() - 1] == 13;
}

bool PieceTreeBase::endWithCR(std::shared_ptr<TreeNode> val)
{
    if (val == SENTINEL || val->piece->lineFeedCnt == 0)
    {
        return false;
    }
    return nodeCharCodeAt(val, val->piece->length - 1) == 13;
}

void PieceTreeBase::validateCRLFWithPrevNode(std::shared_ptr<TreeNode> nextNode)
{
    if (shouldCheckCRLF() && startWithLF(nextNode))
    {
        auto node = nextNode->prev();
        if (endWithCR(node))
        {
            fixCRLF(node, nextNode);
        }
    }
}

void PieceTreeBase::validateCRLFWithNextNode(std::shared_ptr<TreeNode> node)
{
    if (shouldCheckCRLF() && endWithCR(node))
    {
        auto nextNode = node->next();
        if (startWithLF(nextNode))
        {
            fixCRLF(node, nextNode);
        }
    }
}

void PieceTreeBase::fixCRLF(std::shared_ptr<TreeNode> prev, std::shared_ptr<TreeNode> next)
{
    std::vector<std::shared_ptr<TreeNode>> nodesToDel;
    // update node
    auto lineStarts = _buffers[prev->piece->bufferIndex].lineStarts;
    auto newEnd = BufferCursor();
    if (prev->piece->end.column == 0)
    {
        // it means, last line ends with \r, not \r\n
        newEnd = {prev->piece->end.line - 1,
                  lineStarts[prev->piece->end.line] - lineStarts[prev->piece->end.line - 1] - 1};
    }
    else
    {
        // \r\n
        newEnd = {prev->piece->end.line, prev->piece->end.column - 1};
    }
    const auto prevNewLength = prev->piece->length - 1;
    const auto prevNewLFCnt = prev->piece->lineFeedCnt - 1;
    prev->piece = std::move(
        std::make_shared<Piece>(prev->piece->bufferIndex, prev->piece->start, newEnd, prevNewLFCnt, prevNewLength));

    updateTreeMetadata(prev, -1, -1);
    if (prev->piece->length == 0)
    {
        nodesToDel.emplace_back(prev);
    }

    // update nextNode
    auto newStart = BufferCursor(next->piece->start.line + 1, 0);
    const auto newLength = next->piece->length - 1;
    const auto newLineFeedCnt = getLineFeedCnt(next->piece->bufferIndex, newStart, next->piece->end);
    next->piece = std::move(
        std::make_shared<Piece>(next->piece->bufferIndex, newStart, next->piece->end, newLineFeedCnt, newLength));

    updateTreeMetadata(next, -1, -1);
    if (next->piece->length == 0)
    {
        nodesToDel.emplace_back(next);
    }
    // create new piece with contains \r\n
    auto pieces = createNewPieces("\r\n");
    rbInsertRight(prev, pieces[0]);
    // delete empty nodes
    for (auto node : nodesToDel)
    {
        rbDelete(node);
    }
}

bool PieceTreeBase::adjustCarriageReturnFromNext(std::string &value, std::shared_ptr<TreeNode> node)
{
    if (shouldCheckCRLF() && endWithCR(node) && startWithLF(value))
    {
        auto nextNode = node->next();
        if (startWithLF(nextNode))
        {
            // move `\n` forward
            value += "\n";

            if (nextNode->piece->length == 1)
            {
                rbDelete(nextNode);
            }
            else
            {
                const auto piece = nextNode->piece;
                const auto newStart = BufferCursor(piece->start.line + 1, 0);
                const auto newLength = piece->length - 1;
                const auto newLineFeedCnt = getLineFeedCnt(piece->bufferIndex, newStart, piece->end);
                nextNode->piece = std::move(
                    std::make_shared<Piece>(piece->bufferIndex, newStart, piece->end, newLineFeedCnt, newLength));

                updateTreeMetadata(nextNode, -1, -1);
            }
            return true;
        }
    }
    return false;
}

// #endregion

// #region Tree operations
bool PieceTreeBase::iterate(std::shared_ptr<TreeNode> node, std::function<bool(std::shared_ptr<TreeNode>)> callback)
{
    if (node == SENTINEL)
    {
        return callback(SENTINEL);
    }
    auto leftRet = iterate(node->left, callback);
    if (!leftRet)
    {
        return leftRet;
    }
    return callback(node) && iterate(node->right, callback);
}

std::string PieceTreeBase::getNodeContent(std::shared_ptr<TreeNode> node)
{
    if (node == SENTINEL)
    {
        return "";
    }
    auto buffer = _buffers[node->piece->bufferIndex];
    std::string currentContent;
    auto piece = node->piece;
    auto startOffset = offsetInBuffer(piece->bufferIndex, piece->start);
    auto endOffset = offsetInBuffer(piece->bufferIndex, piece->end);
    currentContent = buffer.buffer.substr(startOffset, endOffset - startOffset);
    return currentContent;
}

std::string PieceTreeBase::getPieceContent(const Piece &piece)
{
    auto buffer = _buffers[piece.bufferIndex];
    auto startOffset = offsetInBuffer(piece.bufferIndex, piece.start);
    auto endOffset = offsetInBuffer(piece.bufferIndex, piece.end);
    auto currentContent = buffer.buffer.substr(startOffset, endOffset - startOffset);
    return currentContent;
}

std::shared_ptr<TreeNode> PieceTreeBase::rbInsertRight(std::shared_ptr<TreeNode> node, const Piece &p)
{
    auto z = std::make_shared<TreeNode>(std::make_shared<Piece>(p), NodeColor::Red);
    z->left = SENTINEL;
    z->right = SENTINEL;
    z->parent = SENTINEL;
    z->size_left = 0;
    z->lf_left = 0;

    auto x = root;

    if (x == SENTINEL)
    {
        root = z;
        z->color = NodeColor::Black;
    }
    else if (node->right == SENTINEL)
    {
        node->right = z;
        z->parent = node;
    }
    else
    {
        auto nextNode = leftest(node->right);
        nextNode->left = z;
        z->parent = nextNode;
    }
    fixInsert(z);
    return z;
}

std::shared_ptr<TreeNode> PieceTreeBase::rbInsertLeft(std::shared_ptr<TreeNode> node, const Piece &p)
{
    auto z = std::make_shared<TreeNode>(std::make_shared<Piece>(p), NodeColor::Red);
    z->left = SENTINEL;
    z->right = SENTINEL;
    z->parent = SENTINEL;
    z->size_left = 0;
    z->lf_left = 0;

    if (root == SENTINEL || node == nullptr)
    {
        root = z;
        z->color = NodeColor::Black;
    }
    else if (node->left == SENTINEL)
    {
        node->left = z;
        z->parent = node;
    }
    else
    {
        auto prevNode = rightest(node->left);
        prevNode->right = z;
        z->parent = prevNode;
    }
    fixInsert(z);
    return z;
}

std::string PieceTreeBase::getContentOfSubTree(std::shared_ptr<TreeNode> node)
{
    if (node == SENTINEL)
    {
        return "";
    }
    std::string str;
    str.reserve(1024 * 4);
    iterate(node, [&](std::shared_ptr<TreeNode> node) {
        str += getNodeContent(node);
        return true;
    });
    return str;
}

// More implementation details would follow...
// The file is quite large, so I've covered the main structure and key
// functions.

std::vector<std::string> regex_split(const std::string &str, const std::regex &regex)
{
    std::vector<std::string> result;

    std::sregex_token_iterator iter(str.begin(), str.end(), regex, -1);
    std::sregex_token_iterator end;

    while (iter != end)
    {
        result.push_back(*iter);
        ++iter;
    }
    return result;
}

// Tree rotation functions
void PieceTreeBase::leftRotate(std::shared_ptr<TreeNode> x)
{
    auto y = x->right;

    // fix size_left
    y->size_left += x->size_left + (x->piece ? x->piece->length : 0);
    y->lf_left += x->lf_left + (x->piece ? x->piece->lineFeedCnt : 0);
    x->right = y->left;

    if (y->left != SENTINEL)
    {
        y->left->parent.lock() = x;
    }
    auto xParent = x->parent;
    y->parent = xParent;
    auto parent = xParent.lock();

    if (parent == SENTINEL)
    {
        root = y;
    }
    else if (x == parent->left)
    {
        parent->left = y;
    }
    else
    {
        parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

void PieceTreeBase::rightRotate(std::shared_ptr<TreeNode> y)
{
    auto x = y->left;
    y->left = x->right;

    if (x->right != SENTINEL)
    {
        x->right->parent.lock() = y;
    }
    auto yParent = y->parent;
    x->parent = yParent;

    // fix size_left
    y->size_left -= x->size_left + (x->piece->length ? x->piece->length : 0);
    y->lf_left -= x->lf_left + (x->piece->length ? x->piece->lineFeedCnt : 0);

    auto parent = yParent.lock();
    if (parent == SENTINEL)
    {
        root = x;
    }
    else if (y == parent->right)
    {
        parent->right = x;
    }
    else
    {
        parent->left = x;
    }

    x->right = y;
    y->parent = x;
}

void PieceTreeBase::rbDelete(std::shared_ptr<TreeNode> z)
{
    std::shared_ptr<TreeNode> x;
    std::shared_ptr<TreeNode> y;

    if (z->left == SENTINEL)
    {
        y = z;
        x = y->right;
    }
    else if (z->right == SENTINEL)
    {
        y = z;
        x = y->left;
    }
    else
    {
        y = leftest(z->right);
        x = y->right;
    }

    if (y == root)
    {
        root = x;

        // if x is null, we are removing the only node
        x->color = NodeColor::Black;
        z->detach();
        resetSentinel();
        root->parent = SENTINEL;

        return;
    }

    bool yWasRed = (y->color == NodeColor::Red);
    if (y == y->parent.lock()->left)
    {
        y->parent.lock()->left = x;
    }
    else
    {
        y->parent.lock()->right = x;
    }
    if (y == z)
    {
        x->parent = y->parent;
        recomputeTreeMetadata(x);
    }
    else
    {
        if (y->parent.lock() == z)
        {
            x->parent = y;
        }
        else
        {
            x->parent = y->parent;
        }

        // as we make changes to x's hierarchy, update size_left of subtree first
        recomputeTreeMetadata(x);

        y->left = z->left;
        y->right = z->right;
        y->parent = z->parent;
        y->color = z->color;

        if (z == root)
        {
            root = y;
        }
        else
        {
            if (z == z->parent.lock()->left)
            {
                z->parent.lock()->left = y;
            }
            else
            {
                z->parent.lock()->right = y;
            }
        }

        if (y->left != SENTINEL)
        {
            y->left->parent.lock() = y;
        }
        if (y->right != SENTINEL)
        {
            y->right->parent.lock() = y;
        }

        // update metadata
        // we replace z with y, so in this sub tree, the length change is
        // z.item.length
        y->size_left = z->size_left;
        y->lf_left = z->lf_left;
        recomputeTreeMetadata(y);
    }

    z->detach();

    if (x->parent.lock()->left == x)
    {
        int newSizeLeft = calculateSize(x);
        int newLFLeft = calculateLF(x);
        if (newSizeLeft != x->parent.lock()->size_left || newLFLeft != x->parent.lock()->lf_left)
        {
            int delta = newSizeLeft - x->parent.lock()->size_left;
            int lf_delta = newLFLeft - x->parent.lock()->lf_left;
            x->parent.lock()->size_left = newSizeLeft;
            x->parent.lock()->lf_left = newLFLeft;
            updateTreeMetadata(x->parent.lock(), delta, lf_delta);
        }
    }

    recomputeTreeMetadata(x->parent.lock());

    if (yWasRed)
    {
        resetSentinel();
        return;
    }

    // RB-DELETE-FIXUP
    std::shared_ptr<TreeNode> w;
    while (x != root && x->color == NodeColor::Black)
    {

        if (x == x->parent.lock()->left)
        {
            w = x->parent.lock()->right;

            if (w->color == NodeColor::Red)
            {
                w->color = NodeColor::Black;
                x->parent.lock()->color = NodeColor::Red;
                leftRotate(x->parent.lock());
                w = x->parent.lock()->right;
            }

            if (w->left->color == NodeColor::Black && w->right->color == NodeColor::Black)
            {
                w->color = NodeColor::Red;
                x = x->parent.lock();
            }
            else
            {
                if (w->right->color == NodeColor::Black)
                {
                    w->left->color = NodeColor::Black;
                    w->color = NodeColor::Red;
                    rightRotate(w);
                    w = x->parent.lock()->right;
                }

                w->color = x->parent.lock()->color;
                x->parent.lock()->color = NodeColor::Black;
                w->right->color = NodeColor::Black;
                leftRotate(x->parent.lock());
                x = root;
            }
        }
        else
        {
            w = x->parent.lock()->left;

            if (w->color == NodeColor::Red)
            {
                w->color = NodeColor::Black;
                x->parent.lock()->color = NodeColor::Red;
                rightRotate(x->parent.lock());
                w = x->parent.lock()->left;
            }

            if (w->left->color == NodeColor::Black && w->right->color == NodeColor::Black)
            {
                w->color = NodeColor::Red;
                x = x->parent.lock();
            }
            else
            {
                if (w->left->color == NodeColor::Black)
                {
                    w->right->color = NodeColor::Black;
                    w->color = NodeColor::Red;
                    leftRotate(w);
                    w = x->parent.lock()->left;
                }

                w->color = x->parent.lock()->color;
                x->parent.lock()->color = NodeColor::Black;
                w->left->color = NodeColor::Black;
                rightRotate(x->parent.lock());
                x = root;
            }
        }
    }

    x->color = NodeColor::Black;
    resetSentinel();
}

void PieceTreeBase::fixInsert(std::shared_ptr<TreeNode> x)
{
    recomputeTreeMetadata(x);

    std::shared_ptr<TreeNode> uncle;
    std::shared_ptr<TreeNode> parent = x->parent.lock();
    std::shared_ptr<TreeNode> grandParent;
    while (parent && parent->color == NodeColor::Red && x != root)
    {
        if (!grandParent)
            break;

        if (grandParent->left == parent)
        {
            uncle = grandParent->right;
            if (uncle && uncle->color == NodeColor::Red)
            {
                // Case 1: 叔节点也是红色
                parent->color = NodeColor::Black;
                uncle->color = NodeColor::Black;
                grandParent->color = NodeColor::Red;
                x = grandParent;
                parent = x->parent.lock();
            }
            else
            {
                if (x == parent->right)
                {
                    // Case 2: 左右结构，先左旋父节点
                    x = parent;
                    leftRotate(x);
                    parent = x->parent.lock();
                    grandParent = parent->parent.lock();
                }
                // Case 3: 父节点是黑色，右旋左父节点
                parent->color = NodeColor::Black;
                grandParent->color = NodeColor::Red;
                rightRotate(grandParent);
            }
        }
        else
        {
            uncle = grandParent->left;
            if (uncle && uncle->color == NodeColor::Red)
            {
                // Case 1: 叔节点也是红色
                parent->color = NodeColor::Black;
                uncle->color = NodeColor::Black;
                grandParent->color = NodeColor::Red;
                x = grandParent;
                parent = x->parent.lock();
            }
            else
            {
                if (x == parent->left)
                {
                    // Case 2: 左右结构，先右旋父节点
                    x = parent;
                    rightRotate(x);
                    parent = x->parent.lock();
                    grandParent = parent->parent.lock();
                }
                // Case 3: 父节点是黑色，左旋左父节点
                parent->color = NodeColor::Black;
                grandParent->color = NodeColor::Red;
                leftRotate(grandParent);
            }
        }
    }
    root->color = NodeColor::Black;
}

void PieceTreeBase::updateTreeMetadata(std::shared_ptr<TreeNode> x, int delta, int lineFeedCntDelta)
{
    // node length change or line feed count change
    while (x != root && x != SENTINEL)
    {
        if (x->parent.lock()->left == x)
        {
            x->parent.lock()->size_left += delta;
            x->parent.lock()->lf_left += lineFeedCntDelta;
        }
        x = x->parent.lock();
    }
}

void PieceTreeBase::recomputeTreeMetadata(std::shared_ptr<TreeNode> x)
{
    int delta = 0;
    int lf_delta = 0;

    if (x == root)
    {
        return;
    }
    if (delta == 0)
    {

        while (x != root && x == x->parent.lock()->right)
        {
            // if (parent_lock == SENTINEL) {
            //   break;
            // }
            x = x->parent.lock();
        }

        if (x == root)
        {
            // well, it means we add a node to the end (inorder)
            return;
        }

        // x is the node whose right subtree is changed.
        x = x->parent.lock();

        delta = calculateSize(x->left) - x->size_left;
        lf_delta = calculateLF(x->left) - x->lf_left;
        x->size_left += delta;
        x->lf_left += lf_delta;
    }

    // go upwards till root. O(logN)
    while (x != root && (delta != 0 || lf_delta != 0))
    {

        if (x->parent.lock()->left == x)
        {
            x->parent.lock()->size_left += delta;
            x->parent.lock()->lf_left += lf_delta;
        }

        x = x->parent.lock();
    }
}

} // namespace buffer
