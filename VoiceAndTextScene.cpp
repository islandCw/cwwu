//
//  VoiceAndTextScene.cpp
//  MyCppGame
//
//  Created by 杜红 on 2017/1/22.
//
//

#include "VoiceAndTextScene.hpp"
#include "SimpleAudioEngine.h"
#include <string>
#include <time.h>
#include "iconv.h"

USING_NS_CC;

#include "ui/CocosGUI.h"
#include "MsgCell.hpp"

using namespace cocos2d::ui;
using namespace cocos2d::extension;
using namespace std;

#include "AppDelegate.h"

extern AppDelegate* g_app;
extern string g_roomName;
extern string g_userID;




Scene* VoiceAndTextScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = VoiceAndTextScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool VoiceAndTextScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    scheduleUpdate();
    m_bNewMsg = false ;
    
    loadUI();
    
    addIMEventLister();
    
    return true;
}

void VoiceAndTextScene::addIMEventLister(){
    //处理发送语音消息成功的回调通知
    auto sendVoiceListener = EventListenerCustom::create("SendVoice", [=](EventCustom* event){
        Msg* msg = (Msg*)event->getUserData();
        m_queueMutex.lock();
        m_bNewMsg = true ;
        m_msgList.push_back( *msg );
        m_queueMutex.unlock();
        YouMe_Log("语音文本场景，接收到发送语音消息成功\n" );
        
        delete msg;
    });
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(sendVoiceListener, 1 );
    
    //处理接收语音消息通知
    auto voiceListener = EventListenerCustom::create("RecvVoice", [=](EventCustom* event){
        Msg* msg = (Msg*)event->getUserData();
        m_queueMutex.lock();
        m_bNewMsg = true ;
        m_msgList.push_back( *msg );
        m_queueMutex.unlock();
        YouMe_Log("语音文本场景，接收到语音消息\n" );
        
        delete msg;
    });
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(voiceListener, 1 );
    
    //处理接收文本消息通知
    auto textListener = EventListenerCustom::create("RecvText", [=](EventCustom* event){
        Msg* msg = (Msg*)event->getUserData();
        m_queueMutex.lock();
        m_bNewMsg = true ;
        m_msgList.push_back( *msg );
        m_queueMutex.unlock();
        YouMe_Log("语音文本场景，接收到文本消息\n" );
        delete msg;
    });
    
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(textListener, 1 );
}


void VoiceAndTextScene::updateMsgShow(){
    //insertCellIndex调用后不会刷新位置，暂用reloadData来刷新，不过正式版本不该怎么处理
    //tableView->insertCellAtIndex( m_msgList.size()-1 );
    tableView->reloadData();
    //滚动到最新消息
    tableView->setContentOffset(Point(0, 0));

}

void VoiceAndTextScene::playSound( XString strPath ){
    
   
    if( IMService::Inst()->IsPlaying() ){
        IMService::Inst()->StopPlayAudio();
    }
    
	IMService::Inst()->StartPlayAudio(strPath.c_str());
}

void VoiceAndTextScene::onBtnSendText(Ref * pSender ){
    string text = txtInput->getText();
    
    txtInput->setText("");
    
    //不发送空消息
    if( text.length() == 0 ){
        return ;
    }
    
    YIMErrorcode ymErrorcode;
    XUINT64 iRequestID=0;
    //YOUME
    /*!
     *  发送文本消息
     *  @param receiverID  私聊时，填用户ID；频道聊天时，填频道ID
     *  @param chatType 聊天类型，私聊用YIMChatType_PrivateChat，频道用YIMChatType_RoomChat
     *  @param text 消息内容字符串
     *  @param requestID unsigned long long 型指针，函数调用完成后会把消息ID写入requestID
     *  @return 错误码
     */
	ymErrorcode = IMService::Inst()->GetMessageManager()->SendTextMessage(UTF8TOXString2( g_roomName.c_str()).c_str(),
                                                                     YIMChatType::ChatType_RoomChat,
																	 UTF8TOXString2(text.c_str()).c_str(),
                                                                     &iRequestID);
    if(ymErrorcode != YIMErrorcode_Success)
    {
        YouMe_Log("在聊天频道发送文本消息失败，错误码：%d",  ymErrorcode);
    }
    else{
        YouMe_Log("在聊天频道发送文本消息成功!\n");
        //通知场景自己发送了消息
        EventCustom evtText("RecvText");
        Msg* msg = new Msg();
        msg->sender = UTF8TOXString2( g_userID.c_str());
        msg->bSelf = true;
        msg->createTime = time(NULL);
        msg->bTxt = true;
		msg->content = UTF8TOXString2(text.c_str());
        evtText.setUserData( msg );
        Director::getInstance()->getEventDispatcher()->dispatchEvent(&evtText);
    }
}


void VoiceAndTextScene::onBtnBack( Ref *pSender  ){
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("RecvVoice");
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("RecvText");
    Director::getInstance()->getEventDispatcher()->removeCustomEventListeners("SendVoice");
    
    //退出场景的时候把放的声音掐断
    if( IMService::Inst()->IsPlaying() ){
        IMService::Inst()->StopPlayAudio();
    }
    
    g_app->enterLoginScene();
}

void VoiceAndTextScene::update(float delta ){
    //检查是否有新消息，有就更新消息显示界面
    m_queueMutex.lock();
    if(m_bNewMsg != true ){
        m_queueMutex.unlock();
    }
    else{
        m_bNewMsg = false;
        updateMsgShow();
        
        m_queueMutex.unlock();
    }
}

//处理语音按钮，按下录音，放开发送，移出按钮区域取消发送
void VoiceAndTextScene::onTouchEvent(Ref* pSender, Widget::TouchEventType type ){
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
			ymErrorcode = IMService::Inst()->GetMessageManager()->SendAudioMessage(UTF8TOXString2(g_roomName.c_str()).c_str(),
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
            
            //提示发送录音
            labelTips->setVisible( true );
			labelTips->setString(UTEXT("发送录音"));
            auto delay = DelayTime::create( 1 );
            auto hide = Hide::create();
            auto action = Sequence::create( delay, hide , NULL );
            labelTips->runAction(action);
            
            spSpeaking->setVisible( false );
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
        
            //提示取消录音
            labelTips->setVisible( true );
			labelTips->setString(UTEXT("取消录音"));
            auto delay = DelayTime::create( 1 );
            auto hide = Hide::create();
            auto action = Sequence::create( delay, hide , NULL );
            labelTips->runAction(action);
        
            spSpeaking->setVisible( false );
        }
            break;
        default:
            break;
    }
}

cocos2d::extension::TableViewCell* VoiceAndTextScene::tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx){
    TableViewCell *cell=table->dequeueCell();
    auto msg = m_msgList[idx];
    
    if( cell == NULL ){
        cell = MsgCell::create();
    }

    auto msgCell = dynamic_cast<MsgCell*>(cell);
    msgCell->setMsg( msg );
    
    return msgCell;
}

ssize_t VoiceAndTextScene::numberOfCellsInTableView(cocos2d::extension::TableView *table){
    return m_msgList.size();
}

Size VoiceAndTextScene::tableCellSizeForIndex(cocos2d::extension::TableView *tabel,ssize_t idx){
    auto msg = m_msgList[idx];

    auto height =  MsgCell::getCellHeight( msg );
    
    return Size(800, height );
}

void VoiceAndTextScene::tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell){
    //如果点击了语音消息，就直接播放
    auto idx = cell->getIdx();
    auto msg = m_msgList[idx];
	XString str = msg.sender + UTF8TOXString2(":") + msg.content + UTF8TOXString2("\n");
    
    if( msg.bTxt == false ){
        playSound( msg.voicePath );
    }
}


void VoiceAndTextScene::loadUI(){
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    //退出登录按钮
    auto btnBack = Button::create();
    btnBack->setTitleFontSize( 80 );
    btnBack->setPosition(Vec2(visibleSize.width - 100 , visibleSize.height - 100 ));
	btnBack->setTitleText(UTEXT("退出\n登录"));
    btnBack->setTitleColor(Color3B::WHITE );
    btnBack->addClickEventListener( CC_CALLBACK_1( VoiceAndTextScene::onBtnBack, this  ) );
    addChild( btnBack);
    
    //说话按钮
    auto btnSpeake = Button::create("chat_bottom_voice_nor.png");
    btnSpeake->setPosition(Vec2( 40 , 40 ));
    btnSpeake->addTouchEventListener( CC_CALLBACK_2( VoiceAndTextScene::onTouchEvent,  this ));
    btnSpeake->setAnchorPoint( Vec2(0,0) );
    btnSpeake->setScale( 2 );
    addChild( btnSpeake );
    
    //消息输入
	txtInput = cocos2d::extension::EditBox::create(Size(500, 100), "chat_bottom_textfield.png");
    txtInput->setTextHorizontalAlignment( TextHAlignment::LEFT );
    txtInput->setPosition( Vec2( 150,  50 ) );
	txtInput->setInputMode(cocos2d::extension::EditBox::InputMode::SINGLE_LINE);
    txtInput->setAnchorPoint( Vec2(0,0) );
    txtInput->setFontColor( Color3B::BLACK );
    txtInput->setMaxLength( 200 );
    addChild( txtInput );
    
    //发送消息按钮
    auto btnSend = Button::create();
    btnSend->setTitleFontSize(80);
    btnSend->setPosition(Vec2( 650 , 50 ));
	btnSend->setTitleText(UTEXT("发送"));
    btnSend->setAnchorPoint( Vec2(0,0) );
    btnSend->setContentSize( Size(150, 80 ) );
    btnSend->addClickEventListener( CC_CALLBACK_1( VoiceAndTextScene::onBtnSendText, this ) );
    addChild( btnSend );
    
    
    //消息显示框背景
    auto sp = Sprite::create("chat_bg_default.jpg");
    sp->setPosition(Vec2(50,200));
    sp->setAnchorPoint( Vec2(0,0));
    sp->setContentSize( Size(700, 500 ));
    addChild( sp, 0 );
    
    //消息显示TabelView
    tableView = TableView::create( this, Size(700,500));
    //tableView->setColor(Color3B::GRAY );
    tableView->setDirection( cocos2d::extension::ScrollView::Direction::VERTICAL );
    tableView->setPosition(Vec2(50, 200 ));
    tableView->setDelegate( this );
    tableView->setVerticalFillOrder( cocos2d::extension::TableView::VerticalFillOrder::TOP_DOWN );
    addChild( tableView, 1  );
    tableView->reloadData();
    tableView->setAnchorPoint( Vec2(0,0) );
    
    spSpeaking = Sprite::create("chat_bottom_voice_nor.png");
    spSpeaking->setContentSize( Size(150,150) );
    spSpeaking->setPosition( Vec2( visibleSize.width/2, visibleSize.height/2 ));
    spSpeaking->setVisible( false );
    addChild( spSpeaking , 2 );
    
    //发送成功或者取消的时候的提示
	labelTips = Label::createWithSystemFont(UTEXT("取消语音"), "Arial", 50);
    labelTips->setTextColor( Color4B::BLACK );
    labelTips->setPosition(visibleSize.width/2, visibleSize.height/2 );
    labelTips->setAnchorPoint( Vec2(0,0)) ;
    labelTips->setVisible( false );
    addChild( labelTips, 2  );
}



