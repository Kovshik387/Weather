#include <iostream>
#include <string>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

int main() {
    // Пример строки JSON
    string str = R"({
        "pi": 3.141,
        "happy": true,
        "name": "Niels",
        "nothing": null,
        "answer": {
            "everything": 42
        },
        "list": [1, 0, 2],
        "object": {
            "currency": "USD",
            "value": 42.99
        }
    })";

    // Парсим строку в объект JSON
    json j = json::parse(str);

    // Извлекаем значения из JSON
    double pi = j["pi"];
    bool happy = j["happy"];
    string name = j["name"];

    // Выводим полученные данные
    cout << "pi: " << pi << endl;
    cout << "happy: " << happy << endl;
    cout << "name: " << name << endl;

    double value = j["object"]["value"];
    cout << "value: " << value << endl;

    // Получаем значение из вложенного объекта
    cout << "everything: " << to_string(j["answer"]["everything"].get<int>()) << endl;

    // Идем по массиву "list"
    for (int i = 0; i < j["list"].size(); i++) {
        cout << "list[" << i << "] = " << j["list"][i] << endl;
    }

    // Создаем пустой JSON
    json j2;
    j2["num"] = 1;
    j2["array"] = json::array();
    j2["array"].push_back(1);
    j2["array"].push_back(2);
    j2["object"] = json::object();
    j2["object"].push_back({ "PI", pi });
    j2["object"].push_back({ "exp", 2.71 });

    // Преобразуем JSON в строку и выводим
    std::cout << j2.dump(4) << endl;

    return 0;
}
