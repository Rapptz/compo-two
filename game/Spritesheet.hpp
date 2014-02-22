#ifndef SPRITESHEET_HPP
#define SPRITESHEET_HPP

#include <SFML/Graphics/Texture.hpp>
#include <utility>
#include <stdexcept>
#include <type_traits>

// Circumvent including "unnecessary" dependencies.
namespace sf {
class Sprite;
} // sf

class Animation;

class Spritesheet {
private:
    const sf::Texture* data;
    std::vector<sf::IntRect> sprites;
public:
    Spritesheet() noexcept: data(nullptr) {}

    explicit Spritesheet(const sf::Texture& tex, unsigned width, unsigned height): data(&tex) {
        auto size = data->getSize();
        if(size.x % width || size.y % height) {
            throw std::logic_error("Sizes do not fit the sprite sheet perfectly");
        }

        for(unsigned y = 0; y < size.y; y += height) {
            for(unsigned x = 0; x < size.x; x += width) {
                sprites.push_back(sf::IntRect(x, y, width, height));
            }
        }
    }

    auto operator[](unsigned index) noexcept -> decltype(sprites[0]) {
        return sprites[index];
    }

    auto at(unsigned index) -> decltype(sprites[0]) {
        if(index >= sprites.size())
            throw std::out_of_range("Index exceeds number of sprites");
        return sprites[index];
    }

    unsigned getNumberOfSprites() const noexcept {
        return sprites.size();
    }

    const sf::Texture* getTexture() const noexcept {
        return data;
    }

    void setTexture(const sf::Texture& texture) noexcept {
        data = &texture;
        sprites.clear();
    }

    void setSpriteSize(unsigned width, unsigned height) {
        if(data == nullptr) {
            throw std::logic_error("No texture is set");
        }

        auto size = data->getSize();
        if(size.x % width || size.y % height) {
            throw std::logic_error("Sizes do not fit the sprite sheet perfectly");
        }

        for(unsigned y = 0; y < size.y; y += height) {
            for(unsigned x = 0; x < size.x; x += width) {
                sprites.push_back(sf::IntRect(x,y,width,height));
            }
        }
    }

    template<class T = Animation, typename... Args>
    T createAnimation(Args&&... indices) {
        static_assert(sizeof(T), "Incomplete type. Did you forget to include <Fire/Graphics/Animation.hpp>?");
        if(data == nullptr || sprites.empty())
            return T();
        T result(*data);
        typename std::common_type<Args...>::type frames[] = { std::forward<Args>(indices)... };
        for(auto&& frame : frames) {
            result.addFrame(this->at(frame));
        }
        return result;
    }

    template<class T = sf::Sprite>
    T createSprite(unsigned index) {
        static_assert(sizeof(T), "Incomplete type. Did you forget to include <SFML/Graphics/Sprite.hpp>?");
        if(index >= sprites.size())
            throw std::out_of_range("Index exceeds number of sprites");
        return T(*data, sprites[index]);
    }
};

#endif // SPRITESHEET_HPP
