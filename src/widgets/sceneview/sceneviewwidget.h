//    PanoManager - Interactive panorama tour manager program
//    Copyright (C) 2018  Steve M Clarke
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.

//
// Scene View Widget
//

#ifndef SCENEVIEWWIDGET_H
#define SCENEVIEWWIDGET_H

#include <QGLWidget>
#include <QMatrix4x4>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QMouseEvent>

#include "../../project/node.h"
#include "../../icons/icons.h"
#include "../../sceneimage/face.h"

/*
struct VertexData
{
    VertexData()
    {
    }
    VertexData(QVector3D p, QVector2D t) :
        position(p), texCoord(t)
    {
    }
    QVector3D position ;
    QVector2D texCoord ;
} ;
*/

class SceneViewWidget : public QGLWidget, protected QOpenGLFunctions
{
        Q_OBJECT

signals:
    // Real bering (adjusted for north-offset) changed. (lat/lon is in millidegrees)
    void realBearingChanged(int lat, int lon) ;

public:
    SceneViewWidget(QWidget *parent = 0);
    ~SceneViewWidget() ;

    // Load nodes and skybox, select and refresh the nodes
    bool loadScene(NodeList *nodes, Face& front, Face& right, Face& rear, Face& left, Face& top, Face& bottom, int arrivallon=0) ;
    bool clearScene(bool initialisation=false) ;
    void setSelectedNode(QString selection) ;
    void refresh() ;

    // lat/lon are the raw coordinates with respect to the scene skybox
    // not to be confused with real-world coordinates

    // Set lat/lon (in millidegrees) and refresh
    bool setCamera(int lat, int lon) ;

    // Get current lat/lon
    int lat() ;
    int lon() ;

    // Set the north offset lon (in millidegrees) and refresh
    // This is used to adjust the northoffset compass only
    void setNorthCompassLon(int lon) ;

protected:
    void initializeGL() ;
    void resizeGL(int w, int h) ;
    void paintGL() ;

    void mousePressEvent(QMouseEvent *event) ;
    void mouseMoveEvent(QMouseEvent *event) ;

    void initShaders() ;
    void initSquare(int width) ;
    void loadIcons() ;

    QMatrix4x4 matrixFromLonLat(float lon, float lat, float distance, float scale = 1.0f, bool revlatlon=false) ;
    void drawTexturedSquare(QOpenGLTexture *texture, QMatrix4x4 position, bool applycameraoffset=true) ;

private:

    float m_lat, m_lon ;              // Current view direction

    QOpenGLShaderProgram m_program ;  // Shader Program

    const char *hu_mvpMatrix = "modelViewProjectionMat4Uniform" ;     // Handle: Current view position (rotation etc.)
    const char *ha_vertexCoord = "vertexCoordinatesVec4Attribute" ;   // Handle: Vertices for shape to be drawn
    const char *ha_textureCoord = "textureCoordinatesVec2Attribute" ; // Handle: Coordinates in texture associated with vertices
    const char *hu_texture = "textureDataSampler2DUniform" ;          // Handle: Texture to use for drawing

    QString m_errlog ;                // Compilation Error Log
    QMatrix4x4 m_cameraMatrix ;       // View direction matrix
    QMatrix4x4 m_projectionMatrix ;   // View aspect ration / depth etc.

    QVector<QVector3D> m_squareVertices ; // Square Shape Vertices Coordinates
    QVector<GLuint> m_squareIndices ;     // Square Shape Vertices Order
    QVector<QVector2D> m_squareTexture ;  // Square Shape Texture Coordinates

    // Icon and Skybox Textures
    QOpenGLTexture *m_icon[Icon::numTextures] ;
    QOpenGLTexture *m_front, *m_right, *m_rear, *m_left, *m_top, *m_bottom ;
    QOpenGLTexture *m_xhairr, *m_xhairb ;
    QOpenGLTexture *m_compass ;
    QOpenGLTexture *m_selected ;

    // Current Scene Details & Selected Icon
    NodeList *m_nodes ;

    QString m_selection ;
    int m_northOffsetLon ;

    // Mouse Position
    QPoint lastMousePosition ;
    float startLat, startLon ;

    // Window Size
    int m_width, m_height ;

};

#endif // SCENEVIEWWIDGET_H
