/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _UTIL_H
#define _UTIL_H

#include "Containers.h"
#include "Define.h"
#include "Errors.h"
#include <algorithm>
#include <array>
#include <cctype>
#include <list>
#include <map>
#include <string>
#include <vector>

// Searcher for map of structs
template<typename T, class S> struct Finder
{
    T val_;
    T S::* idMember_;

    Finder(T val, T S::* idMember) : val_(val), idMember_(idMember) {}
    bool operator()(const std::pair<int, S>& obj) { return obj.second.*idMember_ == val_; }
};

class Tokenizer
{
public:
    typedef std::vector<char const*> StorageType;

    typedef StorageType::size_type size_type;

    typedef StorageType::const_iterator const_iterator;
    typedef StorageType::reference reference;
    typedef StorageType::const_reference const_reference;

public:
    Tokenizer(const std::string& src, char const sep, uint32 vectorReserve = 0);
    ~Tokenizer() { delete[] m_str; }

    [[nodiscard]] const_iterator begin() const { return m_storage.begin(); }
    [[nodiscard]] const_iterator end() const { return m_storage.end(); }

    [[nodiscard]] size_type size() const { return m_storage.size(); }

    reference operator [] (size_type i) { return m_storage[i]; }
    const_reference operator [] (size_type i) const { return m_storage[i]; }

private:
    char* m_str;
    StorageType m_storage;
};

struct tm* localtime_r(time_t const* time, struct tm* result);
time_t LocalTimeToUTCTime(time_t time);
time_t GetLocalHourTimestamp(time_t time, uint8 hour, bool onlyAfterTime = true);
tm TimeBreakdown(time_t t);

void stripLineInvisibleChars(std::string& src);

int32 MoneyStringToMoney(const std::string& moneyString);

std::string secsToTimeString(uint64 timeInSecs, bool shortText = false);
uint32 TimeStringToSecs(const std::string& timestring);
std::string TimeToTimestampStr(time_t t);
std::string TimeToHumanReadable(time_t t);

inline void ApplyPercentModFloatVar(float& var, float val, bool apply)
{
    if (val == -100.0f)     // prevent set var to zero
    {
        val = -99.99f;
    }
    var *= (apply ? (100.0f + val) / 100.0f : 100.0f / (100.0f + val));
}

// Percentage calculation
template <class T, class U>
inline T CalculatePct(T base, U pct)
{
    return T(base * static_cast<float>(pct) / 100.0f);
}

template <class T, class U>
inline T AddPct(T& base, U pct)
{
    return base += CalculatePct(base, pct);
}

template <class T, class U>
inline T ApplyPct(T& base, U pct)
{
    return base = CalculatePct(base, pct);
}

template <class T>
inline T RoundToInterval(T& num, T floor, T ceil)
{
    return num = std::min(std::max(num, floor), ceil);
}

// UTF8 handling
AC_COMMON_API bool Utf8toWStr(std::string_view utf8str, std::wstring& wstr);

// in wsize==max size of buffer, out wsize==real string size
AC_COMMON_API bool Utf8toWStr(char const* utf8str, size_t csize, wchar_t* wstr, size_t& wsize);

inline bool Utf8toWStr(std::string_view utf8str, wchar_t* wstr, size_t& wsize)
{
    return Utf8toWStr(utf8str.data(), utf8str.size(), wstr, wsize);
}

AC_COMMON_API bool WStrToUtf8(std::wstring_view wstr, std::string& utf8str);

// size==real string size
AC_COMMON_API bool WStrToUtf8(wchar_t const* wstr, size_t size, std::string& utf8str);

// set string to "" if invalid utf8 sequence
size_t utf8length(std::string& utf8str);
void utf8truncate(std::string& utf8str, size_t len);

inline bool isBasicLatinCharacter(wchar_t wchar)
{
    if (wchar >= L'a' && wchar <= L'z')                      // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
    {
        return true;
    }
    if (wchar >= L'A' && wchar <= L'Z')                      // LATIN CAPITAL LETTER A - LATIN CAPITAL LETTER Z
    {
        return true;
    }
    return false;
}

inline bool isExtendedLatinCharacter(wchar_t wchar)
{
    if (isBasicLatinCharacter(wchar))
    {
        return true;
    }
    if (wchar >= 0x00C0 && wchar <= 0x00D6)                  // LATIN CAPITAL LETTER A WITH GRAVE - LATIN CAPITAL LETTER O WITH DIAERESIS
    {
        return true;
    }
    if (wchar >= 0x00D8 && wchar <= 0x00DE)                  // LATIN CAPITAL LETTER O WITH STROKE - LATIN CAPITAL LETTER THORN
    {
        return true;
    }
    if (wchar == 0x00DF)                                     // LATIN SMALL LETTER SHARP S
    {
        return true;
    }
    if (wchar >= 0x00E0 && wchar <= 0x00F6)                  // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER O WITH DIAERESIS
    {
        return true;
    }
    if (wchar >= 0x00F8 && wchar <= 0x00FE)                  // LATIN SMALL LETTER O WITH STROKE - LATIN SMALL LETTER THORN
    {
        return true;
    }
    if (wchar >= 0x0100 && wchar <= 0x012F)                  // LATIN CAPITAL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK
    {
        return true;
    }
    if (wchar == 0x1E9E)                                     // LATIN CAPITAL LETTER SHARP S
    {
        return true;
    }
    return false;
}

inline bool isCyrillicCharacter(wchar_t wchar)
{
    if (wchar >= 0x0410 && wchar <= 0x044F)                  // CYRILLIC CAPITAL LETTER A - CYRILLIC SMALL LETTER YA
    {
        return true;
    }
    if (wchar == 0x0401 || wchar == 0x0451)                  // CYRILLIC CAPITAL LETTER IO, CYRILLIC SMALL LETTER IO
    {
        return true;
    }
    return false;
}

inline bool isEastAsianCharacter(wchar_t wchar)
{
    if (wchar >= 0x1100 && wchar <= 0x11F9)                  // Hangul Jamo
    {
        return true;
    }
    if (wchar >= 0x3041 && wchar <= 0x30FF)                  // Hiragana + Katakana
    {
        return true;
    }
    if (wchar >= 0x3131 && wchar <= 0x318E)                  // Hangul Compatibility Jamo
    {
        return true;
    }
    if (wchar >= 0x31F0 && wchar <= 0x31FF)                  // Katakana Phonetic Ext.
    {
        return true;
    }
    if (wchar >= 0x3400 && wchar <= 0x4DB5)                  // CJK Ideographs Ext. A
    {
        return true;
    }
    if (wchar >= 0x4E00 && wchar <= 0x9FC3)                  // Unified CJK Ideographs
    {
        return true;
    }
    if (wchar >= 0xAC00 && wchar <= 0xD7A3)                  // Hangul Syllables
    {
        return true;
    }
    if (wchar >= 0xFF01 && wchar <= 0xFFEE)                  // Halfwidth forms
    {
        return true;
    }
    return false;
}

inline bool isNumeric(wchar_t wchar)
{
    return (wchar >= L'0' && wchar <= L'9');
}

inline bool isNumeric(char c)
{
    return (c >= '0' && c <= '9');
}

inline bool isNumeric(char const* str)
{
    for (char const* c = str; *c; ++c)
        if (!isNumeric(*c))
        {
            return false;
        }

    return true;
}

inline bool isNumericOrSpace(wchar_t wchar)
{
    return isNumeric(wchar) || wchar == L' ';
}

inline bool isBasicLatinString(std::wstring_view wstr, bool numericOrSpace)
{
    for (wchar_t i : wstr)
        if (!isBasicLatinCharacter(i) && (!numericOrSpace || !isNumericOrSpace(i)))
        {
            return false;
        }
    return true;
}

inline bool isExtendedLatinString(std::wstring_view wstr, bool numericOrSpace)
{
    for (wchar_t i : wstr)
        if (!isExtendedLatinCharacter(i) && (!numericOrSpace || !isNumericOrSpace(i)))
        {
            return false;
        }
    return true;
}

inline bool isCyrillicString(std::wstring_view wstr, bool numericOrSpace)
{
    for (wchar_t i : wstr)
        if (!isCyrillicCharacter(i) && (!numericOrSpace || !isNumericOrSpace(i)))
        {
            return false;
        }
    return true;
}

inline bool isEastAsianString(std::wstring_view wstr, bool numericOrSpace)
{
    for (wchar_t i : wstr)
        if (!isEastAsianCharacter(i) && (!numericOrSpace || !isNumericOrSpace(i)))
        {
            return false;
        }
    return true;
}

inline char charToUpper(char c) { return std::toupper(c); }
inline char charToLower(char c) { return std::tolower(c); }

inline wchar_t wcharToUpper(wchar_t wchar)
{
    if (wchar >= L'a' && wchar <= L'z')                      // LATIN SMALL LETTER A - LATIN SMALL LETTER Z
    {
        return wchar_t(uint16(wchar) - 0x0020);
    }
    if (wchar == 0x00DF)                                     // LATIN SMALL LETTER SHARP S
    {
        return wchar_t(0x1E9E);
    }
    if (wchar >= 0x00E0 && wchar <= 0x00F6)                  // LATIN SMALL LETTER A WITH GRAVE - LATIN SMALL LETTER O WITH DIAERESIS
    {
        return wchar_t(uint16(wchar) - 0x0020);
    }
    if (wchar >= 0x00F8 && wchar <= 0x00FE)                  // LATIN SMALL LETTER O WITH STROKE - LATIN SMALL LETTER THORN
    {
        return wchar_t(uint16(wchar) - 0x0020);
    }
    if (wchar >= 0x0101 && wchar <= 0x012F)                  // LATIN SMALL LETTER A WITH MACRON - LATIN SMALL LETTER I WITH OGONEK (only %2=1)
    {
        if (wchar % 2 == 1)
        {
            return wchar_t(uint16(wchar) - 0x0001);
        }
    }
    if (wchar >= 0x0430 && wchar <= 0x044F)                  // CYRILLIC SMALL LETTER A - CYRILLIC SMALL LETTER YA
    {
        return wchar_t(uint16(wchar) - 0x0020);
    }
    if (wchar == 0x0451)                                     // CYRILLIC SMALL LETTER IO
    {
        return wchar_t(0x0401);
    }

    return wchar;
}

inline wchar_t wcharToUpperOnlyLatin(wchar_t wchar)
{
    return isBasicLatinCharacter(wchar) ? wcharToUpper(wchar) : wchar;
}

inline wchar_t wcharToLower(wchar_t wchar)
{
    if (wchar >= L'A' && wchar <= L'Z')                      // LATIN CAPITAL LETTER A - LATIN CAPITAL LETTER Z
    {
        return wchar_t(uint16(wchar) + 0x0020);
    }
    if (wchar >= 0x00C0 && wchar <= 0x00D6)                  // LATIN CAPITAL LETTER A WITH GRAVE - LATIN CAPITAL LETTER O WITH DIAERESIS
    {
        return wchar_t(uint16(wchar) + 0x0020);
    }
    if (wchar >= 0x00D8 && wchar <= 0x00DE)                  // LATIN CAPITAL LETTER O WITH STROKE - LATIN CAPITAL LETTER THORN
    {
        return wchar_t(uint16(wchar) + 0x0020);
    }
    if (wchar >= 0x0100 && wchar <= 0x012E)                  // LATIN CAPITAL LETTER A WITH MACRON - LATIN CAPITAL LETTER I WITH OGONEK (only %2=0)
    {
        if (wchar % 2 == 0)
        {
            return wchar_t(uint16(wchar) + 0x0001);
        }
    }
    if (wchar == 0x1E9E)                                     // LATIN CAPITAL LETTER SHARP S
    {
        return wchar_t(0x00DF);
    }
    if (wchar == 0x0401)                                     // CYRILLIC CAPITAL LETTER IO
    {
        return wchar_t(0x0451);
    }
    if (wchar >= 0x0410 && wchar <= 0x042F)                  // CYRILLIC CAPITAL LETTER A - CYRILLIC CAPITAL LETTER YA
    {
        return wchar_t(uint16(wchar) + 0x0020);
    }

    return wchar;
}

void wstrToUpper(std::wstring& str);
void wstrToLower(std::wstring& str);

std::wstring GetMainPartOfName(std::wstring const& wname, uint32 declension);

AC_COMMON_API bool utf8ToConsole(std::string_view utf8str, std::string& conStr);
AC_COMMON_API bool consoleToUtf8(std::string_view conStr, std::string& utf8str);
AC_COMMON_API bool Utf8FitTo(std::string_view str, std::wstring_view search);
AC_COMMON_API void utf8printf(FILE* out, const char* str, ...);
AC_COMMON_API void vutf8printf(FILE* out, const char* str, va_list* ap);
AC_COMMON_API bool Utf8ToUpperOnlyLatin(std::string& utf8String);

bool IsIPAddress(char const* ipaddress);

uint32 CreatePIDFile(const std::string& filename);
uint32 GetPID();

namespace Acore::Impl
{
    AC_COMMON_API std::string ByteArrayToHexStr(uint8 const* bytes, size_t length, bool reverse = false);
    AC_COMMON_API void HexStrToByteArray(std::string_view str, uint8* out, size_t outlen, bool reverse = false);
}

template<typename Container>
std::string ByteArrayToHexStr(Container const& c, bool reverse = false)
{
    return Acore::Impl::ByteArrayToHexStr(std::data(c), std::size(c), reverse);
}

template<size_t Size>
void HexStrToByteArray(std::string_view str, std::array<uint8, Size>& buf, bool reverse = false)
{
    Acore::Impl::HexStrToByteArray(str, buf.data(), Size, reverse);
}

template<size_t Size>
std::array<uint8, Size> HexStrToByteArray(std::string_view str, bool reverse = false)
{
    std::array<uint8, Size> arr;
    HexStrToByteArray(str, arr, reverse);
    return arr;
}

AC_COMMON_API bool StringEqualI(std::string_view str1, std::string_view str2);
inline bool StringStartsWith(std::string_view haystack, std::string_view needle) { return (haystack.substr(0, needle.length()) == needle); }
inline bool StringStartsWithI(std::string_view haystack, std::string_view needle) { return StringEqualI(haystack.substr(0, needle.length()), needle); }
AC_COMMON_API bool StringContainsStringI(std::string_view haystack, std::string_view needle);

template <typename T>
inline bool ValueContainsStringI(std::pair<T, std::string_view> const& haystack, std::string_view needle)
{
    return StringContainsStringI(haystack.second, needle);
}

AC_COMMON_API bool StringCompareLessI(std::string_view a, std::string_view b);

struct StringCompareLessI_T
{
    bool operator()(std::string_view a, std::string_view b) const { return StringCompareLessI(a, b); }
};

// simple class for not-modifyable list
template <typename T>
class HookList
{
    typedef typename std::list<T>::iterator ListIterator;
private:
    typename std::list<T> m_list;
public:
    HookList<T>& operator+=(T t)
    {
        m_list.push_back(t);
        return *this;
    }
    HookList<T>& operator-=(T t)
    {
        m_list.remove(t);
        return *this;
    }
    size_t size()
    {
        return m_list.size();
    }
    ListIterator begin()
    {
        return m_list.begin();
    }
    ListIterator end()
    {
        return m_list.end();
    }
};

class flag96
{
private:
    uint32 part[3];

public:
    flag96(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0)
    {
        part[0] = p1;
        part[1] = p2;
        part[2] = p3;
    }

    [[nodiscard]] inline bool IsEqual(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0) const
    {
        return (part[0] == p1 && part[1] == p2 && part[2] == p3);
    }

    [[nodiscard]] inline bool HasFlag(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0) const
    {
        return (part[0] & p1 || part[1] & p2 || part[2] & p3);
    }

    inline void Set(uint32 p1 = 0, uint32 p2 = 0, uint32 p3 = 0)
    {
        part[0] = p1;
        part[1] = p2;
        part[2] = p3;
    }

    inline bool operator<(flag96 const& right) const
    {
        for (uint8 i = 3; i > 0; --i)
        {
            if (part[i - 1] < right.part[i - 1])
            {
                return true;
            }
            else if (part[i - 1] > right.part[i - 1])
            {
                return false;
            }
        }
        return false;
    }

    inline bool operator==(flag96 const& right) const
    {
        return
            (
                part[0] == right.part[0] &&
                part[1] == right.part[1] &&
                part[2] == right.part[2]
            );
    }

    inline bool operator!=(flag96 const& right) const
    {
        return !(*this == right);
    }

    inline flag96& operator=(flag96 const& right)
    {
        part[0] = right.part[0];
        part[1] = right.part[1];
        part[2] = right.part[2];
        return *this;
    }
    /* requried as of C++ 11 */
#if __cplusplus >= 201103L
    flag96(const flag96&) = default;
    flag96(flag96&&) = default;
#endif

    inline flag96 operator&(flag96 const& right) const
    {
        return flag96(part[0] & right.part[0], part[1] & right.part[1], part[2] & right.part[2]);
    }

    inline flag96& operator&=(flag96 const& right)
    {
        part[0] &= right.part[0];
        part[1] &= right.part[1];
        part[2] &= right.part[2];
        return *this;
    }

    inline flag96 operator|(flag96 const& right) const
    {
        return flag96(part[0] | right.part[0], part[1] | right.part[1], part[2] | right.part[2]);
    }

    inline flag96& operator |=(flag96 const& right)
    {
        part[0] |= right.part[0];
        part[1] |= right.part[1];
        part[2] |= right.part[2];
        return *this;
    }

    inline flag96 operator~() const
    {
        return flag96(~part[0], ~part[1], ~part[2]);
    }

    inline flag96 operator^(flag96 const& right) const
    {
        return flag96(part[0] ^ right.part[0], part[1] ^ right.part[1], part[2] ^ right.part[2]);
    }

    inline flag96& operator^=(flag96 const& right)
    {
        part[0] ^= right.part[0];
        part[1] ^= right.part[1];
        part[2] ^= right.part[2];
        return *this;
    }

    inline operator bool() const
    {
        return (part[0] != 0 || part[1] != 0 || part[2] != 0);
    }

    inline bool operator !() const
    {
        return !(bool(*this));
    }

    inline uint32& operator[](uint8 el)
    {
        return part[el];
    }

    inline uint32 const& operator [](uint8 el) const
    {
        return part[el];
    }
};

enum ComparisionType
{
    COMP_TYPE_EQ = 0,
    COMP_TYPE_HIGH,
    COMP_TYPE_LOW,
    COMP_TYPE_HIGH_EQ,
    COMP_TYPE_LOW_EQ,
    COMP_TYPE_MAX
};

template <class T>
bool CompareValues(ComparisionType type, T val1, T val2)
{
    switch (type)
    {
        case COMP_TYPE_EQ:
            return val1 == val2;
        case COMP_TYPE_HIGH:
            return val1 > val2;
        case COMP_TYPE_LOW:
            return val1 < val2;
        case COMP_TYPE_HIGH_EQ:
            return val1 >= val2;
        case COMP_TYPE_LOW_EQ:
            return val1 <= val2;
        default:
            // incorrect parameter
            ABORT();
            return false;
    }
}

class EventMap
{
    typedef std::multimap<uint32, uint32> EventStore;

public:
    EventMap()  { }

    /**
    * @name Reset
    * @brief Removes all scheduled events and resets time and phase.
    */
    void Reset()
    {
        _eventMap.clear();
        _time = 0;
        _phase = 0;
    }

    /**
     * @name Update
     * @brief Updates the timer of the event map.
     * @param time Value to be added to time.
     */
    void Update(uint32 time)
    {
        _time += time;
    }

    /**
    * @name GetTimer
    * @return Current timer value.
    */
    [[nodiscard]] uint32 GetTimer() const
    {
        return _time;
    }

    void SetTimer(uint32 time)
    {
        _time = time;
    }

    /**
    * @name GetPhaseMask
    * @return Active phases as mask.
    */
    [[nodiscard]] uint8 GetPhaseMask() const
    {
        return _phase;
    }

    /**
    * @name Empty
    * @return True, if there are no events scheduled.
    */
    [[nodiscard]] bool Empty() const
    {
        return _eventMap.empty();
    }

    /**
    * @name SetPhase
    * @brief Sets the phase of the map (absolute).
    * @param phase Phase which should be set. Values: 1 - 8. 0 resets phase.
    */
    void SetPhase(uint8 phase)
    {
        if (!phase)
        {
            _phase = 0;
        }
        else if (phase <= 8)
        {
            _phase = (1 << (phase - 1));
        }
    }

    /**
    * @name AddPhase
    * @brief Activates the given phase (bitwise).
    * @param phase Phase which should be activated. Values: 1 - 8
    */
    void AddPhase(uint8 phase)
    {
        if (phase && phase <= 8)
        {
            _phase |= (1 << (phase - 1));
        }
    }

    /**
    * @name RemovePhase
    * @brief Deactivates the given phase (bitwise).
    * @param phase Phase which should be deactivated. Values: 1 - 8.
    */
    void RemovePhase(uint8 phase)
    {
        if (phase && phase <= 8)
        {
            _phase &= ~(1 << (phase - 1));
        }
    }

    /**
    * @name ScheduleEvent
    * @brief Creates new event entry in map.
    * @param eventId The id of the new event.
    * @param time The time in milliseconds until the event occurs.
    * @param group The group which the event is associated to. Has to be between 1 and 8. 0 means it has no group.
    * @param phase The phase in which the event can occur. Has to be between 1 and 8. 0 means it can occur in all phases.
    */
    void ScheduleEvent(uint32 eventId, uint32 time, uint32 group = 0, uint32 phase = 0)
    {
        if (group && group <= 8)
        {
            eventId |= (1 << (group + 15));
        }

        if (phase && phase <= 8)
        {
            eventId |= (1 << (phase + 23));
        }

        _eventMap.insert(EventStore::value_type(_time + time, eventId));
    }

    /**
    * @name RescheduleEvent
    * @brief Cancels the given event and reschedules it.
    * @param eventId The id of the event.
    * @param time The time in milliseconds until the event occurs.
    * @param group The group which the event is associated to. Has to be between 1 and 8. 0 means it has no group.
    * @param phase The phase in which the event can occur. Has to be between 1 and 8. 0 means it can occur in all phases.
    */
    void RescheduleEvent(uint32 eventId, uint32 time, uint32 groupId = 0, uint32 phase = 0)
    {
        CancelEvent(eventId);
        ScheduleEvent(eventId, time, groupId, phase);
    }

    /**
    * @name RescheduleEvent
    * @brief Cancels the given event and reschedules it.
    * @param eventId The id of the event.
    * @param time The time in milliseconds until the event occurs.
    * @param group The group which the event is associated to. Has to be between 1 and 8. 0 means it has no group.
    * @param phase The phase in which the event can occur. Has to be between 1 and 8. 0 means it can occur in all phases.
    */
    void RepeatEvent(uint32 time)
    {
        _eventMap.insert(EventStore::value_type(_time + time, _lastEvent));
    }

    /**
    * @name ExecuteEvent
    * @brief Returns the next event to execute and removes it from map.
    * @return Id of the event to execute.
    */
    uint32 ExecuteEvent()
    {
        while (!Empty())
        {
            EventStore::iterator itr = _eventMap.begin();

            if (itr->first > _time)
            {
                return 0;
            }
            else if (_phase && (itr->second & 0xFF000000) && !((itr->second >> 24) & _phase))
            {
                _eventMap.erase(itr);
            }
            else
            {
                uint32 eventId = (itr->second & 0x0000FFFF);
                _lastEvent = itr->second;
                _eventMap.erase(itr);
                return eventId;
            }
        }

        return 0;
    }

    /**
    * @name DelayEvents
    * @brief Delays all events in the map. If delay is greater than or equal internal timer, delay will be 0.
    * @param delay Amount of delay.
    */
    void DelayEvents(uint32 delay)
    {
        _time = delay < _time ? _time - delay : 0;
    }

    void DelayEventsToMax(uint32 delay, uint32 group)
    {
        for (EventStore::iterator itr = _eventMap.begin(); itr != _eventMap.end();)
        {
            if (itr->first < _time + delay && (group == 0 || ((1 << (group + 15)) & itr->second)))
            {
                ScheduleEvent(itr->second, delay);
                _eventMap.erase(itr);
                itr = _eventMap.begin();
                continue;
            }

            ++itr;
        }
    }

    /**
    * @name DelayEvents
    * @brief Delay all events of the same group.
    * @param delay Amount of delay.
    * @param group Group of the events.
    */
    void DelayEvents(uint32 delay, uint32 group)
    {
        if (group > 8 || Empty())
        {
            return;
        }

        EventStore delayed;

        for (EventStore::iterator itr = _eventMap.begin(); itr != _eventMap.end();)
        {
            if (!group || (itr->second & (1 << (group + 15))))
            {
                delayed.insert(EventStore::value_type(itr->first + delay, itr->second));
                itr = _eventMap.erase(itr);
                continue;
            }

            ++itr;
        }

        _eventMap.insert(delayed.begin(), delayed.end());
    }

    /**
    * @name CancelEvent
    * @brief Cancels all events of the specified id.
    * @param eventId Event id to cancel.
    */
    void CancelEvent(uint32 eventId)
    {
        if (Empty())
        {
            return;
        }

        for (EventStore::iterator itr = _eventMap.begin(); itr != _eventMap.end();)
        {
            if (eventId == (itr->second & 0x0000FFFF))
            {
                itr = _eventMap.erase(itr);
                continue;
            }

            ++itr;
        }
    }

    /**
    * @name CancelEventGroup
    * @brief Cancel events belonging to specified group.
    * @param group Group to cancel.
    */
    void CancelEventGroup(uint32 group)
    {
        if (!group || group > 8 || Empty())
        {
            return;
        }

        uint32 groupMask = (1 << (group + 15));
        for (EventStore::iterator itr = _eventMap.begin(); itr != _eventMap.end();)
        {
            if (itr->second & groupMask)
            {
                _eventMap.erase(itr);
                itr = _eventMap.begin();
                continue;
            }

            ++itr;
        }
    }

    /**
    * @name GetNextEventTime
    * @brief Returns closest occurence of specified event.
    * @param eventId Wanted event id.
    * @return Time of found event.
    */
    [[nodiscard]] uint32 GetNextEventTime(uint32 eventId) const
    {
        if (Empty())
        {
            return 0;
        }

        for (auto const& itr : _eventMap)
        {
            if (eventId == (itr.second & 0x0000FFFF))
            {
                return itr.first;
            }
        }

        return 0;
    }

    /**
     * @name GetNextEventTime
     * @return Time of next event.
     */
    [[nodiscard]] uint32 GetNextEventTime() const
    {
        return Empty() ? 0 : _eventMap.begin()->first;
    }

    /**
    * @name IsInPhase
    * @brief Returns wether event map is in specified phase or not.
    * @param phase Wanted phase.
    * @return True, if phase of event map contains specified phase.
    */
    bool IsInPhase(uint8 phase)
    {
        return phase <= 8 && (!phase || _phase & (1 << (phase - 1)));
    }

private:
    uint32 _time{0};
    uint32 _phase{0};
    uint32 _lastEvent{0};

    EventStore _eventMap;
};

template<typename E>
constexpr typename std::underlying_type<E>::type AsUnderlyingType(E enumValue)
{
    static_assert(std::is_enum<E>::value, "AsUnderlyingType can only be used with enums");
    return static_cast<typename std::underlying_type<E>::type>(enumValue);
}

template<typename Ret, typename T1, typename... T>
Ret* Coalesce(T1* first, T*... rest)
{
    if constexpr (sizeof...(T) > 0)
        return (first ? static_cast<Ret*>(first) : Coalesce<Ret>(rest...));
    else
        return static_cast<Ret*>(first);
}

AC_COMMON_API std::string GetTypeName(std::type_info const&);

template <typename T>
std::string GetTypeName() { return GetTypeName(typeid(T)); }

template <typename T>
std::enable_if_t<!std::is_same_v<std::decay_t<T>, std::type_info>, std::string> GetTypeName(T&& v) { return GetTypeName(typeid(v)); }

#endif
