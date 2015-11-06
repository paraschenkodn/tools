#include "text.h"

Text::Text()
{

}

void Text::draw(QGLPainter *painter, const QString &_str, const QVector3D &coords, const QFont &_font, const QPen &_pen, const Qt::Alignment &align)
    {
        if (_str.isEmpty())
            return;

        QFontMetrics fm(_font);
        QRect rect = fm.boundingRect(_str); // text bounding box
        rect.adjust(0, 0, 1, 1);            // normalize bounding box

        QImage image(rect.size(), QImage::Format_ARGB32);
        image.fill(0); // set to transparent

        // Draw the text on an image
        QPainter p2d(&image);
        p2d.setFont(_font);
        p2d.setPen(_pen);
        p2d.drawText(0, 0, rect.width(), rect.height(), Qt::AlignCenter, _str);
        p2d.end();

        // Get viewport *************** получаем размер поля рисования, чтобы его использовать как ортогональную проекцию
        // для рисования текста в ней
        GLint view[4];
        glGetIntegerv(GL_VIEWPORT, view);

        // Convert the object coordinate to screen coordinate
        GLdouble winx, winy, winz; // the screen coordinate of the object
        QMatrix4x4 model = painter->modelViewMatrix().top();
        QMatrix4x4 proj = painter->projectionMatrix().top();

        GLdouble modelMatrix[16];
        for (int i = 0; i < 16; i++)
            modelMatrix[i] = model.data()[i];

        GLdouble projMatrix[16];
        for (int i = 0; i < 16; i++)
            projMatrix[i] = proj.data()[i];

        gluProject(coords.x(), coords.y(), coords.z(),
            modelMatrix, projMatrix, view,
            &winx, &winy, &winz);

        // Define the font rectangle
        int x = (int) winx, y = (int) winy;

        if (align & Qt::AlignLeft){
            x -= rect.width();
        }
        if (align & Qt::AlignTop){
            y -= rect.height();
        }
        if (align & Qt::AlignHCenter){
            x -= rect.width() / 2;
        }
        if (align & Qt::AlignVCenter){
            y -= rect.height() / 2;
        }

        /*QVector2DArray vertices;
        vertices.append(x, y);
        vertices.append(x, y + rect.height());
        vertices.append(x + rect.width(), y + rect.height());
        vertices.append(x + rect.width(), y);//*/
        vertices.resize(8);
        vertices[0]=x; vertices[1]=y;
        vertices[2]=x; vertices[3]=y + rect.height();
        vertices[4]=x + rect.width(); vertices[5]=y + rect.height();
        vertices[6]=x + rect.width(); vertices[7]=y;

        // Texture coordinates
        /*QVector2DArray texCoord;
        texCoord.append(0.0f, 0.0f);
        texCoord.append(0.0f, 1.0f);
        texCoord.append(1.0f, 1.0f);
        texCoord.append(1.0f, 0.0f);//*/
        texCoord.resize(8);
        texCoord[0]=0.0f; texCoord[1]=0.0f; texCoord[2]=0.0f; texCoord[3]=1.0f;
        texCoord[4]=1.0f; texCoord[5]=1.0f; texCoord[6]=1.0f; v[7]=0.0f;

        // Map the image to texture
        QGLTexture2D texture;
        texture.setImage(image);

        // Set projection matrix stack
        painter->modelViewMatrix().push();
        painter->modelViewMatrix().setToIdentity();

        // Use ortho projection to draw the text
        QMatrix4x4 projm;
        projm.ortho(view[0], view[2], view[1], view[3], 0, 1);

        painter->projectionMatrix().push();
        painter->projectionMatrix() = projm;

        // Enable blend to make the background transparency of the text
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        painter->clearAttributes();
        painter->setStandardEffect(QGL::FlatReplaceTexture2D);
        texture.bind();
        painter->setVertexAttribute(QGL::Position, vertices);
        painter->setVertexAttribute(QGL::TextureCoord0, texCoord);
        painter->draw(QGL::TriangleFan, 4);
        painter->setStandardEffect (QGL::FlatColor);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);

        // Restore the matrix stack
        painter->projectionMatrix().pop();
        painter->modelViewMatrix().pop();

}

