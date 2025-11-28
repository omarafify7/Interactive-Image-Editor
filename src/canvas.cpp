// canvas.cpp


#include "canvas.h"
#include "main.h"
#include "gpuProgram.h"
#include "strokefont.h"
#include "editor.h"

#include <sstream>


#define BACKGROUND_BLOCK_SIZE 20 // for background checkboard under transparent images


void Canvas::draw()

{
  // Draw image centred within window

  float x = image->width / (float) canvasWidth;
  float y = image->height / (float) canvasHeight;

  background->draw( vec2(-x,-y), vec2(x,y) );
  
  image->draw( vec2(-x,-y), vec2(x,y) );

  // Draw box around image if image is smaller than window
  //
  // Put box one pixel outside of the image

  const float pixWidth  = 1.0 / (float) canvasWidth;
  const float pixHeight = 1.0 / (float) canvasHeight;

  vec4 colourBlack(0,0,0,1);
  vec4 colourWhite(1,1,1,1);
  vec3 lightDir(1,1,1);
  mat4 M = identity4();

  if (image->width < canvasWidth || image->height < canvasHeight) {

    float pixelX = 2 * pixWidth;
    float pixelY = 2 * pixHeight;
    
    vec3 pts[4] = { vec3( -x-pixelX, -y-pixelY, 0 ), 
		    vec3(  x+pixelX, -y-pixelY, 0 ),
		    vec3(  x+pixelX,  y+pixelY, 0 ),
		    vec3( -x-pixelX,  y+pixelY, 0 ) };
    
    segs->drawSegs( GL_LINE_LOOP, pts, colourBlack, NULL, 4, M, M, lightDir );
  }

  // Draw transform graph

  const int BOX_DIM = 200;
  const int BOX_BORDER = 10;

  if (editor->editMode == INTENSITY) {
    
    // white background
  
    vec2 maxP( 1, 1 );
    vec2 minP( 1 - (2*BOX_BORDER + BOX_DIM) * pixWidth, 1 - (2*BOX_BORDER + BOX_DIM) * pixHeight );
  
    vec3 pts[4] = { 
      vec3( minP.x, minP.y, 0 ), 
      vec3( maxP.x, minP.y, 0 ),
      vec3( maxP.x, maxP.y, 0 ),
      vec3( minP.x, maxP.y, 0 ) };
    
    segs->drawSegs( GL_TRIANGLE_FAN, pts, colourWhite, NULL, 4, M, M, lightDir );

    // black border

    maxP = vec2( 1 - BOX_BORDER * pixWidth, 1 - BOX_BORDER * pixHeight );
    minP = vec2( 1 - (BOX_BORDER + BOX_DIM) * pixWidth, 1 - (BOX_BORDER + BOX_DIM) * pixHeight );
  
    vec3 pts2[4] = { 
      vec3( minP.x, minP.y, 0 ), 
      vec3( maxP.x, minP.y, 0 ),
      vec3( maxP.x, maxP.y, 0 ),
      vec3( minP.x, maxP.y, 0 ) };
    
    segs->drawSegs( GL_LINE_LOOP, pts2, colourBlack, NULL, 4, M, M, lightDir );

    // line of transfer function

    float m = editor->accumulatedIntensityScale * editor->recentIntensityScale;
    float b = editor->accumulatedIntensityBias + editor->recentIntensityBias;

    // Find intersections of line with box borders

    vec3 endpoints[4];
    int nEndpoints = 0;

    x = (0-b)/m;  // with y=0
    if (x >= 0 && x <= 1) 
      endpoints[nEndpoints++] = vec3( x, 0, 0 );

    x = (1-b)/m;  // with y=1
    if (x >= 0 && x <= 1) 
      endpoints[nEndpoints++] = vec3( x, 1, 0 );

    y = b; // with x=0
    if (y >= 0 && y <= 1) {
      endpoints[nEndpoints] = vec3( 0, y, 0 );
      bool found = false;
      for (int i=0; i<nEndpoints; i++)
	if (endpoints[nEndpoints] == endpoints[i])
	  found = true;
      if (!found)
	nEndpoints++;
    }

    y = m+b; // with x=1
    if (y >= 0 && y <= 1) {
      endpoints[nEndpoints] = vec3( 1, y, 0 );
      bool found = false;
      for (int i=0; i<nEndpoints; i++)
	if (endpoints[nEndpoints] == endpoints[i]) 
	  found = true;
      if (!found)
	nEndpoints++;
    }

    if (nEndpoints == 2) {

      // Convert to box-in-window coordinates

      vec3 pts3[2] = { 
	vec3( (1-endpoints[0].x) * minP.x + endpoints[0].x * maxP.x,
	      (1-endpoints[0].y) * minP.y + endpoints[0].y * maxP.y,
	      0 ),
	vec3( (1-endpoints[1].x) * minP.x + endpoints[1].x * maxP.x,
	      (1-endpoints[1].y) * minP.y + endpoints[1].y * maxP.y,
	      0 ) };

      // Draw line
    
      vec4 colourLine( 0.67, 0.77, 0.11, 1 );
      glLineWidth( 2 );
      segs->drawSegs( GL_LINES, pts3, colourLine, NULL, 2, M, M, lightDir );
      glLineWidth( 1 );
    }
  }

  // Draw status message

  string modeStr;
  
  if (editor->editMode == SCALE)
    modeStr = "scale";
  else if (editor->editMode == INTENSITY)
    modeStr = "intensity";
  else
    modeStr = "";

  strokeFont->drawStrokeString( modeStr.c_str(), -0.98, -0.95, 0.06, 0, LEFT );

  char radiusStr[100];
  sprintf( radiusStr, "radius %d", editor->histoRadius );

  strokeFont->drawStrokeString( radiusStr, 0, -0.95, 0.06, 0, CENTRE );

  string projStr;
  
  if (editor->projectionMode == FORWARD)
    projStr = "forward";
  else if (editor->projectionMode == BACKWARD)
    projStr = "backward";
  else
    projStr = "";

  strokeFont->drawStrokeString( projStr.c_str(), 0.98, -0.95, 0.06, 0, RIGHT );
}


// Create a checkerboard texture to be used as background below
// transparent images

Texture * Canvas::setupBackgroundTexture( unsigned int width, unsigned int height ) 

{
  Texture *tex = new Texture( width, height );

  for (unsigned int x=0; x<width; x++)
    for (unsigned int y=0; y<height; y++) {

      Pixel &p = tex->pixel(x,y);
	
      if ((x/BACKGROUND_BLOCK_SIZE + y/BACKGROUND_BLOCK_SIZE) % 2 == 0)
	p.r = p.g = p.b = 230; // grey
      else
	p.r = p.g = p.b = 255; // white

      p.a = 255; // opaque
    }

  return tex;
}


// Shaders for canvas rendering


char *Canvas::vertexShader = R"XX(

#version 300 es

uniform mediump mat4 MVP;

layout (location = 0) in mediump vec4 position;
layout (location = 1) in mediump vec4 colour_in;

out mediump vec4 colour;


void main()

{
  gl_Position = MVP * position;
  colour = colour_in;
}

)XX";



char *Canvas::fragmentShader = R"XX(

#version 300 es

in mediump vec4 colour;

out mediump vec4 fragColour;


void main()

{
  fragColour = colour;
}

)XX";



