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

#include <qgzdec.h>

bool gzDecompress(QByteArray qba_compressed, QByteArray &qba_decompressed)
{
    qba_decompressed.clear();
    if(qba_compressed.length() == 0) return 1;

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;

    int result = inflateInit2(&stream, 31);
    if (result != Z_OK) return 0;

    char *compressed_data = qba_compressed.data();
    int compressed_data_left = qba_compressed.length();

    do
    {
        int chunk_len = qMin(32768, compressed_data_left);

        if(chunk_len <= 0)
            break;

        stream.next_in = (unsigned char*)compressed_data;
        stream.avail_in = chunk_len;

        compressed_data += chunk_len;
        compressed_data_left -= chunk_len;

        do
        {
            char out[32768];

            stream.next_out = (unsigned char*)out;
            stream.avail_out = 32768;

            result = inflate(&stream, Z_NO_FLUSH);

            switch (result)
            {
            case Z_NEED_DICT:
                result = Z_DATA_ERROR;
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
            case Z_STREAM_ERROR:
                inflateEnd(&stream);
                return(false);
            }

            int out_len = (32768 - stream.avail_out);
            if(out_len > 0) qba_decompressed.append((char*)out, out_len);
        }
        while (stream.avail_out == 0);

    }
    while (result != Z_STREAM_END);

    inflateEnd(&stream);
    return (result == Z_STREAM_END);
}
