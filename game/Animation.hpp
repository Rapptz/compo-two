#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <vector>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Texture.hpp>

class Animation {
private:
    const sf::Texture* texture;
    std::vector<sf::IntRect> frames;
public:
    Animation() noexcept: texture(nullptr) {}

    Animation(const sf::Texture& texture) noexcept: texture(&texture) {}

    void setTexture(const sf::Texture& texture) noexcept {
        this->texture = &texture;
    }

    void addFrame(sf::IntRect frame) noexcept {
        frames.push_back(frame);
    }

    void addFrames(size_t count, int x, int y, int width, int height) noexcept {
        for(size_t i = 0; i < count; ++i) {
            frames.push_back(sf::IntRect(x, y, width, height));
            x += width;
        }
    }

    const sf::Texture* getTexture() const noexcept {
        return texture;
    }

    const sf::IntRect& getFrame(size_t index) const noexcept {
        return frames[index];
    }

    size_t getFrameCount() const noexcept {
        return frames.size();
    }
};

#endif // ANIMATION_HPP
