/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for
 *license information.
 *--------------------------------------------------------------------------------------------*/

#pragma once

#include <string>

namespace buffer {

/**
 * A position in the editor. This interface is suitable for serialization.
 */
class IPosition {
public:
  virtual ~IPosition() = default;

  /**
   * Line number (starts at 1)
   */
  virtual int getLineNumber() const = 0;

  /**
   * Column (the first character in a line is between column 1 and column 2)
   */
  virtual int getColumn() const = 0;
};

/**
 * A position in the editor.
 */
class Position : public IPosition {
public:
  /**
   * Line number (starts at 1)
   */
  int lineNumber;

  /**
   * Column (the first character in a line is between column 1 and column 2)
   */
  int column;

  /**
   * Constructor for Position
   */
  Position(int lineNumber, int column);

  /**
   * Create a new position from this position.
   *
   * @param newLineNumber new line number
   * @param newColumn new column
   */
  Position with(int newLineNumber = -1, int newColumn = -1) const;

  /**
   * Derive a new position from this position.
   *
   * @param deltaLineNumber line number delta
   * @param deltaColumn column delta
   */
  Position delta(int deltaLineNumber = 0, int deltaColumn = 0) const;

  /**
   * Test if this position equals other position
   */
  bool equals(const IPosition &other) const;

  /**
   * Test if position `a` equals position `b`
   */
  static bool equals(const IPosition *a, const IPosition *b);

  /**
   * Test if this position is before other position.
   * If the two positions are equal, the result will be false.
   */
  bool isBefore(const IPosition &other) const;

  /**
   * Test if position `a` is before position `b`.
   * If the two positions are equal, the result will be false.
   */
  static bool isBefore(const IPosition &a, const IPosition &b);

  /**
   * Test if this position is before other position.
   * If the two positions are equal, the result will be true.
   */
  bool isBeforeOrEqual(const IPosition &other) const;

  /**
   * Test if position `a` is before position `b`.
   * If the two positions are equal, the result will be true.
   */
  static bool isBeforeOrEqual(const IPosition &a, const IPosition &b);

  /**
   * A function that compares positions, useful for sorting
   */
  static int compare(const IPosition &a, const IPosition &b);

  /**
   * Clone this position.
   */
  Position clone() const;

  /**
   * Convert to a human-readable representation.
   */
  std::string toString() const;

  // IPosition interface implementation
  int getLineNumber() const override { return lineNumber; }
  int getColumn() const override { return column; }

  /**
   * Create a `Position` from an `IPosition`.
   */
  static Position lift(const IPosition &pos);

  /**
   * Test if `obj` is an `IPosition`.
   */
  static bool isIPosition(const void *obj);
};

} // namespace buffer