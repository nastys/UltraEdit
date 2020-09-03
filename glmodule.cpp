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

#include "glmodule.h"
#include <QDebug>
#include <QFile>
#include <QStringLiteral>
#include <modulespr.h>
#include <configfile.h>

glmodule::glmodule(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_AlwaysStackOnTop, true);
}

glmodule::~glmodule()
{
    sprbindata.clear();
    if(this->isVisible()) delete qgl;
}

void glmodule::initializeGL()
{
    qgl = new QOpenGLFunctions_3_0;
    qgl->initializeOpenGLFunctions();

    modulespr::extractMipmaps(lumapos, lumasize, chromapos, chromasize, tex_w, tex_h, nrm_x, nrm_y, nrm_w, nrm_h, sprbindata, 0);
    vertices[2]=vertices[6]=vertices[22]=nrm_x;
    vertices[7]=vertices[15]=vertices[23]=-nrm_y;
    vertices[10]=vertices[14]=vertices[18]=nrm_w;
    vertices[3]=vertices[11]=vertices[19]=-nrm_h;

    qgl->glGenTextures(1, &tex0);
    qgl->glBindTexture(GL_TEXTURE_2D, tex0);
    qgl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    qgl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(!program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shader/glsl/modulespr/2dtex.vert")) qDebug() << program.log();

    if(chromasize>0)
    {
        if(!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/glsl/modulespr/ati2ycbcr.frag")) qDebug() << program.log();
        program.link();
        program.bind();

        qgl->glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RG_RGTC2, tex_w, tex_h, 0, lumasize, sprbindata.data()+lumapos);

        qgl->glGenTextures(1, &tex1);
        qgl->glBindTexture(GL_TEXTURE_2D, tex1);
        qgl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        qgl->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        qgl->glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RG_RGTC2, tex_w/2, tex_h/2, 0, chromasize, sprbindata.data()+chromapos);

        int texture1Location =  program.uniformLocation("Texture1");
        qgl->glUniform1i(texture1Location, 1);
    }
    else
    {
        if(!program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shader/glsl/modulespr/rgba8.frag")) qDebug() << program.log();
        program.link();
        program.bind();

        qgl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, sprbindata.data()+lumapos);
    }

    int texture0Location = program.uniformLocation("Texture0");
    qgl->glUniform1i(texture0Location, 0);

    qgl->glGenVertexArrays(1, &vao);
    qgl->glGenBuffers(1, &vbo);
    qgl->glBindVertexArray(vao);

    qgl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    qgl->glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    qgl->glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    qgl->glEnableVertexAttribArray(0);
    qgl->glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    qgl->glEnableVertexAttribArray(1);
    qgl->glBindVertexArray(0);
    qgl->glBindBuffer(GL_ARRAY_BUFFER, 0);

    initialized=true;
}

void glmodule::paintGL()
{
    qgl->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    qgl->glBindVertexArray(vao);
    qgl->glActiveTexture(GL_TEXTURE0);
    qgl->glBindTexture(GL_TEXTURE_2D, tex0);
    qgl->glActiveTexture(GL_TEXTURE1);
    qgl->glBindTexture(GL_TEXTURE_2D, tex1);
    qgl->glDrawArrays(GL_TRIANGLES, 0, 6);
}

void glmodule::resizeGL(int w, int h)
{
}

void glmodule::loadModule(int &num)
{
    if(!initialized) return;

    if(modulespr::extractMipmaps(lumapos, lumasize, chromapos, chromasize, tex_w, tex_h, nrm_x, nrm_y, nrm_w, nrm_h, sprbindata, num))
        if(modulespr::extractMipmaps(lumapos, lumasize, chromapos, chromasize, tex_w, tex_h, nrm_x, nrm_y, nrm_w, nrm_h, sprbindata, 0))
            return;

    qgl->glBindTexture(GL_TEXTURE_2D, tex0);

    if(chromasize>0)
    {
        qgl->glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RG_RGTC2, tex_w, tex_h, 0, lumasize, sprbindata.data()+lumapos);

        qgl->glBindTexture(GL_TEXTURE_2D, tex1);
        qgl->glCompressedTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RG_RGTC2, tex_w/2, tex_h/2, 0, chromasize, sprbindata.data()+chromapos);
    }
    else
        qgl->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_w, tex_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, sprbindata.data()+lumapos);

    repaint();
}
