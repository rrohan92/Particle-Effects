#include "ParticleSystemPCH.h"
#include "ElapsedTime.h"
#include "PivotCamera.h"
#include "ParticleEffect.h"
#include "SphereEmitter.h"
#include "CubeEmitter.h"
#include <algorithm>

PivotCamera g_Camera;
SphereEmitter g_ParticleEmitter;
CubeEmitter g_CubeEmitter;

#if _DEBUG
ParticleEffect g_ParticleEffect(1000);
#else
ParticleEffect g_ParticleEffect(100000);
#endif 

int g_iWindowWidth = 512;
int g_iWindowHeight = 512;
int g_iGLUTWindowHandle = 0;
int g_iErrorCode = 0;

bool g_bLeftMouseDown = false;
bool g_bRightMouseDown = false;

bool g_bUpdate = true;

glm::vec2 g_MouseCurrent = glm::vec2(0);
glm::vec2 g_MousePrevious = glm::vec2(0);
glm::vec2 g_MouseDelta = glm::vec2(0);

glm::vec3 g_DefaultCameraTranslate( 0, 0, 100 );
glm::vec3 g_DefaultCameraRotate( 40, 0, 0 );
glm::vec3 g_DefaultCameraPivot( 0, 0, 0 );

void InitGL( int argc, char* argv[] );
void DisplayGL();
void IdleGL();
void KeyboardGL( unsigned char c, int x, int y );
void MouseGL( int button, int state, int x, int y );
void MotionGL( int x, int y );
void ReshapeGL( int w, int h );

void Cleanup();

int main( int argc, char* argv[] )
{
	std::cout << "Welcome to Particle Effect FireWorks Display" << std::endl;
	std::cout << "Press spacebar to pause fireworks" << std::endl;
	std::cout << "Press left and right mouse button to move the particles and camera movement respectively" << std::endl;
    InitGL( argc, argv );

    g_Camera.SetTranslate( g_DefaultCameraTranslate );
    g_Camera.SetRotate( g_DefaultCameraRotate );
    g_Camera.SetPivot( g_DefaultCameraPivot );

   
    ParticleEffect::ColorInterpolator colors;

    colors.AddValue(0.0f,  glm::vec4(1, 1, 0, 1) );    
    colors.AddValue(0.15f, glm::vec4(1, 0, 1, 1) );   
    colors.AddValue(0.33f, glm::vec4(0, 0, 1, 1) );    
    colors.AddValue(0.5f,  glm::vec4(0, 1, 1, 1) );    
    colors.AddValue(0.67f, glm::vec4(0, 1, 0, 0.75) );  
    colors.AddValue(0.84f, glm::vec4(1, 1, 0, 0.5) );  
    colors.AddValue(1.0f,  glm::vec4(1, 0, 0, 0) );    

    g_ParticleEffect.SetColorInterplator( colors );

    g_ParticleEffect.SetParticleEmitter( &g_ParticleEmitter );
    g_ParticleEffect.EmitParticles();
    g_ParticleEffect.SetCamera( &g_Camera );

    glutMainLoop();
}

void Cleanup()
{
    if ( g_iGLUTWindowHandle != 0 )
    {
        glutDestroyWindow( g_iGLUTWindowHandle );
        g_iGLUTWindowHandle = 0;
    }
}


void InitGL( int argc, char* argv[] )
{

    glutInit(&argc, argv);
    int iScreenWidth = glutGet(GLUT_SCREEN_WIDTH);
    int iScreenHeight = glutGet(GLUT_SCREEN_HEIGHT);

    glutInitDisplayMode( GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH );

    glutInitWindowPosition( ( iScreenWidth - g_iWindowWidth ) / 2,
        ( iScreenHeight - g_iWindowHeight ) / 2 );
    glutInitWindowSize( g_iWindowWidth, g_iWindowHeight );

    g_iGLUTWindowHandle = glutCreateWindow( "CS 637 Project" );

    // Register GLUT callbacks
    glutDisplayFunc(DisplayGL);
    glutIdleFunc(IdleGL);
    glutMouseFunc(MouseGL);
    glutMotionFunc(MotionGL);
    glutKeyboardFunc(KeyboardGL);
    glutReshapeFunc(ReshapeGL);

    // Setup initial GL State
    glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
    glClearDepth( 1.0f );

    glShadeModel( GL_SMOOTH );

}


void DisplayGL()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );                   // Clear the color buffer, and the depth buffer.

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    g_Camera.ApplyViewTransform();


    g_ParticleEffect.Render();

    glutSwapBuffers();
    glutPostRedisplay();
}

void IdleGL()
{
    static ElapsedTime elapsedTime;
    float fDeltaTime = elapsedTime.GetElapsedTime();

    if ( g_bUpdate )
    {
        g_ParticleEffect.Update(fDeltaTime);
    }
    else 
    {
        g_ParticleEffect.BuildVertexBuffer();
    }

    glutPostRedisplay();
}

template< typename T >
void Increment( T& value, const T& step, const T& max )
{
    value = std::min( max, value + step );
}

template< typename T >
void Decrement( T& value, const T& step, const T& min )
{
    value = std::max( min, value - step );
}


void KeyboardGL( unsigned char c, int x, int y )
{
    float fRadiusStep = 1.0f;

    switch ( c )
    {
    case ' ': 
        {
            
            g_bUpdate = !g_bUpdate;
        }
        break;
   
      
    }

    glutPostRedisplay();
}

void MouseGL( int button, int state, int x, int y )
{
    if ( button == GLUT_LEFT_BUTTON )
    {
        g_bLeftMouseDown = ( state == GLUT_DOWN );
    }
    else if ( button == GLUT_RIGHT_BUTTON )
    {
        g_bRightMouseDown = ( state == GLUT_DOWN );
    }
    g_MousePrevious = glm::vec2( x, y );

}

void MotionGL( int x, int y )
{
    g_MouseCurrent = glm::vec2( x, y );
    g_MouseDelta = ( g_MousePrevious - g_MouseCurrent );

    // Update the camera
    if ( g_bLeftMouseDown && g_bRightMouseDown )
    {
        g_Camera.TranslateZ( g_MouseDelta.y );
    }
    else if ( g_bLeftMouseDown )
    {
        g_Camera.AddPitch( -g_MouseDelta.y );
        g_Camera.AddYaw( -g_MouseDelta.x );
    }
    else if ( g_bRightMouseDown )
    {
        g_Camera.TranslatePivotX( g_MouseDelta.x );
        g_Camera.TranslatePivotY( -g_MouseDelta.y );
    }    

    g_MousePrevious = g_MouseCurrent;
}

void ReshapeGL( int w, int h )
{
    h = std::max( h, 1 );
    
    g_iWindowWidth = w;
    g_iWindowHeight = h;

    g_Camera.SetViewport( 0, 0, w, h );
    g_Camera.ApplyViewport();

    g_Camera.SetProjection( 60.0f, w/(float)h, 0.1f, 1000.0f );
    g_Camera.ApplyProjectionTransform();

    glutPostRedisplay();
}

