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
using NaturalSoftware.Leap.Toolkit;

namespace LeapSample01
{
    /// <summary>
    /// MainWindow.xaml の相互作用ロジック
    /// </summary>
    public partial class MainWindow : Window
    {
        Controller leap;
        LeapListener listener;

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded( object sender, RoutedEventArgs e )
        {
            leap = new Controller();

#if false
            // ポーリング
            CompositionTarget.Rendering += CompositionTarget_Rendering;
#else
            // イベント

            // リスナーオブジェクトとFrameイベントを登録する
            listener = new LeapListener();
            listener.OnFrameEvent += listener_OnFrameEvent;

            // リスナーオブジェクトを登録する
            leap.AddListener(listener);
#endif
        }

        void CompositionTarget_Rendering( object sender, EventArgs e )
        {
            // フレームの処理を行う
            var frame = leap.Frame();

            TextLeap.Text = "Polling Frame id: " + frame.Id
                    + ", timestamp: " + frame.Timestamp
                    + ", hands: " + frame.Hands.Count
                    + ", fingers: " + frame.Fingers.Count
                    + ", tools: " + frame.Tools.Count
                    + ", gestures: " + frame.Gestures().Count;
        }

        void listener_OnFrameEvent( Controller leap )
        {
            // ここではWPFのUIオブジェクトも触ることができる
            // フレームの処理を行う
            var frame = leap.Frame();

            TextLeap.Text = "Event Frame id: " + frame.Id
                    + ", timestamp: " + frame.Timestamp
                    + ", hands: " + frame.Hands.Count
                    + ", fingers: " + frame.Fingers.Count
                    + ", tools: " + frame.Tools.Count
                    + ", gestures: " + frame.Gestures().Count;
        }

    }
}
