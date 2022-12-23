/**
 * A C++ port of the JavaScript library "Iuppiter", which is under the following license:
 *
 * Copyright (c) 2010 Nuwa Information Co., Ltd, and individual contributors.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   3. Neither the name of Nuwa Information nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef IUPPITER_H
#define IUPPITER_H

#include <string>
#include <vector>

namespace Iuppiter
{
    /* General macros */
    constexpr short NBBY = 8;
    constexpr short MATCH_BITS = 6;
    constexpr short MATCH_MIN = 3;
    constexpr size_t MATCH_MAX = ((1 << MATCH_BITS) + (MATCH_MIN - 1));
    constexpr size_t OFFSET_MASK = ((1 << (16 - MATCH_BITS)) - 1);
    constexpr short LEMPEL_SIZE = 256;

    /// Encoding characters table.
    constexpr const char *CA = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    /// Encoding characters table for url safe encoding.
    constexpr const char *CAS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

    /// Decoding reference table.
    std::vector<int32_t> IA(256);

    /// Decoding reference table for url safe encoded string.
    std::vector<int32_t> IAS(256);

    static void Init()
    {
        /// Initialize variables for Base64 namespace.
        int i, iS;

        for (i = 0; i < 256; i++)
        {
            IA[i] = -1;
            IAS[i] = -1;
        }

        for (i = 0, iS = strlen(CA); i < iS; i++)
        {
            IA[CA[i]] = i;
            IAS[CAS[i]] = i;
        }

        IA['='] = IAS['='] = 0;
    }

    namespace Base64
    {
        /**
         * Encode base64.
         *
         * @param input A byte array.
         * @param urlsafe True if you want to make encoded string is url safe.
         * @return Encoded base64 string.
         */
        static std::string Encode(std::vector<uint32_t> input, bool urlSafe = false)
        {
            // size_t eLen;
            size_t dLen;
            size_t sLen;
            size_t i;
            std::string ca;

            if (urlSafe)
                ca = CAS;
            else
                ca = CA;

            std::vector<uint32_t> sArr = input;
            sLen = sArr.size();

            // eLen = sLen - (sLen % 3);       // Length of even 24-bits.
            dLen = ((sLen - 1) / 3 + 1) << 2; // Length of returned array

            std::string dArr;
            dArr.resize(dLen);

            // Encode even 24-bits
            size_t b1, b2, b3;
            for (size_t s = 0, d = 0; s < sLen;)
            {
                b1 = sArr.size() > s ? sArr[s++] : 0;
                b2 = sArr.size() > s ? sArr[s++] : 0;
                b3 = sArr.size() > s ? sArr[s++] : 0;

                // Copy next three bytes into lower 24 bits of int, paying attension to sign.
                i = (b1 & 0xff) << 16 | (b2 & 0xff) << 8 | (b3 & 0xff);

                // Encode the int into four chars
                dArr[d++] = ca[(i >> 18) & 0x3f];
                dArr[d++] = ca[(i >> 12) & 0x3f];
                dArr[d++] = ca[(i >> 6) & 0x3f];
                dArr[d++] = ca[i & 0x3f];
            }

            // Pad and encode last bits if source isn't even 24 bits.
            /*uint8_t left = sLen - eLen; // 0 - 2.
            if (left > 0)
            {
                // Prepare the int
                i = (((size_t)sArr[eLen] & 0xff) << 10) | (left == 2 ? (((size_t)sArr[sLen - 1] & 0xff) << 2) : 0);

                // Set last four chars
                dArr[dLen - 4] = ca[i >> 12];
                dArr[dLen - 3] = ca[(i >> 6) & 0x3f];
                dArr[dLen - 2] = left == 2 ? ca[i & 0x3f] : '=';
                dArr[dLen - 1] = '=';
            }*/

            return dArr;
        };

        /**
         * Decode base64 encoded string or byte array.
         *
         * @param input A byte array.
         * @param urlsafe True if the encoded string is encoded by urlsafe.
         * @return A decoded byte array.
         */
        static std::vector<uint32_t> Decode(std::vector<uint32_t> input, bool urlSafe = false)
        {
            std::vector<int32_t> ia;
            size_t sIx;
            size_t eIx;
            size_t cCnt;
            size_t sepCnt;
            size_t pad;
            size_t sLen;
            size_t eLen;
            size_t len;
            size_t d;
            size_t cc;
            size_t i;
            size_t j;
            size_t r;

            if (urlSafe)
            {
                ia = IAS;
            }
            else
            {
                ia = IA;
            }

            std::vector<uint32_t> sArr = input;
            sLen = sArr.size();

            sIx = 0;
            eIx = sLen - 1; // Start and end index after trimming.

            // Trim illegal chars from start
            while (sIx < eIx && ia[sArr[sIx]] < 0)
            {
                sIx++;
            }

            // Trim illegal chars from end
            while (eIx > 0 && ia[sArr[eIx]] < 0)
            {
                eIx--;
            }

            // get the padding count (=) (0, 1 or 2)
            // Count '=' at end.
            pad = sArr[eIx] == '=' ? (sArr[eIx - 1] == '=' ? 2 : 1) : 0;
            cCnt = eIx - sIx + 1; // Content count including possible separators
            sepCnt = sLen > 76 ? (sArr[76] == '\r' ? cCnt / 78 : 0) << 1 : 0;

            // The number of decoded bytes
            len = ((cCnt - sepCnt) * 6 >> 3) - pad;

            std::vector<uint32_t> dArr;
            dArr.resize(len);

            // Decode all but the last 0 - 2 bytes.
            d = 0;
            for (cc = 0, eLen = (len / 3) * 3; d < eLen;)
            {
                // Assemble three bytes into an int from four "valid" characters.
                i = (size_t)ia[sArr[sIx++]] << 18 | (size_t)ia[sArr[sIx++]] << 12 | (size_t)ia[sArr[sIx++]] << 6 | ia[sArr[sIx++]];

                // Add the bytes
                dArr[d++] = (i >> 16) & 0xff;
                dArr[d++] = (i >> 8) & 0xff;
                dArr[d++] = i & 0xff;

                // If line separator, jump over it.
                if (sepCnt > 0 && ++cc == 19)
                {
                    sIx += 2;
                    cc = 0;
                }
            }

            if (d < len)
            {
                // Decode last 1-3 bytes (incl '=') into 1-3 bytes
                i = 0;
                for (j = 0; sIx <= eIx - pad; j++)
                {
                    i |= (size_t)ia[sArr[sIx++]] << (18 - j * 6);
                }

                for (r = 16; d < len; r -= 8)
                {
                    dArr[d++] = (i >> r) & 0xff;
                }
            }

            return dArr;
        }
    };

    /**
     * Compress string or byte array using fast and efficient algorithm.
     *
     * Because of weak of javascript's natural, many compression algorithm
     * become useless in javascript implementation. The main problem is
     * performance, even the simple Huffman, LZ77/78 algorithm will take many
     * many time to operate. We use LZJB algorithm to do that, it suprisingly
     * fulfills our requirement to compress string fastly and efficiently.
     *
     * Our implementation is based on
     * http://src.opensolaris.org/source/raw/onnv/onnv-gate/
     * usr/src/uts/common/os/compress.c
     * It is licensed under CDDL.
     *
     * Please note it depends on ToByteArray utility function.
     *
     * @param input The byte array that you want to compress.
     * @return Compressed byte array.
     */
    static std::vector<uint32_t> Compress(std::vector<uint32_t> input)
    {
        size_t slen;
        size_t src = 0;
        size_t dst = 0;
        int64_t cpy;
        size_t copymap;
        size_t copymask = (size_t)1 << (NBBY - 1);
        size_t mlen;
        size_t offset;
        size_t hp;
        size_t i;

        int64_t *lempel = new int64_t[LEMPEL_SIZE];

        // Initialize lempel array.
        for (i = 0; i < LEMPEL_SIZE; i++)
        {
            lempel[i] = 3435973836;
        }

        // Using byte array or not.
        std::vector<uint32_t> sstart = input;
        std::vector<uint32_t> dstart = {};

        slen = sstart.size();

        while (src < slen)
        {
            if ((copymask <<= 1) == ((size_t)1 << NBBY))
            {
                if (dst >= slen - 1 - 2 * NBBY)
                {
                    mlen = slen;

                    for (src = 0, dst = 0; mlen; mlen--)
                    {
                        if (dstart.size() <= dst)
                        {
                            dstart.push_back(sstart[src]);
                        }
                        else
                        {
                            dstart[dst] = sstart[src];
                        }
                        src++;
                        dst++;
                    }

                    return dstart;
                }

                copymask = 1;
                copymap = dst;

                if (dstart.size() <= dst)
                {
                    dstart.push_back(0);
                }
                else
                {
                    dstart[dst] = 0;
                }

                dst++;
            }

            if (src > slen - MATCH_MAX)
            {
                dstart[dst++] = sstart[src++];
                continue;
            }

            hp = (((size_t)sstart[src] + 13) ^ ((size_t)sstart[src + 1] - 13) ^ sstart[src + 2]) & ((size_t)LEMPEL_SIZE - 1);
            offset = ((int64_t)src - lempel[hp]) & OFFSET_MASK;
            lempel[hp] = src;
            cpy = src - offset;

            if (cpy >= 0 && cpy != src &&
                sstart[src] == sstart[cpy] &&
                sstart[src + 1] == sstart[cpy + 1] &&
                sstart[src + 2] == sstart[cpy + 2])
            {
                dstart[copymap] |= copymask;

                for (mlen = MATCH_MIN; mlen < MATCH_MAX; mlen++)
                {
                    if (sstart[src + mlen] != sstart[cpy + mlen])
                    {
                        break;
                    }
                }

                dstart[dst++] = ((mlen - MATCH_MIN) << (NBBY - MATCH_BITS)) | (offset >> NBBY);
                dstart[dst++] = offset;
                src += mlen;
            }
            else
            {
                if (dstart.size() <= dst)
                {
                    dstart.push_back(sstart[src]);
                }
                else
                {
                    dstart[dst] = sstart[src];
                }
                src++;
                dst++;
            }
        }

        delete[] lempel;

        return dstart;
    }

    /**
     * Decompress string or byte array using fast and efficient algorithm.
     *
     * Our implementation is based on
     * http://src.opensolaris.org/source/raw/onnv/onnv-gate/
     * usr/src/uts/common/os/compress.c
     * It is licensed under CDDL.
     *
     * Please note it depends on ToByteArray utility function.
     *
     * @param input The byte array that you want to compress.
     * @return Decompressed byte array.
     */
    static std::vector<uint32_t> Decompress(std::vector<uint32_t> input)
    {
        size_t copymap;
        size_t copymask = (size_t)1 << (NBBY - 1);
        size_t src = 0;
        size_t slen = 0;
        size_t dst = 0;
        size_t offset = 0;
        int64_t cpy = 0;
        int64_t mlen = 0;

        std::vector<uint32_t> sstart = input;
        std::vector<uint32_t> dstart = {};

        slen = sstart.size();

        while (src < slen)
        {
            if ((copymask <<= 1) == ((size_t)1 << NBBY))
            {
                copymask = 1;
                copymap = sstart[src++];
            }

            if (copymap & copymask)
            {
                mlen = ((size_t)sstart[src] >> (NBBY - MATCH_BITS)) + MATCH_MIN;
                offset = (((size_t)sstart[src] << NBBY) | sstart[src + 1]) & OFFSET_MASK;
                src += 2;

                if ((cpy = dst - offset) >= 0)
                {
                    while (--mlen >= 0)
                    {
                        dstart.push_back(dstart[cpy++]);
                        dst++;
                    }
                }
                else
                {
                    /*
                     * offset before start of destination buffer
                     * indicates corrupt source data
                     */
                    return dstart;
                }
            }
            else
            {
                dstart.push_back(sstart[src++]);
                dst++;
            }
        }

        return dstart;
    }

    /**
     * Convert string value to a byte array.
     *
     * @param input The input string value.
     * @return A byte array from string value.
     */
    static std::vector<uint32_t> StringToByteArray(const std::string &input)
    {
        std::vector<uint32_t> b = {};
        uint32_t unicode;

        for (size_t i = 0; i < input.size(); i++)
        {
            unicode = input[i];
            // 0x00000000 - 0x0000007f -> 0xxxxxxx
            if (unicode <= 0x7f)
            {
                b.push_back(unicode);
                // 0x00000080 - 0x000007ff -> 110xxxxx 10xxxxxx
            }
            else if (unicode <= 0x7ff)
            {
                b.push_back((unicode >> 6) | 0xc0);
                b.push_back((unicode & 0x3F) | 0x80);
                // 0x00000800 - 0x0000ffff -> 1110xxxx 10xxxxxx 10xxxxxx
            }
            else if (unicode <= 0xffff)
            {
                b.push_back((unicode >> 12) | 0xe0);
                b.push_back(((unicode >> 6) & 0x3f) | 0x80);
                b.push_back((unicode & 0x3f) | 0x80);
                // 0x00010000 - 0x001fffff -> 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            }
            else
            {
                b.push_back((unicode >> 18) | 0xf0);
                b.push_back(((unicode >> 12) & 0x3f) | 0x80);
                b.push_back(((unicode >> 6) & 0x3f) | 0x80);
                b.push_back((unicode & 0x3f) | 0x80);
            }
        }

        return b;
    }

    /**
     * Convert byte array to a string value.
     *
     * @param input The input byte array.
     * @return A string value from byte array.
     */
    static std::string ByteArrayToString(std::vector<uint32_t> input)
    {
        std::string s;

        for (auto b : input)
        {
            s += (char)b;
        }

        return s;
    }
};
#endif
