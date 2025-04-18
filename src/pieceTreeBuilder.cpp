/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for
 *license information.
 *--------------------------------------------------------------------------------------------*/

#include "pieceTreeBuilder.h"
#include "pieceTreeBase.h"
#include <memory>
#include <regex>

namespace buffer
{

bool startsWithUTF8BOM(const std::string &str)
{
    return !str.empty() && static_cast<uint16_t>(str[0]) == CharCode::UTF8_BOM;
}

// PieceTreeTextBufferFactory implementation
PieceTreeTextBufferFactory::PieceTreeTextBufferFactory(const std::vector<StringBuffer> &chunks, const std::string &bom,
                                                       int cr, int lf, int crlf, bool normalizeEOL)
    : _chunks(chunks), _bom(bom), _cr(cr), _lf(lf), _crlf(crlf), _normalizeEOL(normalizeEOL),
      std::enable_shared_from_this<PieceTreeTextBufferFactory>()
{
}

std::string PieceTreeTextBufferFactory::_getEOL(DefaultEndOfLine defaultEOL) const
{
    const int totalEOLCount = _cr + _lf + _crlf;
    const int totalCRCount = _cr + _crlf;

    if (totalEOLCount == 0)
    {
        // This is an empty file or a file with precisely one line
        return (defaultEOL == DefaultEndOfLine::LF ? "\n" : "\r\n");
    }

    if (totalCRCount > totalEOLCount / 2)
    {
        // More than half of the file contains \r\n ending lines
        return "\r\n";
    }

    // At least one line more ends in \n
    return "\n";
}

std::shared_ptr<PieceTreeBase> PieceTreeTextBufferFactory::create(DefaultEndOfLine defaultEOL)
{
    auto eol = _getEOL(defaultEOL);
    auto chunks = _chunks;

    // Need to normalize line endings
    if (_normalizeEOL && ((eol == "\r\n" && (_cr > 0 || _lf > 0)) || (eol == "\n" && (_cr > 0 || _crlf > 0))))
    {

        // Normalize pieces
        for (int i = 0, len = chunks.size(); i < len; i++)
        {
            std::string str = std::regex_replace(chunks[i].buffer, std::regex("\r\n|\r|\n", std::regex::icase), eol);
            auto newLineStart = createLineStartsFast(str);
            chunks[i] = StringBuffer(str, newLineStart);
        }
    }

    return std::move(std::make_shared<PieceTreeBase>(chunks, eol, _normalizeEOL));
}

std::string PieceTreeTextBufferFactory::getFirstLineText(int lengthLimit)
{
    if (_chunks.empty() || _chunks[0].buffer.empty())
    {
        return "";
    }

    std::string firstLine =
        _chunks[0].buffer.substr(0, std::min(lengthLimit, static_cast<int>(_chunks[0].buffer.length())));

    // Find first line break
    int lineBreakPos = firstLine.find_first_of("\r\n");
    if (lineBreakPos != std::string::npos)
    {
        firstLine = firstLine.substr(0, lineBreakPos);
    }

    return firstLine;
}

// PieceTreeTextBufferBuilder implementation
PieceTreeTextBufferBuilder::PieceTreeTextBufferBuilder()
    : chunks(), BOM(), _hasPreviousChar(false), _previousChar(0), _tmpLineStarts(), cr(0), lf(0), crlf(0),
      std::enable_shared_from_this<PieceTreeTextBufferBuilder>()
{
}

void PieceTreeTextBufferBuilder::acceptChunk(const std::string &chunk)
{
    if (chunk.empty())
    {
        return;
    }

    if (chunks.empty())
    {
        if (startsWithUTF8BOM(chunk))
        {
            BOM = UTF8_BOM_CHARACTER;
            _acceptChunk1(chunk.substr(1), false);
        }
        else
        {
            _acceptChunk1(chunk, false);
        }
    }
    else
    {
        _acceptChunk1(chunk, false);
    }
}

void PieceTreeTextBufferBuilder::_acceptChunk1(const std::string &chunk, bool allowEmptyStrings)
{
    if (!allowEmptyStrings && chunk.empty())
    {
        // Nothing to do
        return;
    }

    if (_hasPreviousChar)
    {
        std::string combinedChunk = std::string(1, static_cast<char>(_previousChar)) + chunk;
        _acceptChunk2(combinedChunk);
    }
    else
    {
        _acceptChunk2(chunk);
    }
}

void PieceTreeTextBufferBuilder::_acceptChunk2(const std::string &chunk)
{
    auto lineStarts = createLineStarts(chunk);

    chunks.push_back(StringBuffer(chunk, lineStarts.lineStarts));
    cr += lineStarts.cr;
    lf += lineStarts.lf;
    crlf += lineStarts.crlf;
}

std::shared_ptr<PieceTreeTextBufferFactory> PieceTreeTextBufferBuilder::finish(bool normalizeEOL)
{
    _finish();

    return std::make_shared<PieceTreeTextBufferFactory>(chunks, BOM, cr, lf, crlf, normalizeEOL);
}

void PieceTreeTextBufferBuilder::_finish()
{
    if (chunks.empty())
    {
        _acceptChunk1("", true);
    }

    if (_hasPreviousChar)
    {
        _hasPreviousChar = false;

        // Recreate last chunk
        int lastChunkIndex = chunks.size() - 1;
        std::string newBuffer = chunks[lastChunkIndex].buffer + static_cast<char>(_previousChar);

        auto newLineStarts = createLineStartsFast(newBuffer);
        chunks[lastChunkIndex] = StringBuffer(newBuffer, newLineStarts);

        if (_previousChar == CharCode::CarriageReturn)
        {
            cr++;
        }
    }
}

} // namespace buffer