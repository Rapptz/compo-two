#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include <sstream>
#include <typeinfo>
#include <vector>
#include <unordered_map>

struct Object {
    std::string name;
    std::string type;
    bool visible;
    unsigned gid;
    sf::FloatRect rect;
    std::unordered_map<std::string, std::string> properties;
    std::vector<sf::Vector2i> points;
    Object(): name("None"), type("None"), visible(false), gid(0) {}
    template<typename T>
    T getPropertyAs(std::string&& name) {
        return boost::lexical_cast<T>(properties.at(std::move(name)));
    }

    std::string getProperty(std::string&& name) {
        return properties.at(std::move(name));
    }
};

#endif // OBJECT_HPP
