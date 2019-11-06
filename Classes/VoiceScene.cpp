//
//  VoiceScene.cpp
//  MyCppGame
//
//  Created by 杜红 on 2017/1/22.
//
//

#include "VoiceScene.hpp"
USING_NS_CC;

#include "ui/CocosGUI.h"
using namespace cocos2d::ui;

#include"AudioEngine.h"
using namespace cocos2d::experimental;

#include <string>
using namespace std;

#include "AppDelegate.h"
extern AppDelegate* g_app;

extern string g_roomName;

#include "IMService.hpp"

#include "iconv.h"



Scene* VoiceScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = VoiceScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool VoiceScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    scheduleUpdate();
    loadUI();
    
    addIMEventLister();
    
    return true;
}

void VoiceScene::addIMEventLister(){
    //YOUME  收到消息的回调处理
    auto sendVoiceListener = EventListenerCustom::create("SendVoice", [=](EventCustom* event){
        YouMe_Log("语音场景，接收到发送语音成功的消息\n" );
        Msg* msg = (Msg*)event->getUserData();
        m_queueMutex.lock();
		m_voices.push_back(XStringToLocal(msg->voicePath));
        m_queueMutex.unlock();
        
        delete msg;
    });
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(sendVoiceListener, 1 );
    
    
    //处理接收消息通知
    auto voiceListener = EventListenerCustom::create("RecvVoice", [=](EventCustom* event){
        YouMe_Log("语音场景，接收到语音消息\n" );
        //要先下载语音才能开始播放
        Msg* msg = (Msg*)event->getUserData();
        delete msg;
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(voiceListener, 1 );
    
    //处理语音下载结束的回调
    auto downloadListener = EventListenerCustom::create("DownloadOK", [=](EventCustom* event){
        YouMe_Log("语音场景，语音文件下载完成\n" );
        //这里只能得到messageID和本地路径，其他信息如果需要，要在收到消息时存下来
        Msg* msg = (Msg*)event->getUserData();
        //下载完语音文件就可以播放了
        m_queueMutex.lock();
		m_voices.push_back(XStringToLocal(msg->voicePath));
        m_queueMutex.unlock();
        
        delete msg;
        
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(downloadListener, 1 );
    
    
    //YOUME  播放结束的回调处理
    auto playEndListener = EventListenerCustom::create("PlayEnd", [=](EventCustom* event){
        YouMe_Log("语音场景，播放结束\n" );
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(playEndListener, 1 );


}


//处理语音按钮，按下录音，放开发送，移出按钮区域取消发送
void VoiceScene::onTouchEvent(Ref* pSender, Widget::TouchEventType type ){
    YIMErrorcode ymErrorcode;
    switch (type ) {
        case Widget::TouchEventType::BEGAN:
        {
            //开始录音
            XUINT64 iRequestID=0;
            //YOUME
            /*
             *  启动录音（语音转文字）
             *  @param receiverID  私聊时，填用户ID；频道聊天时，填频道ID
             *  @param chatType 聊天类型，私聊用YIMChatType_PrivateChat，频道用YIMChatType_RoomChat
             *  @param requestID unsigned long long 型指针，函数调用完成后会把消息ID写入requestID
             *  @return 错误码
             */
            ymErrorcode = IMService::Inst()->GetMessageManager()->SendAudioMessage(UTF8TOXString2( g_roomName.c_str()).c_str(),
                                                                              YIMChatType::ChatType_RoomChat,
                                                                              &iRequestID);
            
            if(ymErrorcode != YIMErrorcode_Success)
            {
                YouMe_Log("启动录音失败，错误码：%d\n",  ymErrorcode);
            }
            else{
                YouMe_Log("启动录音成功!\n");
            }
            
            spSpeaking->setVisible( true );
            
        }
            break;
        case Widget::TouchEventType::ENDED:
        {
            //YOUME
            /*!
             *  停止并发送语音
             *  @param extraParam  透传消息字符串，可以用来附加一些用户的其他属性，格式自定义
             *  @return 错误码
             */
			YIMErrorcode ymErrorcode = IMService::Inst()->GetMessageManager()->StopAudioMessage(UTF8TOXString2("").c_str());
            if(ymErrorcode != YIMErrorcode_Success)
            {
                YouMe_Log("停止并发送录音失败，错误码：%d\n",  ymErrorcode);
            }
            else{
                YouMe_Log("停止并发送录音成功!\n");
            }

            spSpeaking->setVisible( false );
            
            //提示发送录音
            labelTips->setVisible( true );
			labelTips->setString(UTEXT("发送录音"));
            auto delay = DelayTime::create( 1 );
            auto hide = Hide::create();
            auto action = Sequence::create( delay, hide , NULL );
            labelTips->runAction(action);

        }
            break;
        case Widget::TouchEventType::CANCELED:
        {
            //YOUME
            /*!
             *  取消录音
             *  @return 错误码
             */
            ymErrorcode = IMService::Inst()->GetMessageManager()->CancleAudioMessage();
            if(ymErrorcode != YIMErrorcode_Success)
            {
                YouMe_Log("取消录音失败，错误码：%d\n",  ymErrorcode);
            }
            else{
                YouMe_Log("取消录音成功!\n");
            }

            spSpeaking->setVisible( false );
        
            //提示取消录音
            labelTips->setVisible( true );
			labelTips->setString(UTEXT("取消录音"));
            auto delay = DelayTime::create( 1 );
            auto hide = Hide::create();
            auto action = Sequence::create( delay, hide , NULL );
            labelTips->runAction(action);
        }
            break;
        default:
            break;
    }
}

void VoiceScene::onBtnBack( Ref *pSender  ){
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("RecvVoice");
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("DownloadOK");
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("SendVoice");
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("PlayEnd");
    
    if( IMService::Inst()->IsPlaying() )
    {
        IMService::Inst()->StopPlayAudio();
    }
    
    g_app->enterLoginScene();
}

void VoiceScene::playSound( string strPath){
	IMService::Inst()->StartPlayAudio(UTF8TOXString2(strPath.c_str()).c_str());
}

void VoiceScene::update(float delta ){
    m_queueMutex.lock();
    
    if( !m_voices.empty() && !IMService::Inst()->IsPlaying() ){
        playSound( m_voices[0] );
        m_voices.erase( m_voices.begin() );
    }
  
    m_queueMutex.unlock();
}

void VoiceScene::loadUI(){
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto btnBack = Button::create();
    btnBack->setTitleFontSize( 80 );
    btnBack->setPosition(Vec2(visibleSize.width - 150 , visibleSize.height - 100 ));
	btnBack->setTitleText(UTEXT("退出\n登录"));
    btnBack->setTitleColor(Color3B::WHITE );
    addChild( btnBack);
    btnBack->addClickEventListener( CC_CALLBACK_1( VoiceScene::onBtnBack, this  ) );
    
    
    auto btnSpeake = Button::create();
    btnSpeake->setTitleFontSize(120);
    btnSpeake->setPosition(Vec2(visibleSize.width /2 ,  100  ));
	btnSpeake->setTitleText(UTEXT("按住说话"));
    addChild( btnSpeake );
    
    btnSpeake->addTouchEventListener( CC_CALLBACK_2( VoiceScene::onTouchEvent,  this ));
    
    spSpeaking = Sprite::create("chat_bottom_voice_nor.png");
    spSpeaking->setContentSize( Size(150,150) );
    spSpeaking->setPosition( Vec2( visibleSize.width/2, visibleSize.height/2 ));
    spSpeaking->setVisible( false );
    addChild( spSpeaking , 2 );
    
    //发送成功或者取消的时候的提示
	labelTips = Label::createWithSystemFont(UTEXT("取消语音"), "Arial", 50);
    labelTips->setTextColor( Color4B::WHITE );
    labelTips->setPosition(visibleSize.width/2, visibleSize.height/2 );
    labelTips->setAnchorPoint( Vec2(0,0)) ;
    labelTips->setVisible( false );
    addChild( labelTips );
}
