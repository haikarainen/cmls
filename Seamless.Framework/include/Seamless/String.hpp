#pragma once

#include <Seamless/Export.hpp>

#include <map>
#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <optional>
#include <memory>

namespace cmls
{
  /** Formats a std::string in a printf-like fashion, source: https://stackoverflow.com/a/26221725 */
  template<typename ... Args>
  std::string formatString(const std::string& format, Args ... args)
  {
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
  }

  // String manipulation functions
  CMLS_API bool strContains(std::string const &needle, std::string const &haystack);
  CMLS_API bool isWhitespace(char const & curr);
  CMLS_API std::vector<std::string>  parseArguments(std::string const &ource, bool bashComments = false);
  CMLS_API std::vector<std::string>  split(std::string const &source, std::vector<char> delimiters);
  CMLS_API std::string trimLeft(std::string const &source, std::string const &needles = " \f\n\r\t\v");
  CMLS_API std::string trimRight(std::string const &source, std::string const &needles = " \f\n\r\t\v");
  CMLS_API std::string trim(std::string const &source, std::string const &needles = " \f\n\r\t\v");

  /** Splits a source string into lines, retaining the \n at the end of each line as well as empty lines */
  CMLS_API std::vector<std::string> splitLines(std::string const &source);

  CMLS_API bool strBeginsWith(std::string const &source, std::string const &needle);

  CMLS_API bool strBeginsWithAny(std::string const &source, std::vector<std::string> needles);

  /**
   * Offset can be negative, in which case they count from the end of the string.
   * If size is 0, it means the remainder of the string.
   * Calls std::string::substr under the hood.
   */
  CMLS_API std::string substring(std::string const &source, int32_t offset, uint32_t size = 0);

  /** Replaces every occurence of 'needle with 'replacement in 'haystack */
  CMLS_API std::string strReplace(std::string const &needle, std::string const &replacement, std::string const &haystack);

  /** Makes ASCII-valid (decimal 65-122) characters lowercase, assumes UTF-8 */
  CMLS_API std::string strToLower(std::string const &input);

  /** Makes ASCII-valid (decimal 65-122) characters uppercase, assumes UTF-8 */
  CMLS_API std::string strToUpper(std::string const &input);

  /** Joins a vector of strings together, using delimiter as "glue" */
  CMLS_API std::string join(std::vector<std::string> strings, std::string const &delimiter);

  /**
   * Removes C++-style single-line and multi-line comments from input.
   * Compatible with C, C++, GLSL, however initially written for GLSL.
   * Will use \n for reassembly of lines, and both \r and \n for disassembly.
   * May result in windows line endings (\r\n) getting double newlines.
   */
  CMLS_API std::string removeComments(std::string const &input);

  CMLS_API bool strIsNumber(std::string const & input);

  CMLS_API std::optional<std::string> stringFromFile(std::string const &path);

  CMLS_API std::string escapeString(std::string const &input);

  CMLS_API bool patternMatch(std::string const &source, std::string const &pattern, std::map<std::string, std::string> &namedParameters);

  CMLS_API std::string utf16to8(std::wstring const &wstr);

  CMLS_API std::wstring utf8to16(std::string const &str);

  enum PatternTokenType
  {
    PT_Literal,
    PT_Parameter
  };

  struct PatternToken
  {
    PatternTokenType type = PT_Literal;
    std::string text;
  };

  using PatternTokenList = std::vector<cmls::PatternToken>;
  using PatternVariableSet = std::map<std::string, std::string>;

  class Pattern;

  class CMLS_API PatternResult
  {
  public:
    PatternResult(cmls::Pattern const &pattern, std::string const &reference);

    bool matches() const;

    explicit operator bool() const;
    
    PatternVariableSet const &variables() const;

  protected:
    friend class Pattern;
    bool m_matches = false;
    PatternVariableSet m_variables;
  };

  class CMLS_API Pattern
  {
  public:
    Pattern(std::string const &pattern);

    PatternResult evaluate(std::string const &reference) const;
    PatternTokenList const &tokens() const;

    bool valid() const;
  protected:
    PatternTokenList m_tokens;
    bool m_valid = false;
  };
}