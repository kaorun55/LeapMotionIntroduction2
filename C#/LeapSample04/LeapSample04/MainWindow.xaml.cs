using System;
using System.Collections.Generic;
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

            {
                var frame = leap.Frame();
                var images = frame.Images;
                var fongers = frame.Fingers;

                //Draw the undistorted image using the warp() function
                int targetWidth = 400;
                int targetHeight = 400;

                GridLeft.Width = GridRight.Width = targetWidth;
                GridLeft.Height = GridRight.Height = targetHeight;

                var image = images[0];

                WriteableBitmap writeableBmp = BitmapFactory.New( targetWidth, targetHeight );

                //Iterate over target image pixels, converting xy to ray slope
                for ( float y = 0; y < targetHeight; y++ ) {
                    for ( float x = 0; x < targetWidth; x++ ) {
                        //Normalize from pixel xy to range [0..1]
                        var input = new Leap.Vector( x / targetWidth, y / targetHeight, 0 );

                        //Convert from normalized [0..1] to slope [-4..4]
                        input.x = (input.x - image.RayOffsetX) / image.RayScaleX;
                        input.y = (input.y - image.RayOffsetY) / image.RayScaleY;

                        //Use slope to get coordinates of point in image.Data containing the brightness for this target pixel
                        var pixel = image.Warp( Leap.Vector.Zero);

                        if ( pixel.x >= 0 && pixel.x < image.Width && pixel.y >= 0 && pixel.y < image.Height ) {
                            int dataIndex = (int)(Math.Floor( pixel.y ) * image.Width + Math.Floor( pixel.x )); //xy to buffer index
                            byte brightness = image.Data[dataIndex];
                            writeableBmp.SetPixel( (int)x, (int)y, Color.FromArgb( 255, brightness, brightness, brightness ) );
                        }
                        else {
                            writeableBmp.SetPixel( (int)x, (int)y, Colors.Red );
                        }
                    }
                }
            }
        }

        private void RawImages()
        {
            var frame = leap.Frame();
            var images = frame.Images;
            var fongers = frame.Fingers;

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
