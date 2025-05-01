#pragma once

#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

class Slider : public sf::Drawable {
private:
    sf::RectangleShape track;
    sf::RectangleShape handle;
    sf::Text valueText;
    float minValue;
    float maxValue;
    float currentValue;
    bool isSelected;
    std::function<void(float)> onChange;

public:
    Slider(float x, float y, float width, float height,
           const sf::Font& font, float min, float max, float initial,
           std::function<void(float)> callback = nullptr);

    void setValue(float value);
    float getValue() const;

    void setSelected(bool selected);
    bool getSelected() const;

    void increase(float amount = 5.0f);
    void decrease(float amount = 5.0f);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};