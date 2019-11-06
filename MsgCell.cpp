//
//  MsgCell.cpp
//  MyCppGame
//
//  Created by 杜红 on 2017/1/23.
//
//

#include "MsgCell.hpp"
#include "VoiceAndTextScene.hpp"

#define MsgCell_MaxLineWidth 600

bool MsgCell::init(){
    TableViewCell::init();
    
    titleLabel = Label::create();
    titleLabel->setSystemFontSize( 40 );
    titleLabel->setAlignment( TextHAlignment::LEFT );
    titleLabel->setAnchorPoint(Vec2(0,0)) ;
    titleLabel->setTextColor( Color4B::BLACK );
    addChild( titleLabel, 1  );
    
    spVoiceTag = Sprite::create("chat_bottom_voice_press.png");
    spVoiceTag->setContentSize( Size(40, 40 ) );
    spVoiceTag->setAnchorPoint(Vec2(0,0));
    addChild( spVoiceTag, 1 );
    
    voiceBtn = Button::create();
    voiceBtn->setContentSize( Size(90, 40 ) );
    voiceBtn->setAnchorPoint( Vec2(0,0));
    voiceBtn->setTitleFontSize( 40 );
    voiceBtn->setTitleColor( Color3B::BLACK );
    voiceBtn->setPosition( Vec2(40,0) );
    addChild( voiceBtn, 1  );
    
    contentLabel = Label::create();
    contentLabel->setSystemFontSize( 40 );
    contentLabel->setAlignment( TextHAlignment::LEFT );
    contentLabel->setVerticalAlignment( TextVAlignment::CENTER );
    contentLabel->setAnchorPoint(Vec2(0,0)) ;
    contentLabel->setPosition( Vec2(40,20));
    contentLabel->setWidth( MsgCell_MaxLineWidth );
    contentLabel->setTextColor( Color4B::BLACK );
    addChild( contentLabel, 1  );

    spContentBg = Scale9Sprite::create(Rect(33, 15, 40, 50), "chatfrom_bg_normal.png");
    spContentBg->setAnchorPoint(Vec2(0,0));
    spContentBg->setPosition(Vec2(10, 0 ));
    addChild( spContentBg );
    
    return true;
}

#define BeginX 40

#include <iomanip>
//格式：hour:min
string getTimeStr( unsigned int  time  ){
    time_t tt = time;
    tm* t = localtime( &tt );
    stringstream ss;
    ss<<setw(2)<<setfill('0')<<t->tm_hour<<":"<<setw(2)<<setfill('0')<<t->tm_min<<"";

    return ss.str();
}

void MsgCell::setMsg( Msg msg ){
	string strTitle = getTimeStr(msg.createTime) + " " + XStringToUTF8(msg.sender);
    titleLabel->setString( strTitle );
	contentLabel->setString(XStringToUTF8(msg.content));
    
    float contentWidth = 0;
    float contentHeight = 0;
    
    //气泡要根据消息变化，此处计算消息内容的宽高（系统字体不支持最大行宽）
    //如果文本消息单行会超过宽度，要设置最大宽度，改成多行
    //如果文本太长，width设置为0,会有Texture错误
    if( msg.content.length() >= 50 ){
        contentLabel->setWidth( MsgCell_MaxLineWidth );
    }
    else{
        contentLabel->setWidth( 0 );
    }
    
	contentLabel->setString(XStringToUTF8(msg.content));
    contentWidth = contentLabel->getContentSize().width;
    contentHeight = contentLabel->getContentSize().height;
    if(contentWidth > MsgCell_MaxLineWidth ){
        contentLabel->setWidth( MsgCell_MaxLineWidth );
		contentLabel->setString(XStringToUTF8(msg.content));
        contentWidth = MsgCell_MaxLineWidth;
        contentHeight = contentLabel->getContentSize().height;
    }
    //如果文字内容太短，或者语音无法提取文本，也要保留足够长的气泡宽度
    else if (contentWidth < 100 ){
        contentWidth = 100;
    }
    
    if( msg.bTxt == true ){
        titleLabel->setPosition( BeginX, 20 + contentLabel->getContentSize().height + 20 );
        voiceBtn->setVisible( false );
        spVoiceTag->setVisible( false );
        
        spContentBg->setContentSize( Size( contentWidth + 50 , contentHeight + 40 ));
    }
    else{
        titleLabel->setPosition( BeginX, contentHeight  + 80 );
        voiceBtn->setPosition( Vec2(BeginX + 50 , 30 + contentHeight ));
        spVoiceTag->setPosition( Vec2( BeginX, 30 + contentHeight ));
        
        stringstream ss;
        ss<<msg.voiceLen<<"\"";
        voiceBtn->setTitleText( ss.str() );
        voiceBtn->setVisible( true );
        spVoiceTag->setVisible( true  );
        
        spContentBg->setContentSize( Size( contentWidth + 50, contentHeight + 20 + 40 + 30  ));
    }
}

float MsgCell::getCellHeight( Msg msg ){
    auto testLabel = Label::create();
    testLabel->setSystemFontSize( 40 );
    testLabel->setAlignment( TextHAlignment::LEFT );
    testLabel->setVerticalAlignment( TextVAlignment::CENTER );
    testLabel->setWidth( MsgCell_MaxLineWidth );

	testLabel->setString(XStringToLocal(msg.content));

    float height = testLabel->getContentSize().height;

    if( msg.bTxt == true ){
        return height + 110; //额外加上消息头，和上下和中间的间距
    }
    else{
        return height + 110 + 50 ;//额外加上消息头，语音行，和上下和中间的间距

    }
}

