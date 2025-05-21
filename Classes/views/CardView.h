#pragma once
#include "2d/CCNode.h"
#include "2d/CCSprite.h"
#include <string>
#include <functional>

/**
 * CardView
 * 单张卡牌UI，负责显示牌面和花色，处理点击
 */




class CardView : public cocos2d::Node {
public:
    static CardView* create(int cardFace, int cardSuit, bool isFaceUp = true);
    bool init(int cardFace, int cardSuit, bool isFaceUp);
    void setCardFace(int cardFace);
    void setCardSuit(int cardSuit);
    void setCardId(int cardId);
    int getCardId() const;
    void setOnClickCallback(const std::function<void(int)>& callback);
    void setFaceUp(bool isFaceUp);

private:
    int _cardFace;
    int _cardSuit;
    int _cardId;
    bool _isFaceUp;
    cocos2d::Sprite* _bgSprite;
    cocos2d::Sprite* _suitSprite;
    cocos2d::Sprite* _numberSprite;
    std::function<void(int)> _onClickCallback;
    void onCardClicked();
    std::string getNumberImagePath(int cardFace, int cardSuit);
};