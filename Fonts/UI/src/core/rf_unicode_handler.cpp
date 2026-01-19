#include "rf_unicode_handler.h"
#include "rf_logger.h"
#include <algorithm>
#include <cctype>

namespace RedFiles {
    namespace Core {

        UnicodeHandler::UnicodeHandler()
            : m_isInitialized(false)
            , m_normalizationForm(NormalizationForm::NFC)
            , m_caseFoldingEnabled(false)
            , m_compatibilityEnabled(false)
            , m_decompositionEnabled(true)
        {
            RF_LOG_INFO("UnicodeHandler: Created Unicode handler");
        }

        UnicodeHandler::~UnicodeHandler()
        {
            RF_LOG_INFO("UnicodeHandler: Destroyed Unicode handler");
        }

        bool UnicodeHandler::Initialize()
        {
            if (m_isInitialized)
            {
                RF_LOG_WARNING("UnicodeHandler: Already initialized");
                return true;
            }

            // Initialize Unicode data tables
            if (!InitializeUnicodeData())
            {
                RF_LOG_ERROR("UnicodeHandler: Failed to initialize Unicode data");
                return false;
            }

            // Initialize normalization tables
            if (!InitializeNormalizationTables())
            {
                RF_LOG_ERROR("UnicodeHandler: Failed to initialize normalization tables");
                return false;
            }

            // Initialize case mapping tables
            if (!InitializeCaseMappingTables())
            {
                RF_LOG_ERROR("UnicodeHandler: Failed to initialize case mapping tables");
                return false;
            }

            // Initialize script detection tables
            if (!InitializeScriptTables())
            {
                RF_LOG_ERROR("UnicodeHandler: Failed to initialize script tables");
                return false;
            }

            // Initialize bidirectional tables
            if (!InitializeBidirectionalTables())
            {
                RF_LOG_ERROR("UnicodeHandler: Failed to initialize bidirectional tables");
                return false;
            }

            // Initialize line breaking tables
            if (!InitializeLineBreakTables())
            {
                RF_LOG_ERROR("UnicodeHandler: Failed to initialize line break tables");
                return false;
            }

            m_isInitialized = true;
            RF_LOG_INFO("UnicodeHandler: Successfully initialized");
            return true;
        }

        bool UnicodeHandler::InitializeUnicodeData()
        {
            // Initialize basic Unicode properties
            // This is a simplified implementation
            
            // Initialize character categories
            InitializeCharacterCategories();
            
            // Initialize script ranges
            InitializeScriptRanges();
            
            // Initialize general properties
            InitializeGeneralProperties();

            RF_LOG_INFO("UnicodeHandler: Initialized Unicode data");
            return true;
        }

        void UnicodeHandler::InitializeCharacterCategories()
        {
            // Initialize basic character categories
            // ASCII characters
            for (uint32_t cp = 0x00; cp <= 0x7F; ++cp)
            {
                if (cp >= 0x30 && cp <= 0x39) // Digits
                {
                    m_characterCategories[cp] = CharacterCategory::Number;
                }
                else if (cp >= 0x41 && cp <= 0x5A) // Uppercase letters
                {
                    m_characterCategories[cp] = CharacterCategory::UppercaseLetter;
                }
                else if (cp >= 0x61 && cp <= 0x7A) // Lowercase letters
                {
                    m_characterCategories[cp] = CharacterCategory::LowercaseLetter;
                }
                else if (cp == 0x20) // Space
                {
                    m_characterCategories[cp] = CharacterCategory::SpaceSeparator;
                }
                else if (cp < 0x20 || cp == 0x7F) // Control characters
                {
                    m_characterCategories[cp] = CharacterCategory::Control;
                }
                else
                {
                    m_characterCategories[cp] = CharacterCategory::OtherPunctuation;
                }
            }

            // Extended Latin
            for (uint32_t cp = 0x80; cp <= 0xFF; ++cp)
            {
                if (cp >= 0xC0 && cp <= 0xD6) // Uppercase accented
                {
                    m_characterCategories[cp] = CharacterCategory::UppercaseLetter;
                }
                else if (cp >= 0xD8 && cp <= 0xDE) // Uppercase accented
                {
                    m_characterCategories[cp] = CharacterCategory::UppercaseLetter;
                }
                else if (cp >= 0xE0 && cp <= 0xF6) // Lowercase accented
                {
                    m_characterCategories[cp] = CharacterCategory::LowercaseLetter;
                }
                else if (cp >= 0xF8 && cp <= 0xFE) // Lowercase accented
                {
                    m_characterCategories[cp] = CharacterCategory::LowercaseLetter;
                }
                else
                {
                    m_characterCategories[cp] = CharacterCategory::OtherSymbol;
                }
            }

            // CJK characters
            for (uint32_t cp = 0x4E00; cp <= 0x9FFF; ++cp)
            {
                m_characterCategories[cp] = CharacterCategory::OtherLetter;
            }

            // Arabic
            for (uint32_t cp = 0x0600; cp <= 0x06FF; ++cp)
            {
                if (cp >= 0x0621 && cp <= 0x063A) // Letters
                {
                    m_characterCategories[cp] = CharacterCategory::OtherLetter;
                }
                else if (cp >= 0x0641 && cp <= 0x064A) // Letters
                {
                    m_characterCategories[cp] = CharacterCategory::OtherLetter;
                }
                else
                {
                    m_characterCategories[cp] = CharacterCategory::OtherSymbol;
                }
            }

            // Cyrillic
            for (uint32_t cp = 0x0400; cp <= 0x04FF; ++cp)
            {
                if (cp >= 0x0410 && cp <= 0x044F) // Basic Cyrillic
                {
                    if (cp <= 0x042F) // Uppercase
                    {
                        m_characterCategories[cp] = CharacterCategory::UppercaseLetter;
                    }
                    else // Lowercase
                    {
                        m_characterCategories[cp] = CharacterCategory::LowercaseLetter;
                    }
                }
                else
                {
                    m_characterCategories[cp] = CharacterCategory::OtherSymbol;
                }
            }
        }

        void UnicodeHandler::InitializeScriptRanges()
        {
            // Initialize script ranges for detection
            m_scriptRanges.push_back({0x0000, 0x007F, Script::Latin});
            m_scriptRanges.push_back({0x0080, 0x00FF, Script::Latin});
            m_scriptRanges.push_back({0x0100, 0x017F, Script::Latin});
            m_scriptRanges.push_back({0x0400, 0x04FF, Script::Cyrillic});
            m_scriptRanges.push_back({0x0530, 0x058F, Script::Armenian});
            m_scriptRanges.push_back({0x0590, 0x05FF, Script::Hebrew});
            m_scriptRanges.push_back({0x0600, 0x06FF, Script::Arabic});
            m_scriptRanges.push_back({0x0900, 0x097F, Script::Devanagari});
            m_scriptRanges.push_back({0x4E00, 0x9FFF, Script::Han});
            m_scriptRanges.push_back({0x3040, 0x309F, Script::Hiragana});
            m_scriptRanges.push_back({0x30A0, 0x30FF, Script::Katakana});
            m_scriptRanges.push_back({0xAC00, 0xD7AF, Script::Hangul});
            m_scriptRanges.push_back({0x0E00, 0x0E7F, Script::Thai});
        }

        void UnicodeHandler::InitializeGeneralProperties()
        {
            // Initialize general Unicode properties
            // Whitespace characters
            m_whitespaceChars.insert(0x0020); // Space
            m_whitespaceChars.insert(0x0009); // Tab
            m_whitespaceChars.insert(0x000A); // Line feed
            m_whitespaceChars.insert(0x000D); // Carriage return
            m_whitespaceChars.insert(0x00A0); // Non-breaking space
            m_whitespaceChars.insert(0x2000); // En quad
            m_whitespaceChars.insert(0x2001); // Em quad
            m_whitespaceChars.insert(0x2002); // En space
            m_whitespaceChars.insert(0x2003); // Em space
            m_whitespaceChars.insert(0x2004); // Three-per-em space
            m_whitespaceChars.insert(0x2005); // Four-per-em space
            m_whitespaceChars.insert(0x2006); // Six-per-em space
            m_whitespaceChars.insert(0x2007); // Figure space
            m_whitespaceChars.insert(0x2008); // Punctuation space
            m_whitespaceChars.insert(0x2009); // Thin space
            m_whitespaceChars.insert(0x200A); // Hair space
            m_whitespaceChars.insert(0x202F); // Narrow no-break space
            m_whitespaceChars.insert(0x205F); // Medium mathematical space
            m_whitespaceChars.insert(0x3000); // Ideographic space

            // Line separator characters
            m_lineSeparatorChars.insert(0x000A); // Line feed
            m_lineSeparatorChars.insert(0x000D); // Carriage return
            m_lineSeparatorChars.insert(0x0085); // Next line
            m_lineSeparatorChars.insert(0x2028); // Line separator
            m_lineSeparatorChars.insert(0x2029); // Paragraph separator

            // Paragraph separator characters
            m_paragraphSeparatorChars.insert(0x000D); // Carriage return
            m_paragraphSeparatorChars.insert(0x2029); // Paragraph separator
        }

        bool UnicodeHandler::InitializeNormalizationTables()
        {
            // Initialize Unicode normalization tables
            // This is a simplified implementation
            
            // Add some common normalization mappings
            m_decompositionMap[0x00C5] = {0x0041, 0x030A}; // Å -> A + ◌̊
            m_decompositionMap[0x00E5] = {0x0061, 0x030A}; // å -> a + ◌̊
            m_decompositionMap[0x00C6] = {0x0041, 0x0045}; // Æ -> AE
            m_decompositionMap[0x00E6] = {0x0061, 0x0065}; // æ -> ae
            m_decompositionMap[0x00D8] = {0x004F, 0x002F}; // Ø -> O/
            m_decompositionMap[0x00F8] = {0x006F, 0x002F}; // ø -> o/

            RF_LOG_INFO("UnicodeHandler: Initialized normalization tables");
            return true;
        }

        bool UnicodeHandler::InitializeCaseMappingTables()
        {
            // Initialize case mapping tables
            // This is a simplified implementation
            
            // ASCII case mappings
            for (uint32_t cp = 0x0041; cp <= 0x005A; ++cp) // A-Z
            {
                uint32_t lower = cp + 0x20; // a-z
                m_upperToLowerMap[cp] = lower;
                m_lowerToUpperMap[lower] = cp;
            }

            // Extended Latin case mappings
            m_upperToLowerMap[0x00C5] = 0x00E5; // Å -> å
            m_lowerToUpperMap[0x00E5] = 0x00C5; // å -> Å
            m_upperToLowerMap[0x00C6] = 0x00E6; // Æ -> æ
            m_lowerToUpperMap[0x00E6] = 0x00C6; // æ -> Æ
            m_upperToLowerMap[0x00D8] = 0x00F8; // Ø -> ø
            m_lowerToUpperMap[0x00F8] = 0x00D8; // ø -> Ø

            RF_LOG_INFO("UnicodeHandler: Initialized case mapping tables");
            return true;
        }

        bool UnicodeHandler::InitializeScriptTables()
        {
            // Script tables are already initialized in InitializeScriptRanges()
            RF_LOG_INFO("UnicodeHandler: Initialized script tables");
            return true;
        }

        bool UnicodeHandler::InitializeBidirectionalTables()
        {
            // Initialize bidirectional character classes
            // This is a simplified implementation
            
            // Left-to-Right characters
            for (uint32_t cp = 0x0041; cp <= 0x005A; ++cp) // A-Z
            {
                m_bidiClasses[cp] = BidiClass::LeftToRight;
            }
            for (uint32_t cp = 0x0061; cp <= 0x007A; ++cp) // a-z
            {
                m_bidiClasses[cp] = BidiClass::LeftToRight;
            }
            for (uint32_t cp = 0x0030; cp <= 0x0039; ++cp) // 0-9
            {
                m_bidiClasses[cp] = BidiClass::LeftToRight;
            }

            // Right-to-Left characters
            for (uint32_t cp = 0x0590; cp <= 0x05FF; ++cp) // Hebrew
            {
                m_bidiClasses[cp] = BidiClass::RightToLeft;
            }
            for (uint32_t cp = 0x0600; cp <= 0x06FF; ++cp) // Arabic
            {
                m_bidiClasses[cp] = BidiClass::RightToLeft;
            }

            // Neutral characters
            m_bidiClasses[0x0020] = BidiClass::Neutral; // Space
            m_bidiClasses[0x000A] = BidiClass::Neutral; // Line feed
            m_bidiClasses[0x000D] = BidiClass::Neutral; // Carriage return

            RF_LOG_INFO("UnicodeHandler: Initialized bidirectional tables");
            return true;
        }

        bool UnicodeHandler::InitializeLineBreakTables()
        {
            // Initialize line breaking properties
            // This is a simplified implementation
            
            // Characters that allow line breaks
            m_lineBreakProps[0x0020] = LineBreakClass::BreakOpportunity; // Space
            m_lineBreakProps[0x000D] = LineBreakClass::MandatoryBreak; // Carriage return
            m_lineBreakProps[0x000A] = LineBreakClass::MandatoryBreak; // Line feed
            m_lineBreakProps[0x0009] = LineBreakClass::BreakOpportunity; // Tab
            m_lineBreakProps[0x002D] = LineBreakClass::BreakOpportunity; // Hyphen

            // Characters that don't allow line breaks
            m_lineBreakProps[0x00A0] = LineBreakClass::NoBreak; // Non-breaking space
            m_lineBreakProps[0x202F] = LineBreakClass::NoBreak; // Narrow non-breaking space
            m_lineBreakProps[0x2060] = LineBreakClass::NoBreak; // Word joiner

            RF_LOG_INFO("UnicodeHandler: Initialized line break tables");
            return true;
        }

        std::vector<uint32_t> UnicodeHandler::DecodeUTF8(const char* text)
        {
            std::vector<uint32_t> codePoints;
            if (!text)
                return codePoints;

            size_t i = 0;
            while (text[i] != '\0')
            {
                uint32_t codePoint = 0;
                uint8_t byte = static_cast<uint8_t>(text[i]);

                if (byte < 0x80)
                {
                    // 1-byte sequence
                    codePoint = byte;
                    i += 1;
                }
                else if ((byte & 0xE0) == 0xC0)
                {
                    // 2-byte sequence
                    if (text[i + 1] == '\0') break;
                    codePoint = ((byte & 0x1F) << 6) | (static_cast<uint8_t>(text[i + 1]) & 0x3F);
                    i += 2;
                }
                else if ((byte & 0xF0) == 0xE0)
                {
                    // 3-byte sequence
                    if (text[i + 1] == '\0' || text[i + 2] == '\0') break;
                    codePoint = ((byte & 0x0F) << 12) | 
                               ((static_cast<uint8_t>(text[i + 1]) & 0x3F) << 6) |
                               (static_cast<uint8_t>(text[i + 2]) & 0x3F);
                    i += 3;
                }
                else if ((byte & 0xF8) == 0xF0)
                {
                    // 4-byte sequence
                    if (text[i + 1] == '\0' || text[i + 2] == '\0' || text[i + 3] == '\0') break;
                    codePoint = ((byte & 0x07) << 18) |
                               ((static_cast<uint8_t>(text[i + 1]) & 0x3F) << 12) |
                               ((static_cast<uint8_t>(text[i + 2]) & 0x3F) << 6) |
                               (static_cast<uint8_t>(text[i + 3]) & 0x3F);
                    i += 4;
                }
                else
                {
                    // Invalid UTF-8, skip
                    i += 1;
                    continue;
                }

                codePoints.push_back(codePoint);
            }

            return codePoints;
        }

        std::string UnicodeHandler::EncodeUTF8(const std::vector<uint32_t>& codePoints)
        {
            std::string result;
            
            for (uint32_t cp : codePoints)
            {
                if (cp <= 0x7F)
                {
                    // 1-byte sequence
                    result.push_back(static_cast<char>(cp));
                }
                else if (cp <= 0x7FF)
                {
                    // 2-byte sequence
                    result.push_back(static_cast<char>(0xC0 | ((cp >> 6) & 0x1F)));
                    result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
                }
                else if (cp <= 0xFFFF)
                {
                    // 3-byte sequence
                    result.push_back(static_cast<char>(0xE0 | ((cp >> 12) & 0x0F)));
                    result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
                }
                else if (cp <= 0x10FFFF)
                {
                    // 4-byte sequence
                    result.push_back(static_cast<char>(0xF0 | ((cp >> 18) & 0x07)));
                    result.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                    result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
                }
            }

            return result;
        }

        std::vector<uint32_t> UnicodeHandler::Normalize(const std::vector<uint32_t>& codePoints, NormalizationForm form)
        {
            std::vector<uint32_t> normalized = codePoints;

            switch (form)
            {
                case NormalizationForm::NFC:
                    normalized = NormalizeNFC(normalized);
                    break;
                case NormalizationForm::NFD:
                    normalized = NormalizeNFD(normalized);
                    break;
                case NormalizationForm::NFKC:
                    normalized = NormalizeNFKC(normalized);
                    break;
                case NormalizationForm::NFKD:
                    normalized = NormalizeNFKD(normalized);
                    break;
            }

            return normalized;
        }

        std::vector<uint32_t> UnicodeHandler::NormalizeNFC(const std::vector<uint32_t>& codePoints)
        {
            // Simplified NFC normalization
            std::vector<uint32_t> result;
            
            for (uint32_t cp : codePoints)
            {
                // Check if character has decomposition
                auto it = m_decompositionMap.find(cp);
                if (it != m_decompositionMap.end())
                {
                    // Add decomposed characters
                    for (uint32_t decomposed : it->second)
                    {
                        result.push_back(decomposed);
                    }
                }
                else
                {
                    result.push_back(cp);
                }
            }

            // Apply canonical ordering (simplified)
            return result;
        }

        std::vector<uint32_t> UnicodeHandler::NormalizeNFD(const std::vector<uint32_t>& codePoints)
        {
            // Simplified NFD normalization (decomposition only)
            std::vector<uint32_t> result;
            
            for (uint32_t cp : codePoints)
            {
                auto it = m_decompositionMap.find(cp);
                if (it != m_decompositionMap.end())
                {
                    for (uint32_t decomposed : it->second)
                    {
                        result.push_back(decomposed);
                    }
                }
                else
                {
                    result.push_back(cp);
                }
            }

            return result;
        }

        std::vector<uint32_t> UnicodeHandler::NormalizeNFKC(const std::vector<uint32_t>& codePoints)
        {
            // Simplified NFKC normalization
            // For now, just use NFC
            return NormalizeNFC(codePoints);
        }

        std::vector<uint32_t> UnicodeHandler::NormalizeNFKD(const std::vector<uint32_t>& codePoints)
        {
            // Simplified NFKD normalization
            // For now, just use NFD
            return NormalizeNFD(codePoints);
        }

        uint32_t UnicodeHandler::ToLower(uint32_t codePoint)
        {
            auto it = m_upperToLowerMap.find(codePoint);
            if (it != m_upperToLowerMap.end())
            {
                return it->second;
            }
            
            // Default case: use simple ASCII conversion
            if (codePoint >= 0x0041 && codePoint <= 0x005A)
            {
                return codePoint + 0x20;
            }
            
            return codePoint;
        }

        uint32_t UnicodeHandler::ToUpper(uint32_t codePoint)
        {
            auto it = m_lowerToUpperMap.find(codePoint);
            if (it != m_lowerToUpperMap.end())
            {
                return it->second;
            }
            
            // Default case: use simple ASCII conversion
            if (codePoint >= 0x0061 && codePoint <= 0x007A)
            {
                return codePoint - 0x20;
            }
            
            return codePoint;
        }

        std::vector<uint32_t> UnicodeHandler::ToLower(const std::vector<uint32_t>& codePoints)
        {
            std::vector<uint32_t> result;
            result.reserve(codePoints.size());
            
            for (uint32_t cp : codePoints)
            {
                result.push_back(ToLower(cp));
            }
            
            return result;
        }

        std::vector<uint32_t> UnicodeHandler::ToUpper(const std::vector<uint32_t>& codePoints)
        {
            std::vector<uint32_t> result;
            result.reserve(codePoints.size());
            
            for (uint32_t cp : codePoints)
            {
                result.push_back(ToUpper(cp));
            }
            
            return result;
        }

        CharacterCategory UnicodeHandler::GetCategory(uint32_t codePoint)
        {
            auto it = m_characterCategories.find(codePoint);
            if (it != m_characterCategories.end())
            {
                return it->second;
            }
            
            return CharacterCategory::OtherSymbol;
        }

        Script UnicodeHandler::GetScript(uint32_t codePoint)
        {
            for (const auto& range : m_scriptRanges)
            {
                if (codePoint >= range.start && codePoint <= range.end)
                {
                    return range.script;
                }
            }
            
            return Script::Unknown;
        }

        BidiClass UnicodeHandler::GetBidiClass(uint32_t codePoint)
        {
            auto it = m_bidiClasses.find(codePoint);
            if (it != m_bidiClasses.end())
            {
                return it->second;
            }
            
            return BidiClass::Neutral;
        }

        LineBreakClass UnicodeHandler::GetLineBreakClass(uint32_t codePoint)
        {
            auto it = m_lineBreakProps.find(codePoint);
            if (it != m_lineBreakProps.end())
            {
                return it->second;
            }
            
            return LineBreakClass::Unknown;
        }

        bool UnicodeHandler::IsWhitespace(uint32_t codePoint)
        {
            return m_whitespaceChars.find(codePoint) != m_whitespaceChars.end();
        }

        bool UnicodeHandler::IsLineSeparator(uint32_t codePoint)
        {
            return m_lineSeparatorChars.find(codePoint) != m_lineSeparatorChars.end();
        }

        bool UnicodeHandler::IsParagraphSeparator(uint32_t codePoint)
        {
            return m_paragraphSeparatorChars.find(codePoint) != m_paragraphSeparatorChars.end();
        }

        bool UnicodeHandler::IsControl(uint32_t codePoint)
        {
            return GetCategory(codePoint) == CharacterCategory::Control;
        }

        bool UnicodeHandler::IsLetter(uint32_t codePoint)
        {
            CharacterCategory category = GetCategory(codePoint);
            return category == CharacterCategory::UppercaseLetter ||
                   category == CharacterCategory::LowercaseLetter ||
                   category == CharacterCategory::TitlecaseLetter ||
                   category == CharacterCategory::ModifierLetter ||
                   category == CharacterCategory::OtherLetter;
        }

        bool UnicodeHandler::IsNumber(uint32_t codePoint)
        {
            CharacterCategory category = GetCategory(codePoint);
            return category == CharacterCategory::Number ||
                   category == CharacterCategory::DecimalNumber ||
                   category == CharacterCategory::LetterNumber ||
                   category == CharacterCategory::OtherNumber;
        }

        bool UnicodeHandler::IsPunctuation(uint32_t codePoint)
        {
            CharacterCategory category = GetCategory(codePoint);
            return category == CharacterCategory::ConnectorPunctuation ||
                   category == CharacterCategory::DashPunctuation ||
                   category == CharacterCategory::OpenPunctuation ||
                   category == CharacterCategory::ClosePunctuation ||
                   category == CharacterCategory::InitialPunctuation ||
                   category == CharacterCategory::FinalPunctuation ||
                   category == CharacterCategory::OtherPunctuation;
        }

        bool UnicodeHandler::IsSymbol(uint32_t codePoint)
        {
            CharacterCategory category = GetCategory(codePoint);
            return category == CharacterCategory::MathSymbol ||
                   category == CharacterCategory::CurrencySymbol ||
                   category == CharacterCategory::ModifierSymbol ||
                   category == CharacterCategory::OtherSymbol;
        }

        bool UnicodeHandler::IsValidCodePoint(uint32_t codePoint)
        {
            return codePoint <= 0x10FFFF && 
                   (codePoint < 0xD800 || codePoint > 0xDFFF); // Not surrogate
        }

        bool UnicodeHandler::IsPrintable(uint32_t codePoint)
        {
            return !IsControl(codePoint) && codePoint != 0x2028 && codePoint != 0x2029;
        }

        // Configuration methods
        void UnicodeHandler::SetNormalizationForm(NormalizationForm form)
        {
            m_normalizationForm = form;
            RF_LOG_INFO("UnicodeHandler: Set normalization form to ", (int)form);
        }

        void UnicodeHandler::SetCaseFoldingEnabled(bool enabled)
        {
            m_caseFoldingEnabled = enabled;
            RF_LOG_INFO("UnicodeHandler: Set case folding to ", enabled);
        }

        void UnicodeHandler::SetCompatibilityEnabled(bool enabled)
        {
            m_compatibilityEnabled = enabled;
            RF_LOG_INFO("UnicodeHandler: Set compatibility to ", enabled);
        }

        void UnicodeHandler::SetDecompositionEnabled(bool enabled)
        {
            m_decompositionEnabled = enabled;
            RF_LOG_INFO("UnicodeHandler: Set decomposition to ", enabled);
        }

        // Getters
        NormalizationForm UnicodeHandler::GetNormalizationForm() const
        {
            return m_normalizationForm;
        }

        bool UnicodeHandler::IsCaseFoldingEnabled() const
        {
            return m_caseFoldingEnabled;
        }

        bool UnicodeHandler::IsCompatibilityEnabled() const
        {
            return m_compatibilityEnabled;
        }

        bool UnicodeHandler::IsDecompositionEnabled() const
        {
            return m_decompositionEnabled;
        }

        bool UnicodeHandler::IsInitialized() const
        {
            return m_isInitialized;
        }

    } // namespace Core
} // namespace RedFiles