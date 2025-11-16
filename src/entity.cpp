#include "entity.hpp"
#include <iostream>
#include <utility>

Entity::Entity(int code) {
        this->code = EntityType(code);
}

Entity::operator std::string() const {
        return std::to_string(std::to_underlying(this->code));
};

bool Entity::operator==(const EntityType etype) const {
        return (this->code == etype);
}
