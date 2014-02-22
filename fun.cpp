#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<> dist(5, 30);
static std::uniform_int_distribution<> negator(0, 1);
static std::uniform_int_distribution<> dir(1, 15);

struct animal {
private:
    int answer;
public:
    animal(): answer(dist(gen)) { /*std::cout << answer << '\n';*/ }
    bool guess(int g) const {
        if(g >= answer) {
            std::cout << "Caught one!\n";
            return true;
        }
        return false;
    }
};

struct player {
private:
    const unsigned max_animals = 20;
    std::vector<animal> animals{max_animals};
    int answer = 0;
    int up = dir(gen);
    int down = dir(gen);
    int left = dir(gen);
    int right = dir(gen);
public:
    player() = default;
    void guess() {
        // get command
        std::string temp;
        std::cin >> temp;

        // update total
        if(temp == "up") {
            answer += negator(gen) ? -up : up;
        }
        else if(temp == "down") {
            answer += negator(gen) ? -down : down;
        }
        else if(temp == "right") {
            answer += negator(gen) ? -right : right;
        }
        else if(temp == "left") {
            answer += negator(gen) ? -left : left;
        }

        if(answer < 0) {
            answer = 0;
        }

        // refresh command values
        up = dir(gen);
        down = dir(gen);
        left = dir(gen);
        right = dir(gen);

        // herd the animals guessed correctly
        animals.erase(std::remove_if(std::begin(animals), std::end(animals), [this](const animal& x) {
            return x.guess(answer);
        }), std::end(animals));
    }

    void status() {
        std::cout << "[" << max_animals - animals.size() << "/" << max_animals << "] animals herded" << std::endl;
    }

    bool won() {
        return animals.empty();
    }
};

int main() {
    player p;
    std::cout << "The animals have escaped! Try to herd them using the four directions:\n";
    std::cout << "- up\n"
                 "- down\n"
                 "- left\n"
                 "- right\n";
    while(true) {
        p.guess();
        p.status();

        if(p.won()) {
            std::cout << "You win!\n";
            return 0;
        }
    }
}