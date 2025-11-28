// Image editor


#include "headers.h"
#include "gpuProgram.h"
#include "canvas.h"
#include "editor.h"
#include "strokefont.h"
#include "main.h"


Canvas     *canvas;      // code to display image
StrokeFont *strokeFont;  // code to draw characters
Editor     *editor;	 // code to edit image

#define IMAGE_BORDER 50  // border around image


vec2 currentMousePosition;
bool mousePositionChanged;


// Handle a keypress and record the state (UP or DOWN) of the arrows

void keyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )

{
  // Note that GLFW always returns the uppercase letter, even if the lowercase letter was pressed.
  
  if (action == GLFW_PRESS)
    
    switch (key) {
      
    case GLFW_KEY_ESCAPE: // quit upon ESC
      glfwSetWindowShouldClose( window, GL_TRUE );
      break;

    default: // Inform the editor of a keypress
      editor->keyPress( key );
    }
}


  
// Error callback

void errorCallback( int error, const char* description )

{
  cerr << "Error " << error << ": " << description << endl;
  exit(1);
}



// Callbacks for when window size changes


void windowReshapeCallback( GLFWwindow* window, int width, int height )

{
  canvas->reshape( width, height );
}

void framebufferReshapeCallback( GLFWwindow* window, int width, int height )

{
  glViewport( 0, 0, width, height );
}



// Mouse motion callback
//
// Only enabled when mouse button is down (which is done in mouseButtonCallback())

void mousePositionCallback( GLFWwindow* window, double x, double y )

{
  mousePositionChanged = true;

  currentMousePosition = vec2( x, y );
}



// Mouse button callback

void mouseButtonCallback( GLFWwindow* window, int button, int action, int mods )

{
  if (action == GLFW_PRESS) {

    double x, y;
    glfwGetCursorPos(window, &x, &y );

    // Inform the editor
    //
    // Convert mouse coords to image coords with y going up
    
    editor->startMouseMotion( x - canvas->imageOrigin.x, canvas->imageOrigin.y - y );

    mousePositionChanged = false;

    // enable mouse motion events
    
    glfwSetCursorPosCallback( window, mousePositionCallback );

  } else if (action == GLFW_RELEASE) {

    // disable mouse movement events
      
    glfwSetCursorPosCallback( window, NULL );

    // Inform the editor
    
    editor->stopMouseMotion();
  }
    
}



// Main program


int main( int argc, char **argv )

{
  // Read the image

  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " imageFilename" << endl;
    exit(1);
  }

  Texture *image = new Texture( argv[1] );

  // Trap all errors (do this *before* creating the window)

  glfwSetErrorCallback( errorCallback );
  
  // Init everything

  if (!glfwInit())
    return 1;
  
#ifdef MACOS
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
  glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
#else
  glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_ES_API );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );
#endif

  unsigned int windowWidth  = image->width + 2*IMAGE_BORDER;
  unsigned int windowHeight = image->height + 2*IMAGE_BORDER;

  GLFWwindow* window = glfwCreateWindow( windowWidth, windowHeight, "Editor", NULL, NULL );
  
  if (!window) {
    glfwTerminate();
    return 1;
  }

  glfwSetWindowPos( window, 100, 100 ); // position near the upper-left corner at (100,100)

  glfwMakeContextCurrent( window );
  glfwSwapInterval( 1 );
  gladLoadGLLoader( (GLADloadproc) glfwGetProcAddress );

  // Define callbacks

  glfwSetWindowSizeCallback( window, windowReshapeCallback );
  glfwSetFramebufferSizeCallback( window, framebufferReshapeCallback );
  glfwSetKeyCallback( window, keyCallback );
  glfwSetMouseButtonCallback( window, mouseButtonCallback );

  // Set up 
  
  strokeFont = new StrokeFont();
  canvas = new Canvas( windowWidth, windowHeight, image );
  editor = new Editor( image );

  // Main loop

  while (!glfwWindowShouldClose( window )) {

    // Clear, display, and check for events

    glClearColor( 1, 1, 1, 1 );	// white background
    glClear( GL_COLOR_BUFFER_BIT );
    
    canvas->draw();
      
    glfwSwapBuffers( window );
    glfwPollEvents();

    // Inform the editor if the mouse moved.

    if (mousePositionChanged) {

      // Convert mouse coords to image coords with y going up
    
      editor->mouseMotion( currentMousePosition.x - canvas->imageOrigin.x, 
			   canvas->imageOrigin.y - currentMousePosition.y );

      mousePositionChanged = false;
    }
  }

  // Clean up

  glfwDestroyWindow( window );
  glfwTerminate();

  return 0;
}
