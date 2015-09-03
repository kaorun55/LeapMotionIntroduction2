using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using Leap;

namespace LeapSample04
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        Controller leap;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded( object sender, RoutedEventArgs e )
        {
            leap = new Controller();
            leap.SetPolicy( Controller.PolicyFlag.POLICY_IMAGES );

            CompositionTarget.Rendering += CompositionTarget_Rendering;
        }

        void CompositionTarget_Rendering( object sender, EventArgs e )
        {
            //RawImages();

            CalibrationImage();
        }

#region 位置補正した画像
        private void CalibrationImage()
        {
            var frame = leap.Frame();
            var images = frame.Images;
            var fongers = frame.Fingers;

            //Draw the undistorted image using the warp() function
            // Warp()が速度低下につながるため、解像度を落としている
            int targetWidth = 100;
            int targetHeight = 100;
            int scale = 400 / targetWidth;

            // グリッドのサイズを設定する
            GridLeft.Width = GridRight.Width = targetWidth * scale;
            GridLeft.Height = GridRight.Height = targetHeight * scale;

            Stopwatch sw = new Stopwatch();
            sw.Restart();

            // 左カメラ
            if ( images[0].IsValid ) {
                // カメラ画像を表示する
                ImageLeft.Source = ToCalibratedBitmap( targetWidth, targetHeight, images[0] );

                // 指の座標を表示する
                var leftPoints = MapCalibratedCameraToColor( images[0], fongers, targetWidth * scale, targetHeight * scale );
                DrawPoints( CanvasLeft, leftPoints );
            }

            // 右カメラ
            if ( images[1].IsValid ) {
                // カメラ画像を表示する
                ImageRight.Source = ToCalibratedBitmap( targetWidth, targetHeight, images[1] );

                // 指の座標を表示する
                var rightPoints = MapCalibratedCameraToColor( images[1], fongers, targetWidth * scale, targetHeight * scale );
                DrawPoints( CanvasRight, rightPoints );
            }

            Trace.WriteLine( sw.ElapsedMilliseconds );
        }

        private static BitmapSource ToCalibratedBitmap( int targetWidth, int targetHeight, Leap.Image image )
        {
            var buffer = new byte[targetWidth * targetHeight];

            //Iterate over target image pixels, converting xy to ray slope
            for ( int y = 0; y < targetHeight; y++ ) {
                for ( int x = 0; x < targetWidth; x++ ) {
                    //Normalize from pixel xy to range [0..1]
                    var input = new Leap.Vector( x / (float)targetWidth, y / (float)targetHeight, 0 );

                    //Convert from normalized [0..1] to slope [-4..4]
                    input.x = (input.x - image.RayOffsetX) / image.RayScaleX;
                    input.y = (input.y - image.RayOffsetY) / image.RayScaleY;

                    //Use slope to get coordinates of point in image.Data containing the brightness for this target pixel
                    var pixel = image.Warp( input );

                    int bufferIndex = (y * targetWidth) + x;

                    if ( pixel.x >= 0 && pixel.x < image.Width && pixel.y >= 0 && pixel.y < image.Height ) {
                        int dataIndex = (int)(Math.Floor( pixel.y ) * image.Width + Math.Floor( pixel.x )); //xy to buffer index
                        buffer[bufferIndex] = image.Data[dataIndex];
                    }
                    else {
                        buffer[bufferIndex] = 255;
                    }
                }
            }

            return BitmapSource.Create( targetWidth, targetHeight, 96, 96,
                    PixelFormats.Gray8, null, buffer, targetWidth );
        }

        private static Leap.Vector[] MapCalibratedCameraToColor( Leap.Image image, FingerList fingers,
                                                                    int targetWidth, int targetHeight )
        {
            var colorPoints =new List<Leap.Vector>();

            float cameraXOffset = 20; //millimeters

            foreach ( Finger finger in fingers ) {
                var tip = finger.TipPosition;
                float hSlope = -(tip.x + cameraXOffset * (2 * image.Id - 1)) / tip.y;
                float vSlope = tip.z / tip.y;

                var ray = new Leap.Vector( hSlope * image.RayScaleX + image.RayOffsetX,
                                     vSlope * image.RayScaleY + image.RayOffsetY, 0 );

                //Pixel coordinates from [0..1] to [0..width/height]
                colorPoints.Add( new Leap.Vector( ray.x * targetWidth, ray.y * targetHeight, 0 ) );
            }

            return colorPoints.ToArray();
        }
#endregion

        #region 生画像の取得と表示
        private void RawImages()
        {
            var frame = leap.Frame();
            var images = frame.Images;
            var fongers = frame.Fingers;

            // グリッドのサイズを設定する
            GridLeft.Width = GridRight.Width = images[0].Width;
            GridLeft.Height = GridRight.Height = images[0].Height;

            // 左カメラ
            if ( images[0].IsValid ) {
                // カラー画像を作成する
                ImageLeft.Source = ToBitmapSource( images[0] );

                // カメラ座標を取得する
                var leftPoints = MapCameraToColor( images[0], fongers );
                DrawPoints( CanvasLeft, leftPoints );
            }

            // 右カメラ
            if ( images[1].IsValid ) {
                // カラー画像を作成する
                ImageRight.Source = ToBitmapSource( images[1] );

                // カメラ座標を取得する
                var rightPoints = MapCameraToColor( images[1], fongers );
                DrawPoints( CanvasRight, rightPoints );
            }
        }

        /// <summary>
        /// カメラ画像を作成する
        /// </summary>
        /// <param name="image"></param>
        /// <returns></returns>
        private static BitmapSource ToBitmapSource( Leap.Image image )
        {
            return BitmapSource.Create( image.Width, image.Height, 96, 96,
                                PixelFormats.Gray8, null, image.Data, image.Width * image.BytesPerPixel );
        }

        /// <summary>
        /// 3次元の手の座標をカメラの2次元座標に変換する
        /// </summary>
        /// <param name="image"></param>
        /// <param name="fingers"></param>
        /// <returns></returns>
        private static Leap.Vector[] MapCameraToColor( Leap.Image image, FingerList fingers )
        {
            var colorPoints =new List<Leap.Vector>();

            float cameraOffset = 20; //x-axis offset in millimeters
            foreach ( Finger finger in fingers ) {
                // 3次元座標を2次元座標に変換する
                var tip = finger.TipPosition;
                float hSlope = -(tip.x + cameraOffset * (2 * image.Id - 1)) / tip.y;
                float vSlope = tip.z / tip.y;

                colorPoints.Add( image.Warp( new Leap.Vector( hSlope, vSlope, 0 ) ) );
            }

            return colorPoints.ToArray();
        }
        #endregion

        /// <summary>
        /// 点を描画する
        /// </summary>
        /// <param name="canvas"></param>
        /// <param name="leftPoints"></param>
        private static void DrawPoints( Canvas canvas, Leap.Vector[] leftPoints )
        {
            canvas.Children.Clear();
            foreach ( var point in leftPoints ) {
                // Canvasに表示する
                var ellipse = new Ellipse()
                {
                    Width = 10,
                    Height = 10,
                    Fill = Brushes.Red,
                };

                Canvas.SetLeft( ellipse, point.x );
                Canvas.SetTop( ellipse, point.y );

                canvas.Children.Add( ellipse );
            }
        }
    }
}
