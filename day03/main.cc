#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>

struct SchematicSymbols
{
    std::string iLine;

    SchematicSymbols(const std::string& SchematicLine)
        : iLine(SchematicLine)
    {
        for (char i = '0'; i <= '9'; i++)
        {
            std::replace( iLine.begin(), iLine.end(), i, '.');
        }
    }

    SchematicSymbols& operator += (const SchematicSymbols& rhs)
    {
        for (int i = 0; i < iLine.length(); i++)
        {
            if (iLine[i] == '.')
            {
                iLine[i] = rhs.iLine[i];
            }
        }
        return *this;
    }
};

class SchematicLine
{
public:
    SchematicLine(const std::string& aSchematicLine)
        : iSchematicLine(aSchematicLine), iAdjacentSymbols(aSchematicLine)
    {
        for (int i = 0; i < iSchematicLine.length(); i++)
        {
            if ((iSchematicLine[i] < '0') || (iSchematicLine[i] > '9'))
            {
                iSchematicLine[i] = '.';
            }
        }
    }

    SchematicLine& operator += (const SchematicLine& rhs)
    {
        iAdjacentSymbols += rhs.iAdjacentSymbols;
        return *this;
    }

    void AddAdjacentLine(const std::string& SchematicLine)
    {
        iAdjacentSymbols += SchematicLine;
    }

    int PartNumberSum()
    {
        std::stringstream partNumberList(iSchematicLine);
        std::string partNumberString;
        int sum = 0;
        int pos = 0;

        for (std::size_t i = 0; i < iSchematicLine.length();)
        {
            if (std::isdigit(iSchematicLine[i]))
            {
                std::size_t found = iSchematicLine.find_first_of(".", i);
                partNumberString = iSchematicLine.substr(i, found - i); 
                if (IsAdjasted(partNumberString, i))
                {
                    int partNumber = std::stoi(partNumberString);
                    sum += partNumber;
                }
                i += partNumberString.length();
            }
            else i += 1;
        }
        return sum;
    }
private:
    bool IsAdjasted(const std::string& partNumberString, size_t aPos)
    {
        std::size_t pos = aPos;
        std::size_t lenght = partNumberString.length();
        if (pos > 0)
        {
            pos -= 1;
            lenght += 1;
        }
        if (pos + lenght != iSchematicLine.length())
        {
            lenght += 1;
        }

        for (int i = pos; i < pos + lenght; i++)
        {
            if (iAdjacentSymbols.iLine[i] != '.') return true;
        }
        return false;
    }
private:
    std::string iSchematicLine;
    SchematicSymbols iAdjacentSymbols;
};

class Schematic
{
public:
    Schematic()
    {

    }
    void AddSchematicLine(const SchematicLine& aSchematicLine)
    {
        iSchematicVector.push_back(aSchematicLine);
    }
    int PartNumberSum()
    {
        int sum = 0;
        for (SchematicLine line : iSchematicVector)
        {
            sum += line.PartNumberSum();
        }
        return sum;
    }

private:
    std::vector<SchematicLine> iSchematicVector;
};

class GearRatios
{
public:
    GearRatios(const std::string& aFileName)
    {
        std::ifstream list(aFileName);

        std::string prev1("");
        std::string prev2("");
        for (std::string line; std::getline(list, line); ) 
        {
            if (prev1 != "")
            {
                SchematicLine schematicLine(prev1);
                SchematicLine nextSchematicLine(line);
                schematicLine += nextSchematicLine;
                if (prev2 != "")
                {
                    SchematicLine prevSchematicLine(prev2);
                    schematicLine += prevSchematicLine;
                }
                iSchematic.AddSchematicLine(schematicLine);
            }
            prev2 = prev1;
            prev1 = line;
        }
        SchematicLine lastSchematicLine(prev1);
        SchematicLine prevSchematicLine(prev2);
        lastSchematicLine += prevSchematicLine;
        iSchematic.AddSchematicLine(lastSchematicLine);

        std::cout << "Day 2.1 : " << iSchematic.PartNumberSum() << std::endl;
    }
private:
    Schematic iSchematic;
};

class RadioGear
{
public:
    RadioGear(
        const std::string& aPrevLine,
        const std::string& aCurLine,
        const std::string& aNextLine)

        : iGearRatio(0)
    {
        for (std::size_t i = 0; i < aCurLine.length();)
        {
            std::size_t found = aCurLine.find_first_of("*", i);
            if (found != std::string::npos)
            {
                std::vector<int> adjustedNumbers;
                GetAdjusted(aPrevLine, found, adjustedNumbers);
                GetAdjusted(aCurLine, found, adjustedNumbers);
                GetAdjusted(aNextLine, found, adjustedNumbers);
                if (adjustedNumbers.size() == 2)
                {
                    iGearRatio += adjustedNumbers[0]*adjustedNumbers[1];
                }
            }
            else break;
            i = found + 1;
        }
    }
    void GetAdjusted(
        const std::string& aLine,
        std::size_t aPos,
        std::vector<int>& aNumbers)
    {
        if (std::isdigit(aLine[aPos]))
        {
            aNumbers.push_back(GetNumberAtPos(aLine, aPos));
        }
        else
        {
            if ((aPos > 0) && std::isdigit(aLine[aPos - 1]))
            {
                aNumbers.push_back(GetNumberFromLeft(aLine, aPos));
            }
            if ((aPos < aLine.length()) && std::isdigit(aLine[aPos + 1]))
            {
                aNumbers.push_back(GetNumberFromRight(aLine, aPos));
            }
        }
    }

    int GetNumberAtPos(const std::string& aLine, std::size_t aPos)
    {
        std::size_t start;
        for (start = aPos; start >= 0; start--)
        {
            if (!std::isdigit(aLine[start]))
            {
                start += 1;
                break;
            }
        }
        std::size_t end;
        for (end = aPos; end < aLine.length(); end++)
        {
            if (!std::isdigit(aLine[end]))
            {
                end -= 1;
                break;
            }
        }
        int result = std::stoi(aLine.substr(start, end));

        return result;
    }
    int GetNumberFromLeft(const std::string& aLine, std::size_t aPos)
    {
        std::size_t start;
        for (start = aPos - 1; start >= 0; start--)
        {
            if (!std::isdigit(aLine[start]))
            {
                start += 1;
                break;
            }
        }
        int result = std::stoi(aLine.substr(start, aPos - start));
        return result;
    }
    int GetNumberFromRight(const std::string& aLine, std::size_t aPos)
    {
        std::size_t end;
        for (end = aPos + 1; end < aLine.length(); end++)
        {
            if (!std::isdigit(aLine[end]))
            {
                end -= 1;
                break;
            }
        }
        int result = std::stoi(aLine.substr(aPos + 1, end - aPos));

        return result;
    }
    int GearRation(void)
    {
        return iGearRatio;
    }
private:
    int iGearRatio;
};


class NewGearRatios
{
public:
    NewGearRatios(const std::string& aFileName)
    {
        int sum = 0;
        std::ifstream list(aFileName);

        std::string prevLine("");
        std::string curLine("");
        for (std::string nextLine; std::getline(list, nextLine); ) 
        {
            if (curLine != "")
            {
                RadioGear radioGear(prevLine, curLine, nextLine);
                sum += radioGear.GearRation();
            }
            prevLine = curLine;
            curLine = nextLine;
        }
        RadioGear radioGear(prevLine, curLine, "");
        sum += radioGear.GearRation();

        std::cout << "Day 2.1 : " << sum << std::endl;
    }
private:
    Schematic iSchematic;
};

int main(int argc, char ** argv)
{
    if (argc < 2) return -1;

    GearRatios game1(argv[1]);
    NewGearRatios game2(argv[1]);

    return 0;
}
