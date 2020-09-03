/*
Copyright (c) 2020 nastys

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef TYPES_H
#define TYPES_H

namespace types
{
    namespace DT1
    {
        struct editSaveEntry
        {
            int enable;
            int editN;
            int internalSongId;
            int unk1;
            int unk2;
            int unk3;
            char author[128];
            char title[256];
            char audioFile[32];
        };
    };

    namespace DT2
    {
        struct editSaveEntry
        {
            unsigned char enable;
            unsigned char duet;
            unsigned char unk1[5];
            unsigned char editN;
            int internalSongId;
            short moduleA;
            short moduleB;
            int unkmdl;
            unsigned char unk2[20];
            char author[128];
            char title[256];
            char audioFile[32];
        };
    };
};

#endif // TYPES_H
