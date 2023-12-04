#include <fstream>
#include <iostream>
#include <map>
#include <sstream>

struct CubeSet
{
    std::map<std::string, int> iCubes;

    CubeSet()
        : iCubes({{"blue", 0}, {"red", 0}, {"green", 0}})
    {}

    CubeSet(const std::string& aSet)
        : CubeSet()
    {
        std::stringstream cubeSet(aSet);
        std::string cube;
        // Split a subsets of cubes that were revealed from the bag 
        // (like 3 red, 5 green, 4 blue) to detect an amount for each color
        while (getline(cubeSet, cube, ','))
        {
            cube.erase(0, cube.find_first_not_of(' '));
            int amount = std::stoi(cube.substr(0, cube.find(" ")));
            std::string color = cube.substr(cube.find(" ") + 1);

            iCubes[color] = amount;
        }
    }

    // A game could have been impossible if cubes of some color
    // were greater than available in the given set.
    bool operator <= (const CubeSet& rhs) const
    {
        for (const auto& cube: iCubes)
        {
            if (cube.second > rhs.iCubes.at(cube.first)) return false;
        }
        return true;
    }

    // Define a minimum set required for the game
    CubeSet& operator += (const CubeSet& rhs)
    {
        for (const auto& cube: iCubes)
        {
            iCubes[cube.first] = (rhs.iCubes.at(cube.first) > iCubes.at(cube.first))
                ? rhs.iCubes.at(cube.first)
                : iCubes.at(cube.first);
        }
        return *this;
    }

    // The power of a set of cubes is equal to the numbers of red, green, 
    // and blue cubes multiplied together.
    int Power()
    {
        int result = 1;
        for (const auto& cube: iCubes)
        {
            result *= cube.second;
        }
        return result;
    }
};

class Game
{
public:
    Game(const std::string& aGameRecord)
    {
        std::stringstream cubeSetList(aGameRecord);
        std::string cubeSubetDescription;
        // A game record is a semicolon-separated list of subsets of cubes
        // that were revealed from the bag (like 3 red, 5 green, 4 blue).
        while (getline(cubeSetList, cubeSubetDescription, ';'))
        {
            CubeSet cubeSet(cubeSubetDescription);
            // Define the minimum set required for the game
            iMinimumSet += cubeSet;
        }
    }

    bool HaveBeenPossibleWith(const CubeSet& aGivenCubeSet)
    {
        return iMinimumSet <= aGivenCubeSet;
    }

    int PowerOfMinimumSet()
    {
        return iMinimumSet.Power();
    }
private:
    CubeSet iMinimumSet;
};

class CubeConundrum
{
public:
    CubeConundrum(const std::string& aFileName, const CubeSet& aGivenCubeSet)
        : iSumOfIds(0), iSumOfPower(0)
    {
        std::ifstream list(aFileName);
        for (std::string line; std::getline(list, line); ) 
        {
            // Each game is listed with its ID number (like the 11 in Game 11: ...)
            int gameId = std::stoi(line.substr(line.find("Game") + 5, line.find(":")));
            // followed by list of subsets of cubes that were revealed from the bag
            Game game(Game(line.substr(line.find(":") + 1)));

            if (game.HaveBeenPossibleWith(aGivenCubeSet))
            {
                // Add up the IDs of the games that would have been possible
                iSumOfIds += gameId;
            }
            // Find the sum of the power of the minimum set of cubes
            // that must have been present for each game
            iSumOfPower += game.PowerOfMinimumSet();
        }

        std::cout << "Day 2.1 : " << iSumOfIds << std::endl;
        std::cout << "Day 2.2 : " << iSumOfPower << std::endl;
    }
private:
    int iSumOfIds;
    int iSumOfPower;
};

int main(int argc, char ** argv)
{
    if (argc < 2) return -1;

    CubeSet givenCubes("12 red, 13 green, 14 blue");
    CubeConundrum game(argv[1], givenCubes);

    return 0;
}
