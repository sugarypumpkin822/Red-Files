#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <iomanip>
#include <memory>

namespace RFUtils {

// String encoding types
enum class StringEncoding {
    ASCII,
    UTF8,
    UTF16,
    UTF32,
    Latin1,
    Windows1252,
    ISO88591,
    Base64,
    Hex,
    URL,
    HTML,
    JSON,
    XML
};

// String case types
enum class StringCase {
    Lower,
    Upper,
    Title,
    Camel,
    Pascal,
    Snake,
    Kebab,
    ScreamingSnake,
    ScreamingKebab
};

// String trim options
enum class TrimOption {
    None,
    Left,
    Right,
    Both
};

// String comparison options
enum class StringComparison {
    Exact,
    CaseInsensitive,
    IgnoreWhitespace,
    Normalize,
    Locale
};

// String manipulation class
class StringUtils {
public:
    // Basic operations
    static std::string trim(const std::string& str, TrimOption option = TrimOption::Both);
    static std::string trimLeft(const std::string& str);
    static std::string trimRight(const std::string& str);
    static std::string trimCharacters(const std::string& str, const std::string& characters);
    
    // Case operations
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static std::string toTitle(const std::string& str);
    static std::string toCamelCase(const std::string& str);
    static std::string toPascalCase(const std::string& str);
    static std::string toSnakeCase(const std::string& str);
    static std::string toKebabCase(const std::string& str);
    static std::string toScreamingSnakeCase(const std::string& str);
    static std::string toScreamingKebabCase(const std::string& str);
    
    // Case checking
    static bool isLower(const std::string& str);
    static bool isUpper(const std::string& str);
    static bool isTitle(const std::string& str);
    static bool isCamelCase(const std::string& str);
    static bool isPascalCase(const std::string& str);
    static bool isSnakeCase(const std::string& str);
    static bool isKebabCase(const std::string& str);
    
    // Character operations
    static bool isAlpha(char c);
    static bool isDigit(char c);
    static bool isAlphaNumeric(char c);
    static bool isWhitespace(char c);
    static bool isControl(char c);
    static bool isPrintable(char c);
    static bool isPunctuation(char c);
    static bool isHexDigit(char c);
    static bool isOctalDigit(char c);
    static bool isBinaryDigit(char c);
    
    static bool isAlpha(const std::string& str);
    static bool isDigit(const std::string& str);
    static bool isAlphaNumeric(const std::string& str);
    static bool isWhitespace(const std::string& str);
    static bool isControl(const std::string& str);
    static bool isPrintable(const std::string& str);
    static bool isPunctuation(const std::string& str);
    static bool isHex(const std::string& str);
    static bool isOctal(const std::string& str);
    static bool isBinary(const std::string& str);
    static bool isNumeric(const std::string& str);
    static bool isInteger(const std::string& str);
    static bool isFloat(const std::string& str);
    static bool isEmail(const std::string& str);
    static bool isURL(const std::string& str);
    static bool isUUID(const std::string& str);
    static bool isIPAddress(const std::string& str);
    static bool isMACAddress(const std::string& str);
    static bool isPhoneNumber(const std::string& str);
    static bool isCreditCard(const std::string& str);
    static bool isHexColor(const std::string& str);
    static bool isBase64(const std::string& str);
    static bool isJSON(const std::string& str);
    static bool isXML(const std::string& str);
    static bool isHTML(const std::string& str);
    static bool isSQL(const std::string& str);
    static bool isJavaScript(const std::string& str);
    static bool isCSS(const std::string& str);
    
    // String comparison
    static int compare(const std::string& str1, const std::string& str2, StringComparison comparison = StringComparison::Exact);
    static bool equals(const std::string& str1, const std::string& str2, StringComparison comparison = StringComparison::Exact);
    static bool startsWith(const std::string& str, const std::string& prefix, StringComparison comparison = StringComparison::Exact);
    static bool endsWith(const std::string& str, const std::string& suffix, StringComparison comparison = StringComparison::Exact);
    static bool contains(const std::string& str, const std::string& substr, StringComparison comparison = StringComparison::Exact);
    
    // Searching
    static size_t find(const std::string& str, const std::string& substr, size_t pos = 0, StringComparison comparison = StringComparison::Exact);
    static size_t findLast(const std::string& str, const std::string& substr, size_t pos = std::string::npos, StringComparison comparison = StringComparison::Exact);
    static size_t findFirstOf(const std::string& str, const std::string& chars, size_t pos = 0);
    static size_t findFirstNotOf(const std::string& str, const std::string& chars, size_t pos = 0);
    static size_t findLastOf(const std::string& str, const std::string& chars, size_t pos = std::string::npos);
    static size_t findLastNotOf(const std::string& str, const std::string& chars, size_t pos = std::string::npos);
    
    static bool containsAny(const std::string& str, const std::string& chars);
    static bool containsAll(const std::string& str, const std::string& chars);
    static bool containsOnly(const std::string& str, const std::string& chars);
    
    // Counting
    static size_t count(const std::string& str, const std::string& substr);
    static size_t count(const std::string& str, char c);
    static size_t countWords(const std::string& str);
    static size_t countLines(const std::string& str);
    static size_t countCharacters(const std::string& str);
    static size_t countNonWhitespace(const std::string& str);
    static size_t countWhitespace(const std::string& str);
    
    // Substring operations
    static std::string substring(const std::string& str, size_t start, size_t length = std::string::npos);
    static std::string left(const std::string& str, size_t length);
    static std::string right(const std::string& str, size_t length);
    static std::string mid(const std::string& str, size_t start, size_t length);
    
    // Replacement operations
    static std::string replace(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceFirst(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceLast(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    static std::string replaceChars(const std::string& str, char from, char to);
    
    // Insertion and deletion
    static std::string insert(const std::string& str, size_t pos, const std::string& insert);
    static std::string erase(const std::string& str, size_t pos, size_t length = 1);
    static std::string remove(const std::string& str, const std::string& substr);
    static std::string removeAll(const std::string& str, const std::string& substr);
    static std::string removeChars(const std::string& str, const std::string& chars);
    static std::string removeWhitespace(const std::string& str);
    static std::string removeControl(const std::string& str);
    static std::string removePunctuation(const std::string& str);
    
    // Padding
    static std::string padLeft(const std::string& str, size_t width, char fillChar = ' ');
    static std::string padRight(const std::string& str, size_t width, char fillChar = ' ');
    static std::string padCenter(const std::string& str, size_t width, char fillChar = ' ');
    
    // Repeating
    static std::string repeat(const std::string& str, size_t count);
    static std::string repeat(char c, size_t count);
    
    // Reversing
    static std::string reverse(const std::string& str);
    static std::string reverseWords(const std::string& str);
    
    // Splitting
    static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static std::vector<std::string> splitLines(const std::string& str);
    static std::vector<std::string> splitWords(const std::string& str);
    static std::vector<std::string> splitWhitespace(const std::string& str);
    static std::vector<std::string> splitAny(const std::string& str, const std::string& delimiters);
    
    // Joining
    static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
    static std::string join(const std::vector<std::string>& strings, char delimiter);
    static std::string join(const std::vector<std::string>& strings, const std::function<std::string(size_t)>& delimiterFunc);
    
    // Formatting
    template<typename... Args>
    static std::string format(const std::string& format, Args... args);
    
    static std::string format(const std::string& format, const std::vector<std::string>& args);
    static std::string format(const std::string& format, const std::map<std::string, std::string>& args);
    
    // Number conversion
    static std::string toString(int32_t value);
    static std::string toString(int64_t value);
    static std::string toString(uint32_t value);
    static std::string toString(uint64_t value);
    static std::string toString(float32 value, int precision = 6);
    static std::string toString(float64 value, int precision = 6);
    static std::string toString(bool value);
    static std::string toString(char value);
    
    static int32_t toInt32(const std::string& str, int32_t defaultValue = 0);
    static int64_t toInt64(const std::string& str, int64_t defaultValue = 0);
    static uint32_t toUint32(const std::string& str, uint32_t defaultValue = 0);
    static uint64_t toUint64(const std::string& str, uint64_t defaultValue = 0);
    static float32 toFloat32(const std::string& str, float32 defaultValue = 0.0f);
    static float64 toFloat64(const std::string& str, float64 defaultValue = 0.0);
    static bool toBool(const std::string& str, bool defaultValue = false);
    
    static bool tryParseInt32(const std::string& str, int32_t& result);
    static bool tryParseInt64(const std::string& str, int64_t& result);
    static bool tryParseUint32(const std::string& str, uint32_t& result);
    static bool tryParseUint64(const std::string& str, uint64_t& result);
    static bool tryParseFloat32(const std::string& str, float32& result);
    static bool tryParseFloat64(const std::string& str, float64& result);
    static bool tryParseBool(const std::string& str, bool& result);
    
    // Encoding/decoding
    static std::string encode(const std::string& str, StringEncoding encoding);
    static std::string decode(const std::string& str, StringEncoding encoding);
    
    static std::string toBase64(const std::string& str);
    static std::string fromBase64(const std::string& str);
    static std::string toHex(const std::string& str);
    static std::string fromHex(const std::string& str);
    static std::string toURL(const std::string& str);
    static std::string fromURL(const std::string& str);
    static std::string toHTML(const std::string& str);
    static std::string fromHTML(const std::string& str);
    static std::string toJSON(const std::string& str);
    static std::string fromJSON(const std::string& str);
    static std::string toXML(const std::string& str);
    static std::string fromXML(const std::string& str);
    
    // Hashing
    static uint32_t hash(const std::string& str);
    static uint64_t hash64(const std::string& str);
    static std::string md5(const std::string& str);
    static std::string sha1(const std::string& str);
    static std::string sha256(const std::string& str);
    static std::string sha512(const std::string& str);
    
    // Compression
    static std::string compress(const std::string& str);
    static std::string decompress(const std::string& str);
    
    // Encryption
    static std::string encrypt(const std::string& str, const std::string& key);
    static std::string decrypt(const std::string& str, const std::string& key);
    
    // Validation
    static bool isValid(const std::string& str, const std::string& pattern);
    static bool isValidEmail(const std::string& str);
    static bool isValidURL(const std::string& str);
    static bool isValidUUID(const std::string& str);
    static bool isValidIPAddress(const std::string& str);
    static bool isValidMACAddress(const std::string& str);
    static bool isValidPhoneNumber(const std::string& str);
    static bool isValidCreditCard(const std::string& str);
    static bool isValidHexColor(const std::string& str);
    static bool isValidBase64(const std::string& str);
    static bool isValidJSON(const std::string& str);
    static bool isValidXML(const std::string& str);
    static bool isValidHTML(const std::string& str);
    static bool isValidSQL(const std::string& str);
    static bool isValidJavaScript(const std::string& str);
    static bool isValidCSS(const std::string& str);
    
    // Sanitization
    static std::string sanitize(const std::string& str);
    static std::string sanitizeHTML(const std::string& str);
    static std::string sanitizeSQL(const std::string& str);
    static std::string sanitizeJavaScript(const std::string& str);
    static std::string sanitizeCSS(const std::string& str);
    static std::string sanitizeFilename(const std::string& str);
    static std::string sanitizePath(const std::string& str);
    static std::string sanitizeURL(const std::string& str);
    static std::string sanitizeEmail(const std::string& str);
    static std::string sanitizePhone(const std::string& str);
    
    // Escaping
    static std::string escape(const std::string& str);
    static std::string escapeHTML(const std::string& str);
    static std::string escapeJSON(const std::string& str);
    static std::string escapeXML(const std::string& str);
    static std::string escapeSQL(const std::string& str);
    static std::string escapeRegex(const std::string& str);
    static std::string escapeShell(const std::string& str);
    static std::string escapeCSV(const std::string& str);
    
    // Unescaping
    static std::string unescape(const std::string& str);
    static std::string unescapeHTML(const std::string& str);
    static std::string unescapeJSON(const std::string& str);
    static std::string unescapeXML(const std::string& str);
    static std::string unescapeSQL(const std::string& str);
    static std::string unescapeRegex(const std::string& str);
    static std::string unescapeShell(const std::string& str);
    static std::string unescapeCSV(const std::string& str);
    
    // Regular expressions
    static bool matches(const std::string& str, const std::string& pattern);
    static std::vector<std::string> extract(const std::string& str, const std::string& pattern);
    static std::string replaceRegex(const std::string& str, const std::string& pattern, const std::string& replacement);
    static std::vector<std::string> splitRegex(const std::string& str, const std::string& pattern);
    
    // Distance and similarity
    static int levenshteinDistance(const std::string& str1, const std::string& str2);
    static int damerauLevenshteinDistance(const std::string& str1, const std::string& str2);
    static int hammingDistance(const std::string& str1, const std::string& str2);
    static int jaroDistance(const std::string& str1, const std::string& str2);
    static int jaroWinklerDistance(const std::string& str1, const std::string& str2);
    static double cosineSimilarity(const std::string& str1, const std::string& str2);
    static double jaccardSimilarity(const std::string& str1, const std::string& str2);
    static double diceCoefficient(const std::string& str1, const std::string& str2);
    
    // Fuzzy matching
    static std::vector<std::pair<std::string, double>> fuzzyFind(const std::string& query, const std::vector<std::string>& candidates, double threshold = 0.5);
    static std::string fuzzyMatch(const std::string& query, const std::vector<std::string>& candidates);
    static bool fuzzyContains(const std::string& str, const std::string& substr, double threshold = 0.8);
    
    // Soundex and phonetic algorithms
    static std::string soundex(const std::string& str);
    static std::string metaphone(const std::string& str);
    static std::string doubleMetaphone(const std::string& str);
    static std::string nysiis(const std::string& str);
    static std::string matchRatingCodex(const std::string& str);
    
    // Text analysis
    static size_t countSyllables(const std::string& str);
    static size_t countSentences(const std::string& str);
    static size_t countParagraphs(const std::string& str);
    static double readabilityScore(const std::string& str);
    static double fleschKincaidGradeLevel(const std::string& str);
    static double gunningFogIndex(const std::string& str);
    static double colemanLiauIndex(const std::string& str);
    static double automatedReadabilityIndex(const std::string& str);
    
    // Text generation
    static std::string generateLoremIpsum(size_t words = 100);
    static std::string generateRandomString(size_t length, const std::string& charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    static std::string generatePassword(size_t length = 12, bool includeUppercase = true, bool includeLowercase = true, bool includeDigits = true, bool includeSymbols = true);
    static std::string generateSlug(const std::string& str);
    static std::string generateUUID();
    static std::string generateGUID();
    static std::string generateHash(const std::string& str);
    
    // Text manipulation
    static std::string abbreviate(const std::string& str, size_t maxLength, const std::string& suffix = "...");
    static std::string truncate(const std::string& str, size_t maxLength, const std::string& suffix = "...");
    static std::string elide(const std::string& str, size_t maxLength, const std::string& suffix = "...");
    static std::string wrap(const std::string& str, size_t width, const std::string& indent = "", const std::string& newline = "\n");
    static std::string unwrap(const std::string& str);
    static std::string indent(const std::string& str, const std::string& indent);
    static std::string outdent(const std::string& str, const std::string& indent);
    static std::string quote(const std::string& str, const std::string& quoteChar = "\"");
    static std::string unquote(const std::string& str, const std::string& quoteChar = "\"");
    
    // Path operations
    static std::string combinePath(const std::string& path1, const std::string& path2);
    static std::string combinePath(const std::vector<std::string>& paths);
    static std::string normalizePath(const std::string& path);
    static std::string getDirectoryName(const std::string& path);
    static std::string getFileName(const std::string& path);
    static std::string getFileExtension(const std::string& path);
    static std::string getFileNameWithoutExtension(const std::string& path);
    static std::string changeExtension(const std::string& path, const std::string& extension);
    static bool isAbsolutePath(const std::string& path);
    static bool isRelativePath(const std::string& path);
    static std::string makeRelativePath(const std::string& path, const std::string& base);
    static std::string makeAbsolutePath(const std::string& path, const std::string& base);
    
    // URL operations
    static std::string encodeURL(const std::string& url);
    static std::string decodeURL(const std::string& url);
    static std::string buildURL(const std::string& scheme, const std::string& host, const std::string& path, const std::map<std::string, std::string>& params = {});
    static std::string buildURL(const std::string& scheme, const std::string& host, int port, const std::string& path, const std::map<std::string, std::string>& params = {});
    static std::string getURLScheme(const std::string& url);
    static std::string getURLHost(const std::string& url);
    static int getURLPort(const std::string& url);
    static std::string getURLPath(const std::string& url);
    static std::map<std::string, std::string> getURLParams(const std::string& url);
    static std::string getURLFragment(const std::string& url);
    
    // Template operations
    static std::string replaceTemplate(const std::string& templateStr, const std::map<std::string, std::string>& variables);
    static std::string replaceTemplate(const std::string& templateStr, const std::function<std::string(const std::string&)>& callback);
    static std::vector<std::string> extractTemplateVariables(const std::string& templateStr);
    static bool hasTemplateVariables(const std::string& templateStr);
    
    // Advanced operations
    static std::string transliterate(const std::string& str, const std::map<char, char>& mapping);
    static std::string transliterate(const std::string& str, const std::map<std::string, std::string>& mapping);
    static std::string normalize(const std::string& str);
    static std::string normalizeUnicode(const std::string& str);
    static std::string removeDiacritics(const std::string& str);
    static std::string removeAccents(const std::string& str);
    
    // Performance utilities
    static std::string optimize(const std::string& str);
    static std::string compressWhitespace(const std::string& str);
    static std::string removeDuplicateLines(const std::string& str);
    static std::string removeDuplicateWords(const std::string& str);
    static std::string removeDuplicateCharacters(const std::string& str);
    
    // Debug utilities
    static std::string debugString(const std::string& str);
    static std::string debugStringWithQuotes(const std::string& str);
    static std::string debugStringWithEscapes(const std::string& str);
    static std::string debugStringWithHex(const std::string& str);
    
    // Constants
    static constexpr size_t MAX_STRING_LENGTH = 1024 * 1024; // 1MB
    static constexpr size_t DEFAULT_WRAP_WIDTH = 80;
    static constexpr double DEFAULT_FUZZY_THRESHOLD = 0.5;
};

// String builder class for efficient string construction
class StringBuilder {
public:
    StringBuilder();
    explicit StringBuilder(size_t capacity);
    explicit StringBuilder(const std::string& str);
    
    // Appending
    StringBuilder& append(const std::string& str);
    StringBuilder& append(const char* str);
    StringBuilder& append(char c);
    StringBuilder& append(int32_t value);
    StringBuilder& append(int64_t value);
    StringBuilder& append(uint32_t value);
    StringBuilder& append(uint64_t value);
    StringBuilder& append(float32 value);
    StringBuilder& append(float64 value);
    StringBuilder& append(bool value);
    
    template<typename T>
    StringBuilder& append(const T& value);
    
    StringBuilder& appendLine();
    StringBuilder& appendLine(const std::string& str);
    StringBuilder& appendLine(const char* str);
    StringBuilder& appendLine(char c);
    StringBuilder& appendLine(int32_t value);
    StringBuilder& appendLine(int64_t value);
    StringBuilder& appendLine(uint32_t value);
    StringBuilder& appendLine(uint64_t value);
    StringBuilder& appendLine(float32 value);
    StringBuilder& appendLine(float64 value);
    StringBuilder& appendLine(bool value);
    
    template<typename T>
    StringBuilder& appendLine(const T& value);
    
    // Formatting
    template<typename... Args>
    StringBuilder& appendFormat(const std::string& format, Args... args);
    
    template<typename... Args>
    StringBuilder& appendLineFormat(const std::string& format, Args... args);
    
    // Insertion
    StringBuilder& insert(size_t pos, const std::string& str);
    StringBuilder& insert(size_t pos, const char* str);
    StringBuilder& insert(size_t pos, char c);
    
    // Removal
    StringBuilder& remove(size_t pos, size_t length = 1);
    StringBuilder& removeLast();
    StringBuilder& removeLast(size_t count);
    StringBuilder& clear();
    
    // Replacement
    StringBuilder& replace(size_t pos, size_t length, const std::string& str);
    StringBuilder& replace(const std::string& from, const std::string& to);
    StringBuilder& replaceAll(const std::string& from, const std::string& to);
    
    // Case operations
    StringBuilder& toLower();
    StringBuilder& toUpper();
    StringBuilder& toTitle();
    StringBuilder& toCamelCase();
    StringBuilder& toPascalCase();
    StringBuilder& toSnakeCase();
    StringBuilder& toKebabCase();
    
    // Trimming
    StringBuilder& trim();
    StringBuilder& trimLeft();
    StringBuilder& trimRight();
    
    // Padding
    StringBuilder& padLeft(size_t width, char fillChar = ' ');
    StringBuilder& padRight(size_t width, char fillChar = ' ');
    StringBuilder& padCenter(size_t width, char fillChar = ' ');
    
    // Repeating
    StringBuilder& repeat(const std::string& str, size_t count);
    StringBuilder& repeat(char c, size_t count);
    
    // Reversing
    StringBuilder& reverse();
    
    // Utility
    size_t length() const;
    size_t capacity() const;
    bool isEmpty() const;
    char operator[](size_t index) const;
    char& operator[](size_t index);
    
    std::string toString() const;
    const char* c_str() const;
    
    // Conversion
    operator std::string() const;
    
private:
    std::vector<char> buffer_;
    size_t length_;
    
    void ensureCapacity(size_t requiredCapacity);
    void growBuffer();
};

// Template implementations
template<typename... Args>
std::string StringUtils::format(const std::string& format, Args... args) {
    size_t size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}

template<typename T>
StringBuilder& StringBuilder::append(const T& value) {
    return append(std::to_string(value));
}

template<typename T>
StringBuilder& StringBuilder::appendLine(const T& value) {
    return appendLine(std::to_string(value));
}

template<typename... Args>
StringBuilder& StringBuilder::appendFormat(const std::string& format, Args... args) {
    return append(StringUtils::format(format, args...));
}

template<typename... Args>
StringBuilder& StringBuilder::appendLineFormat(const std::string& format, Args... args) {
    return appendLine(StringUtils::format(format, args...));
}

// String utilities namespace
namespace StringUtilities {
    // Advanced string operations
    std::string interpolate(const std::string& str, const std::map<std::string, std::string>& variables);
    std::string interpolate(const std::string& str, const std::function<std::string(const std::string&)>& resolver);
    
    // String analysis
    std::vector<std::string> extractWords(const std::string& str);
    std::vector<std::string> extractSentences(const std::string& str);
    std::vector<std::string> extractParagraphs(const std::string& str);
    std::vector<std::string> extractLines(const std::string& str);
    std::vector<std::string> extractURLs(const std::string& str);
    std::vector<std::string> extractEmails(const std::string& str);
    std::vector<std::string> extractPhoneNumbers(const std::string& str);
    std::vector<std::string> extractIPAddresses(const std::string& str);
    std::vector<std::string> extractMACAddresses(const std::string& str);
    std::vector<std::string> extractUUIDs(const std::string& str);
    std::vector<std::string> extractHashes(const std::string& str);
    std::vector<std::string> extractCreditCards(const std::string& str);
    std::vector<std::string> extractHexColors(const std::string& str);
    std::vector<std::string> extractBase64Strings(const std::string& str);
    
    // String transformation
    std::string transliterateToASCII(const std::string& str);
    std::string removeNonASCII(const std::string& str);
    std::string removeNonPrintable(const std::string& str);
    std::string removeControlCharacters(const std::string& str);
    std::string removeZeroWidthCharacters(const std::string& str);
    std::string removeInvisibleCharacters(const std::string& str);
    
    // String validation
    bool isValidUTF8(const std::string& str);
    bool isValidUTF16(const std::string& str);
    bool isValidUTF32(const std::string& str);
    bool isValidASCII(const std::string& str);
    bool isValidLatin1(const std::string& str);
    bool isValidWindows1252(const std::string& str);
    bool isValidISO88591(const std::string& str);
    
    // String conversion
    std::string convertToUTF8(const std::string& str, StringEncoding fromEncoding);
    std::string convertFromUTF8(const std::string& str, StringEncoding toEncoding);
    std::string convertEncoding(const std::string& str, StringEncoding fromEncoding, StringEncoding toEncoding);
    
    // String metrics
    size_t getByteCount(const std::string& str, StringEncoding encoding);
    size_t getCharacterCount(const std::string& str, StringEncoding encoding);
    size_t getDisplayWidth(const std::string& str);
    size_t getDisplayHeight(const std::string& str, size_t maxWidth);
    
    // String comparison
    int compareLocale(const std::string& str1, const std::string& str2, const std::string& locale = "");
    int compareUnicode(const std::string& str1, const std::string& str2);
    int collate(const std::string& str1, const std::string& str2);
    
    // String sorting
    void sort(std::vector<std::string>& strings);
    void sortCaseInsensitive(std::vector<std::string>& strings);
    void sortNatural(std::vector<std::string>& strings);
    void sortLocale(std::vector<std::string>& strings, const std::string& locale = "");
    
    // String searching
    std::vector<size_t> findAllOccurrences(const std::string& str, const std::string& substr);
    std::vector<size_t> findAllOccurrences(const std::string& str, char c);
    std::vector<size_t> findAllWords(const std::string& str, const std::string& word);
    std::vector<size_t> findAllLines(const std::string& str, const std::string& line);
    
    // String patterns
    bool matchesPattern(const std::string& str, const std::string& pattern);
    std::vector<std::string> extractPattern(const std::string& str, const std::string& pattern);
    std::string replacePattern(const std::string& str, const std::string& pattern, const std::string& replacement);
    
    // String templates
    class StringTemplate {
    public:
        StringTemplate(const std::string& templateStr);
        
        void setVariable(const std::string& name, const std::string& value);
        void setVariable(const std::string& name, const std::function<std::string()> callback);
        void setVariables(const std::map<std::string, std::string>& variables);
        
        std::string render() const;
        std::string render(const std::map<std::string, std::string>& additionalVariables) const;
        
        std::vector<std::string> getVariables() const;
        bool hasVariable(const std::string& name) const;
        void removeVariable(const std::string& name);
        void clearVariables();
        
    private:
        std::string templateStr_;
        std::map<std::string, std::function<std::string()>> variables_;
        
        std::string replaceVariables(const std::string& str, const std::map<std::string, std::function<std::string()>>& variables) const;
    };
    
    // String cache
    class StringCache {
    public:
        StringCache(size_t maxSize = 1000);
        
        void put(const std::string& key, const std::string& value);
        std::string get(const std::string& key) const;
        bool contains(const std::string& key) const;
        void remove(const std::string& key);
        void clear();
        
        size_t size() const;
        size_t maxSize() const;
        void setMaxSize(size_t maxSize);
        
        double getHitRate() const;
        size_t getHitCount() const;
        size_t getMissCount() const;
        
    private:
        struct CacheEntry {
            std::string value;
            size_t accessCount;
            uint64_t lastAccessTime;
        };
        
        std::map<std::string, CacheEntry> cache_;
        size_t maxSize_;
        size_t hitCount_;
        size_t missCount_;
        
        void evictLeastRecentlyUsed();
    };
}

} // namespace RFUtils