/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for
 *license information.
 *--------------------------------------------------------------------------------------------*/

#pragma once

#include "common/charCode.h"
#include "pieceTreeBase.h"
#include <memory>
#include <string>
#include <vector>

namespace buffer
{

// UTF8 BOM character
inline const std::string UTF8_BOM_CHARACTER = std::string(1, static_cast<char>(CharCode::UTF8_BOM));

/**
 * Check if a string starts with the UTF8 BOM character
 */
bool startsWithUTF8BOM(const std::string &str);

/**
 * End-of-line preferences
 */
enum class DefaultEndOfLine
{
    /**
     * Use line feed (\n) as the end of line character.
     */
    LF = 1,
    /**
     * Use carriage return and line feed (\r\n) as the end of line character.
     */
    CRLF = 2
};

/**
 * Factory for creating PieceTreeBase instances
 */
class PieceTreeTextBufferFactory : public std::enable_shared_from_this<PieceTreeTextBufferFactory>
{
  private:
    std::vector<StringBuffer> _chunks;
    std::string _bom;
    int _cr;
    int _lf;
    int _crlf;
    bool _normalizeEOL;

    /**
     * Determine the most appropriate EOL style based on content and preferences
     */
    std::string _getEOL(DefaultEndOfLine defaultEOL) const;

  public:
    /**
     * Constructor
     */
    PieceTreeTextBufferFactory(const std::vector<StringBuffer> &chunks, const std::string &bom, int cr, int lf,
                               int crlf, bool normalizeEOL);

    /**
     * Create a new PieceTreeBase
     */
    std::shared_ptr<PieceTreeBase> create(DefaultEndOfLine defaultEOL);

    /**
     * Get the text of the first line, up to a specified length
     */
    std::string getFirstLineText(int lengthLimit);
};

/**
 * Builder for creating PieceTreeTextBufferFactory instances
 */
class PieceTreeTextBufferBuilder : public std::enable_shared_from_this<PieceTreeTextBufferBuilder>
{
  private:
    std::vector<StringBuffer> chunks;
    std::string BOM;

    bool _hasPreviousChar;
    uint16_t _previousChar;
    std::vector<int> _tmpLineStarts;

    int cr;
    int lf;
    int crlf;

    /**
     * Accept a chunk with additional processing
     */
    void _acceptChunk1(const std::string &chunk, bool allowEmptyStrings);

    /**
     * Final processing step for chunk acceptance
     */
    void _acceptChunk2(const std::string &chunk);

    /**
     * Complete the building process by finalizing any pending chunks
     */
    void _finish();

  public:
    /**
     * Constructor
     */
    PieceTreeTextBufferBuilder();

    /**
     * Add a chunk of text to the builder
     */
    void acceptChunk(const std::string &chunk);

    /**
     * Finish building and return the factory
     */
    std::shared_ptr<PieceTreeTextBufferFactory> finish(bool normalizeEOL = true);
};

} // namespace buffer