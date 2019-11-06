//  聊天窗口中，一条消息的显示
//  包括消息头部，语音消息，文字消息， 共3部分
//  消息头部：消息发送的时间，发送人
//  语音消息：如果是语音消息，才会有这部分。包括1个语音标志图片和语音时长信息
//  文字消息：如果是文本消息，则是消息内容，如果是语音消息，则是从语音中提取的文本
//  MsgCell.hpp
//  MyCppGame
//
//  Created by 杜红 on 2017/1/23.
//
//

#ifndef MsgCell_hpp
#define MsgCell_hpp

#include <stdio.h>
#include "cocos-ext.h"
#include "cocos2d.h"
#include "ui/CocosGUI.h"
using namespace cocos2d::ui;
using namespace cocos2d;

class Msg;

class MsgCell : public cocos2d::extension::TableViewCell
{
public:
    virtual bool init();
    
    void setMsg( Msg msg );
   
    //根据消息内容的不同，消息显示占用的高度不同
    static float getCellHeight( Msg msg );
    
public:
    //消息头部
    Label*  titleLabel = NULL;
    //消息文本
    Label*  contentLabel = NULL;
    
    //语音标志
    Button*  voiceBtn = NULL;
    
    Sprite*  spVoiceTag = NULL;
    
    //气泡
    Scale9Sprite*  spContentBg = NULL;
    
    
    
    
    CREATE_FUNC(MsgCell);
};

#endif /* MsgCell_hpp */
