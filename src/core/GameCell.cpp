#include "octa-core/core/GameCell.h"

GameCell::GameCell(const Coordinate& coordinate)
    : coordinate_(coordinate), occupant_(Player::NONE), neighbors_(NUM_DIRECTIONS, nullptr) {
}

std::shared_ptr<GameCell> GameCell::getNeighbor(Direction direction) const {
    size_t index = static_cast<size_t>(direction);
    if (index < neighbors_.size()) {
        return neighbors_[index];
    }
    return nullptr;
}

void GameCell::setNeighbor(Direction direction, std::shared_ptr<GameCell> neighbor) {
    size_t index = static_cast<size_t>(direction);
    if (index < neighbors_.size()) {
        neighbors_[index] = neighbor;
    }
}

std::vector<std::shared_ptr<GameCell>> GameCell::getAllNeighbors() const {
    return neighbors_;
}

std::vector<std::shared_ptr<GameCell>> GameCell::getValidNeighbors() const {
    std::vector<std::shared_ptr<GameCell>> validNeighbors;
    for (const auto& neighbor : neighbors_) {
        if (neighbor != nullptr) {
            validNeighbors.push_back(neighbor);
        }
    }
    return validNeighbors;
} 