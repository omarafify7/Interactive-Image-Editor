// texture.h


#ifndef TEXTURE_H
#define TEXTURE_H

#include "headers.h"
#include "gpuProgram.h"


#define TEX_UNIT_ID 0 // texture unit to use for full-window texture


class Pixel { 
 public:

  unsigned char r, g, b, a;

  Pixel() {}

  Pixel( unsigned char rr, unsigned char gg, unsigned char bb, unsigned char aa ) {
    r = rr; g = gg; b = bb; a = aa;
  }
  
  Pixel( unsigned char rr, unsigned char gg, unsigned char bb ) {
    r = rr; g = gg; b = bb; a = 255;
  }
};


class Texture {

  static char *vertexShader;
  static char *fragmentShader;
  GPUProgram  *GPUProg;

  void registerWithOpenGL();
  void loadTexture( string filename );

  bool registeredWithOpenGL; // true once texture is registerd with OpenGL

 public:

  GLubyte *texmap; 

  string name;
  GLuint textureID;
  unsigned int width, height;
  bool hasAlpha;
  bool updated; // true if texture was changed.  forces a re-transmission to the GPU.

  static bool useMipMaps;

  Texture() {
    GPUProg = NULL;
    texmap = NULL;
    updated = false;
  }

  // texture from file

  Texture( string filename ) {

    name = filename;
    loadTexture( filename ); // sets 'texmap'
    GPUProg = NULL;
    registeredWithOpenGL = false;
    updated = false;
  }

  // empty texture
  
  Texture( unsigned int texWidth, unsigned int texHeight ) {

    name = "texture";
    width = texWidth;
    height = texHeight;
    createEmptyTexture(); // sets 'texmap'
    GPUProg = NULL;
    registeredWithOpenGL = false;
    updated = false;
  }

  // copy constructor

  Texture( Texture &t ) {

    width    = t.width;
    height   = t.height;
    hasAlpha = t.hasAlpha;
    name     = t.name;
    
    texmap = new GLubyte[ width * height * (hasAlpha ? 4 : 3) ];
    memcpy( texmap, t.texmap, width * height * (hasAlpha ? 4 : 3) );

    // must register this as a new texture, at which time 'GPUProg'
    // and 'textureID' will be set

    GPUProg = NULL;
    registeredWithOpenGL = false;
    updated = false;
  }

  // destructor

  ~Texture() {

    if (texmap != NULL)
      delete [] texmap;

    if (GPUProg != NULL)
      delete GPUProg;
  }

  void activate( int textureUnit ) {
    
    if (!registeredWithOpenGL) {
      GPUProg = new GPUProgram();
      GPUProg->init( vertexShader, fragmentShader, "texture" );
      registerWithOpenGL();
      registeredWithOpenGL = true;
    }

    glActiveTexture( GL_TEXTURE0 + textureUnit );
    glBindTexture( GL_TEXTURE_2D, textureID );
    if (hasAlpha) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    } else
      glDisable(GL_BLEND);
  }

  void deactivate( int textureUnit ) {
    glBindTexture( GL_TEXTURE_2D, 0 );
  }


  void createEmptyTexture();
  void draw( vec2 lowerLeft, vec2 upperRight );
  void copyImageFrom( Texture *src );

  Pixel & pixel( int i, int j );
};


#endif
