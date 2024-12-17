#include <iostream>
#include "httplib.h"
#include "json.hpp"
#include <fstream>
#include <string>
#include <cmath>

using namespace httplib;
using json = nlohmann::json;

const std::string API_KEY = "a01c542c437bfc5fa740885c94805dc8";

json weather_cache;
long cache_time = 0;

long get_current_unix_time() {
    Client cli("http://timeapi.io");
    auto res = cli.Get("/api/Time/current/zone?timeZone=Europe/Moscow");
    if (res && res->status == 200) {
        json time_data = json::parse(res->body);
        return time_data["seconds"].get<long>();
    }
    return 0;
}

json fetch_weather_data(const std::string& city) {
    Client cli("http://api.openweathermap.org");
    auto weather_res = cli.Get(("/data/2.5/forecast?q=" + city + "&appid=" + API_KEY + "&units=metric&lang=ru").c_str());

    if (weather_res && weather_res->status == 200) {
        return json::parse(weather_res->body);
    }
    return json();
}

void handle_weather(const Request& req, Response& res) {
    std::string city = req.get_param_value("city");
    if (city.empty()) city = "Moscow";

    long current_time = get_current_unix_time();
    if (weather_cache.empty() || current_time > cache_time + 3600) { 
        weather_cache = fetch_weather_data(city);
        cache_time = current_time;
    }

    if (!weather_cache["list"].is_null()) {
        auto hourly = weather_cache["list"];
        for (const auto& item : hourly) {
            if (item["dt"].get<long>() > current_time) {
                std::ifstream file("widget_template.html");
                if (!file) {
                    res.set_content("Ошибка: шаблон не найден", "text/plain; charset=UTF-8");
                    return;
                }

                std::string html_template((std::istreambuf_iterator<char>(file)),
                    std::istreambuf_iterator<char>());

                if (item.contains("weather") && item["weather"].is_array() &&
                    item.contains("main") && item["main"].contains("temp")) {

                    std::string description = item["weather"][0]["description"];
                    std::string icon = item["weather"][0]["icon"];
                    int temp = std::round(item["main"]["temp"].get<float>());

                    auto replace_placeholder = [](std::string& text, const std::string& placeholder, const std::string& value) {
                        size_t pos;
                        while ((pos = text.find(placeholder)) != std::string::npos) {
                            text.replace(pos, placeholder.length(), value);
                        }
                        };

                    replace_placeholder(html_template, "{description}", description);
                    replace_placeholder(html_template, "{icon}", icon);
                    replace_placeholder(html_template, "{temp}", std::to_string(temp));

                    res.set_content(html_template, "text/html; charset=UTF-8");
                    return;
                }
                else {
                    res.set_content("Ошибка: некорректный формат данных", "text/plain; charset=UTF-8");
                    return;
                }
            }
        }
    }

    res.set_content("Ошибка получения данных о погоде", "text/plain");
}

void handle_raw(const Request& req, Response& res) {
    std::string city = req.get_param_value("city");
    if (city.empty()) city = "Moscow";

    long current_time = get_current_unix_time();
    if (weather_cache.empty() || current_time > cache_time + 3600) {
        weather_cache = fetch_weather_data(city);
        cache_time = current_time;
    }

    if (!weather_cache["list"].is_null()) {
        auto hourly = weather_cache["list"];
        for (const auto& item : hourly) {
            if (item["dt"].get<long>() > current_time) {
                json result = {
                    {"temperature", std::round(item["main"]["temp"].get<float>())},
                    {"description", item["weather"][0]["description"]}
                };
                res.set_content(result.dump(), "application/json");
                return;
            }
        }
    }

    res.set_content("{\"error\":\"Ошибка получения данных\"}", "application/json");
}

int main() {
    setlocale(LC_ALL, "RU");
    Server svr;

    svr.Get("/", handle_weather);
    svr.Get("/raw", handle_raw);

    std::cout << "Сервер запущен на http://localhost:3000\n";
    svr.listen("localhost", 3000);
    return 0;
}
