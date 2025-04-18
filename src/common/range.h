/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for
 *license information.
 *--------------------------------------------------------------------------------------------*/

#pragma once

#include "position.h"
#include <string>

namespace buffer {

/**
 * A range in the editor. This interface is suitable for serialization.
 */
class IRange {
public:
  virtual ~IRange() = default;

  /**
   * Line number on which the range starts (starts at 1).
   */
  virtual int getStartLineNumber() const = 0;

  /**
   * Column on which the range starts in line `startLineNumber` (starts at 1).
   */
  virtual int getStartColumn() const = 0;

  /**
   * Line number on which the range ends.
   */
  virtual int getEndLineNumber() const = 0;

  /**
   * Column on which the range ends in line `endLineNumber`.
   */
  virtual int getEndColumn() const = 0;
};

/**
 * A range in the editor. (startLineNumber,startColumn) is <=
 * (endLineNumber,endColumn)
 */
class Range : public IRange {
public:
  /**
   * Line number on which the range starts (starts at 1).
   */
  int startLineNumber;

  /**
   * Column on which the range starts in line `startLineNumber` (starts at 1).
   */
  int startColumn;

  /**
   * Line number on which the range ends.
   */
  int endLineNumber;

  /**
   * Column on which the range ends in line `endLineNumber`.
   */
  int endColumn;

  /**
   * Create a new range using the given positions.
   */
  Range(int startLineNumber, int startColumn, int endLineNumber, int endColumn);

  /**
   * Create a new range using the given positions.
   */
  Range(const Position &start, const Position &end);

  // IRange interface implementation
  int getStartLineNumber() const override { return startLineNumber; }
  int getStartColumn() const override { return startColumn; }
  int getEndLineNumber() const override { return endLineNumber; }
  int getEndColumn() const override { return endColumn; }

  /**
   * Test if this range is empty.
   */
  bool isEmpty() const;

  /**
   * Test if `range` is empty.
   */
  static bool isEmpty(const IRange &range);

  /**
   * Test if position is in this range. If the position is at the edge, returns
   * true.
   */
  bool containsPosition(const Position &position) const;

  /**
   * Test if `position` is in `range`. If the position is at the edge, returns
   * true.
   */
  static bool containsPosition(const IRange &range, const IPosition &position);

  /**
   * Test if range is in this range. If the range is equal to this range,
   * returns true.
   */
  bool containsRange(const Range &range) const;

  /**
   * Test if `otherRange` is in `range`. If the ranges are equal, returns true.
   */
  static bool containsRange(const IRange &range, const IRange &otherRange);

  /**
   * Test if `range` is strictly contained in this range.
   * If the range is equal to this range, returns false.
   */
  bool strictContainsRange(const Range &range) const;

  /**
   * Test if `otherRange` is strictly contained in `range`.
   * If the ranges are equal, returns false.
   */
  static bool strictContainsRange(const IRange &range,
                                  const IRange &otherRange);

  /**
   * A reunion of the two ranges.
   * The smallest position will be used as the start point, and the largest
   * position as the end point.
   */
  Range plusRange(const Range &range) const;

  /**
   * A reunion of the two ranges.
   * The smallest position will be used as the start point, and the largest
   * position as the end point.
   */
  static Range plusRange(const IRange &a, const IRange &b);

  /**
   * A intersection of the two ranges.
   */
  Range intersectRanges(const Range &range) const;

  /**
   * A intersection of the two ranges.
   */
  static Range intersectRanges(const IRange &a, const IRange &b);

  /**
   * Test if this range equals other.
   */
  bool equalsRange(const IRange &other) const;

  /**
   * Test if range `a` equals `b`.
   */
  static bool equalsRange(const IRange &a, const IRange &b);

  /**
   * Return the end position (which will be after or equal to the start
   * position)
   */
  Position getEndPosition() const;

  /**
   * Return the start position (which will be before or equal to the end
   * position)
   */
  Position getStartPosition() const;

  /**
   * Transform to a user-presentable string representation.
   */
  std::string toString() const;

  /**
   * Create a new range similar to this range.
   */
  Range setEndPosition(int endLineNumber, int endColumn) const;

  /**
   * Create a new range similar to this range.
   */
  Range setStartPosition(int startLineNumber, int startColumn) const;

  /**
   * Create a new empty range using this range's start position.
   */
  Range collapseToStart() const;

  /**
   * Create a new empty range using this range's end position.
   */
  Range collapseToEnd() const;

  static Range fromPositions(const Position &start,
                             const Position &end = Position(0, 0));

  /**
   * Create a `Range` from an `IRange`.
   */
  static Range lift(const IRange &range);

  /**
   * Test if `obj` is an `IRange`.
   */
  static bool isIRange(const void *obj);

  /**
   * Test if the two ranges are touching in any way.
   */
  static bool areIntersectingOrTouching(const IRange &a, const IRange &b);

  /**
   * Test if the two ranges are intersecting. If the ranges are touching it
   * returns true.
   */
  static bool areIntersecting(const IRange &a, const IRange &b);

  /**
   * A function that compares ranges, useful for sorting ranges
   * It will first compare ranges on the startPosition and then on the
   * endPosition
   */
  static int compareRangesUsingStarts(const IRange &a, const IRange &b);

  /**
   * A function that compares ranges, useful for sorting ranges
   * It will first compare ranges on the endPosition and then on the
   * startPosition
   */
  static int compareRangesUsingEnds(const IRange &a, const IRange &b);

  /**
   * Test if the range spans multiple lines.
   */
  bool spansMultipleLines() const;
};

} // namespace buffer