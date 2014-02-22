#ifndef ACTION_HPP
#define ACTION_HPP

#include <SFML/Window/Event.hpp>
#include <memory>

enum class ActionType : char {
    PressedOnce,
    Held,
    Released,
    Triggered
};

class Action {
private:
    using pointer = std::shared_ptr<Action>;

    bool active  = false;
    bool state   = false;

    const sf::Event::EventType* event = nullptr;
    const sf::Keyboard::Key* key      = nullptr;
    const sf::Mouse::Button* mouse    = nullptr;

    ActionType type = ActionType::Triggered;

    enum class ChainType : char {
        None,
        And,
        Or
    } chain = ChainType::None;

    pointer left;
    pointer right;

    bool isExpression() const noexcept {
        return left != nullptr && right != nullptr;
    }

    template<typename Function>
    void evaluate(Action* node, Function f) noexcept {
        if(node == nullptr)
            return;
        evaluate(node->left.get(), f);
        evaluate(node->right.get(), f);

        if(node->isExpression()) {
            switch(node->chain) {
            case Action::ChainType::And:
                node->active = node->left->active && node->right->active;
                break;
            case Action::ChainType::Or:
                node->active = node->left->active || node->right->active;
                break;
            case Action::ChainType::None:
                break;
            }

            active = node->active;
        }
        else {
            node->updateStatus(f(*node));
        }
    }

    void updateStatus(bool value) noexcept {
        switch(type) {
        case ActionType::Triggered:
            active = value;
            break;
        case ActionType::Held:
            active = value && state;
            break;
        case ActionType::PressedOnce:
            active = value && !state;
            break;
        case ActionType::Released:
            active = !value && state;
            break;
        }

        state = value;
    }

    Action(ChainType type, pointer lhs, pointer rhs): chain(type), left(std::move(lhs)), right(std::move(rhs)) {}

    template<typename Key>
    friend class ActionMap;

public:
    Action() = default;
    explicit Action(sf::Event::EventType event, ActionType type = ActionType::Triggered) noexcept: event(&event), type(type) {}
    explicit Action(sf::Keyboard::Key key, ActionType type = ActionType::Triggered) noexcept: key(&key), type(type) {}
    explicit Action(sf::Mouse::Button mouse, ActionType type = ActionType::Triggered) noexcept: mouse(&mouse), type(type) {}
    Action(Action&&) = default;
    Action(const Action&) = default;

    friend Action operator&(const Action& lhs, const Action& rhs) {
        return Action(Action::ChainType::And, std::make_shared<Action>(lhs), std::make_shared<Action>(rhs));
    }

    friend Action operator|(const Action& lhs, const Action& rhs) {
        return Action(Action::ChainType::Or, std::make_shared<Action>(lhs), std::make_shared<Action>(rhs));
    }

    explicit operator bool() const noexcept {
        return active;
    }

};

#endif // ACTION_HPP
