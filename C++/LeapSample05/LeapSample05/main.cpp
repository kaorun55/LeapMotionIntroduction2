#include <iostream>
#include <Leap.h>

#define SAMPLE_NO 1

// ポーリング処理の例
#if SAMPLE_NO == 1
void main()
{
    Leap::Controller leap;
    int64_t previousFrameId = -1;

    // 初期化処理（oninit()相当）をここに書く

    // 無限ループ内で、前回のフレームのIDと比較して新しいフレームを取得する
    while ( 1 ) {
        auto frame = leap.frame();
        if ( previousFrameId == frame.id() ) {
            continue;
        }

        previousFrameId = frame.id();

        // フレーム更新処理（onFrame()相当）をここに書く
    }

    // 終了処理（onExit()相当）をここに書く
}
#endif


// ポーリング時の接続状態およびフォーカス状態を取得する
#if SAMPLE_NO == 2
void main()
{
    bool isPrevConnected = false;
    bool isPrevServiceConnected = false;
    bool hadPrevFocus = false;
    int64_t previousFrameId = -1;

    Leap::Controller leap;

    // 初期化処理(oninit()相当)

    while ( 1 ) {
        auto frame = leap.frame();

        // Leap Motionコントローラーとの接続状態を確認する
        {
            bool isCurrentConnected = leap.isConnected();
            if ( isPrevConnected != isCurrentConnected ) {
                if ( isCurrentConnected ) {
                    // Leap Motion コントローラーが接続された(onConnected()相当)
                    std::cout << "Leap Motion Controller connected." << std::endl;
                }
                else {
                    // Leap Motion コントローラーが抜かれた(onDisconnected()相当)
                    std::cout << "Leap Motion Controller disconnected." << std::endl;
                }
            }

            isPrevConnected = isCurrentConnected;
        }

        // Leap Motionサービスとの接続状態を確認する
        {
            bool isCurrentServiceConnected = leap.isServiceConnected();
            if ( isPrevServiceConnected != isCurrentServiceConnected ) {
                if ( isCurrentServiceConnected ) {
                    // Leap Motionサービスが接続された(onServiceConnect()相当)
                    std::cout << "Leap Motion Service connected." << std::endl;
                }
                else {
                    // Leap Motionサービスが切断された(onServiceDisconnect()相当)
                    std::cout << "Leap Motion Service disconnected." << std::endl;
                }
            }

            isPrevServiceConnected = isCurrentServiceConnected;
        }

        // フォーカス状態を確認する
        {
            bool hadCurrentFocus = leap.hasFocus();
            if ( hadPrevFocus != hadCurrentFocus ) {
                if ( hadCurrentFocus ) {
                    // アプリケーションのフォーカスが有効になった(onFocusGained()相当)
                    std::cout << "Focus gained." << std::endl;
                }
                else {
                    // アプリケーションのフォーカスが無効になった(onFocusLost()相当)
                    std::cout << "Focus lost." << std::endl;
                }
            }

            hadPrevFocus = hadCurrentFocus;
        }

        // フレームが更新されていなければ何もしない
        {
            if ( previousFrameId == frame.id() ) {
                continue;
            }

            previousFrameId = frame.id();
        }

        // フレーム更新処理(onFrame()相当)
    }

    // 終了処理(onExit()相当)
}
#endif

// 直前のフレームを取得する
#if SAMPLE_NO == 3
void main()
{
    Leap::Controller leap;
    int64_t previousFrameId = -1;

    while ( 1 ) {
        // 最新のフレームを取得する (leap.frame( 0 ) と同じ)
        auto currentFrame = leap.frame();
        if ( previousFrameId == currentFrame.id() ) {
            continue;
        }

        previousFrameId = currentFrame.id();

        // 直前の5フレームを取得する
        std::cout << currentFrame.id() << ", ";
        for ( int i = 1; i <= 5; ++i ) {
            auto previousFrame = leap.frame(i);
            std::cout << previousFrame.id() << ", ";
        }
        std::cout << std::endl;
    }

    // 終了処理(onExit()相当)
}
#endif


#if SAMPLE_NO == 4
void main()
{
    Leap::Controller leap;

    int64_t lastFrame = 0;

    // 初期化処理(oninit()相当)

    while ( 1 ) {
        auto frame = leap.frame();
        if ( !leap.isConnected() ) {
            std::cout << "leap is not connected." << std::endl;
            continue;
        }

        bool focus = leap.hasFocus();
        if ( !focus ) {
            std::cout << "application is not focus." << std::endl;
            continue;
        }

        auto currentFrameid = frame.id();
        if ( currentFrameid == lastFrame ) {
            //std::cout << "processed frame id." << std::endl;
            continue;
        }

        // 前回と今回のフレーム差分
        //std::cout << "frame count : " << (currentFrameid - lastFrame) << std::endl;

        lastFrame = currentFrameid;

        // 指の位置の平均値を計算する
        int count = 0;
        Leap::Vector average = Leap::Vector();
        Leap::Finger fingerToAverage = frame.fingers()[0];
        for ( int i = 0; i < 10; i++ ) {
            // フレーム履歴と指のIDで、過去の指の位置を取得する
            Leap::Finger fingerFromFrame = leap.frame(i).finger(fingerToAverage.id());
            if ( fingerFromFrame.isValid() ) {
                average += fingerFromFrame.tipPosition();
                count++;
            }
        }

        if ( count > 0 ) {
            average /= count;
            std::cout << "finger point : " << average << std::endl;
        }
    }

    // 終了処理(onExit()相当)
}
#endif

#if SAMPLE_NO == 5
// ポーリングでフレームを取得する
void GettingFramesByPolling()
{
    Leap::Controller leap;

    int64_t lastFrame = 0;
    while ( 1 ) {
        auto frame = leap.frame();
        if ( !leap.isConnected() ) {
            std::cout << "leap is not connected." << std::endl;
            continue;
        }

        bool focus = leap.hasFocus();
        if ( !focus ) {
            std::cout << "application is not focus." << std::endl;
            continue;
        }

        auto currentFrameid = frame.id();
        if ( currentFrameid == lastFrame ) {
            //std::cout << "processed frame id." << std::endl;
            continue;
        }

        //std::cout << "frame count : " << (currentFrameid - lastFrame) << std::endl;

        lastFrame = currentFrameid;

        // 指の位置の平均値を計算する
        int count = 0;
        Leap::Vector average = Leap::Vector();
        Leap::Finger fingerToAverage = frame.fingers()[0];
        for ( int i = 0; i < 10; i++ ) {
            // フレーム履歴と指のIDで、過去の指の位置を取得する
            Leap::Finger fingerFromFrame = leap.frame(i).finger(fingerToAverage.id());
            if ( fingerFromFrame.isValid() ) {
                average += fingerFromFrame.tipPosition();
                count++;
            }
        }
        average /= count;
        std::cout << "finger point : " << average << std::endl;
    }
}

// コールバック(イベント駆動)でイベントを取得する
class SampleListrener : public Leap::Listener
{
public:

    virtual void onFocusGained(const Leap::Controller&) {
        std::cout << __FUNCTION__ << std::endl;
    }

    virtual void onFocusLost(const Leap::Controller&) {
        std::cout << __FUNCTION__ << std::endl;
    }

};

void GettingFramesWithCallbacks()
{
    SampleListrener listner;
    Leap::Controller leap;
    leap.addListener(listner);

    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    leap.removeListener(listner);
}

// フレームの取り方のサンプル
int main()
{
#if 0
    // ポーリング
    GettingFramesByPolling();
#else
    // コールバック
    GettingFramesWithCallbacks();
#endif

    return 0;
}
#endif

