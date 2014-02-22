#ifndef TILEMAP_HPP
#define TILEMAP_HPP

// License TBD

// Includes
#include "Object.hpp"
#include <pugixml.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace detail {
struct Layer {
    unsigned width, height;
    unsigned char opacity;
    std::vector<unsigned> tiles;
    sf::VertexArray vertices;
    std::string name;
    void setUpVertices(unsigned tileWidth, unsigned tileHeight, unsigned firstgid, sf::Vector2u tileInfo) {
        vertices.setPrimitiveType(sf::Quads);
        vertices.resize(width * height * 4);

        for(unsigned i = 0; i < width; ++i) {
            for(unsigned j = 0; j < width; ++j) {
                unsigned gid = tiles[i + j * width];
                gid -= (firstgid - gid) > 0;
                unsigned mod = gid % (tileInfo.x / tileWidth);
                unsigned div = gid / (tileInfo.x / tileWidth);

                sf::Vertex* quad = &vertices[(i + j * width) * 4];

                quad[0].position = sf::Vector2f(i * tileWidth, j * tileHeight);
                quad[1].position = sf::Vector2f((i + 1) * tileWidth, j * tileHeight);
                quad[2].position = sf::Vector2f((i + 1) * tileWidth, (j + 1) * tileHeight);
                quad[3].position = sf::Vector2f(i * tileWidth, (j + 1) * tileHeight);

                quad[0].texCoords = sf::Vector2f(mod * tileWidth, div * tileHeight);
                quad[1].texCoords = sf::Vector2f((mod + 1) * tileWidth, div * tileHeight);
                quad[2].texCoords = sf::Vector2f((mod + 1) * tileWidth, (div + 1) * tileHeight);
                quad[3].texCoords = sf::Vector2f(mod * tileWidth, (div + 1) * tileHeight);
            }
        }
    }
};

} // detail

class TileMap : public sf::Drawable, public sf::Transformable {
private:
    unsigned width, height;
    unsigned tileWidth, tileHeight, spacing, margin, firstTileID;
    sf::Texture spritesheet;
    std::vector<sf::FloatRect> solidObjects;
    std::vector<detail::Layer> layers;
    std::vector<Object> objects;

    void clearMap() {
        objects.clear();
        solidObjects.clear();
        layers.clear();
    }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
        states.transform *= getTransform();
        states.texture = &spritesheet;
        for(auto&& layer : layers)
            target.draw(layer.vertices, states);
    }

public:
    TileMap(): width(0), height(0), tileWidth(0), tileHeight(0), spacing(0), margin(0), firstTileID(1) {}

    bool loadFromFile(const std::string& filename) {
        clearMap();

        pugi::xml_document file;
        pugi::xml_parse_result result = file.load_file(filename.c_str());
        if(!result) {
            return result;
        }

        pugi::xml_node map = file.child("map");
        std::string orientation = map.attribute("orientation").as_string();
        if(orientation != "orthogonal") {
            return false; // Non-orthogonal maps not supported
        }

        // Map info
        width = map.attribute("width").as_uint();
        height = map.attribute("height").as_uint();
        tileWidth = map.attribute("tilewidth").as_uint();
        tileHeight = map.attribute("tileheight").as_uint();

        auto children = map.child("tileset");
        firstTileID = children.attribute("firstgid").as_uint();
        spacing = children.attribute("spacing").as_uint();
        margin = children.attribute("margin").as_uint();

        auto imagenode = children.child("image");
        std::string imagePath = imagenode.attribute("source").as_string();

        if(!spritesheet.loadFromFile(imagePath)) {
            return false;
        }

        // Initialize layers

        for(auto node = map.child("layer"); node; node = node.next_sibling("layer")) {
            detail::Layer layer;
            bool visibility = node.attribute("visible").as_bool(true);
            if(visibility) {
                float opacity = node.attribute("opacity").as_float();
                if(opacity != 0.f)
                    layer.opacity = static_cast<unsigned char>(opacity * 255.f);
                else
                    layer.opacity = 255;
            }
            else {
                layer.opacity = 0;
            }

            layer.name = node.attribute("name").as_string();
            layer.width = node.attribute("width").as_uint();
            layer.height = node.attribute("height").as_uint();

            auto tiledata = node.child("data");
            std::string data = tiledata.text().as_string();
            std::stringstream ss(data);
            unsigned id;
            while(ss >> id) {
                layer.tiles.push_back(id);
                if(ss.peek() == ',')
                    ss.ignore();
            }

            layer.setUpVertices(tileWidth, tileHeight, firstTileID, spritesheet.getSize());
            layers.push_back(layer);
        }

        // Initialize objects
        for(auto node = map.child("objectgroup"); node; node = node.next_sibling("objectgroup")) {
            for(auto element = node.child("object"); element; element = element.next_sibling("object")) {
                Object object;
                sf::FloatRect objrect;
                object.type = element.attribute("type").as_string();
                object.name = element.attribute("name").as_string();
                objrect.left = element.attribute("x").as_float();
                objrect.top = element.attribute("y").as_float();
                objrect.width = element.attribute("width").as_float();
                objrect.height = element.attribute("height").as_float();
                object.gid = element.attribute("gid").as_int();
                object.visible = element.attribute("visible").as_bool(true);

                if(object.type == "solid")
                    solidObjects.push_back(objrect);
                object.rect = objrect;

                auto properties = element.child("properties");
                if(properties) {
                    for(auto property = properties.child("property"); property; property = property.next_sibling()) {
                        std::string name = property.attribute("name").as_string();
                        std::string value = property.attribute("value").as_string();
                        if(!name.empty())
                            object.properties[name] = value;
                    }
                }
                auto poly = element.child("polygon");
                if(!poly)
                    poly = element.child("polyline");
                if(poly) {
                    std::string pointlist = poly.attribute("points").as_string();
                    std::stringstream ss(pointlist);
                    std::vector<std::string> points;
                    std::string str;
                    while(std::getline(ss, str, ' '))
                        points.push_back(str);

                    for(auto&& point : points) {
                        std::vector<unsigned> coords;
                        std::stringstream pt(point);
                        unsigned coord;
                        while(pt >> coord) {
                            coords.push_back(coord);
                            if(pt.peek() == ',')
                                pt.ignore();
                        }
                        object.points.push_back(sf::Vector2i(coords[0],coords[1]));
                    }
                }

                objects.push_back(object);
            }
        }

        return true;
    }

    std::vector<Object> getObjects(const std::string& name) const {
        if(name.empty())
            return objects;
        std::vector<Object> vec;
        for(auto&& obj : objects) {
            if(obj.name == name)
                vec.push_back(obj);
        }
        return vec;
    }
};

#endif // TILEMAP_HPP
