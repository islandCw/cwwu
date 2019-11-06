//  演示纯语音聊天场景
//  收到的语音消息，立刻自动播放
//  VoiceScene.hpp
//  MyCppGame
//
//  Created by 杜红 on 2017/1/22.
//
//

#ifndef VoiceScene_hpp
#define VoiceScene_hpp

#include <stdio.h>
#include <string>
using namespace std;

#include "cocos2d.h"
#include "ui/CocosGUI.h"
using namespace cocos2d::ui;
USING_NS_CC;

#include "IMService.hpp"

#include <vector>



class VoiceScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    
    
    virtual bool init();
    
    void loadUI();
    
    void addIMEventLister();
    
    //界面响应
    void onBtnBack( Ref *pSender  );
    void onTouchEvent(Ref* pSender, Widget::TouchEventType type );

    //收到的声音要按序播放，所以要处理播放结束问题
    void playSound( string strPath);
    void onPlaySoundFinished();
    
    //IM与COCOs不在同一线程，收到回调不能直接处理，注意多线程
    void update(float delta );
    std::mutex m_queueMutex;

    vector<string> m_voices;
    bool m_bPlaying;
    
    Sprite*  spSpeaking;
    Label*  labelTips;
    
    
    CREATE_FUNC(VoiceScene);
};

#endif /* VoiceScene_hpp */
