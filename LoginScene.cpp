//
//  LoginScene.cpp
//  MyCppGame
//
//  Created by 杜红 on 2017/1/22.
//
//

#include "LoginScene.hpp"
#include "ui/CocosGUI.h"
#include "cocos-ext.h"
#include "iconv.h"


USING_NS_CC;
using namespace cocos2d::ui;
using namespace cocos2d::extension;
#include <string>
using namespace std;

#include "AppDelegate.h"
extern AppDelegate* g_app;

extern string g_roomName;
extern string g_userID;


Scene* LoginScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = LoginScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool LoginScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
    loadUI();
    
    return true;
}


void LoginScene::loginChatRoom(){
    g_userID = idInput->getText();
    g_roomName = roomInput->getText();
    
    printf("user:%s, room:%s\n", g_userID.c_str(), g_roomName.c_str() );
    


	


    //YOUME
    /*!
     *  玩家登录
     *
     *  @param userID   用户ID
     *  @param password 用户密码，不可为空字符串，一般固定为"123456"即可
     *  @param token  使用服务器token验证模式时使用该参数，否则使用空字符串
     *  @return 错误码
     */
    //如果要进入聊天频道，需要等Login的回调OnLogin成功之后
	YIMErrorcode ymErrorcode = IMService::Inst()->Login(UTF8TOXString2(g_userID.c_str()).c_str(), UTF8TOXString2("123456").c_str(), UTF8TOXString2("").c_str());
    if(YIMErrorcode_Success == ymErrorcode)
    {
		//char chInput[512];
		//sprintf(chInput, "11111:%d\n", "你好啊");
		//OutputDebugString((LPCWSTR)&chInput);
		
#ifdef WIN32
		OutputDebugStringW(L"登录成功");
#endif
		printf("登录回调");
        YouMe_Log("调用登录成功\n");
    }
    else
    {
		printf("登录回调");
        YouMe_Log("调用登录失败，错误码：%d\n",  ymErrorcode);
    }
}


void LoginScene::onBtnVoiceScene(Ref *pSender ){
    
    loginChatRoom();
    
    g_app->enterVoiceScene();
}

void LoginScene::onBtnVoiceAndTextScene(Ref * pSender ){
    loginChatRoom();
    
    g_app->enterVoiceAndTextScene();

}

void LoginScene::loadUI(){
    const float labelX = 50;
    auto visibleSize = Director::getInstance()->getVisibleSize();

    
    //label
	auto labelID = Label::createWithSystemFont(UTEXT("输入用户ID"), "Arial", 80);
    labelID->setPosition(labelX, 600);
    labelID->setAnchorPoint( Vec2(0,0)) ;
    addChild( labelID );
    
	auto labelRoom = Label::createWithSystemFont(UTEXT("输入房间名"), "Arial", 80);
    labelRoom->setPosition(labelX, 450 );
    labelRoom->setAnchorPoint( Vec2(0,0)) ;
    addChild( labelRoom );
    
    //input
    idInput = cocos2d::extension::EditBox::create( Size( 400, 100 ), "chat_bottom_textfield.png");
    idInput->setTextHorizontalAlignment( TextHAlignment::LEFT );
    idInput->setInputMode( cocos2d::extension::EditBox::InputMode::SINGLE_LINE );
    idInput->setAnchorPoint( Vec2(0,0) );
    idInput->setFontColor( Color3B::BLACK );
    idInput->setPosition( Vec2( labelX + 450,  600 ) );
    idInput->setText( g_userID.c_str() );
    addChild( idInput );
    
    roomInput = cocos2d::extension::EditBox::create( Size( 400, 100 ), "chat_bottom_textfield.png");
    roomInput->setTextHorizontalAlignment( TextHAlignment::LEFT );
    roomInput->setInputMode( cocos2d::extension::EditBox::InputMode::SINGLE_LINE );
    roomInput->setAnchorPoint( Vec2(0,0) );
    roomInput->setFontColor( Color3B::BLACK );
    roomInput->setPosition( Vec2( labelX + 450,  450 ) );
    roomInput->setText( g_roomName.c_str());
    addChild( roomInput );
    
    //button
    auto btnVoice = Button::create();
    btnVoice->setTitleFontSize( 80 );
    btnVoice->setPosition(Vec2(visibleSize.width/2,200 ));
    btnVoice->setAnchorPoint( Vec2(0.5, 0) );
	btnVoice->setTitleText(UTEXT("进入纯语音房间"));
    btnVoice->addClickEventListener( CC_CALLBACK_1( LoginScene::onBtnVoiceScene, this  ) );
    addChild( btnVoice);
    
    auto btnVoiceTxt = Button::create();
    btnVoiceTxt->setTitleFontSize( 80 );
	btnVoiceTxt->setTitleText(UTEXT("进入语音文本房间"));
    btnVoiceTxt->setPosition( Vec2(visibleSize.width/2, 80 ) );
    btnVoiceTxt->setAnchorPoint( Vec2(0.5, 0) );
    btnVoiceTxt->addClickEventListener( CC_CALLBACK_1( LoginScene::onBtnVoiceAndTextScene, this  ) );
    addChild( btnVoiceTxt );
    
    
}





