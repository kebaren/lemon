/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for
 *license information.
 *--------------------------------------------------------------------------------------------*/

#include "common/range.h"
#include <algorithm>
#include <sstream>

namespace buffer {

Range::Range(int startLineNumber, int startColumn, int endLineNumber,
             int endColumn)
    : startLineNumber(startLineNumber), startColumn(startColumn),
      endLineNumber(endLineNumber), endColumn(endColumn) {}

Range::Range(const Position &start, const Position &end)
    : startLineNumber(start.lineNumber), startColumn(start.column),
      endLineNumber(end.lineNumber), endColumn(end.column) {}

bool Range::isEmpty() const { return isEmpty(*this); }

bool Range::isEmpty(const IRange &range) {
  return (range.getStartLineNumber() == range.getEndLineNumber() &&
          range.getStartColumn() == range.getEndColumn());
}

bool Range::containsPosition(const Position &position) const {
  return containsPosition(*this, position);
}

bool Range::containsPosition(const IRange &range, const IPosition &position) {
  if (position.getLineNumber() < range.getStartLineNumber() ||
      position.getLineNumber() > range.getEndLineNumber()) {
    return false;
  }
  if (position.getLineNumber() == range.getStartLineNumber() &&
      position.getColumn() < range.getStartColumn()) {
    return false;
  }
  if (position.getLineNumber() == range.getEndLineNumber() &&
      position.getColumn() > range.getEndColumn()) {
    return false;
  }
  return true;
}

bool Range::containsRange(const Range &range) const {
  return containsRange(*this, range);
}

bool Range::containsRange(const IRange &range, const IRange &otherRange) {
  if (otherRange.getStartLineNumber() < range.getStartLineNumber() ||
      otherRange.getEndLineNumber() > range.getEndLineNumber()) {
    return false;
  }
  if (otherRange.getStartLineNumber() == range.getStartLineNumber() &&
      otherRange.getStartColumn() < range.getStartColumn()) {
    return false;
  }
  if (otherRange.getEndLineNumber() == range.getEndLineNumber() &&
      otherRange.getEndColumn() > range.getEndColumn()) {
    return false;
  }
  return true;
}

bool Range::strictContainsRange(const Range &range) const {
  return strictContainsRange(*this, range);
}

bool Range::strictContainsRange(const IRange &range, const IRange &otherRange) {
  if (otherRange.getStartLineNumber() < range.getStartLineNumber() ||
      otherRange.getEndLineNumber() > range.getEndLineNumber()) {
    return false;
  }
  if (otherRange.getStartLineNumber() == range.getStartLineNumber() &&
      otherRange.getStartColumn() <= range.getStartColumn()) {
    return false;
  }
  if (otherRange.getEndLineNumber() == range.getEndLineNumber() &&
      otherRange.getEndColumn() >= range.getEndColumn()) {
    return false;
  }
  return true;
}

Range Range::plusRange(const Range &range) const {
  return plusRange(*this, range);
}

Range Range::plusRange(const IRange &a, const IRange &b) {
  int startLineNumber;
  int startColumn;
  int endLineNumber;
  int endColumn;

  if (a.getStartLineNumber() < b.getStartLineNumber()) {
    startLineNumber = a.getStartLineNumber();
    startColumn = a.getStartColumn();
  } else if (a.getStartLineNumber() > b.getStartLineNumber()) {
    startLineNumber = b.getStartLineNumber();
    startColumn = b.getStartColumn();
  } else {
    startLineNumber = a.getStartLineNumber();
    startColumn = std::min(a.getStartColumn(), b.getStartColumn());
  }

  if (a.getEndLineNumber() > b.getEndLineNumber()) {
    endLineNumber = a.getEndLineNumber();
    endColumn = a.getEndColumn();
  } else if (a.getEndLineNumber() < b.getEndLineNumber()) {
    endLineNumber = b.getEndLineNumber();
    endColumn = b.getEndColumn();
  } else {
    endLineNumber = a.getEndLineNumber();
    endColumn = std::max(a.getEndColumn(), b.getEndColumn());
  }

  return Range(startLineNumber, startColumn, endLineNumber, endColumn);
}

Range Range::intersectRanges(const Range &range) const {
  return intersectRanges(*this, range);
}

Range Range::intersectRanges(const IRange &a, const IRange &b) {
  int resultStartLineNumber =
      std::max(a.getStartLineNumber(), b.getStartLineNumber());
  int resultEndLineNumber =
      std::min(a.getEndLineNumber(), b.getEndLineNumber());

  if (resultStartLineNumber > resultEndLineNumber) {
    // Ranges have no overlap
    return Range(resultStartLineNumber, 0, resultStartLineNumber, 0);
  }

  int resultStartColumn =
      a.getStartLineNumber() == resultStartLineNumber ? a.getStartColumn() : 0;
  int resultEndColumn =
      a.getEndLineNumber() == resultEndLineNumber ? a.getEndColumn() : 0;

  if (b.getStartLineNumber() == resultStartLineNumber) {
    resultStartColumn = std::max(resultStartColumn, b.getStartColumn());
  }

  if (b.getEndLineNumber() == resultEndLineNumber) {
    resultEndColumn = std::min(resultEndColumn, b.getEndColumn());
  }

  if (resultStartLineNumber == resultEndLineNumber &&
      resultStartColumn > resultEndColumn) {
    // Ranges have no overlap
    return Range(resultStartLineNumber, 0, resultStartLineNumber, 0);
  }

  return Range(resultStartLineNumber, resultStartColumn, resultEndLineNumber,
               resultEndColumn);
}

bool Range::equalsRange(const IRange &other) const {
  return equalsRange(*this, other);
}

bool Range::equalsRange(const IRange &a, const IRange &b) {
  return (a.getStartLineNumber() == b.getStartLineNumber() &&
          a.getStartColumn() == b.getStartColumn() &&
          a.getEndLineNumber() == b.getEndLineNumber() &&
          a.getEndColumn() == b.getEndColumn());
}

Position Range::getEndPosition() const {
  return Position(endLineNumber, endColumn);
}

Position Range::getStartPosition() const {
  return Position(startLineNumber, startColumn);
}

std::string Range::toString() const {
  std::stringstream ss;
  ss << "[" << startLineNumber << "," << startColumn << " -> " << endLineNumber
     << "," << endColumn << "]";
  return ss.str();
}

Range Range::setEndPosition(int endLineNumber, int endColumn) const {
  return Range(startLineNumber, startColumn, endLineNumber, endColumn);
}

Range Range::setStartPosition(int startLineNumber, int startColumn) const {
  return Range(startLineNumber, startColumn, endLineNumber, endColumn);
}

Range Range::collapseToStart() const {
  return Range(startLineNumber, startColumn, startLineNumber, startColumn);
}

Range Range::collapseToEnd() const {
  return Range(endLineNumber, endColumn, endLineNumber, endColumn);
}

Range Range::fromPositions(const Position &start, const Position &end) {
  return Range(start.lineNumber, start.column, end.lineNumber, end.column);
}

Range Range::lift(const IRange &range) {
  return Range(range.getStartLineNumber(), range.getStartColumn(),
               range.getEndLineNumber(), range.getEndColumn());
}

bool Range::isIRange(const void *obj) {
  // In C++, we can use dynamic_cast to check if an object is an instance of
  // IRange But here we'll check if the object is not null, as a simple
  // alternative
  return obj != nullptr;
}

bool Range::areIntersectingOrTouching(const IRange &a, const IRange &b) {
  // Check if `a` is before `b`
  if (a.getEndLineNumber() < b.getStartLineNumber()) {
    return false;
  }
  if (a.getEndLineNumber() == b.getStartLineNumber() &&
      a.getEndColumn() < b.getStartColumn()) {
    return false;
  }

  // Check if `b` is before `a`
  if (b.getEndLineNumber() < a.getStartLineNumber()) {
    return false;
  }
  if (b.getEndLineNumber() == a.getStartLineNumber() &&
      b.getEndColumn() < a.getStartColumn()) {
    return false;
  }

  // These ranges must intersect or touch
  return true;
}

bool Range::areIntersecting(const IRange &a, const IRange &b) {
  // Check if `a` is before `b`
  if (a.getEndLineNumber() < b.getStartLineNumber()) {
    return false;
  }
  if (a.getEndLineNumber() == b.getStartLineNumber() &&
      a.getEndColumn() <= b.getStartColumn()) {
    return false;
  }

  // Check if `b` is before `a`
  if (b.getEndLineNumber() < a.getStartLineNumber()) {
    return false;
  }
  if (b.getEndLineNumber() == a.getStartLineNumber() &&
      b.getEndColumn() <= a.getStartColumn()) {
    return false;
  }

  // These ranges must intersect
  return true;
}

int Range::compareRangesUsingStarts(const IRange &a, const IRange &b) {
  if (a.getStartLineNumber() == b.getStartLineNumber()) {
    if (a.getStartColumn() == b.getStartColumn()) {
      if (a.getEndLineNumber() == b.getEndLineNumber()) {
        return a.getEndColumn() - b.getEndColumn();
      }
      return a.getEndLineNumber() - b.getEndLineNumber();
    }
    return a.getStartColumn() - b.getStartColumn();
  }
  return a.getStartLineNumber() - b.getStartLineNumber();
}

int Range::compareRangesUsingEnds(const IRange &a, const IRange &b) {
  if (a.getEndLineNumber() == b.getEndLineNumber()) {
    if (a.getEndColumn() == b.getEndColumn()) {
      if (a.getStartLineNumber() == b.getStartLineNumber()) {
        return a.getStartColumn() - b.getStartColumn();
      }
      return a.getStartLineNumber() - b.getStartLineNumber();
    }
    return a.getEndColumn() - b.getEndColumn();
  }
  return a.getEndLineNumber() - b.getEndLineNumber();
}

bool Range::spansMultipleLines() const {
  return startLineNumber < endLineNumber;
}

} // namespace buffer