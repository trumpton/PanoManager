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

#include "sceneviewwidget.h"
#include <QtMath>
#include <QDebug>

//
// Projection:
//
//     +---------+
//     |         |
//     |    X    |
//     +--- | ---+
//          |
//          +----Y
//           \
//            \ Z
//


SceneViewWidget::SceneViewWidget(QWidget *parent) : QGLWidget(parent)
{
    clearScene(true) ;
    m_compass=NULL ; m_selected=NULL ;
    m_xhairr=NULL ; m_xhairb=NULL ;
    for (int i=0; i<Icon::numTextures; i++) m_icon[i]=NULL ;
    m_selection="" ;
}

SceneViewWidget::~SceneViewWidget()
{
    clearScene() ;
    if (m_selected) delete m_selected ;
    if (m_compass) delete m_compass ;
    if (m_xhairr) delete m_xhairr ;
    if (m_xhairb) delete m_xhairb ;
    for (int i=0; i<Icon::numTextures; i++) if (m_icon[i]) delete m_icon[i] ;
}


//===========================================================
//
// Clear the Skybox Faces
//

bool SceneViewWidget::clearScene(bool initialisation)
{
    if (!initialisation) {
        if (m_front) delete m_front ;
        if (m_left) delete m_left ;
        if (m_right) delete m_right ;
        if (m_rear) delete m_rear ;
        if (m_top) delete m_top ;
        if (m_bottom) delete m_bottom ;
    }
    m_nodes=NULL ;
    m_northOffsetLon=0 ;
    m_front=NULL ; m_right=NULL ; m_rear=NULL ;
    m_left=NULL ; m_top=NULL ; m_bottom=NULL ;
    m_selection="" ;
    return true ;
}

//===========================================================
//
// Load the Skybox Faces
//

bool SceneViewWidget::loadScene(NodeList *nodes, Face &front, Face &right, Face &rear, Face &left, Face &top, Face &bottom, int arrivallon)
{
    m_nodes = nodes ;
    m_lat=0.0f ;
    m_lon=0.0f ;
    m_northOffsetLon=0 ;
    m_selection = "" ;

    if (m_front) delete m_front ;
    m_front = new QOpenGLTexture(front.image().mirrored()) ;
    m_front->setMinificationFilter(QOpenGLTexture::Nearest) ;
    m_front->setMagnificationFilter(QOpenGLTexture::Linear) ;
    m_front->setWrapMode(QOpenGLTexture::Repeat) ;

    if (m_right) delete m_right ;
    m_right = new QOpenGLTexture(right.image().mirrored()) ;
    m_right->setMinificationFilter(QOpenGLTexture::Nearest) ;
    m_right->setMagnificationFilter(QOpenGLTexture::Linear) ;
    m_right->setWrapMode(QOpenGLTexture::Repeat) ;

    if (m_rear) delete m_rear ;
    m_rear = new QOpenGLTexture(rear.image().mirrored()) ;
    m_rear->setMinificationFilter(QOpenGLTexture::Nearest) ;
    m_rear->setMagnificationFilter(QOpenGLTexture::Linear) ;
    m_rear->setWrapMode(QOpenGLTexture::Repeat) ;

    if (m_left) delete m_left ;
    m_left = new QOpenGLTexture(left.image().mirrored()) ;
    m_left->setMinificationFilter(QOpenGLTexture::Nearest) ;
    m_left->setMagnificationFilter(QOpenGLTexture::Linear) ;
    m_left->setWrapMode(QOpenGLTexture::Repeat) ;

    if (m_top) delete m_top ;
    m_top = new QOpenGLTexture(top.image().mirrored()) ;
    m_top->setMinificationFilter(QOpenGLTexture::Nearest) ;
    m_top->setMagnificationFilter(QOpenGLTexture::Linear) ;
    m_top->setWrapMode(QOpenGLTexture::Repeat) ;


    if (m_bottom) delete m_bottom ;
    m_bottom = new QOpenGLTexture(bottom.image().mirrored()) ;
    m_bottom->setMinificationFilter(QOpenGLTexture::Nearest) ;
    m_bottom->setMagnificationFilter(QOpenGLTexture::Linear) ;
    m_bottom->setWrapMode(QOpenGLTexture::Repeat) ;

    updateGL() ;

    return true ;
}

//===========================================================
//
// OpenGL Drawing Functions
//

void SceneViewWidget::initializeGL()
{
    float red = 0.8f, green = 0.8f, blue = 0.8f ;

    initializeOpenGLFunctions(); // NIO

    glEnable(GL_DEPTH_TEST) ;
    glEnable(GL_CULL_FACE) ;

    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glClearColor(red, green, blue, 1.0f);

    // Load the Shaders
    initShaders() ;

    // Initialise the Square SHape
    initSquare(1.0f) ;

    // Load the Textures
    loadIcons() ;
}

void SceneViewWidget::resizeGL(int w, int h)
{
    const float verticalAngle = 70.0f ;
    const float nearPlane = 1.0f ;
    const float farPlane = 60.0f ;
    m_width = w ;
    m_height = h ;
    float aspectRatio = w / (float)h ;
    m_projectionMatrix.setToIdentity();
    m_projectionMatrix.perspective(verticalAngle, aspectRatio, nearPlane, farPlane) ;
    glViewport(0, 0, w, h) ; // NIO
}

void SceneViewWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) ;

    // Draw the Skybox
    drawTexturedSquare(m_front,  matrixFromLonLat(   0.0f,   0.0f, 20.0f, 40.1f)) ;
    drawTexturedSquare(m_right,  matrixFromLonLat(  90.0f,   0.0f, 20.0f, 40.1f)) ;
    drawTexturedSquare(m_rear,   matrixFromLonLat( 180.0f,   0.0f, 20.0f, 40.1f)) ;
    drawTexturedSquare(m_left,   matrixFromLonLat( -90.0f,   0.0f, 20.0f, 40.1f)) ;
    drawTexturedSquare(m_top,    matrixFromLonLat(   0.0f,  90.0f, 20.0f, 40.1f)) ;
    drawTexturedSquare(m_bottom, matrixFromLonLat(   0.0f, -90.0f, 20.0f, 40.1f)) ;

    // Draw the Compass adjusted for north offset
    float northoffsetlongitude = m_northOffsetLon / 1000.0f ;
    QMatrix4x4 nol = matrixFromLonLat(northoffsetlongitude, 0.0f, 0.0f) ;
    drawTexturedSquare(m_compass, nol * matrixFromLonLat(0.0f, -90.0f, 19.5f, 15.0f)) ;

    // Draw the icons
    if (m_nodes) {

        // TODO: This crashes if all nodes deleted for a scene, then file re-opened

        int numnodes = m_nodes->count() ;
        for (int i=0; i<numnodes; i++) {
            Node *node = (Node*)&(m_nodes->at(i)) ;
            int icon = node->type() ;
            float lat = node->lat() / 1000.0f ;
            float lon = node->lon() / 1000.0f ;
            drawTexturedSquare(m_icon[icon], matrixFromLonLat(lon, lat, 19.0f, 3.0f, true)) ;
            if (node->id().compare(m_selection)==0)
                drawTexturedSquare(m_selected, matrixFromLonLat(lon, lat , 18.99f, 3.0f, true)) ;
        }
    }

    // Draw the crosshairs (no translation for compass or north offset)
    drawTexturedSquare(m_xhairb, matrixFromLonLat(0.0f, 0.0f, 1.0f, 0.2f), false) ;
}

void SceneViewWidget::drawTexturedSquare(QOpenGLTexture *texture, QMatrix4x4 position, bool applycameraoffset)
{
    if (!texture) return ;

    m_program.bind() ;

    texture->bind(0) ; // Use texture, and set id to 0

    // Rotate / scale etc. the square
    QMatrix4x4 transform = m_projectionMatrix ;
    if (applycameraoffset) transform = transform * m_cameraMatrix ;
    transform = transform * position ;

    m_program.setUniformValue(hu_mvpMatrix, transform) ;
    m_program.setUniformValue(hu_texture, 0) ; // Use texture id 0

    // Texture Coordinates
    m_program.setAttributeArray(ha_textureCoord, m_squareTexture.constData()) ;
    m_program.enableAttributeArray(ha_textureCoord) ;

    // Vertex Coordinates
    m_program.setAttributeArray(ha_vertexCoord, m_squareVertices.constData()) ;
    m_program.enableAttributeArray(ha_vertexCoord) ;

    // Draw
    glDrawArrays(GL_TRIANGLES, 0, m_squareVertices.size()) ;

    // Release
    m_program.disableAttributeArray(ha_vertexCoord);
//    m_program.disableAttributeArray(ha_textureCoord);
    m_program.release() ;
}

void SceneViewWidget::initShaders()
{
    if (!m_program.addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vshader.vsh")) {
        m_errlog = "VShader: " + m_program.log() ;
        close() ;
    } else if (!m_program.addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fshader.fsh")) {
        m_errlog = "FShader: " + m_program.log() ;
        close() ;
    } else if (!m_program.link()) {
        m_errlog = "Link: " + m_program.log() ;
        close() ;
    }
}

//===========================================================
//
// Shape Initialisation
//

//
// Square in the x/y plane, located at the origin.
//
void SceneViewWidget::initSquare(int width)
{
    //
    // The 3D and Texture coordinate origins are not the same
    // Consequently, two different maps are required, one for the
    // model (QVector3D) and one for the texture.
    //
    //    0------3    +-------+   +-------+
    //    |      |    |       |   |       |
    //    |      |    |  0,0  |   |       |
    //    |      |    |       |   |       |
    //    1------2    +-------+  0,0------+
    //
    //    Vertices    3D Space    Texture
    //    Order       xy Origin   xy Origin
    //

    // List of the 4 vertices for the square which is width x width in size
    float side = width * 0.5f ;

    // Coordinates: 0: top-left, 1: bottom-left, 2: bottom-right,
    //              2: bottom-left, 3: top-right, 0: top-left
    m_squareVertices << QVector3D(-side, side, 0) << QVector3D(-side, -side, 0) << QVector3D(side, -side, 0)
                     << QVector3D(side, -side, 0) << QVector3D(side, side, 0) << QVector3D(-side, side, 0) ;
    m_squareTexture << QVector2D(0, 1) << QVector2D(0, 0) << QVector2D(1,0)
                    << QVector2D(1,0) << QVector2D(1, 1) << QVector2D(0, 1);
}


void SceneViewWidget::loadIcons()
{
    // Load the Cross-Hairs (no need to mirror as symmetrical)
    if (m_xhairr) delete m_xhairr ;
    m_xhairr = new QOpenGLTexture(QImage(":/texture/crosshairs-red.png")) ;
    m_xhairr->setMinificationFilter(QOpenGLTexture::Nearest) ;
    m_xhairr->setMagnificationFilter(QOpenGLTexture::Linear) ;
    m_xhairr->setWrapMode(QOpenGLTexture::Repeat) ;

    if (m_xhairb) delete m_xhairb ;
    m_xhairb = new QOpenGLTexture(QImage(":/texture/crosshairs-black.png")) ;
    m_xhairb->setMinificationFilter(QOpenGLTexture::Nearest) ;
    m_xhairb->setMagnificationFilter(QOpenGLTexture::Linear) ;
    m_xhairb->setWrapMode(QOpenGLTexture::Repeat) ;

    // Load Compass

    if (m_compass) delete m_compass ;
    m_compass = new QOpenGLTexture(QImage(":/texture/compass.png").mirrored()) ;
    m_compass->setMinificationFilter(QOpenGLTexture::Nearest) ;
    m_compass->setMagnificationFilter(QOpenGLTexture::Linear) ;
    m_compass->setWrapMode(QOpenGLTexture::Repeat) ;

    // Load Selected Highlight

    if (m_selected) delete m_selected ;
    m_selected = new QOpenGLTexture(QImage(":/texture/selected.png").mirrored()) ;
    m_selected->setMinificationFilter(QOpenGLTexture::Nearest) ;
    m_selected->setMagnificationFilter(QOpenGLTexture::Linear) ;
    m_selected->setWrapMode(QOpenGLTexture::Repeat) ;

    // Load the different textures that can be drawn on the square and presented as icons
    for (int i=0; i<Icon::numTextures; i++) {
        int orientation = Icon::textureOrientation((Icon::IconType)i) ;
        QString file = Icon::textureFile((Icon::IconType)i) ;
        if (m_icon[i]) delete m_icon[i] ;
        QMatrix matrix ;
        matrix = matrix.rotate(orientation) ;
        m_icon[i] = new QOpenGLTexture(QImage(file).mirrored().transformed(matrix)) ;
        m_icon[i]->setMinificationFilter(QOpenGLTexture::Nearest) ;
        m_icon[i]->setMagnificationFilter(QOpenGLTexture::Linear) ;
        m_icon[i]->setWrapMode(QOpenGLTexture::Repeat) ;
    }
}

//===========================================================
//
// Movement
//

void SceneViewWidget::mousePressEvent(QMouseEvent *event)
{
    startLat = m_lat ;
    startLon = m_lon ;
    lastMousePosition = event->pos() ;
    event->accept() ;
}

void SceneViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    int deltaX = lastMousePosition.x() - event->x() ;
    int deltaY = event->y() - lastMousePosition.y() ;
    if (event->buttons() && Qt::LeftButton) {
        // Crude: need to scale and take width/height of viewport into consideration
        setCamera( (startLat - (deltaY*500.0f/m_height))*1000,
                   (startLon - (deltaX*500.0f/m_width))*1000) ;
    }
    event->accept() ;
}

//===========================================================
//
// Set up the current camera view
// Set the current compass position
// Set the current selected node
// Refresh (used when node icons added / removed / changed)
//

bool SceneViewWidget::setCamera(int lat, int lon) {
    while (lon>=180000) lon-=360000 ;
    while (lon<-180000) lon+=360000 ;
    if (lat<-60000) lat=-60000 ;
    if (lat>60000) lat=60000 ;
    m_lat = lat / 1000.0f ;
    m_lon = lon / 1000.0f ;
    m_cameraMatrix = matrixFromLonLat(-m_lon, -m_lat, 0.0f) ;
    updateGL() ;
    int reallon = lon - m_northOffsetLon ;
    while (reallon>=360000) reallon-=360000 ;
    while (reallon<0) reallon+=360000 ;
    emit realBearingChanged(lat, reallon) ;
    return true ;
}

void SceneViewWidget::setNorthCompassLon(int lon)
{
    m_northOffsetLon = lon ;
    updateGL() ;

    int realbearing = m_lon*1000-lon ;
    while (realbearing>360000) realbearing-=360000 ;
    while (realbearing<0) realbearing+=360000 ;
    emit realBearingChanged(m_lat, realbearing) ;
}

void SceneViewWidget::setSelectedNode(QString selection)
{
    m_selection = selection ;
    updateGL() ;
}

void SceneViewWidget::refresh()
{
    updateGL() ;
}


//===========================================================
//
// Get Current lat/lon with respect to the skybox
//
int SceneViewWidget::lat()
{
    return m_lat * 1000 ;
}

int SceneViewWidget::lon()
{
    return m_lon * 1000 ;
}


//===========================================================
//
// Transformations
// rotate the world in the opposite direction
// rather than rotating the object
// Then translate away from the origin by distance, and then scale
//
QMatrix4x4 SceneViewWidget::matrixFromLonLat(float lon, float lat, float distance, float scale, bool revlatlon)
{
    QMatrix4x4 scalematrix, transmatrix, latmatrix, lonmatrix ;
    latmatrix.rotate(lat, 1.0f, 0.0f, 0.0f) ;
    lonmatrix.rotate(-lon, 0.0f, 1.0f, 0.0f) ;
    transmatrix.translate(0.0f, 0.0f, -distance) ;
    scalematrix.scale(scale) ;

    QMatrix4x4 matrix ;

    if (revlatlon) {
        matrix = lonmatrix * latmatrix * transmatrix * scalematrix ;
    } else {
        matrix = latmatrix * lonmatrix * transmatrix * scalematrix ;
    }

    // Now translate and scale
//    matrix.translate(0.0f, 0.0f, -distance) ;
//    matrix.scale(scale) ;

    return matrix ;
}


