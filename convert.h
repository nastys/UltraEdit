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

#ifndef CONVERT_H
#define CONVERT_H

#include <QFile>
#include <QBuffer>

namespace convert
{
    void psp_to_dt_do(QBuffer &input, QFile &output);
    void psp_to_dt(QFile &input, QFile &output, bool decrypt=true);
    void psp2nd_to_dt2nd_do(QBuffer &input, QFile &output, bool isExtend=false);
    void psp2nd_to_dt2nd(QFile &input, QFile &output, bool isExtend=false, bool decrypt=true);
    void dt2nd_to_psp2nd(QFile &input, QFile &outputFile, bool isExtend=false, bool encrypt=false);
    void fixCrc32(QFile &file);

    inline const unsigned char mdl_psp1_to_dt1_psmiku[]={11,14,12,13,10,9,8}; // 8 = 11, etc...
    inline const unsigned char mdl_psp2_to_dt2_tbl[]={0,3,4,5,6,7,11,14,12,13,10,9,8,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,46,47,84,85,82,81,79,54,69,70,87,55,56,57,80,78,88,89,83,58,86,90,71,40,48,91,93,72,92,59,94,41,49,95,73,60,96,42,50,97,99,98,61,74,100,45,52,101,75,102,103,104,44,51,105,76,106,108,107,2,109,110,1,111,112,43,113,115};
    inline const unsigned char mdl_psp3_to_dt3_tbl[]={0,3,4,5,6,7,11,14,12,13,10,9,8,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,46,47,84,85,82,81,79,54,69,70,87,55,56,57,80,78,88,89,83,58,86,90,71,53,0,62,68,77,124,125,126,127,128,129,130,131,132,133,134,135,136,137,40,48,91,93,72,92,59,94,63,40,117,141,142,143,144,145,146,41,49,95,73,60,96,64,118,147,148,149,150,151,42,50,97,74,98,61,74,100,65,119,42,121,153,154,155,156,157,45,52,101,75,102,103,104,67,120,158,159,160,161,44,51,105,76,106,108,107,116,66,122,112,163,164,165,2,109,110,138,1,111,0,139,43,113,115,166,114};
    int mdl_psp_to_dt(int psp_mdl);
    int mdl_dt_to_psp(int dt_mdl);
    int mdl_psp2_to_dt2(int psp_mdl, const unsigned char *tbl, int tblsize);
    int mdl_dt2_to_psp2(int dt2_mdl, const unsigned char *tbl, int tblsize);
}

#endif // CONVERT_H
