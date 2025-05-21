#include "PlayfieldView.h"
#include "CardView.h"
USING_NS_CC;

PlayfieldView* PlayfieldView::create() {
    PlayfieldView* ret = new (std::nothrow) PlayfieldView();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

static const int PLAYFIELD_WIDTH = 1080;
static const int PLAYFIELD_HEIGHT = 1024;

bool PlayfieldView::init() {
    if (!Node::init()) return false;

    auto draw = DrawNode::create();
    Vec2 rect[5] = {
        Vec2(0, 0),
        Vec2(PLAYFIELD_WIDTH, 0),
        Vec2(PLAYFIELD_WIDTH, PLAYFIELD_HEIGHT),
        Vec2(0, PLAYFIELD_HEIGHT),
        Vec2(0, 0)
    };
    draw->drawPoly(rect, 5, false, Color4F::WHITE);
    this->addChild(draw, 2);

    return true;
}

void PlayfieldView::addCard(CardView* cardView) {
    if (!cardView) return;
    
    this->addChild(cardView);
    _cards.push_back(cardView);
    // 为新卡牌设置点击回调
    cardView->setOnClickCallback([this](int cardId) {
        CCLOG("PlayfieldView: Card clicked, id=%d", cardId);
        if (_onCardClickCallback) {
            _onCardClickCallback(cardId);
        } else {
            CCLOG("PlayfieldView: _onCardClickCallback is null for card id=%d", cardId);
        }
    });
    CCLOG("PlayfieldView: Added card, id=%d, pos=(%f, %f)", cardView->getCardId(), cardView->getPosition().x, cardView->getPosition().y);
}

void PlayfieldView::removeCard(CardView* cardView) {
    this->removeChild(cardView);
    _cards.erase(std::remove(_cards.begin(), _cards.end(), cardView), _cards.end());
    layoutCards();
}

void PlayfieldView::setOnCardClickCallback(const std::function<void(int)>& callback) {
    _onCardClickCallback = callback;
    CCLOG("PlayfieldView: Set card click callback, callback=%p", _onCardClickCallback ? 1 : 0);
    for (auto card : _cards) {
        card->setOnClickCallback([this](int cardId) {
            CCLOG("PlayfieldView: Card clicked, id=%d", cardId);
            if (_onCardClickCallback) {
                _onCardClickCallback(cardId);
            } else {
                CCLOG("PlayfieldView: _onCardClickCallback is null for card id=%d", cardId);
            }
        });
    }
}

void PlayfieldView::layoutCards(LayoutType type) {
    _currentLayout = type;
    
    switch (type) {
        case LayoutType::KEEP_ORIGINAL:
            for (auto card : _cards) {
                CCLOG("PlayfieldView: Card id=%d, pos=(%f, %f)", card->getCardId(), card->getPosition().x, card->getPosition().y);
            }
            break;
        case LayoutType::HORIZONTAL_LINE: {
            float startX = 150;
            float gap = 120;
            float y = 1000;
            for (size_t i = 0; i < _cards.size(); ++i) {
                _cards[i]->setPosition(Vec2(startX + i * gap, y));
                CCLOG("PlayfieldView: Laid out card id=%d, pos=(%f, %f)", _cards[i]->getCardId(), startX + i * gap, y);
            }
            break;
        }
        case LayoutType::CUSTOM:
            break;
    }
}

const std::vector<CardView*>& PlayfieldView::getCards() const {
    return _cards;
}