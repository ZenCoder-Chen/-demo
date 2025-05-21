// configs/loaders/LevelConfigLoader.cpp
#include "LevelConfigLoader.h"
#include "cocos2d.h"
#include "json/document.h"

using namespace rapidjson;
using namespace cocos2d;

LevelConfig LevelConfigLoader::loadFromFile(const std::string& filename) {
    LevelConfig config;
    std::string fileData = FileUtils::getInstance()->getStringFromFile(filename);
    Document doc;
    doc.Parse(fileData.c_str());
    if (doc.HasParseError()) {
        CCLOG("JSON faild!!!!!!!!!!!!!!!!!!!!!!!!");
        return config;
    }
    // 解析 Playfield
    const auto& playfield = doc["Playfield"];
    for (rapidjson::SizeType i = 0; i < playfield.Size(); ++i) {
        CardConfig card;
        card.face = playfield[i]["CardFace"].GetInt();
        card.suit = playfield[i]["CardSuit"].GetInt();
        card.position.x = playfield[i]["Position"]["x"].GetInt();
        card.position.y = playfield[i]["Position"]["y"].GetInt();
        config.playfieldCards.push_back(card);
    }

    // 解析 Stack
    const auto& stack = doc["Stack"];
    for (rapidjson::SizeType i = 0; i < stack.Size(); ++i) {
        CardConfig card;
        card.face = stack[i]["CardFace"].GetInt();
        card.suit = stack[i]["CardSuit"].GetInt();
        card.position.x = stack[i]["Position"]["x"].GetInt();
        card.position.y = stack[i]["Position"]["y"].GetInt();
        config.stackCards.push_back(card);
    }

    return config;
}

// LevelConfig level = LevelConfigLoader::loadFromFile("level1.json");
// for (const auto& cardCfg : level.playfieldCards) {
//     auto card = CardView::create(cardCfg.face, cardCfg.suit, true);
//     card->setCardId(nextCardId++);
//     card->setPosition(cardCfg.position);
//     _playfieldView->addCard(card);
//     CCLOG("Playfield card: face=%d, suit=%d, pos=(%.1f,%.1f)", cardCfg.face, cardCfg.suit, cardCfg.position.x, cardCfg.position.y);
// }