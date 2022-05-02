#include "Utility.hpp"
#include <curl/curl.h>

size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
	data->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

fetchResult fetch(const std::string& url) {
	auto curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_USERPWD, "user:pass");
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "curl/7.42.0");
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
		curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);		// probably shouldn't do this...

		std::string response_string;
		std::string header_string;
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_string);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

		auto result = curl_easy_perform(curl);
		long response_code = 0;
		if (result != CURLE_OK) {
			Fatal("Curl failed: {}", curl_easy_strerror(result));
		}
		else {
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		}
		curl_easy_cleanup(curl);
		curl = NULL;
		return { response_string, response_code };
	}
	else {
		Fatal("curl init failed");
		return { "" , -1};
	}
}