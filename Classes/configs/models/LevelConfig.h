#pragma once
#include "cocos2d.h"
#include <vector>
struct CardConfig {
    int face;
    int suit;
    cocos2d::Vec2 position;
};

struct LevelConfig {
    std::vector<CardConfig> playfieldCards;
    std::vector<CardConfig> stackCards;
};