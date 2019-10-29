
#include "Seamless/String.hpp"
#include "Seamless/Error.hpp"

#include <Windows.h>

#include <algorithm>
#include <cctype>
#include <fstream>
#include <streambuf>
#include <locale>
#include <codecvt>

bool cmls::strContains(const std::string& needle, const std::string& haystack)
{
  return (std::strstr((char*)haystack.c_str(), needle.c_str()) != nullptr);
}

bool cmls::isWhitespace(char const & curr)
{
  return (curr == ' ' || curr == '\f' || curr == '\n' || curr == '\r' || curr == '\t' || curr == '\v');
}

std::vector<std::string> cmls::parseArguments(const std::string& source, bool bashComments)
{
  // Create a vector (list) with words to return
  std::vector<std::string> returner;

  // If the length of the source string is 0, return the empty list
  if (source.size() < 1)
  {
    return returner;
  }

  // Setup some variables we will need when we parse the string
  unsigned int currPosition = 0; // Current position in string
  std::string currArg = "";      // Current argument (we fill this with characters, and copy it on splits, then we reset it)
  bool isQuoted = false;         // If the current character is inside quotation marks

                                 // Iterate the source buffer, character by character
  while (currPosition < source.size())
  {
    // Create a copy of the character at the current position in the string
    char currChar = source[currPosition];

    // If the current character is a quotationmark
    if (currChar == '"')
    {
      // Flip the isQuoted-bool, so it's the opposite of what it was
      isQuoted = !isQuoted;

      // If currArg is longer than 0 characters, add a copy to the list and reset currArg to "" (to prepare for the next argument)
      if (currArg.size() > 0)
      {
        returner.push_back(currArg);
        currArg = "";
      }
    }
    // Otherwise, if the current character isn't a quotation mark
    else
    {
      // If the current character is inside a quotation mark, add it to the current argument
      if (isQuoted)
      {
        currArg.push_back(currChar);
      }
      // Otherwise,
      else
      {
        // If the current character is a # (while not in  quotes), add the current argument to the list and return the list
        // (we are done here, since a # outside of quotes tells us the rest of the line is a comment)
        if (currChar == '#' && bashComments)
        {
          if (currArg.size() > 0)
          {
            returner.push_back(currArg);
          }

          return returner;
        }

        // If the current character is whitespace, add the current argument to the list and reset currArg
        if (isWhitespace(currChar))
        {
          if (currArg.size() > 0)
          {
            returner.push_back(currArg);
            currArg = "";
          }
        }
        else
          // Otherwise add the current character to the current argument
        {
          currArg.push_back(currChar);
        }
      }
    }

    // Step forward in the source string
    currPosition++;
  }

  // We have now iterated through our source string
  // Add the remaining argument to our list and then return it
  if (currArg.size() > 0)
  {
    returner.push_back(currArg);
  }

  return returner;
}

std::vector<std::string> cmls::split(const std::string& source, std::vector<char> delimiters)
{
  std::vector<std::string> returner;
  if (source.size() < 1)
  {
    return returner;
  }

  unsigned int currPosition = 0;
  std::string currstr = "";
  while (currPosition < source.size())
  {
    char curr = source[currPosition];

    if (std::find(delimiters.begin(), delimiters.end(), curr) != delimiters.end())
    {
      if (currstr.size() > 0)
      {
        returner.push_back(currstr);
        currstr = "";
      }
    }
    else
    {
      currstr.push_back(curr);
    }

    currPosition++;
  }
  if (currstr.size() > 0)
  {
    returner.push_back(currstr);
  }

  return returner;
}

std::string cmls::trimLeft(const std::string & source, const std::string & needles)
{
  std::string str = source;
  size_t startpos = str.find_first_not_of(needles.c_str());
  if (std::string::npos != startpos)
  {
    str = str.substr(startpos);
  }
  return str;
}

std::string cmls::trimRight(const std::string& source, const std::string & needles)
{
  std::string str = source;
  size_t endpos = str.find_last_not_of(needles.c_str());
  if (std::string::npos != endpos)
  {
    str = str.substr(0, endpos + 1);
  }
  return str;
}

std::string cmls::trim(const std::string& source, const std::string & needles)
{
  return cmls::trimLeft(cmls::trimRight(source, needles), needles);
}

std::vector<std::string> cmls::splitLines(std::string const &source)
{
  std::vector<std::string> returner;
  if (source.size() < 1)
  {
    return returner;
  }

  std::string buffer = "";
  for(uint32_t cursor = 0; cursor < source.size(); cursor++)
  {
    char curr = source[cursor];

    buffer.push_back(curr);

    if (curr == '\n')
    {
      returner.push_back(buffer);
      buffer = "";
    }
  }

  if (buffer.size() > 0)
  {
    returner.push_back(buffer);
  }

  return returner;
}

bool cmls::strBeginsWith(std::string const &source, std::string const &needle)
{
  return source.substr(0, needle.size()) == needle;
}

bool cmls::strBeginsWithAny(std::string const &source, std::vector<std::string> needles)
{
  for (auto needle : needles)
  {
    if (strBeginsWith(source, needle))
    {
      return true;
    }
  }

  return false;
}

std::string cmls::substring(std::string const &source, int32_t offset, uint32_t size)
{
  uint32_t length = (uint32_t)source.size();

  if (length == 0)
  {
    return "";
  }

  // The resolved offset, e.g. which index to start at
  uint32_t resolvedOffset = 0;

  // The resolved size, e.g. how many characters to substring
  uint32_t resolvedSize = 0;

  // Resolve actual offset
  if (offset < 0)
  {
    if (offset < -int32_t(length))
    {
      offset = -int32_t(length);
    }

    resolvedOffset = length + offset;
  }
  else
  {
    if (offset > int32_t(length))
    {
      offset = length;
    }

    resolvedOffset = offset;
  }

  // Zero means remainder
  if (size == 0)
  {
    size = length - resolvedOffset;
  }

  // Resolve the actual size
  auto cap = length - resolvedOffset;
  resolvedSize = size > cap ? cap : size;

  return source.substr(resolvedOffset, resolvedSize);
}

std::string cmls::strReplace(std::string const & needle, std::string const & replacement, std::string const & haystack)
{
  std::string newString = haystack;
  size_t finder = newString.find(needle);
  while (finder != std::string::npos)
  {
    if (newString.size() < needle.size())
    {
      break;
    }

    newString = newString.replace(finder, needle.size(), replacement);
    finder = newString.find(needle);
  }

  return newString;
}

std::string cmls::strToLower(std::string const & input)
{
  std::string returner = input;
  for (char & character : returner)
  {
    if (character >= 65 && character <= 90)
    {
      character += 32;
    }
  }

  return returner;
}

std::string cmls::strToUpper(std::string const & input)
{
  std::string returner = input;

  for (char & character : returner)
  {
    if (character >= 97 && character <= 122)
    {
      character -= 32;
    }
  }

  return returner;
}

std::string cmls::removeComments(const std::string& input)
{
  // Make a copy of input
  std::string inputCopy = input;

  // Find and erase multiline comments
  size_t end = 0;
  while (true)
  {
    size_t begin = inputCopy.find("/*", end);
    if (begin == std::string::npos)
    {
      break;
    }

    end = inputCopy.find("*/", begin);

    if (end == std::string::npos)
    {
      end = inputCopy.size() - 1;
    }
    else
    {
      end += 2;
    }

    inputCopy.erase(begin, end - begin);
  }

  // Go trough each line to remove single line comments
  std::vector<std::string> lines = cmls::split(inputCopy, { '\n', '\r' });
  std::string output;
  for (auto currLine : lines)
  {
    size_t finder = currLine.find("//");

    if (finder == std::string::npos)
    {
      output.append(currLine);
    }
    else
    {
      output.append(currLine.substr(0, finder));
    }

    output.append("\n");
  }

  return output;
}

bool cmls::strIsNumber(std::string const & input)
{
  return !input.empty() && std::find_if(input.begin(), input.end(), [](char c) { return !std::isdigit(c); }) == input.end();
}

std::optional<std::string> cmls::stringFromFile(std::string const &path)
{
  std::ifstream handle(path);
  if (!handle.is_open())
  {
    return {};
  }

  std::string contents((std::istreambuf_iterator<char>(handle)), std::istreambuf_iterator<char>());

  handle.close();

  return contents;
}

std::string cmls::join(std::vector<std::string> strings, std::string const &delimiter)
{
  std::string returner;
  int i = 0;
  for (auto str : strings)
  {
    returner.append(str);
    if (++i < strings.size())
    {
      returner.append(delimiter);
    }
  }
  return returner;
}

std::string cmls::escapeString(std::string const &input)
{
  std::string output;

  output.push_back('"');
  for (auto i : input)
  {
    if (i >= ' ' && i <= '~' && i != '\\' && i != '"')
    {
      output.push_back(i);
    }
    else
    {
      output.push_back('\\');
      switch (i)
      {
      case '"':
        output.push_back(i);
        break;
      case '\t':
        output.push_back('t');
        break;
      case '\n':
        output.push_back('n');
        break;
      case '\r':
        output.push_back('r');
        break;
      default:
        char const* const hexTable = "0123456789ABCDEF";
        output.push_back('x');
        output.push_back(hexTable[i >> 4]);
        output.push_back(hexTable[i & 0xF]);
        break;
      }
    }
  }
  output.push_back('"');

  return output;
}

bool cmls::patternMatch(std::string const& source, std::string const& patternString, std::map<std::string, std::string> &namedParameters)
{
  auto pattern = cmls::Pattern(patternString);
  auto result = pattern.evaluate(source);

  namedParameters = result.variables();

  return result.matches();
}


std::string cmls::utf16to8(std::wstring const& wstr)
{
  int32_t buffSize = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int32_t)wstr.size(), NULL, 0, NULL, NULL);
  char *buffer = new char[buffSize];
  WideCharToMultiByte(CP_UTF8, 0, wstr.data(), (int32_t)wstr.size(), buffer, buffSize, NULL, NULL);
  std::string returner(buffer, buffer + buffSize);
  delete[] buffer;
  return returner;
}

std::wstring cmls::utf8to16(std::string const& str)
{
  int32_t buffSize = MultiByteToWideChar(CP_UTF8, 0, str.data(), (int32_t)str.size(), NULL, 0);
  wchar_t *buffer = new wchar_t[buffSize];
  MultiByteToWideChar(CP_UTF8, 0, str.data(), (int32_t)str.size(), buffer, buffSize);
  std::wstring returner(buffer, buffer + buffSize);
  delete[] buffer;
  return returner;
}

cmls::PatternResult::PatternResult(cmls::Pattern const& pattern, std::string const& reference)
{
  if (!pattern.valid())
  {
    LogWarning("Attempted to evaluate invalid pattern. Result will be inconclusive.");
    m_matches = false;
    return;
  }

  auto buffer = reference;
  for (auto i = pattern.tokens().begin(); i != pattern.tokens().end(); i++)
  {
    auto &token = *i;

    if (token.type == PT_Literal)
    {
      if (cmls::strBeginsWith(buffer, token.text))
      {
        buffer = cmls::substring(buffer, token.text.size());
        continue;
      }
      else
      {
        m_matches = false;
        return;
      }
    }
    else if (token.type == PT_Parameter)
    {
      uint64_t parameterSize = 0;
      auto nextToken = i + 1;
      if (nextToken == pattern.tokens().end())
      {
        m_variables[token.text] = buffer;
        buffer.clear();
      }
      else
      {
        // token guaranteed to be text
        auto finder = buffer.find(nextToken->text);
        if (finder == std::string::npos)
        {
          // no match
          m_matches = false;
          return;
        }
        else
        {
          // match, cut the buffer
          m_variables[token.text] = cmls::substring(buffer, 0, finder);
          buffer = cmls::substring(buffer, finder);
        }
      }
    }
  }

  // If there remains text, it's not a match
  if (buffer.size() != 0)
  {
    m_matches = false;

    return;
  }

  m_matches = true;
}

bool cmls::PatternResult::matches() const
{
  return m_matches;
}

cmls::PatternResult::operator bool() const
{
  return matches();
}

cmls::PatternVariableSet const &cmls::PatternResult::variables() const
{
  return m_variables;
}

cmls::Pattern::Pattern(std::string const& pattern)
{
  std::string tokenBuffer = "";
  bool insideParameter = false;
  char p = '\0';
  for (auto c : pattern)
  {
    if (c == '{')
    {
      if (p == '}')
      {
        LogError("Invalid pattern, two sequential parameters are not allowed.");
        m_valid = false;
        return;
      }

      if (insideParameter)
      {
        LogError("Invalid pattern, unexpected opening curly brace inside of parameter.");
        m_valid = false;
        return;
      }

      if (tokenBuffer.size() > 0)
      {
        m_tokens.push_back({ PT_Literal, tokenBuffer });
        tokenBuffer.clear();
      }

      insideParameter = true;
      p = c;
      continue;
    }
    else if (c == '}')
    {
      if (!insideParameter)
      {
        LogError("Invalid pattern, unexpected closing curly brace outside of parameter.");
        m_valid = false;
        return;
      }

      if (tokenBuffer.size() > 0)
      {
        m_tokens.push_back({ PT_Parameter, tokenBuffer });
        tokenBuffer.clear();
      }

      insideParameter = false;
      p = c;
      continue;
    }
    else
    {
      tokenBuffer.push_back(c);
      p = c;
    }
  }

  if (tokenBuffer.size() > 0)
  {
    m_tokens.push_back({ insideParameter ? PT_Parameter : PT_Literal, tokenBuffer });
  }

  m_valid = true;
}

cmls::PatternResult cmls::Pattern::evaluate(std::string const& reference) const
{
  return cmls::PatternResult(*this, reference);
}

cmls::PatternTokenList const& cmls::Pattern::tokens() const
{
  return m_tokens;
}

bool cmls::Pattern::valid() const
{
  return m_valid;
}
