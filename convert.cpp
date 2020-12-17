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

#include <constants.h>
#include <data.h>
#include <qt-crc32/crc32.h>
#include <convert.h>
#include <QtEndian>
#include <configfile.h>
extern "C"
{
    #include <psp-save/libkirk/kirk_engine.h>
    #include <psp-save/pspcrypt.h>
}

namespace convert
{
    void psp_to_dt_do(QBuffer &input, QFile &output)
    {
        input.open(QIODevice::ReadOnly);
        output.open(QIODevice::ReadWrite);

        output.resize(0);

        QDataStream in(&input);
        QDataStream out(&output);

        in.setByteOrder(QDataStream::ByteOrder::LittleEndian);
        out.setByteOrder(QDataStream::ByteOrder::LittleEndian);

        out << 1677721600 << 16846880 << 0 << -192217088;
        int i;
        while(input.pos()<0x89CC)
        {
            in >> i;
            out << i;
        }
        in >> i;
        out << mdl_psp_to_dt(i);
        while(!input.atEnd())
        {
            in >> i;
            out << i;
        }

        fixCrc32(output);

        input.close();
        output.close();
    }

    void psp_to_dt(QFile &input, QFile &output, bool decrypt)
    {
        input.open(QIODevice::ReadOnly);
        QByteArray data = input.readAll();
        input.close();
        if(decrypt)
        {
            int size = data.size();
            int aligned_size = ((size + 0xF) >> 4) << 4;
            kirk_init();
            int result;
            result = decrypt_data(5, (unsigned char*)(data.data()), &size, &aligned_size, configfile::standard::savekeys::key_pd1);
            qDebug() << "Decryption result" << result;
            data.chop(16);
        }
        QBuffer inputDecrypted(&data);
        psp_to_dt_do(inputDecrypted, output);
    }

    void psp2nd_to_dt2nd_do(QBuffer &input, QFile &output, bool isExtend)
    {
        input.open(QIODevice::ReadOnly);
        output.open(QIODevice::ReadWrite);

        output.resize(0);

        QDataStream in(&input);
        QDataStream out(&output);

        in.setByteOrder(QDataStream::ByteOrder::LittleEndian);
        out.setByteOrder(QDataStream::ByteOrder::BigEndian);


        out << 100 << 537919745 << 0;
        if(isExtend) out << constants::DTE::header_edit_magic_be;
        else out << constants::DT2::header_edit_magic_be;

        int i;
        unsigned char b;

        data::copy(COPY_INT, 0x2C2AC);
        data::copy(COPY_BYTE, 0x322C4); // lyrics
        data::copy(COPY_INT, 0x35400);
        if(isExtend)
        {
            in >> i;
            out << mdl_psp2_to_dt2(i, mdl_psp3_to_dt3_tbl, sizeof(mdl_psp3_to_dt3_tbl));
            in >> i;
            out << mdl_psp2_to_dt2(i, mdl_psp3_to_dt3_tbl, sizeof(mdl_psp3_to_dt3_tbl));
        }
        else
        {
            in >> i;
            out << mdl_psp2_to_dt2(i, mdl_psp2_to_dt2_tbl, sizeof(mdl_psp2_to_dt2_tbl));
            in >> i;
            out << mdl_psp2_to_dt2(i, mdl_psp2_to_dt2_tbl, sizeof(mdl_psp2_to_dt2_tbl));
        }
        data::copy(COPY_INT, 0x35524);
        data::copy(COPY_BYTE, 0x3554C); // author
        data::copy(COPY_INT, 0x35550);
        data::copy(COPY_BYTE, 0x35578); // title
        data::copy(COPY_INT, 0x35580);
        data::copy(COPY_BYTE, 0x36844);
        data::copy(COPY_INT, FILE_SIZE);

        fixCrc32(output);

        input.close();
        output.close();
    }

    void psp2nd_to_dt2nd(QFile &input, QFile &output, bool isExtend, bool decrypt)
    {
        input.open(QIODevice::ReadOnly);
        QByteArray data = input.readAll();
        input.close();
        if(decrypt)
        {
            int size = data.size();
            int aligned_size = ((size + 0xF) >> 4) << 4;
            kirk_init();
            int result;
            if(isExtend) result = decrypt_data(5, (unsigned char*)(data.data()), &size, &aligned_size, configfile::standard::savekeys::key_pde);
            else result = decrypt_data(5, (unsigned char*)(data.data()), &size, &aligned_size, configfile::standard::savekeys::key_pd2);
            qDebug() << "Decryption result" << result;
            data.chop(16);
        }
        QBuffer inputDecrypted(&data);
        psp2nd_to_dt2nd_do(inputDecrypted, output, isExtend);
    }

    void dt2nd_to_psp2nd(QFile &input, QFile &outputFile, bool isExtend, bool encrypt)
    {
        QByteArray outputData;
        QBuffer output(&outputData);

        input.open(QIODevice::ReadOnly);
        output.open(QIODevice::ReadWrite);

        QDataStream in(&input);
        QDataStream out(&output);

        in.setByteOrder(QDataStream::ByteOrder::BigEndian);
        out.setByteOrder(QDataStream::ByteOrder::LittleEndian);

        input.seek(16);

        int i;
        unsigned char b;

        data::copy(COPY_INT, 0x2C2AC + 16);
        data::copy(COPY_BYTE, 0x322C4 + 16); // lyrics
        data::copy(COPY_INT, 0x35400 + 16);
        if(isExtend)
        {
            in >> i;
            out << mdl_dt2_to_psp2(i, mdl_psp3_to_dt3_tbl, sizeof(mdl_psp3_to_dt3_tbl));
            in >> i;
            out << mdl_dt2_to_psp2(i, mdl_psp3_to_dt3_tbl, sizeof(mdl_psp3_to_dt3_tbl));
        }
        else
        {
            in >> i;
            out << mdl_dt2_to_psp2(i, mdl_psp2_to_dt2_tbl, sizeof(mdl_psp2_to_dt2_tbl));
            in >> i;
            out << mdl_dt2_to_psp2(i, mdl_psp2_to_dt2_tbl, sizeof(mdl_psp2_to_dt2_tbl));
        }

        data::copy(COPY_INT, 0x35524 + 16);
        data::copy(COPY_BYTE, 0x3554C + 16); // author
        data::copy(COPY_INT, 0x35550 + 16);
        data::copy(COPY_BYTE, 0x35578 + 16); // title
        data::copy(COPY_INT, 0x35580 + 16);
        data::copy(COPY_BYTE, 0x36844 + 16);
        data::copy(COPY_INT, FILE_SIZE);
        outputData.append(16, 0);

        if(encrypt)
        {
            unsigned char hash[16];
            kirk_init();
            int result;
            int size = outputData.size();
            int aligned_size = ((size + 0xF) >> 4) << 4;
            if(isExtend) result = encrypt_data(5, (unsigned char*)(outputData.data()), &size, &aligned_size, hash, configfile::standard::savekeys::key_pde);
            else result = encrypt_data(5, (unsigned char*)(outputData.data()), &size, &aligned_size, hash, configfile::standard::savekeys::key_pd2);
            qDebug() << "Encryption result" << result;
        }

        input.close();
        output.close();

        outputFile.open(QIODevice::ReadWrite);
        outputFile.resize(0);
        outputFile.write(outputData);
    }

    void fixCrc32(QFile &file)
    {
        file.seek(0x10);
        Crc32 crc32worker;
        unsigned int crc32 = qToBigEndian(crc32worker.calculateFromQFile(file));
        file.seek(0x8);
        file.write((char*)&crc32, sizeof(crc32));
        qDebug() << "Wrote CRC32:" << crc32;
    }

    int mdl_psp2_to_dt2(int psp_mdl, const unsigned char *tbl, int tblsize)
    {
        if(psp_mdl<0) return psp_mdl;
        if(psp_mdl>tblsize) return 0;
        return static_cast<int>(tbl[psp_mdl]);
    }

    int mdl_dt2_to_psp2(int dt2_mdl, const unsigned char *tbl, int tblsize)
    {
        if(dt2_mdl<0) return dt2_mdl;
        for(int i=0; i<tblsize; i++)
        {
            if(tbl[i]==dt2_mdl) return i;
        }
        return 0;
    }

    int mdl_psp_to_dt(int psp_mdl)
    {
        if(psp_mdl<8||psp_mdl>14) return psp_mdl;
        return mdl_psp1_to_dt1_psmiku[psp_mdl-8];
    }

    int mdl_dt_to_psp(int dt_mdl)
    {
        for(unsigned int i=0; i<sizeof(mdl_psp1_to_dt1_psmiku); i++)
            if(mdl_psp1_to_dt1_psmiku[i]==dt_mdl) return i+8;
        return dt_mdl;
    }
}
