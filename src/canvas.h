// canvas.h


#ifndef CANVAS_H
#define CANVAS_H


#include <vector>
#include "headers.h"
#include "main.h"
#include "texture.h"
#include "seq.h"
#include "gpuProgram.h"
#include "drawSegs.h"


class Canvas {

  Texture *image;         // image being drawn.  This is edited elsewhere.
  Texture *background;    // checkerboard background to be drawn below a transparent image
  
  unsigned int canvasWidth, canvasHeight;

  static char *vertexShader;
  static char *fragmentShader;
  GPUProgram  *GPUProg;

  Segs *segs;

 public:

  vec2 imageOrigin; // location of lower-left corner of image in canvas pixels
  

  Canvas( unsigned int width, unsigned int height, Texture *texImage ) {

    canvasWidth  = width;
    canvasHeight = height;

    GPUProg = new GPUProgram();
    GPUProg->init( vertexShader, fragmentShader, "canvas" );

    segs = new Segs();

    image = texImage;

    background = setupBackgroundTexture( image->width, image->height );

    imageOrigin = vec2( 0.5 * (canvasWidth - image->width), 
			0.5 * (canvasHeight + image->height) );
  }

  void draw();

  Texture * setupBackgroundTexture( unsigned int width, unsigned int height );

  void reshape( unsigned int width, unsigned int height ) {

    canvasWidth = width;
    canvasHeight = height;
  }
};


#endif
