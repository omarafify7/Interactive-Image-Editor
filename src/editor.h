// editor.h


#ifndef EDITOR_H
#define EDITOR_H

#include "headers.h"
#include "texture.h"


typedef enum { INTENSITY, SCALE } EditMode;
typedef enum { FORWARD, BACKWARD } ProjectionMode;


class Editor {

  Texture *originalImage;       // original, never changed
  Texture *baseImage;           // base image being edited
  Texture *displayedImage;      // is 'baseImage' after geometric and intensity transforms

  vec2 initMousePosition;       // position on initial mouse click
  bool mouseDragging;		// true while mouse is being dragged to edit

  mat3 RGBtoYUV;
  mat3 YUVtoRGB;

  void initEditingParams() {

    histoRadius = 3;

    accumulatedIntensityScale = 1;
    accumulatedIntensityBias  = 0;
    accumulatedTransform      = identity4();

    recentIntensityScale    = 1;
    recentIntensityBias     = 0;
    recentMovementTransform = identity4();
  }

  unsigned char clamp255( float x ) {
    if (x < 0)
      return 0;
    else if (x > 255)
      return 255;
    else
      return x;
  }

 public:

  EditMode       editMode;
  ProjectionMode projectionMode;

  int   histoRadius;               // neighbourhood for histogram equalization

  float recentIntensityScale;      // current intensity scale through mouse dragging
  float recentIntensityBias;       // current intensity bias through mouse dragging
  mat4  recentMovementTransform;   // current geometric transform through mouse dragging

  float accumulatedIntensityScale; // all slope transforms of pixel intensity so far, accumulated
  float accumulatedIntensityBias;  // all intercept transforms of pixel intensity so far, accumulated
  mat4  accumulatedTransform;      // all geometry transforms so far, accumulated in one matrix

  Editor( Texture *image ) {

    displayedImage = image; // this is the image that the Canvas class draws
    originalImage  = new Texture( *image );
    baseImage      = new Texture( *image );

    editMode = SCALE;
    projectionMode = FORWARD;

    mouseDragging = false;

    initEditingParams();

    RGBtoYUV.rows[0] = {  0.299,    0.587,    0.114   };
    RGBtoYUV.rows[1] = { -0.14713, -0.28886,  0.436   };
    RGBtoYUV.rows[2] = {  0.615,   -0.51499, -0.10001 };

    YUVtoRGB = RGBtoYUV.inverse();
  }

  void histogramEqualization( Texture *srcImage, Texture *destImage, int histoRadius );
  Pixel applyIntensityTransform( Pixel p );
  void project( Texture *srcImage, Texture *destImage );
  
  Pixel rgb_to_yuv( Pixel rgb );
  Pixel yuv_to_rgb( Pixel yuv );
  
  void startMouseMotion( float x, float y );
  void mouseMotion( float x, float y );
  void stopMouseMotion();
  void keyPress( int key );
};

#endif
