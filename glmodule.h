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

#ifndef GLMODULE_H
#define GLMODULE_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QByteArray>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>

class glmodule : public QOpenGLWidget
{
public:
    glmodule(QWidget *parent = 0);
    ~glmodule();
    int chromasize, lumasize, tex_w, tex_h;
    float nrm_x, nrm_y, nrm_w, nrm_h;
    void loadModule(int &num);

    const float normalized_X=0;
    const float normalized_Y=0;
    const float normalized_width=0;
    const float normalized_height=0;

    float vertices[24] = {
            -1.0f, -1.0f, normalized_X,     -normalized_height,      // left bottom;        b-l tri
            -1.0f,  1.0f, normalized_X,     -normalized_Y,           // left top (1);       b-l tri
             1.0f, -1.0f, normalized_width, -normalized_height,      // right bottom (1);   b-l tri
             1.0f,  1.0f, normalized_width, -normalized_Y,           // right top;          t-r tri
             1.0f, -1.0f, normalized_width, -normalized_height,      // right bottom (2);   t-r tri
            -1.0f,  1.0f, normalized_X,     -normalized_Y,           // left top (2);       t-r tri
        };

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

private:
    GLuint vao, vbo;
    QOpenGLShaderProgram program;
    GLuint tex0, tex1;
	QOpenGLFunctions_3_3_Core *qgl;
    QByteArray sprbindata;
    long long lumapos, chromapos;

    bool initialized=false;
};

#endif // GLMODULE_H
