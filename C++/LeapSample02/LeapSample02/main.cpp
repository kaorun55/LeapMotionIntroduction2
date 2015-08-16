#include <iostream>
#include <Leap.h>

class SampleListener : public Leap::Listener
{
    int handId;

public:

    SampleListener()
    {
        handId = -1;
    }

    void onFrame(const Leap::Controller& leap)
    {
        auto frame = leap.frame();

#if 0
    // 今回のフレームで検出したすべての手、指、ツール
    Leap::HandList hands = frame.hands();
    Leap::FingerList fingers = frame.fingers();
    Leap::ToolList tools = frame.tools();
    Leap::PointableList pointables = frame.pointables();

    std::cout << "Frame Data : "
        << " Hands : " << hands.count()
        << " Fingers : " << fingers.count()
        << " Extended Fingers : " << fingers.extended().count()
        << " Tools : " << tools.count()
        << " Pointers : " << pointables.count()
        << std::endl;
#endif

#if 0
        // 手のIDから、同じ手を追跡し続ける
        if ( handId == -1 ) {
            handId = frame.hands()[0].id();
        }
        else {
            Leap::Hand hand = frame.hand(handId);
            handId = hand.id();

            // 手の情報を表示する
            std::cout << "ID : " << hand.id()
                << " 位置 : " << hand.palmPosition()
                << " 速度 : " << hand.palmVelocity()
                << " 法線 : " << hand.palmNormal()
                << " 向き : " << hand.direction()
                << std::endl;
        }
#endif

#if 0
        // 一番左、右、手前の手を取得する
        Leap::HandList hands = frame.hands();
        Leap::Hand leftMost = hands.leftmost();
        Leap::Hand rightMost = hands.leftmost();
        Leap::Hand frontMost = hands.frontmost();

        std::cout << " 左 : " << leftMost.palmPosition()
            << " 右 : " << rightMost.palmPosition()
            << " 手前 : " << frontMost.palmPosition()
            << std::endl;
#endif

#if 0
        // 手の情報取得する
        Leap::Hand hand = frame.hands()[0];

        std::cout << " 右手 : " << hand.isRight()
                  << " ピンチ : " << hand.pinchStrength()
                  << " グラブ : " << hand.grabStrength()
                  << " 信頼性 : " << hand.confidence()
                  << std::endl;
#endif

#if 0
        // 手に属している指とツールを取得する
        for ( auto hand : frame.hands() ) {
            std::cout << "ID : " << hand.id()
                      << " Fingers : " << hand.fingers().count()
                      << " Extended Fingers : " << hand.fingers().extended().count()
                      << " Tools : " << hand.tools().count()
                      << " Pointers : " << hand.pointables().count()
                      << std::endl;
        }
#endif

#if 0
        // 指の情報を取得する
        for ( auto finger : frame.fingers().extended() ) {
            std::cout << "ID : " << finger.id()
                      << " 種類 : " << finger.type()
                      << " 位置 : " << finger.tipPosition()
                      << " 速度 : " << finger.tipVelocity()
                      << " 向き : " << finger.direction()
                      << std::endl;
        }
#endif

#if 1
        // 指の関節情報を取得する
        for ( auto finger : frame.fingers() ) {
            // 末節骨(指先の骨)
            auto bone = finger.bone(Leap::Bone::TYPE_DISTAL);
            std::cout << "種類 : " << bone.type()
                      << " 中心 : " << bone.center()
                      << " 上端 : " << bone.prevJoint()
                      << " 下端 : " << bone.nextJoint()
                      << std::endl;
        }
#endif

#if 1
        // 親指の定義を確認する
        for ( auto finger : frame.fingers() ) {
            if ( finger.type() == Leap::Finger::Type::TYPE_THUMB ){
                for ( int t = Leap::Bone::TYPE_METACARPAL;  t <= (int)Leap::Bone::TYPE_DISTAL; ++t ){
                    auto bone = finger.bone( (Leap::Bone::Type)t );
                    std::cout << "種類 : " << bone.type()
                              << " 長さ : " << bone.length() << std::endl;
                }
            }
        }
#endif
    }
};

int main(int argc, const char * argv[])
{
    SampleListener listener;
    Leap::Controller leap(listener);

    std::cout << "Press Enter to quit..." << std::endl;
    std::cin.get();

    return 0;
}
