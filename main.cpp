#include <iostream>
#include <sstream>
#include <nlohmann/json.hpp>
using nlohmann::json;

int main()
{
    std::string read = R"([
    ["Si-9.15", "RTS-9.15", "GAZP-9.15"],
    [100024, 100027, 100050],
    ["Futures contract for USD/RUB", "Futures contract for index RTS", "Futures contract for Gazprom shares"]
    ])";
    json data = json::parse(read);
    json array = json::array();
    unsigned int i = 0;
    for (auto j : array)
    {
        json object
            = { { "ticker", data[0][i] }, { "id", data[1][i] }, { "description", data[2][i++] } };
        array.push_back(object);
    }
    return 0;
}
