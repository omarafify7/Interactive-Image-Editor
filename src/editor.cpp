// editor.cpp


#include "editor.h"



// Take the source image, apply the provided transform, T, and store
// the transformed image in the destination image.
//
// Where a pixel in the destination image has no corresponding (valid)
// pixel in the source image, use the 'transparentPixel'
//
// Also apply the intensity transform.


void Editor::project( Texture *srcImage, Texture *destImage )

{
  // Check that dimensions match
  
  if (srcImage->width != destImage->width || srcImage->height != destImage->height) {
    cerr << "in Editor::project() the source and destination images have different dimensions" << endl;
    exit(1);
  }

  // Project

  mat4 T = recentMovementTransform * accumulatedTransform;
  
  Pixel transparentPixel = { 0,0,0,0 }; // fully transparent pixel (alpha = 0, so r,g,b doesn't matter)
    
  if (projectionMode == FORWARD) { // Forward projection
    
    // Set all of the image to transparent pixels in case there are
    // destination locations that do not get written to with forward
    // projection.

    for (unsigned int x=0; x<destImage->width; x++)
      for (unsigned int y=0; y<destImage->height; y++)
        destImage->pixel( x, y ) = transparentPixel;

    // Do the forward projection

    for (unsigned int x=0; x<srcImage->width; x++)
      for (unsigned int y=0; y<srcImage->height; y++) {
        vec4 destPos = T * vec4(x,y,0,1);
        if (destPos.x >= 0 && destPos.x < destImage->width && destPos.y >= 0 && destPos.y < destImage->height) {

	  Pixel p = srcImage->pixel(x,y);

	  p = applyIntensityTransform( p );
	  
          destImage->pixel( (int) destPos.x, (int) destPos.y ) = p;
	}
      }

  } else { // Backward projection

    // Where a destination pixel has no corresponding source pixel,
    // set the destination pixel to 'transparentPixel'.
    // 
    // Note that the inverse must not be recalculated with every
    // iteration of the loops.  That's incredibly slow.  Use 'inverse'
    // from linalg.h.
    //
    // Use 'applyIntensityTransform()' as above.
    
    // YOUR CODE HERE

    // Calculate inverse transformation matrix once for efficiency
    mat4 T_inverse = T.inverse();
    
    // For each destination pixel
    for (unsigned int x=0; x<destImage->width; x++) {
      for (unsigned int y=0; y<destImage->height; y++) {
        
        // Apply inverse transform to find corresponding source position
        vec4 srcPos = T_inverse * vec4(x, y, 0, 1);
        
        // Check if source position is within valid bounds
        if (srcPos.x >= 0 && srcPos.x < srcImage->width && 
            srcPos.y >= 0 && srcPos.y < srcImage->height) {
          
          // Valid source pixel - copy it and apply intensity transform
          Pixel p = srcImage->pixel((int) srcPos.x, (int) srcPos.y); // The casting performs nearest-neighbor sampling
          p = applyIntensityTransform(p);
          destImage->pixel(x, y) = p;
          
        } else {
          // No valid source pixel - use transparent pixel
          destImage->pixel(x, y) = transparentPixel;
        }
      }
    }
  }
  destImage->updated = true; // necessary to get new image shipped to GPU
}



void Editor::startMouseMotion( float x, float y )

{
  initMousePosition = vec2(x,y);
  mouseDragging = true;
}



void Editor::mouseMotion( float x, float y )

{
  if (!mouseDragging)
    return;
  
  vec2 mousePosition( x, y );
  vec2 imageCentre( displayedImage->width/2, displayedImage->height/2 );

  if (editMode == INTENSITY) {

    // Find mouse movement as fraction of window dimensions

    float vertical   = (mousePosition.y - initMousePosition.y) / (float)(displayedImage->height); // in [-1,+1]
    float horizontal = (mousePosition.x - initMousePosition.x) / (float)(displayedImage->width ); // in [-1,+1]

    // Incorporate that movement into M and B of the transformation
    //
    // Consider pixel intensities to be in [0,1]
    //
    // horizontal movement of H should result in the X position of the
    // transform line moving H units on the x scale, without changing
    // slope.  Note that H is in [-1,1].
    //
    // vertical movement of +1 should result in slope increase by 45
    // degrees, while vertical movement of -1 should result in slope
    // decrease by 45 degrees, with linear interpolation between
    //
    // Do not allow the slope to go outside [5,85] degrees.
    //
    // Note that trig functions use radians.
    //
    // Your code should update 'recentIntensityBias' and 'recentIntensityScale'.

    const float minAngle =  5 / 180.0 * M_PI;
    const float maxAngle = 85 / 180.0 * M_PI;
    
    // YOUR CODE HERE

    // Treat vertical drag as a change in slope angle (in radians):
    // desired angle = atan(accumulatedSlope) + vertical * (pi/4) clamped to [5,85] degrees

    // Then M_total = tan(desired angle). Because total slope = recent * accumulated
    // Set recent = M_total / accumulated
    float theta0 = atanf( accumulatedIntensityScale );
    float desiredTheta = theta0 + vertical * M_PI / 4.0f;  // +/- 45 degrees
    
    // Clamp to [5,85] degrees
    if (desiredTheta < minAngle) desiredTheta = minAngle;
    if (desiredTheta > maxAngle) desiredTheta = maxAngle;

    float M_total = tanf( desiredTheta ); // Total desired slope
    recentIntensityScale = M_total / accumulatedIntensityScale; // Set recent slope accordingly

    // Horizontal drag H in [-1,1] should shift the line horizontally by H
    // without changing slope. A horizontal shift by +H for a
    // line with slope M_total corresponds to intercept change of -M_total*H.
    // Anchor to the accumulated intercept at drag start:
    float H = horizontal; // already normalized to image width
    float B_base = accumulatedIntensityBias;
    float B_total = B_base - M_total * H; // Finding on the x-axis where y=0 is B_total = -M_total*H

    // Since B_total = recent + accumulated, set recent accordingly
    recentIntensityBias = B_total - accumulatedIntensityBias;

    // Apply the new transform using the project() function
    
    project( baseImage, displayedImage );

  } else if (editMode == SCALE) {

    // scale about the imageCentre

    float initDist    = (initMousePosition - imageCentre).length();
    float currentDist = (mousePosition - imageCentre).length();

    float scaleFactor = currentDist / initDist;

    recentMovementTransform =   translate( imageCentre.x, imageCentre.y, 0 )
                              * scale( scaleFactor, scaleFactor, 1 )
                              * translate( -imageCentre.x, -imageCentre.y, 0 );

    project( baseImage, displayedImage );
  }
}



// Handle the button release at the end of mouse dragging
//
// This requires that any movement change be incorporated into the
// accumulated transforms.

void Editor::stopMouseMotion()

{
  if (editMode == SCALE) {

    // Incorporate the transform from the mouse drag into the 'accumulatedTransform'.

    accumulatedTransform = recentMovementTransform * accumulatedTransform;
    recentMovementTransform = identity4();

  } else if (editMode == INTENSITY) {

    // Incorporate the transform from the mouse drag into the 'accumulatedTransform'.

    accumulatedIntensityScale = recentIntensityScale * accumulatedIntensityScale;
    recentIntensityScale = 1;
    
    accumulatedIntensityBias  = recentIntensityBias  + accumulatedIntensityBias;
    recentIntensityBias = 0;
}
  
  mouseDragging = false;
}



void Editor::keyPress( int keyInt )

{
  unsigned char key = (unsigned char) keyInt;

  // ignore key presses while the mouse is being dragged
  
  if (mouseDragging)
    return;

  // handle key press
  
  switch (key) {

    // Transformation modes
    
  case 'I':
    editMode = INTENSITY;
    break;
  case 'S':
    editMode = SCALE;
    break;

    // Forward/backward projection
    
  case 'F':
    projectionMode = FORWARD;
    project( baseImage, displayedImage );
    break;

  case 'B':
    projectionMode = BACKWARD;
    project( baseImage, displayedImage );
    break;

    // Histogram equalization
    
  case 'E':
    histogramEqualization( originalImage, baseImage, histoRadius );
    project( baseImage, displayedImage );
    break;

  case '+':
  case '=':
    histoRadius++;
    break;

  case '-':
  case '_':
    histoRadius--;
    if (histoRadius < 1)
      histoRadius = 1;
    break;

    // Zero the image (i.e. restore to original)
    
  case 'Z':
    initEditingParams();
    delete baseImage;
    baseImage = new Texture( *originalImage );
    project( baseImage, displayedImage );
    break;
  }
}



// Convert RGB pixel in [0,255]x[0,255]x[0,255] into YUV pixel in same
// ranges.  The Y channel [0,255] maps to [0,1], while the U and V
// channels [0,255] maps to [-0.5,+0.5].

Pixel Editor::rgb_to_yuv( Pixel rgbPixel )

{
  vec3 rgb( rgbPixel.r / 255.0, 
	    rgbPixel.g / 255.0, 
	    rgbPixel.b / 255.0 );

  vec3 yuv = RGBtoYUV * rgb;

  return Pixel( clamp255( rintf(yuv.x * 255) ), 
		clamp255( rintf((yuv.y + 0.5) * 255) ), 
		clamp255( rintf((yuv.z + 0.5) * 255) ), 
		rgbPixel.a );
}



// Convert YUV pixel in [0,255]x[0,255]x[0,255] into RGB pixel in same
// ranges.

Pixel Editor::yuv_to_rgb( Pixel yuvPixel )

{
  vec3 yuv( yuvPixel.r / 255.0, 
	    yuvPixel.g / 255.0 - 0.5, 
	    yuvPixel.b / 255.0 - 0.5 );

  vec3 rgb = YUVtoRGB * yuv;

  return Pixel( clamp255( rint(rgb.x * 255) ), 
		clamp255( rint(rgb.y * 255) ), 
		clamp255( rint(rgb.z * 255) ),
		yuvPixel.a );
}



// Given an RGB pixel, p, convert it to YUV, then apply the intensity
// transform as
//
//   Y' = Y * M + B
//
// where M is the combination of the 'recentIntensityScale' and
// 'accumulatedIntensityScale', and B is the combination of
// 'recentIntensityBias' and 'accumulatedIntensityBias'.
//
// A pixel p has components p.r, p.g, p.b.  After conversion to YUV,
// those components store p.r = Y, p.g = U, p.b = V.
//
// Notet that Y is in the range [0,255] and must be converted to [0,1]
// before applying the transform.
//
// Ensure that Y' is not transformed outside the range [0,1].
//
// Convert Y' back from [0,1] to [0,255] before storing.


Pixel Editor::applyIntensityTransform( Pixel rgb )

{
  // YOUR CODE HERE
  // Convert to YUV (Y in [0,255], U,V encoded in [0,255] with +/-0.5 bias)
  Pixel yuv = rgb_to_yuv(rgb);

  // Build total slope & intercept
  float M = recentIntensityScale * accumulatedIntensityScale;
  float B = recentIntensityBias  + accumulatedIntensityBias;

  // Map Y in [0,255] -> [0,1]
  float Y  = (float)yuv.r / 255.0f;

  // Apply transform and clamp to [0,1]
  float Y_prime = Y * M + B;
  if (Y_prime < 0.0f) Y_prime = 0.0f;
  if (Y_prime > 1.0f) Y_prime = 1.0f;

  // Store back into Y channel in [0,255]
  yuv.r = clamp255( rintf( Y_prime * 255.0f ) );

  // Convert back to RGB
  Pixel out = yuv_to_rgb(yuv);
  
  // Note that both rgb_to_yuv and yuv_to_rgb preserve alpha
  return out;
}


// Perform LOCAL histogram equalization on 'srcImage'.  Fill in
// 'destImage' with the result.  Do the local histogram in a square
// neighbourhood around each pixel.  If 'histoRadius' is R, the
// neighbourhood is (2R+1) x (2R+1).
//
// Do not build the full histogram.  This code should be efficient.


void Editor::histogramEqualization( Texture *srcImage, Texture *destImage, int histoRadius )

{
  // YOUR CODE HERE

  // For each pixel:
  // Define a local neighborhood
  // Build histogram of Y values in that neighborhood
  // Calculate CDF (cumulative distribution function)
  // Use CDF to transform the center pixel's Y value
  // Preserve U and V (color) components
  //
  // Formula for histogram equalization:
  // Y' = (CDF[Y] - CDF_min) / (totalPixels - CDF_min) * 255
  //
  // Where CDF_min is the minimum non-zero CDF value

  // Process each pixel in the source image
  for (int centerX = 0; centerX < srcImage->width; centerX++) {
    for (int centerY = 0; centerY < srcImage->height; centerY++) {
      
      // Build histogram for local neighborhood
      int histogram[256] = {0};  // Initialize all bins to 0
      int totalPixels = 0;
      
      // Define neighborhood bounds
      int minX = centerX - histoRadius;
      int maxX = centerX + histoRadius;
      int minY = centerY - histoRadius;
      int maxY = centerY + histoRadius;
      
      // Clamp to image boundaries
      if (minX < 0) minX = 0;
      if (maxX >= (int)srcImage->width) maxX = srcImage->width - 1;
      if (minY < 0) minY = 0;
      if (maxY >= (int)srcImage->height) maxY = srcImage->height - 1;
      
      // Build histogram from neighborhood
      for (int x = minX; x <= maxX; x++) {
        for (int y = minY; y <= maxY; y++) {
          // Get pixel and convert to YUV
          Pixel rgb = srcImage->pixel(x, y);
          Pixel yuv = rgb_to_yuv(rgb);
          
          // Increment histogram bin for this Y value which is the first channel
          histogram[yuv.r]++;
          totalPixels++;
        }
      }
      
      // Calculate CDF from histogram
      int cdf[256] = {0};
      cdf[0] = histogram[0]; // Initialize first bin
      for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i-1] + histogram[i];
      }
      
      // Find minimum non-zero CDF value
      int cdf_min = 0;
      for (int i = 0; i < 256; i++) {
        if (cdf[i] > 0) {
          cdf_min = cdf[i];
          break;
        }
      }
      
      // Get center pixel and convert to YUV
      Pixel centerRgb = srcImage->pixel(centerX, centerY);
      Pixel centerYuv = rgb_to_yuv(centerRgb);
      
      // Apply histogram equalization to Y component
      // Formula: Y' = ((CDF[Y] - CDF_min) / (totalPixels - CDF_min)) * 255
      int oldY = centerYuv.r;
      unsigned char newY;
      
      if (totalPixels - cdf_min > 0) {
        // Apply equalization formula
        float normalized = (float)(cdf[oldY] - cdf_min) / (float)(totalPixels - cdf_min);
        newY = (unsigned char)(normalized * 255.0);
      } else {
        // Edge case: all pixels in neighborhood have same value
        newY = oldY;
      }
      
      // Update Y component, keep U and V unchanged (preserves color)
      centerYuv.r = newY;
      
      // Convert back to RGB and store in destination
      Pixel resultRgb = yuv_to_rgb(centerYuv);
      destImage->pixel(centerX, centerY) = resultRgb;
    }
  }
  
  // Mark destination as updated so it gets sent to GPU
  destImage->updated = true;


  
}

