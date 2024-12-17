#include "httplib.h"

using namespace httplib;

	void gen_response(const Request& req, Response& res) {
		res.set_content("Hello, World!", "text/plain");
	}

	int main() {
		Client cli("http://worldtimeapi.org");
		auto res = cli.Get("/api/timezone/Europe/Simferopol");
		if (res) 
		{
			if (res->status == 200) {
				std::cout << res->body << std::endl;
			}
			else {
				std::cout << "Status code: " << res->status << std::endl;
			}
		}
		else {
			auto err = res.error();
			std::cout << "Error code: " << err << std::endl;
		}
	}