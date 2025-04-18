/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for
 *license information.
 *--------------------------------------------------------------------------------------------*/

#include "common/position.h"
#include <sstream>

namespace buffer {

Position::Position(int lineNumber, int column)
    : lineNumber(lineNumber), column(column) {}

Position Position::with(int newLineNumber, int newColumn) const {
  if (newLineNumber == -1) {
    newLineNumber = lineNumber;
  }
  if (newColumn == -1) {
    newColumn = column;
  }

  if (newLineNumber == lineNumber && newColumn == column) {
    return *this;
  } else {
    return Position(newLineNumber, newColumn);
  }
}

Position Position::delta(int deltaLineNumber, int deltaColumn) const {
  return with(lineNumber + deltaLineNumber, column + deltaColumn);
}

bool Position::equals(const IPosition &other) const {
  return equals(this, &other);
}

bool Position::equals(const IPosition *a, const IPosition *b) {
  if (!a && !b) {
    return true;
  }
  return (a && b && a->getLineNumber() == b->getLineNumber() &&
          a->getColumn() == b->getColumn());
}

bool Position::isBefore(const IPosition &other) const {
  return isBefore(*this, other);
}

bool Position::isBefore(const IPosition &a, const IPosition &b) {
  if (a.getLineNumber() < b.getLineNumber()) {
    return true;
  }
  if (b.getLineNumber() < a.getLineNumber()) {
    return false;
  }
  return a.getColumn() < b.getColumn();
}

bool Position::isBeforeOrEqual(const IPosition &other) const {
  return isBeforeOrEqual(*this, other);
}

bool Position::isBeforeOrEqual(const IPosition &a, const IPosition &b) {
  if (a.getLineNumber() < b.getLineNumber()) {
    return true;
  }
  if (b.getLineNumber() < a.getLineNumber()) {
    return false;
  }
  return a.getColumn() <= b.getColumn();
}

int Position::compare(const IPosition &a, const IPosition &b) {
  int aLineNumber = a.getLineNumber();
  int bLineNumber = b.getLineNumber();

  if (aLineNumber == bLineNumber) {
    int aColumn = a.getColumn();
    int bColumn = b.getColumn();
    return aColumn - bColumn;
  }

  return aLineNumber - bLineNumber;
}

Position Position::clone() const { return Position(lineNumber, column); }

std::string Position::toString() const {
  std::stringstream ss;
  ss << "(" << lineNumber << "," << column << ")";
  return ss.str();
}

Position Position::lift(const IPosition &pos) {
  return Position(pos.getLineNumber(), pos.getColumn());
}

bool Position::isIPosition(const void *obj) {
  // In C++, we can use dynamic_cast to check if an object is an instance of
  // IPosition But here we'll check if the object is not null, as a simple
  // alternative
  return obj != nullptr;
}

} // namespace buffer