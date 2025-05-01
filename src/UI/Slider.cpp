#include "../../include/UI/Slider.h"
#include <sstream>
#include <iomanip>



// constructor del slider
Slider::Slider(float x, float y, float width, float height,
               const sf::Font& font, float min, float max, float initial,
               std::function<void(float)> callback)
    : minValue(min), maxValue(max), currentValue(initial),
      isSelected(false), onChange(callback) {

    // configurar la barra de fondo
    track.setPosition(x, y + height / 2 - 5);
    track.setSize(sf::Vector2f(width, 10));
    track.setFillColor(sf::Color(50, 50, 50));

    // configurar el control deslizante
    float handlePos = x + width * (initial - min) / (max - min);
    handle.setPosition(handlePos - 7.5f, y);
    handle.setSize(sf::Vector2f(15, height));
    handle.setFillColor(sf::Color(120, 120, 120));

    // configurar el texto de valor
    valueText.setFont(font);
    valueText.setCharacterSize(18);
    valueText.setFillColor(sf::Color::White);

    // actualizar el texto con el valor inicial
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << initial;
    valueText.setString(ss.str() + "%");

    // posicionar el texto
    sf::FloatRect textRect = valueText.getLocalBounds();
    valueText.setPosition(
        x + width + 20,
        y + (height - textRect.height) / 2 - textRect.top
    );
}



// establece el valor del slider
void Slider::setValue(float value) {
    // limitar el valor entre min y max
    currentValue = std::max(minValue, std::min(maxValue, value));

    // actualizar la posicion del control deslizante
    float trackWidth = track.getSize().x;
    float x = track.getPosition().x;
    float handlePos = x + trackWidth * (currentValue - minValue) / (maxValue - minValue);
    handle.setPosition(handlePos - 7.5f, handle.getPosition().y);

    // actualizar el texto
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << currentValue;
    valueText.setString(ss.str() + "%");

    // llamar al callback si existe
    if (onChange) {
        onChange(currentValue);
    }
}



// obtiene el valor actual del slider
float Slider::getValue() const {
    return currentValue;
}



// establece si el slider esta seleccionado
void Slider::setSelected(bool selected) {
    isSelected = selected;
    handle.setFillColor(isSelected ? sf::Color(175, 30, 30) : sf::Color(120, 120, 120));
}



// obtiene si el slider esta seleccionado
bool Slider::getSelected() const {
    return isSelected;
}



// incrementa el valor del slider
void Slider::increase(float amount) {
    setValue(currentValue + amount);
}



// decrementa el valor del slider
void Slider::decrease(float amount) {
    setValue(currentValue - amount);
}



// dibuja el slider en la ventana
void Slider::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(track, states);
    target.draw(handle, states);
    target.draw(valueText, states);
}
