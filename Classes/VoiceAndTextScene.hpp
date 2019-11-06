//  演示一个既能发语音，又能发文本的聊天室场景
//  收到的语音消息，需要点击才能听到
//  VoiceAndTextScene.hpp
//  MyCppGame
//
//  Created by 杜红 on 2017/1/22.
//
//

#ifndef VoiceAndTextScene_hpp
#define VoiceAndTextScene_hpp

#include <stdio.h>
#include <string>
#include <vector>
using namespace std;

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocos-ext.h"
using namespace cocos2d::ui;
USING_NS_CC;


#include "IMService.hpp"


//



class VoiceAndTextScene : public cocos2d::Layer, cocos2d::extension::TableViewDataSource , cocos2d::extension::TableViewDelegate
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    void loadUI();
    
    void addIMEventLister();

    
    //界面回调
    void onBtnBack( Ref *pSender  );
    void onTouchEvent(Ref* pSender, Widget::TouchEventType type );
    void onBtnSendText(Ref * pSender );
    
    void updateMsgShow();
    void update(float delta );
    
    void playSound( XString strPath );
    
    //TableViewDataSource
    virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table);
    virtual Size tableCellSizeForIndex(cocos2d::extension::TableView *tabe,ssize_t idx);
    
    //TableViewDelegate
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell);
   
    //property
public:
    cocos2d::extension::EditBox *  txtInput;
    
    //录音过程中显示
    Sprite*  spSpeaking;
    Label*  labelTips;

    //是否收到新消息
    bool m_bNewMsg;
    //IM与COCOS不在同一个线程,所以收到IM的回调时不能直接处理，需要注意多线程
    std::mutex m_queueMutex;
    
    //聊天室里需要显示的消息列表
    vector<Msg> m_msgList;
    
    //用于显示消息列表
    cocos2d::extension::TableView* tableView;
    
    int m_nCurSoundID;
    
    CREATE_FUNC(VoiceAndTextScene);
};

#endif /* VoiceAndTextScene_hpp */
