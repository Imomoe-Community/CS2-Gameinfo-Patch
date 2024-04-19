#include <exception>
#include <filesystem>
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <string>
#include <vector>

static bool CheckEntry(std::string file)
{
    std::ifstream inputFile(file);
    std::string line;
    
    while (std::getline(inputFile, line))
    {
        if (line.find("metamod") != std::string::npos)
        {
            std::cerr << "ERROR: Metamod:Source entry already exists." << std::endl;
            return true;
        }
    }

    inputFile.close();

    return false;
}

static bool UpdateFile(std::string file)
{
    const std::string pattern = "Game_LowViolence";
    const std::string metamodEntry = "\n			Game	csgo/addons/metamod";

    if (std::filesystem::exists(file + ".bak"))
    {
        std::filesystem::remove(file + ".bak");
    }
    std::filesystem::copy(file, file + ".bak");
    std::ifstream inputFile(file);

    if (!inputFile.is_open())
    {
        std::cerr << "ERROR: Unable to open file." << std::endl;
        return false;
    }

    std::vector<std::string> fileContent;
    std::string line;
    while (std::getline(inputFile, line))
    {
        fileContent.push_back(line);
    }
    inputFile.close();

    
    // auto == std::vector<std::string>::iterator
    auto it = std::find_if(fileContent.begin(), fileContent.end(),
                           [&](const std::string & str)
    {
        return str.find(pattern) != std::string::npos;
    });

    if (it == fileContent.end())
    {
        std::cerr << "ERROR: Pattern not found in file." << std::endl;
        return false;
    }

    fileContent.insert(it + 1, metamodEntry);
    std::ofstream outputFile(file);
    
    if (!outputFile.is_open())
    {
        std::cerr << "ERROR: Unable to write file." << std::endl;
        return false;
    }

    // auto == std::vector<std::string>::iterator::value_type
    for (const auto & line : fileContent)
    {
        outputFile << line << '\n';
    }
    outputFile.close();

    return true;
}

int main(int argc, const char * const * argv)
{
    std::string filePath;
    if (argc > 1)
    {
        filePath = argv[1];
        if (!std::filesystem::exists(filePath))
        {
            std::cerr << "ERROR: Specified file path does not exist." << std::endl;
            return 1;
        }
    }
    else
    {
        filePath = "./game/csgo/gameinfo.gi";
        if (!std::filesystem::exists(filePath))
        {
            std::cerr << "ERROR: Gameinfo.gi does not exist." << std::endl;
            return 1;
        }
    }

    try
    {
        if (CheckEntry(filePath))
        {
            return 2;
        }
        UpdateFile(filePath);
    }
    catch (const std::exception & e)
    {
        std::cerr << "ERROR: An error occurred: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
