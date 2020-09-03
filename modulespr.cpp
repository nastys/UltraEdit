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

#include <QFile>
#include <QDir>
#include <QDataStream>
#include <QVector>
#include <qgzdec.h>
#include <configfile.h>

namespace modulespr
{
    class FileEntry
    {
    public:
        QString name;
        unsigned int offset;
        unsigned int compressedSize;
        unsigned int uncompressedSize;
        long fixedSize;
    };

    struct SprBinHeader
    {
        int unusedFlags;
        unsigned int texturesOffset;
        int textureCount;
        int spriteCount;
        int spritesOffset;
        int textureNamesOffset;
        int spriteNamesOffset;
        int spriteModesOffset;
    };

    struct TexSetHeader
    {
        int textureCount;
        int textureCountWithRubbish;
    };

    struct TexHeader
    {
        int subTexCount;
        int info;
    };

    struct SubTexHeader
    {
        int width;
        int height;
        int format;
        int id;
        int size;
    };

    class SubTex
    {
    public:
        SubTexHeader *header;
        int pos;
    };

    struct SprHeader
    {
        int id;
        int unknown1;
        float ndcX;
        float ndcY;
        float ndcWidth;
        float ndcHeight;
        float X;
        float Y;
        float width;
        float height;
    };

    bool extractMipmaps(long long& lumapos, int& lumasize, long long& chromapos, int& chromasize, int& tex_w, int& tex_h, float& nrm_x, float& nrm_y, float& nrm_w, float& nrm_h, QByteArray &uncompressedData, int filenum)
    {
        QFile file(*configfile::keys::romdir+"/2d/spr_ps3_mdl"+QStringLiteral("%1").arg(filenum, 3, 10, QLatin1Char('0'))+".farc");
        if(!file.open(QIODevice::ReadOnly)) return 1;
        QDataStream filestream(&file);
        filestream.setByteOrder(QDataStream::BigEndian);

        char signature[4];
        file.read(signature, 4);
        if(strcmp(signature, "FArC") != 0) return 1;

        int fileListSize;
        filestream >> fileListSize;

        int alignment;
        filestream >> alignment;

        QVector<FileEntry> files;
        while(file.pos() < fileListSize + 0x8)
        {
            FileEntry fe;

            char c;
            do
            {
                file.read(&c, 1);
                fe.name.append(c);
            }
            while(c != '\0');

            filestream >> fe.offset;
            filestream >> fe.compressedSize;
            filestream >> fe.uncompressedSize;

            fe.fixedSize = qMin(static_cast<long long>(fe.compressedSize), file.size() - fe.offset);

            files.append(fe);
        }

        for(int i=0; i<files.length(); i++)
        {
            file.seek(files.at(i).offset);
            gzDecompress(file.read(files.at(i).compressedSize), uncompressedData);

            SprBinHeader *header = (SprBinHeader*)uncompressedData.data();

            long long pos=header->spritesOffset;
            SprHeader *sprheader=(SprHeader*)(uncompressedData.data()+pos);

            pos=header->texturesOffset;
            int *textureSetSignature = (int*)(uncompressedData.data()+pos);
            if(*textureSetSignature==0x03505854)
            {
                pos+=4;
                TexSetHeader *texsetheader = (TexSetHeader*)(uncompressedData.data()+pos);
                for(int i=0; i<texsetheader->textureCount; i++)
                {
                    pos+=12;
                    int *textureSignature = (int*)(uncompressedData.data()+pos);
                    if(*textureSignature==0x04505854||*textureSignature==0x05505854)
                    {
                        pos+=4;
                        TexHeader *texheader = (TexHeader*)(uncompressedData.data()+pos);

                        int mipMapCount = texheader->info & 0xFF;
                        int depth = (texheader->info >> 8) & 0xFF;

                        if (depth == 1 && mipMapCount != texheader->subTexCount) mipMapCount = (unsigned char)texheader->subTexCount;

                        if(configfile::keys::dtmgr_limited) pos+=12;
                        else pos+=16;

                        QVector<SubTex> stv;
                        for(int i=0; i<depth; i++) for(int j=0; j<mipMapCount; j++)
                        {
                            int *subTexSignature = (int*)(uncompressedData.data()+pos);
                            if(*subTexSignature==0x02505854)
                            {
                                pos+=4;
                                SubTex st;
                                st.header=(SubTexHeader*)(uncompressedData.data()+pos);
                                pos+=sizeof(*st.header);
                                st.pos=pos;
                                pos+=st.header->size;
                                stv.append(st);
#ifdef QT_DEBUG
                                QFile out("dump"+QString::number(i)+"."+QString::number(j)+".data");
                                out.open(QIODevice::ReadWrite);
                                out.resize(0);
                                out.write(uncompressedData.data()+st.pos, st.header->size);
                                out.close();
#endif
                            }
                        }
                        lumapos=stv.at(0).pos;
                        lumasize=stv.at(0).header->size;
                        if(configfile::keys::dtmgr_limited)
                        {
                            chromapos=-1;
                            chromasize=0;
                        }
                        else
                        {
                            chromapos=stv.at(1).pos;
                            chromasize=stv.at(1).header->size;
                        }
                        tex_w=static_cast<int>(stv.at(0).header->width);
                        tex_h=static_cast<int>(stv.at(0).header->height);
                        nrm_x=sprheader->ndcX;
                        nrm_y=sprheader->ndcY;
                        nrm_w=sprheader->ndcWidth;
                        nrm_h=sprheader->ndcHeight;
                    }
                }
            }
        }

        file.close();

        return 0;
    }
};
