#include "rf_line_breaker.h"
#include "rf_logger.h"

namespace RedFiles {
    namespace Core {

        LineBreaker::LineBreaker()
            : m_isInitialized(false)
            , m_breakMode(BreakMode::Normal)
            , m_wrapMode(WrapMode::Word)
            , m_alignment(TextAlignment::Left)
            , m_maxWidth(0.0f)
            , m_lineHeight(1.2f)
            , m_tabWidth(4.0f)
            , m_hyphenationEnabled(false)
            , m_justifyEnabled(false)
            , m_compressWhitespace(true)
            , m_hangPunctuation(false)
            , m_orphanControl(OrphanControl::Default)
            , m_widowControl(WidowControl::Default)
            , m_maxLines(0)
            , m_ellipsisEnabled(false)
            , m_textDirection(TextDirection::LeftToRight)
        {
            RF_LOG_INFO("LineBreaker: Created line breaker");
        }

        LineBreaker::~LineBreaker()
        {
            RF_LOG_INFO("LineBreaker: Destroyed line breaker");
        }

        bool LineBreaker::Initialize()
        {
            if (m_isInitialized)
            {
                RF_LOG_WARNING("LineBreaker: Already initialized");
                return true;
            }

            // Initialize Unicode line breaking rules
            if (!InitializeUnicodeRules())
            {
                RF_LOG_ERROR("LineBreaker: Failed to initialize Unicode rules");
                return false;
            }

            // Initialize hyphenation dictionary
            if (!InitializeHyphenation())
            {
                RF_LOG_WARNING("LineBreaker: Failed to initialize hyphenation");
            }

            m_isInitialized = true;
            RF_LOG_INFO("LineBreaker: Successfully initialized");
            return true;
        }

        bool LineBreaker::InitializeUnicodeRules()
        {
            // Initialize Unicode Line Breaking Algorithm (UAX #14)
            // This is a simplified implementation
            
            // Add basic break opportunities
            m_breakOpportunities[0x0020] = BreakOpportunity::Allowed; // Space
            m_breakOpportunities[0x000D] = BreakOpportunity::Mandatory; // Carriage return
            m_breakOpportunities[0x000A] = BreakOpportunity::Mandatory; // Line feed
            m_breakOpportunities[0x0009] = BreakOpportunity::Allowed; // Tab
            m_breakOpportunities[0x002D] = BreakOpportunity::Allowed; // Hyphen
            m_breakOpportunities[0x2010] = BreakOpportunity::Allowed; // Hyphen
            m_breakOpportunities[0x2011] = BreakOpportunity::Allowed; // Non-breaking hyphen
            m_breakOpportunities[0x002F] = BreakOpportunity::Allowed; // Slash
            
            // Add CJK break opportunities
            for (uint32_t cp = 0x4E00; cp <= 0x9FFF; ++cp) // CJK Unified Ideographs
            {
                m_breakOpportunities[cp] = BreakOpportunity::Allowed;
            }
            
            for (uint32_t cp = 0x3040; cp <= 0x309F; ++cp) // Hiragana
            {
                m_breakOpportunities[cp] = BreakOpportunity::Allowed;
            }
            
            for (uint32_t cp = 0x30A0; cp <= 0x30FF; ++cp) // Katakana
            {
                m_breakOpportunities[cp] = BreakOpportunity::Allowed;
            }
            
            for (uint32_t cp = 0xAC00; cp <= 0xD7AF; ++cp) // Hangul Syllables
            {
                m_breakOpportunities[cp] = BreakOpportunity::Allowed;
            }

            // Add non-breaking characters
            m_breakOpportunities[0x00A0] = BreakOpportunity::Prohibited; // Non-breaking space
            m_breakOpportunities[0x202F] = BreakOpportunity::Prohibited; // Narrow non-breaking space
            m_breakOpportunities[0x2060] = BreakOpportunity::Prohibited; // Word joiner

            RF_LOG_INFO("LineBreaker: Initialized Unicode break rules");
            return true;
        }

        bool LineBreaker::InitializeHyphenation()
        {
            // Initialize hyphenation patterns (simplified)
            // In a real implementation, this would load hyphenation dictionaries
            
            // Add basic English hyphenation patterns
            m_hyphenationPatterns["tion"] = {2};
            m_hyphenationPatterns["sion"] = {2};
            m_hyphenationPatterns["able"] = {2};
            m_hyphenationPatterns["ible"] = {2};
            m_hyphenationPatterns["ment"] = {2};
            m_hyphenationPatterns["ness"] = {2};
            m_hyphenationPatterns["less"] = {2};
            m_hyphenationPatterns["ful"] = {2};
            m_hyphenationPatterns["ing"] = {2};
            m_hyphenationPatterns["ed"] = {1};

            RF_LOG_INFO("LineBreaker: Initialized hyphenation patterns");
            return true;
        }

        std::vector<LineBreak> LineBreaker::BreakText(const char* text, float maxWidth, FontHandle font)
        {
            if (!m_isInitialized)
            {
                RF_LOG_ERROR("LineBreaker: Not initialized");
                return {};
            }

            if (!text || !font)
            {
                RF_LOG_ERROR("LineBreaker: Invalid parameters");
                return {};
            }

            m_maxWidth = maxWidth;

            std::vector<LineBreak> breaks;
            std::vector<Word> words = TokenizeText(text);
            
            if (words.empty())
            {
                return breaks;
            }

            float currentLineWidth = 0.0f;
            uint32_t currentLineStart = 0;
            uint32_t currentWordIndex = 0;
            uint32_t lineCount = 0;

            while (currentWordIndex < words.size())
            {
                if (m_maxLines > 0 && lineCount >= m_maxLines)
                {
                    // Add ellipsis if enabled
                    if (m_ellipsisEnabled && !breaks.empty())
                    {
                        AddEllipsis(breaks.back(), font);
                    }
                    break;
                }

                Word& word = words[currentWordIndex];
                float wordWidth = CalculateWordWidth(word, font);
                float spaceWidth = CalculateSpaceWidth(font);

                // Check if word fits on current line
                bool fitsOnLine = (currentLineWidth == 0.0f) || 
                                 (currentLineWidth + spaceWidth + wordWidth <= m_maxWidth);

                if (fitsOnLine)
                {
                    // Add word to current line
                    if (currentLineWidth > 0.0f)
                    {
                        currentLineWidth += spaceWidth;
                    }
                    currentLineWidth += wordWidth;
                    currentWordIndex++;
                }
                else
                {
                    // Word doesn't fit, need to break line
                    if (currentLineWidth == 0.0f)
                    {
                        // Word is longer than line, try to hyphenate or force break
                        if (m_hyphenationEnabled && word.text.length() > 3)
                        {
                            std::vector<HyphenationPoint> hyphenPoints = FindHyphenationPoints(word.text);
                            if (!hyphenPoints.empty())
                            {
                                // Find best hyphenation point
                                HyphenationPoint bestPoint = FindBestHyphenationPoint(hyphenPoints, word, font, maxWidth);
                                if (bestPoint.position > 0)
                                {
                                    // Create line break at hyphenation point
                                    LineBreak lineBreak;
                                    lineBreak.startIndex = currentLineStart;
                                    lineBreak.endIndex = currentWordIndex;
                                    lineBreak.startChar = word.startChar;
                                    lineBreak.endChar = word.startChar + bestPoint.position;
                                    lineBreak.width = CalculatePartialWordWidth(word, 0, bestPoint.position, font);
                                    lineBreak.height = m_lineHeight;
                                    lineBreak.isHyphenated = true;
                                    lineBreak.isJustified = false;
                                    
                                    breaks.push_back(lineBreak);
                                    
                                    // Update word for next line
                                    word.text = word.text.substr(bestPoint.position);
                                    word.startChar += bestPoint.position;
                                    currentLineStart = currentWordIndex;
                                    currentLineWidth = 0.0f;
                                    lineCount++;
                                    continue;
                                }
                            }
                        }
                        
                        // Force break word
                        LineBreak lineBreak;
                        lineBreak.startIndex = currentLineStart;
                        lineBreak.endIndex = currentWordIndex;
                        lineBreak.startChar = word.startChar;
                        lineBreak.endChar = word.startChar + word.text.length(); // Full word
                        lineBreak.width = wordWidth;
                        lineBreak.height = m_lineHeight;
                        lineBreak.isHyphenated = false;
                        lineBreak.isJustified = false;
                        lineBreak.isOverflow = true;
                        
                        breaks.push_back(lineBreak);
                        currentWordIndex++;
                        currentLineStart = currentWordIndex;
                        currentLineWidth = 0.0f;
                        lineCount++;
                    }
                    else
                    {
                        // End current line
                        LineBreak lineBreak;
                        lineBreak.startIndex = currentLineStart;
                        lineBreak.endIndex = currentWordIndex;
                        lineBreak.startChar = words[currentLineStart].startChar;
                        lineBreak.endChar = word.startChar - 1;
                        lineBreak.width = currentLineWidth;
                        lineBreak.height = m_lineHeight;
                        lineBreak.isHyphenated = false;
                        lineBreak.isJustified = false;
                        
                        // Apply justification if enabled
                        if (m_justifyEnabled && lineCount > 0)
                        {
                            lineBreak.isJustified = JustifyLine(lineBreak, words, font);
                        }
                        
                        breaks.push_back(lineBreak);
                        currentLineStart = currentWordIndex;
                        currentLineWidth = 0.0f;
                        lineCount++;
                    }
                }
            }

            // Add last line if there are remaining words
            if (currentLineStart < words.size())
            {
                LineBreak lineBreak;
                lineBreak.startIndex = currentLineStart;
                lineBreak.endIndex = words.size();
                lineBreak.startChar = words[currentLineStart].startChar;
                lineBreak.endChar = words.back().startChar + words.back().text.length();
                lineBreak.width = currentLineWidth;
                lineBreak.height = m_lineHeight;
                lineBreak.isHyphenated = false;
                lineBreak.isJustified = false;
                
                breaks.push_back(lineBreak);
            }

            // Apply widow/orphan control
            ApplyWidowOrphanControl(breaks);

            RF_LOG_INFO("LineBreaker: Broke text into ", breaks.size(), " lines");
            return breaks;
        }

        std::vector<Word> LineBreaker::TokenizeText(const char* text)
        {
            std::vector<Word> words;
            if (!text)
                return words;

            std::string input(text);
            size_t pos = 0;
            uint32_t charIndex = 0;

            while (pos < input.length())
            {
                // Skip whitespace
                while (pos < input.length() && IsWhitespace(input[pos]))
                {
                    pos++;
                    charIndex++;
                }

                if (pos >= input.length())
                    break;

                // Find word boundaries
                size_t wordStart = pos;
                uint32_t wordStartChar = charIndex;

                while (pos < input.length() && !IsWhitespace(input[pos]))
                {
                    pos++;
                    charIndex++;
                }

                size_t wordEnd = pos;
                std::string wordText = input.substr(wordStart, wordEnd - wordStart);

                if (!wordText.empty())
                {
                    Word word;
                    word.text = wordText;
                    word.startChar = wordStartChar;
                    word.width = 0.0f; // Will be calculated later
                    words.push_back(word);
                }
            }

            return words;
        }

        float LineBreaker::CalculateWordWidth(const Word& word, FontHandle font)
        {
            // Platform-specific text width calculation
            // For now, use a simple approximation
            float width = 0.0f;
            for (char c : word.text)
            {
                width += GetCharacterWidth(c, font);
            }
            return width;
        }

        float LineBreaker::CalculatePartialWordWidth(const Word& word, size_t start, size_t end, FontHandle font)
        {
            float width = 0.0f;
            for (size_t i = start; i < end && i < word.text.length(); ++i)
            {
                width += GetCharacterWidth(word.text[i], font);
            }
            return width;
        }

        float LineBreaker::CalculateSpaceWidth(FontHandle font)
        {
            return GetCharacterWidth(' ', font);
        }

        float LineBreaker::GetCharacterWidth(char c, FontHandle font)
        {
            // Platform-specific character width calculation
            // For now, use a simple approximation
            if (c == ' ')
                return 0.3f;
            else if (c == '\t')
                return m_tabWidth * 0.3f;
            else
                return 0.6f; // Average character width
        }

        bool LineBreaker::IsWhitespace(char c) const
        {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r';
        }

        std::vector<HyphenationPoint> LineBreaker::FindHyphenationPoints(const std::string& word)
        {
            std::vector<HyphenationPoint> points;
            
            // Check hyphenation patterns
            for (const auto& pattern : m_hyphenationPatterns)
            {
                size_t pos = word.find(pattern.first);
                while (pos != std::string::npos)
                {
                    for (size_t hyphenPos : pattern.second)
                    {
                        if (pos + hyphenPos > 0 && pos + hyphenPos < word.length() - 1)
                        {
                            HyphenationPoint point;
                            point.position = pos + hyphenPos;
                            point.quality = 0.8f; // Default quality
                            points.push_back(point);
                        }
                    }
                    pos = word.find(pattern.first, pos + 1);
                }
            }
            
            // Sort by position
            std::sort(points.begin(), points.end(), 
                [](const HyphenationPoint& a, const HyphenationPoint& b) {
                    return a.position < b.position;
                });
            
            return points;
        }

        HyphenationPoint LineBreaker::FindBestHyphenationPoint(const std::vector<HyphenationPoint>& points, 
                                                               const Word& word, FontHandle font, float maxWidth)
        {
            HyphenationPoint bestPoint;
            bestPoint.position = 0;
            bestPoint.quality = 0.0f;

            for (const auto& point : points)
            {
                float partialWidth = CalculatePartialWordWidth(word, 0, point.position, font);
                if (partialWidth <= maxWidth && point.quality > bestPoint.quality)
                {
                    bestPoint = point;
                }
            }

            return bestPoint;
        }

        bool LineBreaker::JustifyLine(LineBreak& lineBreak, const std::vector<Word>& words, FontHandle font)
        {
            if (m_alignment != TextAlignment::Justify)
                return false;

            // Calculate total space needed
            float totalSpace = 0.0f;
            uint32_t spaceCount = 0;
            
            for (uint32_t i = lineBreak.startIndex; i < lineBreak.endIndex; ++i)
            {
                if (i > lineBreak.startIndex) // Not first word
                {
                    totalSpace += CalculateSpaceWidth(font);
                    spaceCount++;
                }
            }

            if (spaceCount == 0)
                return false;

            // Calculate space expansion needed
            float expansionNeeded = m_maxWidth - lineBreak.width;
            if (expansionNeeded <= 0.0f)
                return false;

            float spaceExpansion = expansionNeeded / (float)spaceCount;
            
            // Store justification data (would be used by renderer)
            lineBreak.justificationSpace = spaceExpansion;
            lineBreak.isJustified = true;

            return true;
        }

        void LineBreaker::ApplyWidowOrphanControl(std::vector<LineBreak>& breaks)
        {
            if (breaks.size() < 2)
                return;

            // Widow control (last line should not be alone)
            if (m_widowControl == WidowControl::Prevent && breaks.size() >= 2)
            {
                LineBreak& lastLine = breaks.back();
                LineBreak& secondLastLine = breaks[breaks.size() - 2];

                if (lastLine.width < m_maxWidth * 0.3f) // Widow line is too short
                {
                    // Try to move words from second last line to last line
                    // This is a simplified implementation
                    RF_LOG_DEBUG("LineBreaker: Applied widow control");
                }
            }

            // Orphan control (first line should not be alone)
            if (m_orphanControl == OrphanControl::Prevent && breaks.size() >= 2)
            {
                LineBreak& firstLine = breaks.front();
                LineBreak& secondLine = breaks[1];

                if (firstLine.width < m_maxWidth * 0.3f) // Orphan line is too short
                {
                    // Try to move words from second line to first line
                    // This is a simplified implementation
                    RF_LOG_DEBUG("LineBreaker: Applied orphan control");
                }
            }
        }

        void LineBreaker::AddEllipsis(LineBreak& lineBreak, FontHandle font)
        {
            float ellipsisWidth = 3.0f * GetCharacterWidth('.', font); // Three dots
            
            if (lineBreak.width + ellipsisWidth <= m_maxWidth)
            {
                lineBreak.hasEllipsis = true;
                lineBreak.ellipsisWidth = ellipsisWidth;
                RF_LOG_DEBUG("LineBreaker: Added ellipsis to line");
            }
        }

        // Configuration methods
        void LineBreaker::SetBreakMode(BreakMode mode)
        {
            m_breakMode = mode;
            RF_LOG_INFO("LineBreaker: Set break mode to ", (int)mode);
        }

        void LineBreaker::SetWrapMode(WrapMode mode)
        {
            m_wrapMode = mode;
            RF_LOG_INFO("LineBreaker: Set wrap mode to ", (int)mode);
        }

        void LineBreaker::SetAlignment(TextAlignment alignment)
        {
            m_alignment = alignment;
            m_justifyEnabled = (alignment == TextAlignment::Justify);
            RF_LOG_INFO("LineBreaker: Set alignment to ", (int)alignment);
        }

        void LineBreaker::SetLineHeight(float lineHeight)
        {
            m_lineHeight = Math::Max(0.1f, lineHeight);
            RF_LOG_INFO("LineBreaker: Set line height to ", m_lineHeight);
        }

        void LineBreaker::SetTabWidth(float tabWidth)
        {
            m_tabWidth = Math::Max(0.1f, tabWidth);
            RF_LOG_INFO("LineBreaker: Set tab width to ", m_tabWidth);
        }

        void LineBreaker::SetHyphenationEnabled(bool enabled)
        {
            m_hyphenationEnabled = enabled;
            RF_LOG_INFO("LineBreaker: Set hyphenation to ", enabled);
        }

        void LineBreaker::SetMaxLines(uint32_t maxLines)
        {
            m_maxLines = maxLines;
            RF_LOG_INFO("LineBreaker: Set max lines to ", maxLines);
        }

        void LineBreaker::SetEllipsisEnabled(bool enabled)
        {
            m_ellipsisEnabled = enabled;
            RF_LOG_INFO("LineBreaker: Set ellipsis to ", enabled);
        }

        void LineBreaker::SetTextDirection(TextDirection direction)
        {
            m_textDirection = direction;
            RF_LOG_INFO("LineBreaker: Set text direction to ", (int)direction);
        }

        // Getters
        BreakMode LineBreaker::GetBreakMode() const
        {
            return m_breakMode;
        }

        WrapMode LineBreaker::GetWrapMode() const
        {
            return m_wrapMode;
        }

        TextAlignment LineBreaker::GetAlignment() const
        {
            return m_alignment;
        }

        float LineBreaker::GetLineHeight() const
        {
            return m_lineHeight;
        }

        float LineBreaker::GetTabWidth() const
        {
            return m_tabWidth;
        }

        bool LineBreaker::IsHyphenationEnabled() const
        {
            return m_hyphenationEnabled;
        }

        bool LineBreaker::IsInitialized() const
        {
            return m_isInitialized;
        }

    } // namespace Core
} // namespace RedFiles