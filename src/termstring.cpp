//
// C++ Implementation: TermString
//
// Description:
//
//
// Author: hooey <hephooey@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "termstring.h"
#include <QtDebug>
//FIXME: check the range of index!!!
namespace QTerm
{

TermString::TermString()
    :m_string(),m_index()
{
}

TermString::TermString(const QString & str)
    :m_string(str),m_index()
{
    updateIndex();
}

TermString::~TermString()
{
}

void TermString::replace(int index, int length, const QString & str)
{
    if (index >= m_index.size()) {
        m_string += QString(index - m_index.size(), ' ');
        append(str);
        return;
    }
    int startpos = m_index.at(index);
    int index2 = index+length < m_index.size() ? index+length - 1 : m_index.size() - 1;
    int endpos = m_index.at(index2);
    if (startpos == -1 && index > 0) {
        //qDebug("TermString::replace: start pos is in the middle of a char");
        startpos = m_index.at(index-1);
    }
    if (index2 < (m_index.size() - 1) && m_index.at(index2+1) == -1) {
        //qDebug("TermString::mid: end pos is in the middle of a char");
    }
    if (endpos == -1 && index2 > 0) {
        endpos = m_index.at(index2 - 1); //Could this happen at the endof a string?
    }
    //qDebug() << "startpos: " << startpos << " endpos: " << endpos;
    m_string.replace(startpos, endpos-startpos + 1, str);
    updateIndex();
}

void TermString::append(const QString & str)
{
    m_string.append(str);
    updateIndex();
}

void TermString::insert(int index, const QString & str)
{
    if (index > m_index.size()) {
        m_string += QString(index - m_index.size(), ' ');
    }
    m_string.insert(index, str);
    updateIndex();
}

void TermString::dumpIndex()
{
    qDebug() << "Index for string: " << m_string;
    for (int i=0; i < m_index.length(); i++) {
        printf("%d ",(char)m_index[i]);
    }
    printf("\n");
}

void TermString::updateIndex()
{
    m_index.clear();
    int cellCounter = 0;
    for (int i = 0; i < m_string.length(); i++) {
        int len = wcwidth(m_string.at(i));
        if (len <= 0) {
            qDebug("TermString::updateIndex: Non printable char");
            continue;
        }
        m_index.append(i);
        if (len > 1) {
            m_index.append(-1);
            cellCounter++;
        }
        cellCounter++;
        //qDebug() << "Char: " << m_string.at(i) << " index " << i << " counter " << cellCounter;
    }
}

void TermString::remove(int index, int len)
{
    if (index >= m_index.size()) {
        qDebug("TermString::remove: pos larger than the size of the array");
        return;
    }
    int pos = m_index.at(index);
    if (pos == -1) {
        qDebug("TermString::remove: pos is in the middle of a char");
        pos = m_index.at(index-1);
    }
    m_string.remove(index, len);
}

QString TermString::string()
{
    return m_string;
}

QString TermString::mid(int index, int len)
{
    if (index < 0) {
        return QString();
    }
    if (index >= m_index.size()) {
        //qDebug("TermString::mid: pos larger than the size of the array");
        return QString();
    }
    int startpos= m_index.at(index);
    if (startpos == -1) {
        //qDebug("TermString::mid: start pos is in the middle of a char");
        startpos = m_index.at(index-1);
    }
    if (len == -1) {
        return m_string.mid(startpos,-1);
    }
    int index2 = index+len < m_index.size() ? index+len - 1 : m_index.size() - 1;
    int endpos = m_index.at(index2);
    if (index2 > 0 && index2 < (m_index.size() - 1) && m_index.at(index2+1) == -1) {
        //qDebug("TermString::mid: end pos is in the middle of a char");
        return mid(index, len - 1);
    }
    if (endpos == -1) {
        endpos = m_index.at(index2-1);
    }
    return m_string.mid(startpos, endpos-startpos + 1);
}

int TermString::length()
{
    return m_index.size();
}

int TermString::beginIndex(int pos)
{
    return m_index.indexOf(pos);
}

int TermString::size(int index)
{
    if (m_index.at(index) == -1 ) {
        return 2;
    }
    if ((index+1) < m_index.size() && m_index.at(index+1) == -1) {
        return 2;
    }
    return 1;
}

bool TermString::isPartial(int index)
{
    //qDebug() << "length " << m_index.size() << ", index: " << index;
    if (m_index.at(index) == -1) {
        return true;
    }
    return false;
}

int TermString::pos(int index)
{
    if (index < 0 || index >= m_index.length()) {
        //qDebug("pos: invalid index: %d", index);
        return -1;
    }
    if (m_index.at(index) == -1) {
        return m_index.at(index-1);
    }
    return m_index.at(index);
}

bool TermString::isEmpty()
{
    if (m_string.isNull()) {
        return true;
    }
    return m_string.isEmpty();
}

// Shameless copied from git

int TermString::bisearch(QChar ucs, const struct interval *table, int max)
{
    int min = 0;
    int mid;

    if (ucs < table[0].first || ucs > table[max].last)
        return 0;
    while (max >= min) {
        mid = (min + max) / 2;
        if (ucs > table[mid].last)
            min = mid + 1;
        else if (ucs < table[mid].first)
            max = mid - 1;
        else
            return 1;
    }

    return 0;
}

/* The following two functions define the column width of an ISO 10646
 * character as follows:
 *
 *    - The null character (U+0000) has a column width of 0.
 *
 *    - Other C0/C1 control characters and DEL will lead to a return
 *      value of -1.
 *
 *    - Non-spacing and enclosing combining characters (general
 *      category code Mn or Me in the Unicode database) have a
 *      column width of 0.
 *
 *    - SOFT HYPHEN (U+00AD) has a column width of 1.
 *
 *    - Other format characters (general category code Cf in the Unicode
 *      database) and ZERO WIDTH SPACE (U+200B) have a column width of 0.
 *
 *    - Hangul Jamo medial vowels and final consonants (U+1160-U+11FF)
 *      have a column width of 0.
 *
 *    - Spacing characters in the East Asian Wide (W) or East Asian
 *      Full-width (F) category as defined in Unicode Technical
 *      Report #11 have a column width of 2.
 *
 *    - All remaining characters (including all printable
 *      ISO 8859-1 and WGL4 characters, Unicode control characters,
 *      etc.) have a column width of 1.
 *
 * This implementation assumes that ucs_char_t characters are encoded
 * in ISO 10646.
 */

int TermString::wcwidth(QChar ch)
{
    /*
     * Sorted list of non-overlapping intervals of non-spacing characters,
     * generated by
     *   "uniset +cat=Me +cat=Mn +cat=Cf -00AD +1160-11FF +200B c".
     */
    static const struct interval combining[] = {
        { 0x0300, 0x0357 }, { 0x035D, 0x036F }, { 0x0483, 0x0486 },
        { 0x0488, 0x0489 }, { 0x0591, 0x05A1 }, { 0x05A3, 0x05B9 },
        { 0x05BB, 0x05BD }, { 0x05BF, 0x05BF }, { 0x05C1, 0x05C2 },
        { 0x05C4, 0x05C4 }, { 0x0600, 0x0603 }, { 0x0610, 0x0615 },
        { 0x064B, 0x0658 }, { 0x0670, 0x0670 }, { 0x06D6, 0x06E4 },
        { 0x06E7, 0x06E8 }, { 0x06EA, 0x06ED }, { 0x070F, 0x070F },
        { 0x0711, 0x0711 }, { 0x0730, 0x074A }, { 0x07A6, 0x07B0 },
        { 0x0901, 0x0902 }, { 0x093C, 0x093C }, { 0x0941, 0x0948 },
        { 0x094D, 0x094D }, { 0x0951, 0x0954 }, { 0x0962, 0x0963 },
        { 0x0981, 0x0981 }, { 0x09BC, 0x09BC }, { 0x09C1, 0x09C4 },
        { 0x09CD, 0x09CD }, { 0x09E2, 0x09E3 }, { 0x0A01, 0x0A02 },
        { 0x0A3C, 0x0A3C }, { 0x0A41, 0x0A42 }, { 0x0A47, 0x0A48 },
        { 0x0A4B, 0x0A4D }, { 0x0A70, 0x0A71 }, { 0x0A81, 0x0A82 },
        { 0x0ABC, 0x0ABC }, { 0x0AC1, 0x0AC5 }, { 0x0AC7, 0x0AC8 },
        { 0x0ACD, 0x0ACD }, { 0x0AE2, 0x0AE3 }, { 0x0B01, 0x0B01 },
        { 0x0B3C, 0x0B3C }, { 0x0B3F, 0x0B3F }, { 0x0B41, 0x0B43 },
        { 0x0B4D, 0x0B4D }, { 0x0B56, 0x0B56 }, { 0x0B82, 0x0B82 },
        { 0x0BC0, 0x0BC0 }, { 0x0BCD, 0x0BCD }, { 0x0C3E, 0x0C40 },
        { 0x0C46, 0x0C48 }, { 0x0C4A, 0x0C4D }, { 0x0C55, 0x0C56 },
        { 0x0CBC, 0x0CBC }, { 0x0CBF, 0x0CBF }, { 0x0CC6, 0x0CC6 },
        { 0x0CCC, 0x0CCD }, { 0x0D41, 0x0D43 }, { 0x0D4D, 0x0D4D },
        { 0x0DCA, 0x0DCA }, { 0x0DD2, 0x0DD4 }, { 0x0DD6, 0x0DD6 },
        { 0x0E31, 0x0E31 }, { 0x0E34, 0x0E3A }, { 0x0E47, 0x0E4E },
        { 0x0EB1, 0x0EB1 }, { 0x0EB4, 0x0EB9 }, { 0x0EBB, 0x0EBC },
        { 0x0EC8, 0x0ECD }, { 0x0F18, 0x0F19 }, { 0x0F35, 0x0F35 },
        { 0x0F37, 0x0F37 }, { 0x0F39, 0x0F39 }, { 0x0F71, 0x0F7E },
        { 0x0F80, 0x0F84 }, { 0x0F86, 0x0F87 }, { 0x0F90, 0x0F97 },
        { 0x0F99, 0x0FBC }, { 0x0FC6, 0x0FC6 }, { 0x102D, 0x1030 },
        { 0x1032, 0x1032 }, { 0x1036, 0x1037 }, { 0x1039, 0x1039 },
        { 0x1058, 0x1059 }, { 0x1160, 0x11FF }, { 0x1712, 0x1714 },
        { 0x1732, 0x1734 }, { 0x1752, 0x1753 }, { 0x1772, 0x1773 },
        { 0x17B4, 0x17B5 }, { 0x17B7, 0x17BD }, { 0x17C6, 0x17C6 },
        { 0x17C9, 0x17D3 }, { 0x17DD, 0x17DD }, { 0x180B, 0x180D },
        { 0x18A9, 0x18A9 }, { 0x1920, 0x1922 }, { 0x1927, 0x1928 },
        { 0x1932, 0x1932 }, { 0x1939, 0x193B }, { 0x200B, 0x200F },
        { 0x202A, 0x202E }, { 0x2060, 0x2063 }, { 0x206A, 0x206F },
        { 0x20D0, 0x20EA }, { 0x302A, 0x302F }, { 0x3099, 0x309A },
        { 0xFB1E, 0xFB1E }, { 0xFE00, 0xFE0F }, { 0xFE20, 0xFE23 },
        { 0xFEFF, 0xFEFF }, { 0xFFF9, 0xFFFB }, { 0x1D167, 0x1D169 },
        { 0x1D173, 0x1D182 }, { 0x1D185, 0x1D18B },
        { 0x1D1AA, 0x1D1AD }, { 0xE0001, 0xE0001 },
        { 0xE0020, 0xE007F }, { 0xE0100, 0xE01EF }
    };

    /* test for 8-bit control characters */
    if (ch == 0)
        return 0;
    if (ch < 32 || (ch >= 0x7f && ch < 0xa0))
        return -1;

    /* binary search in table of non-spacing characters */
    if (bisearch(ch, combining, sizeof(combining)
                / sizeof(struct interval) - 1))
        return 0;

    /*
     * If we arrive here, ch is neither a combining nor a C0/C1
     * control character.
     */

    return 1 +
        (ch >= 0x1100 &&
         /* Hangul Jamo init. consonants */
         (ch <= 0x115f ||
          ch == 0x2329 || ch == 0x232a ||
          /* CJK ... Yi */
          (ch >= 0x2e80 && ch <= 0xa4cf &&
           ch != 0x303f) ||
          /* Hangul Syllables */
          (ch >= 0xac00 && ch <= 0xd7a3) ||
          /* CJK Compatibility Ideographs */
          (ch >= 0xf900 && ch <= 0xfaff) ||
          /* CJK Compatibility Forms */
          (ch >= 0xfe30 && ch <= 0xfe6f) ||
          /* Fullwidth Forms */
          (ch >= 0xff00 && ch <= 0xff60) ||
          (ch >= 0xffe0 && ch <= 0xffe6) ||
          (ch >= 0x20000 && ch <= 0x2fffd) ||
          (ch >= 0x30000 && ch <= 0x3fffd)));
}

} // namespace QTerm
