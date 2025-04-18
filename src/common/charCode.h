/*---------------------------------------------------------------------------------------------
 *  Copyright (c) Microsoft Corporation. All rights reserved.
 *  Licensed under the MIT License. See License.txt in the project root for
 *license information.
 *--------------------------------------------------------------------------------------------*/

#pragma once

#include <cstdint>

namespace buffer {

namespace CharCode {
// Basic ASCII character codes
constexpr uint16_t Null = 0;
constexpr uint16_t Tab = 9;
constexpr uint16_t LineFeed = 10;
constexpr uint16_t CarriageReturn = 13;
constexpr uint16_t Space = 32;
constexpr uint16_t ExclamationMark = 33;
constexpr uint16_t DoubleQuote = 34;
constexpr uint16_t Hash = 35;
constexpr uint16_t Dollar = 36;
constexpr uint16_t Percent = 37;
constexpr uint16_t Ampersand = 38;
constexpr uint16_t SingleQuote = 39;
constexpr uint16_t OpenParen = 40;
constexpr uint16_t CloseParen = 41;
constexpr uint16_t Asterisk = 42;
constexpr uint16_t Plus = 43;
constexpr uint16_t Comma = 44;
constexpr uint16_t Minus = 45;
constexpr uint16_t Period = 46;
constexpr uint16_t Slash = 47;

// Numbers
constexpr uint16_t Digit0 = 48;
constexpr uint16_t Digit1 = 49;
constexpr uint16_t Digit2 = 50;
constexpr uint16_t Digit3 = 51;
constexpr uint16_t Digit4 = 52;
constexpr uint16_t Digit5 = 53;
constexpr uint16_t Digit6 = 54;
constexpr uint16_t Digit7 = 55;
constexpr uint16_t Digit8 = 56;
constexpr uint16_t Digit9 = 57;

// Punctuation
constexpr uint16_t Colon = 58;
constexpr uint16_t Semicolon = 59;
constexpr uint16_t LessThan = 60;
constexpr uint16_t Equals = 61;
constexpr uint16_t GreaterThan = 62;
constexpr uint16_t QuestionMark = 63;
constexpr uint16_t AtSign = 64;

// Upper case letters
constexpr uint16_t A = 65;
constexpr uint16_t B = 66;
constexpr uint16_t C = 67;
constexpr uint16_t D = 68;
constexpr uint16_t E = 69;
constexpr uint16_t F = 70;
constexpr uint16_t G = 71;
constexpr uint16_t H = 72;
constexpr uint16_t I = 73;
constexpr uint16_t J = 74;
constexpr uint16_t K = 75;
constexpr uint16_t L = 76;
constexpr uint16_t M = 77;
constexpr uint16_t N = 78;
constexpr uint16_t O = 79;
constexpr uint16_t P = 80;
constexpr uint16_t Q = 81;
constexpr uint16_t R = 82;
constexpr uint16_t S = 83;
constexpr uint16_t T = 84;
constexpr uint16_t U = 85;
constexpr uint16_t V = 86;
constexpr uint16_t W = 87;
constexpr uint16_t X = 88;
constexpr uint16_t Y = 89;
constexpr uint16_t Z = 90;

// Punctuation
constexpr uint16_t OpenSquareBracket = 91;
constexpr uint16_t Backslash = 92;
constexpr uint16_t CloseSquareBracket = 93;
constexpr uint16_t Caret = 94;
constexpr uint16_t Underline = 95;
constexpr uint16_t BackTick = 96;

// Lower case letters
constexpr uint16_t a = 97;
constexpr uint16_t b = 98;
constexpr uint16_t c = 99;
constexpr uint16_t d = 100;
constexpr uint16_t e = 101;
constexpr uint16_t f = 102;
constexpr uint16_t g = 103;
constexpr uint16_t h = 104;
constexpr uint16_t i = 105;
constexpr uint16_t j = 106;
constexpr uint16_t k = 107;
constexpr uint16_t l = 108;
constexpr uint16_t m = 109;
constexpr uint16_t n = 110;
constexpr uint16_t o = 111;
constexpr uint16_t p = 112;
constexpr uint16_t q = 113;
constexpr uint16_t r = 114;
constexpr uint16_t s = 115;
constexpr uint16_t t = 116;
constexpr uint16_t u = 117;
constexpr uint16_t v = 118;
constexpr uint16_t w = 119;
constexpr uint16_t x = 120;
constexpr uint16_t y = 121;
constexpr uint16_t z = 122;

// Punctuation
constexpr uint16_t OpenCurlyBrace = 123;
constexpr uint16_t Pipe = 124;
constexpr uint16_t CloseCurlyBrace = 125;
constexpr uint16_t Tilde = 126;

// Unicode BOM
constexpr uint16_t UTF8_BOM = 65279;
} // namespace CharCode

} // namespace buffer