#pragma once
#include "cocos2d.h"
#include <vector>
#include "CardView.h"
#include <functional>

class PlayfieldView : public cocos2d::Node {
public:
    enum class LayoutType {
        KEEP_ORIGINAL,
        HORIZONTAL_LINE,
        CUSTOM
    };

    static PlayfieldView* create();
    virtual bool init();
    void addCard(CardView* cardView);
    void removeCard(CardView* cardView);
    void setOnCardClickCallback(const std::function<void(int)>& callback);
    void layoutCards(LayoutType type = LayoutType::KEEP_ORIGINAL);
    const std::vector<CardView*>& getCards() const;
    static const int PLAYFIELD_WIDTH = 1080;
    static const int PLAYFIELD_HEIGHT = 1500;

private:
    std::vector<CardView*> _cards;
    std::function<void(int)> _onCardClickCallback; // 用于转发点击事件
    LayoutType _currentLayout{LayoutType::KEEP_ORIGINAL};
};

