#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LeapSample04App : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void LeapSample04App::setup()
{
}

void LeapSample04App::mouseDown( MouseEvent event )
{
}

void LeapSample04App::update()
{
}

void LeapSample04App::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( LeapSample04App, RendererGl )
