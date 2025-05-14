#pragma once

enum ItemType { Heal, AttackUp, DefenseUp, Key };

class Item {
public:
    int x, y;
    ItemType type;
    Item(int x, int y, ItemType type) : x(x), y(y), type(type) {}

};
