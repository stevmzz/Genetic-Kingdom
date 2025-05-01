#include "../include/UI/Button.h"



// constructor del boton
Button::Button(float x, float y, float width, float height,
               const sf::Font& font, const std::string& text,
               std::function<void()> callback)
    : callback(callback), isSelected(false), useTexture(false) {

    shape.setPosition(x, y);
    shape.setSize(sf::Vector2f(width, height));
    shape.setFillColor(sf::Color(70, 70, 70));
    shape.setOutlineThickness(0);

    this->text.setFont(font);
    this->text.setString(text);
    this->text.setCharacterSize(20);
    this->text.setFillColor(sf::Color(175, 30, 30));

    // centrar texto en el boton
    sf::FloatRect textRect = this->text.getLocalBounds();
    this->text.setPosition(
        x + (width - textRect.width) / 2,
        y + (height - textRect.height) / 2 - textRect.top
    );
}



// establecer la textura del boton
void Button::setTextures(const std::string& normalTexture, const std::string& selectedTexture) {
    if (buttonTexture.loadFromFile(normalTexture) &&
        buttonSelectedTexture.loadFromFile(selectedTexture)) {

        useTexture = true;

        // configuramos la textura para que se ajuste al tamaño del botón
        shape.setTexture(&buttonTexture);
        shape.setTextureRect(sf::IntRect(0, 0, buttonTexture.getSize().x, buttonTexture.getSize().y));
        }
}



// establece si el boton esta seleccionado
void Button::setSelected(bool selected) {
    isSelected = selected;
}



// obtiene si el boton esta seleccionado
bool Button::getSelected() const {
    return isSelected;
}



// activa el boton al llamar a su callback
void Button::activate() {
    if (callback) {
        callback();
    }
}



// dibuja el boton en la ventana
void Button::draw(sf::RenderWindow& window) {
    if (useTexture) {
        if (isSelected) {
            shape.setTexture(&buttonSelectedTexture);
        } else {
            shape.setTexture(&buttonTexture);
        }
    }

    window.draw(shape);
    window.draw(text);
}



// establece el texto del boton y lo centra
void Button::setText(const std::string& text) {
    this->text.setString(text);

    // recentrar el texto
    sf::FloatRect textRect = this->text.getLocalBounds();
    sf::Vector2f buttonPos = shape.getPosition();
    sf::Vector2f buttonSize = shape.getSize();

    this->text.setPosition(
        buttonPos.x + (buttonSize.x - textRect.width) / 2,
        buttonPos.y + (buttonSize.y - textRect.height) / 2 - textRect.top
    );
}
