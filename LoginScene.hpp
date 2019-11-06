//  输入用户ID和房间
//  可以选择进入纯语音场景，或者语音和文本场景
//  LoginScene.hpp
//  MyCppGame
//
//  Created by 杜红 on 2017/1/22.
//
//

#ifndef LoginScene_hpp
#define LoginScene_hpp

#include <stdio.h>
#include "cocos2d.h"

#include "ui/CocosGUI.h"
using namespace cocos2d::ui;

#include "cocos-ext.h"
using namespace cocos2d::extension;

class EditBox;


class LoginScene : public cocos2d::Layer
{
public:
    static cocos2d::Scene* createScene();
    
    virtual bool init();
    
    void loadUI();
    
    //
    void onBtnVoiceScene(Ref *pSender );
    void onBtnVoiceAndTextScene(Ref * pSender );
    
    //进入聊天频道
    void loginChatRoom();

    CREATE_FUNC(LoginScene);
    
    //
    cocos2d::extension::EditBox *  idInput;
    cocos2d::extension::EditBox *  roomInput;
};

#endif /* LoginScene_hpp */
