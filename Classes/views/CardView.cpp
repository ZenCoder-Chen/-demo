#include "CardView.h"
#include "cocos2d.h"
#include "configs/loaders/LevelConfigLoader.h"
#include "controllers/GameController.h"
#include "StackView.h"
#include "PlayfieldView.h"

USING_NS_CC;

CardView* CardView::create(int cardFace, int cardSuit, bool isFaceUp) {
    CardView* ret = new (std::nothrow) CardView();
    if (ret && ret->init(cardFace, cardSuit, isFaceUp)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool CardView::init(int cardFace, int cardSuit, bool isFaceUp) {
    if (!Node::init()) return false;
    _cardFace = cardFace;
    _cardSuit = cardSuit;
    _isFaceUp = isFaceUp;
    _cardId = -1;

    // 卡牌底图
    _bgSprite = Sprite::create("card_general.png");
    if (!_bgSprite) {
        CCLOG("card_general.png NO!");
        return false;
    } else {
        CCLOG("card_general.png YES!");
    }
    this->addChild(_bgSprite);

    // 花色
    std::string suitPath;
    switch (_cardSuit) {
        case 0: suitPath = "club.png"; break;
        case 1: suitPath = "diamond.png"; break;
        case 2: suitPath = "heart.png"; break;
        case 3: suitPath = "spade.png"; break;
        default: suitPath = "club.png"; break;
    }
    _suitSprite = Sprite::create(suitPath);
    if (!_suitSprite) {
        CCLOG("%s NO!", suitPath.c_str());
        return false;
    } else {
        CCLOG("%s YES!", suitPath.c_str());
    }
    _suitSprite->setPosition(Vec2(33, 57));
    _bgSprite->addChild(_suitSprite);

    // 点数
    std::string numberPath = getNumberImagePath(cardFace, cardSuit);
    _numberSprite = Sprite::create(numberPath);
    if (!_numberSprite) {
        CCLOG("%s NO!", numberPath.c_str());
        return false;
    } else {
        CCLOG("%s YES!", numberPath.c_str());
    }
    _numberSprite->setPosition(Vec2(22, 26));
    _bgSprite->addChild(_numberSprite);

    // 缩放适配
    float targetCardWidth = 150;
    float scale = targetCardWidth / _bgSprite->getContentSize().width;
    _bgSprite->setScale(scale);
    CCLOG("Card position: (%f, %f)", this->getPosition().x, this->getPosition().y);

    // 点击事件
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        if (!this->isVisible()) return false;
        Vec2 worldPos = touch->getLocation();
        Vec2 locationInNode = this->convertToNodeSpace(worldPos);
        Size s = _bgSprite->getContentSize() * _bgSprite->getScale();
        Rect rect(-s.width / 2, -s.height / 2, s.width, s.height);
        if (rect.containsPoint(locationInNode)) {
            CCLOG("Card id=%d, Touch at (%f, %f), converted to (%f, %f), Rect: x=%f, y=%f, w=%f, h=%f",
                  _cardId, worldPos.x, worldPos.y, locationInNode.x, locationInNode.y,
                  rect.origin.x, rect.origin.y, rect.size.width, rect.size.height);
            CCLOG("CardView clicked! id=%d", _cardId);
            onCardClicked();
            return true;
        }
        return false;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    setFaceUp(isFaceUp);
    CCLOG("Card id=%d initialized, parent=%s", _cardId, 
          this->getParent() ? this->getParent()->getName().c_str() : "None");
    return true;
}

void CardView::setCardFace(int cardFace) { _cardFace = cardFace; }
void CardView::setCardSuit(int cardSuit) { _cardSuit = cardSuit; }
void CardView::setCardId(int cardId) { _cardId = cardId; }
int CardView::getCardId() const { return _cardId; }
void CardView::setOnClickCallback(const std::function<void(int)>& callback) { _onClickCallback = callback; }
void CardView::setFaceUp(bool isFaceUp) {
    _isFaceUp = isFaceUp;
    if (_suitSprite) {
        _suitSprite->setVisible(isFaceUp);
    }
    if (_numberSprite) {
        _numberSprite->setVisible(isFaceUp);
    }
    CCLOG("Card id=%d setFaceUp=%d", _cardId, isFaceUp);
}
void CardView::onCardClicked() { 
    if (_onClickCallback) _onClickCallback(_cardId); 
    CCLOG("Calling _onClickCallback for cardId=%d", _cardId);
}

std::string CardView::getNumberImagePath(int cardFace, int cardSuit) {
    std::string color = (cardSuit == 1 || cardSuit == 2) ? "red" : "black";
    std::string faceStr;
    switch (cardFace) {
        case 1: faceStr = "A"; break;
        case 2: faceStr = "2"; break;
        case 3: faceStr = "3"; break;
        case 4: faceStr = "4"; break;
        case 5: faceStr = "5"; break;
        case 6: faceStr = "6"; break;
        case 7: faceStr = "7"; break;
        case 8: faceStr = "8"; break;
        case 9: faceStr = "9"; break;
        case 10: faceStr = "10"; break;
        case 11: faceStr = "J"; break;
        case 12: faceStr = "Q"; break;
        case 13: faceStr = "K"; break;
        default: faceStr = "A"; break;
    }
    return "big_" + color + "_" + faceStr + ".png";
}