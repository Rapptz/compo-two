#ifndef ANIMATEDSPRITE_HPP
#define ANIMATEDSPRITE_HPP

#include "Animation.hpp"
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

class AnimatedSprite : public sf::Drawable, public sf::Transformable {
protected:
    const Animation* animation = nullptr;
    bool looped = true;
    bool paused = false;
    sf::Time delay{sf::seconds(0.1f)};
    sf::Time currentTime;
    size_t currentFrame = 0;
    sf::Vertex vertices[4];

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        if(animation != nullptr && animation->getTexture() != nullptr) {
            states.transform *= getTransform();
            states.texture = animation->getTexture();
            target.draw(vertices, 4, sf::Quads, states);
        }
    }

public:
    AnimatedSprite() = default;
    explicit AnimatedSprite(const Animation& animation, sf::Time delay = sf::seconds(0.1f)) noexcept: animation(&animation), delay(delay) {}
    virtual ~AnimatedSprite() = default;

    void setFrame(size_t newFrame, bool resetTime = true) noexcept {
        if(animation != nullptr) {
            sf::IntRect rect = animation->getFrame(newFrame);

            vertices[0].position = sf::Vector2f(0,0);
            vertices[1].position = sf::Vector2f(0,rect.height);
            vertices[2].position = sf::Vector2f(rect.width,rect.height);
            vertices[3].position = sf::Vector2f(rect.width,0);

            float left = static_cast<float>(rect.left) + 0.0001f;
            float right = left + rect.width;
            float top = static_cast<float>(rect.top);
            float bottom = top + rect.height;

            vertices[0].texCoords = sf::Vector2f(left,top);
            vertices[1].texCoords = sf::Vector2f(left,bottom);
            vertices[2].texCoords = sf::Vector2f(right,bottom);
            vertices[3].texCoords = sf::Vector2f(right,top);
        }

        if(resetTime) {
            currentTime = sf::Time::Zero;
        }
    }

    sf::IntRect getFrame(size_t frame) const noexcept {
        return animation->getFrame(frame);
    }

    void setAnimation(const Animation& animation) noexcept {
        this->animation = &animation;
        currentFrame = 0;
        setFrame(currentFrame);
    }

    void play() noexcept {
        if(paused) {
            currentTime = sf::Time::Zero;
            paused = false;
        }
    }

    void pause() noexcept {
        paused = true;
    }

    void stop() noexcept {
        paused = true;
        currentFrame = 0;
    }

    void setLooped(bool looped) noexcept {
        this->looped = looped;
    }

    bool isLooped() const noexcept {
        return looped;
    }

    bool isPlaying() const noexcept {
        return !paused;
    }

    sf::Time getDelay() const noexcept {
        return delay;
    }

    void setDelay(const sf::Time& time) noexcept {
        delay = time;
    }

    sf::FloatRect getLocalBounds() const noexcept {
        sf::IntRect rect = animation->getFrame(currentFrame);
        float width = static_cast<float>(rect.width);
        float height = static_cast<float>(rect.height);

        return sf::FloatRect(0.0f,0.0f,width,height);
    }

    sf::FloatRect getGlobalBounds() const noexcept {
        return getTransform().transformRect(getLocalBounds());
    }

    void setColor(const sf::Color& color) noexcept {
        vertices[0].color = color;
        vertices[1].color = color;
        vertices[2].color = color;
        vertices[3].color = color;
    }

    virtual void update(sf::Time deltaTime) noexcept {
        if(!paused && (animation != nullptr)) {
            currentTime += deltaTime;

            if(currentTime >= delay) {
                currentTime = sf::Time::Zero;

                if(currentFrame + 1 < animation->getFrameCount()) {
                    ++currentFrame;
                }
                else {
                    currentFrame = 0;
                    if(!looped) {
                        paused = true;
                    }
                }
            }
            setFrame(currentFrame,false);
        }
    }

    const sf::Texture* getTexture() const noexcept {
        return animation->getTexture();
    }

    const Animation* getAnimation() const noexcept {
        return animation;
    }
};

#endif // ANIMATEDSPRITE_HPP
