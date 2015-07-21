#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"

#include "Leap.h"

using namespace ci;
using namespace ci::app;
using namespace std;

//#define RAW_DATA

class LeapSample04App : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

    Leap::Controller controller;

#ifdef RAW_DATA
    // 生データ
    const int IMAGE_WIDTH = 640;
    const int IMAGE_HEIGHT = 240;
#else
    // 歪み補正
    const int IMAGE_WIDTH = 400;
    const int IMAGE_HEIGHT = 400;
#endif
};

void LeapSample04App::setup()
{
    // 画像の取得を有効にする
    controller.setPolicy( Leap::Controller::PolicyFlag::POLICY_IMAGES );

    //ウィンドウサイズを設定する
    setWindowSize( Vec2i( IMAGE_WIDTH, IMAGE_HEIGHT * 2 ) );
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

    // 画像を取得する
    Leap::Frame frame = controller.frame();
    Leap::ImageList images = frame.images();

#ifdef RAW_DATA
    // カメラ画像を表示する
    for ( int i = 0; i < 2; i++ ){
        Leap::Image image = images[i];
        if ( !image.isValid() ){
            continue;
        }

        const unsigned char* image_buffer = image.data();

        // グレースケールビットマップで描画する
        Surface surface( image.width(), image.height(), image.width() * 4,
                            SurfaceChannelOrder::RGBA );
        int cursor = 0;
        Surface::Iter iter = surface.getIter();
        while ( iter.line() ) {
            while ( iter.pixel() ) {
                iter.r() = image_buffer[cursor];
                iter.g() = iter.b() = iter.r();
                iter.a() = 255;
                cursor++;
            }
        }

        // 画像を描画する
        gl::Texture texture = gl::Texture( surface );
        gl::draw( texture,
                    Area( 0, IMAGE_HEIGHT * i, IMAGE_WIDTH, IMAGE_HEIGHT * (i + 1) ) );
    }


    // 指の位置を表示する
    for ( int i = 0; i < 2; i++ ){
        Leap::Image image = images[i];
        if ( !image.isValid() ){
            continue;
        }

        Vec2f origin = Vec2f( 0, (IMAGE_HEIGHT * i) );
        const float camera_offset = 20; //x-axis offset of cameras in millimeters
        Leap::FingerList allTheFingers = frame.fingers();
        for ( Leap::FingerList::const_iterator fl = allTheFingers.begin();
                fl != allTheFingers.end(); fl++ ) {
            Leap::Vector tip = (*fl).tipPosition();
            float h_slope = -(tip.x + camera_offset * (2 * i - 1)) / tip.y;
            float v_slope = tip.z / tip.y;

            Leap::Vector pixel = image.warp( Leap::Vector( h_slope, v_slope, 0 ) );
            //gl::color( .5, 0, 1, .5 );
            gl::drawSolidCircle( Vec2f( pixel.x + origin.x, pixel.y + origin.y ), 10 );
        }
    }

#else
    //Draw the undistorted image using the warp() function
    Vec2f origin;
    int targetWidth = IMAGE_WIDTH;
    int targetHeight = IMAGE_HEIGHT;

    // 歪み補正した画像を作成する
    for ( int i = 0; i < 2; i++ ){
        Leap::Image image = images[i];
        if ( !image.isValid() ){
            return;
        }

        // 表示開始位置
        origin = Vec2f( 0, IMAGE_HEIGHT * i );

        Surface targetImage( targetWidth, targetHeight, targetWidth * 4,
            SurfaceChannelOrder::RGBA );

        // 歪みを補正した画像を作成する
        //Iterate over target image pixels, converting xy to ray slope
        //An array to hold the rgba color components
        unsigned char brightness[4] = { 0, 0, 0, 255 }; 
        Surface::Iter tI = targetImage.getIter();
        while ( tI.line() ) {
            while ( tI.pixel() ) {

                //Normalize from pixel xy to range [0..1]
                Leap::Vector input = Leap::Vector( (float)tI.x() / targetWidth,
                    (float)tI.y() / targetHeight, 0 );

                //Convert from normalized [0..1] to slope [-4..4]
                input.x = (input.x - image.rayOffsetX()) / image.rayScaleX();
                input.y = (input.y - image.rayOffsetY()) / image.rayScaleY();

                Leap::Vector pixel = image.warp( input );

                if ( (pixel.x >= 0 && pixel.x < image.width()) &&
                    (pixel.y >= 0 && pixel.y < image.height()) ) {

                    //xy to buffer index
                    int data_index = floor( pixel.y ) * image.width() + floor( pixel.x ); 
                    brightness[0] = image.data()[data_index]; //Look up brightness value
                    brightness[2] = brightness[1] = brightness[0]; //Greyscale
                }
                else {
                    // カメラ画像外は白で表示する
                    brightness[0] = brightness[2] = brightness[1] = 255;
                }
                tI.r() = brightness[0];
                tI.g() = brightness[1];
                tI.b() = brightness[2];
                tI.a() = brightness[3];
            }
        }
        gl::Texture targetTexture( targetImage, gl::Texture::Format() );
        gl::draw( targetTexture, Rectf( origin, 
                                origin + Vec2f( targetWidth, targetHeight ) ) );
    }

    // 歪み補正した画像に合わせて指を表示する
    for ( int i = 0; i < 2; i++ ){
        Leap::Image image = images[i];
        if ( !image.isValid() ){
            return;
        }

        // 表示開始位置
        origin = Vec2f( 0, IMAGE_HEIGHT * i );

        // 指を表示する
        Leap::FingerList frameFingers = frame.fingers();
        for ( Leap::FingerList::const_iterator fl = frameFingers.begin();
                fl != frameFingers.end(); fl++ ) {

            //Convert finger tip position to a ray from the camera POV
            Leap::Vector tip = (*fl).tipPosition();
            float horizontal_slope = -(tip.x + 20 * (2 * i - 1)) / tip.y;
            float vertical_slope = tip.z / tip.y;

            // Normalize ray from [-4..4] to [0..1] (the inverse of how the undistorted image was drawn earlier)
            Leap::Vector ray = Leap::Vector(
                horizontal_slope * image.rayScaleX() + image.rayOffsetX(),
                vertical_slope   * image.rayScaleY() + image.rayOffsetY(),
                0 );

            //Pixel coordinates from [0..1] to [0..width/height]
            Leap::Vector pixel = Leap::Vector( 
                ray.x * targetWidth, 
                ray.y * targetHeight, 
                0 );
            //gl::color( .5, 0, 1, .5 );
            gl::drawSolidCircle( Vec2f( pixel.x + origin.x, pixel.y + origin.y ), 5 );
        }
    }
#endif
}

CINDER_APP_NATIVE( LeapSample04App, RendererGl )
