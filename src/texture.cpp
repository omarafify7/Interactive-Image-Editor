// texture.cpp


#include "texture.h"
#include "lodepng.h"


bool Texture::useMipMaps = false;


// Register the current texture with OpenGL, assigning it a textureID.


void Texture::registerWithOpenGL( )

{
  // Register it with OpenGL

  glGenTextures( 1, &textureID );
  glBindTexture( GL_TEXTURE_2D, textureID );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

  glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

  glTexImage2D( GL_TEXTURE_2D, 0, (hasAlpha ? GL_RGBA : GL_RGB), width, height, 0,
                (hasAlpha ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, texmap );

  glGenerateMipmap( GL_TEXTURE_2D );
}



void Texture::loadTexture( string filename )

{
  // Read image
  
  std::vector<unsigned char> image;

  unsigned error = lodepng::decode( image, width, height, filename.c_str() );

  if (error)
    std::cerr << "Error loading '" << filename << "': " << lodepng_error_text(error) << std::endl;

  // Copy image to our own texmap
  
  texmap = new GLubyte[ width * height * 4 ];

  GLubyte *p = texmap;
  for (unsigned int i=0; i<width*height*4; i++)
    *p++ = image[i];

  hasAlpha = true;
}




void Texture::createEmptyTexture()

{
  texmap = new GLubyte[ width * height * 4 ];

  GLubyte *p = texmap;
  for (unsigned int i=0; i<width*height; i++) {
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 1; // alpha
  }

  hasAlpha = true;
}



void Texture::copyImageFrom( Texture *src )

{
  // Check that dimensions match
  
  if (src->width != width || src->height != height) {
    cerr << "in Texture::copyImageFrom() the dimensions do not match" << endl;
    exit(1);
  }

  if (src->hasAlpha != hasAlpha) {
    cerr << "in Texture::copyImageFrom() the 'hasAlpha' are not the same" << endl;
    exit(1);
  }

  // Copy

  memcpy( texmap, src->texmap, width * height * (hasAlpha ? 4 : 3) );
}




// Find the texel at x,y for x,y in [0,width-1]x[0,height-1]
//
// Return a reference to the texel so that it can be read to and
// written from.
//
// Note that the texel contains 3 bytes if 'hasAlpha' is false and 4
// bytes if 'hasAlpha' is true.


Pixel & Texture::pixel( int x, int y )

{
  if (x<0) x = 0;
  if (x>(int)width-1) x = width-1;
  if (y<0) y = 0;
  if (y>(int)height-1) y = height-1;

  return * (Pixel*) (texmap + (hasAlpha ? 4 : 3) * (y*width + x));
}



// Draw the texture in a region of the window


void Texture::draw( vec2 lowerLeft, vec2 upperRight )

{
  // Update texture on GPU if necessary (slow ... should be using FBOs)

  if (updated) {
    glBindTexture( GL_TEXTURE_2D, textureID );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, width, height, (hasAlpha ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, texmap );
    updated = false;
  }

  // Draw texture
  
  vec2 verts[4] = { vec2( lowerLeft.x, upperRight.y ), 
		    vec2( lowerLeft.x, lowerLeft.y ), 
		    vec2( upperRight.x, upperRight.y ), 
		    vec2( upperRight.x, lowerLeft.y ) };

  vec2 texcoords[4] = { vec2( 0, 0 ), vec2( 0, 1 ), vec2( 1, 0 ), vec2( 1, 1 ) }; // full texture
    
  GLuint VAO, VBO[2];

  glGenVertexArrays( 1, &VAO );
  glBindVertexArray( VAO );

  glGenBuffers( 2, VBO );

  glBindBuffer( GL_ARRAY_BUFFER, VBO[0] );
  glBufferData( GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW );
  glEnableVertexAttribArray( 0 );
  glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, 0 );

  glBindBuffer( GL_ARRAY_BUFFER, VBO[1] );
  glBufferData( GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW );
  glEnableVertexAttribArray( 1 );
  glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, 0 );

  activate( TEX_UNIT_ID );

  GPUProg->activate();
  GPUProg->setInt( "texUnitID", TEX_UNIT_ID );

  glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 ); // draw the texture

  GPUProg->deactivate();

  deactivate( TEX_UNIT_ID );

  glDeleteBuffers( 2, VBO );
  glDeleteVertexArrays( 1, &VAO );
}



// Shaders for texture rendering


char *Texture::vertexShader = R"XX(

#version 300 es

layout (location = 0) in mediump vec2 position;
layout (location = 1) in mediump vec2 texCoords_in;

out mediump vec2 texCoords;


void main()

{
  gl_Position = vec4( position.x, position.y, 0, 1 );
  texCoords = texCoords_in;
}

)XX";



char *Texture::fragmentShader = R"XX(

#version 300 es

uniform sampler2D texUnitID;

in mediump vec2 texCoords;

out mediump vec4 fragColour;


void main()

{
  fragColour = texture( texUnitID, texCoords );
}

)XX";



