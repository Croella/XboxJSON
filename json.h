#pragma once

namespace utils 
{
	namespace json 
	{
		typedef std::map<std::string, std::string> JsonObject;

		inline bool read_file_to_string(const std::string& path, std::string& outContent) {
			std::ifstream file;
            
            XexUtils::Xam::MountHdd();
            file.open(("hdd:/" + path).c_str());

			if (!file.is_open())
				return false;

			std::ostringstream ss;
			ss << file.rdbuf();
			outContent = ss.str();
			file.close();
			return true;
		}

		inline void trim_quotes_and_whitespace(std::string& s) {
			const char* ws = " \t\n\r";
			size_t start = s.find_first_not_of(ws);
			size_t end = s.find_last_not_of(ws);
			if (start == std::string::npos || end == std::string::npos) {
				s.clear();
				return;
			}
			s = s.substr(start, end - start + 1);

			if (s.size() >= 2 && s[0] == '"' && s[s.size()-1] == '"') {
				s = s.substr(1, s.size() - 2);
			}
		}

		inline bool parse_json(const std::string& jsonStr, JsonObject& outObj) {
			outObj.clear();

			size_t start = jsonStr.find_first_of('{');
			size_t end = jsonStr.find_last_of('}');
			if (start == std::string::npos || end == std::string::npos || end <= start)
				return false;

			std::string content = jsonStr.substr(start + 1, end - start - 1);
			std::istringstream ss(content);
			std::string pair;

			while (std::getline(ss, pair, ',')) {
				size_t colon = pair.find(':');
				if (colon == std::string::npos) continue;

				std::string key = pair.substr(0, colon);
				std::string val = pair.substr(colon + 1);

				trim_quotes_and_whitespace(key);
				trim_quotes_and_whitespace(val);

				if (!val.empty() && val[val.size()-1] == ',')
					val.erase(val.size()-1);

				if (!key.empty()) {
					outObj[key] = val;
				}
			}
			return true;
		}

		inline bool write_json(const std::string& path, const JsonObject& obj) {
			std::ostringstream ss;
			ss << "{\n";

			for (JsonObject::const_iterator it = obj.begin(); it != obj.end(); ++it) {
				ss << "  \"" << it->first << "\": \"" << it->second << "\"";

				JsonObject::const_iterator nextIt = it;
				++nextIt;
				if (nextIt != obj.end()) ss << ",";
				ss << "\n";
			}

			ss << "}\n";

			std::string jsonStr = ss.str();
			return utils::filesystem::write_file(path, (void*)jsonStr.c_str(), static_cast<int32_t>(jsonStr.size()));
		}

		inline std::string get_section_key(const std::string& section, const std::string& key) {
			return section + "." + key;
		}

		template<typename T>
		struct FromString {
			static bool convert(const std::string& str, T& outVal) {
				std::istringstream ss(str);
				ss >> outVal;
				if (ss.fail())
					return false;

				char c;
				if (ss >> c) return false;

				return true;
			}
		};

		template<>
		struct FromString<std::string> {
			static bool convert(const std::string& str, std::string& outVal) {
				outVal = str;
				return true;
			}
		};

		template<>
		struct FromString<int> {
			static bool convert(const std::string& str, int& outVal) {
				char* endptr = NULL;
				long val = strtol(str.c_str(), &endptr, 10);
				if (endptr == str.c_str()) return false;
				while (*endptr == ' ' || *endptr == '\t' || *endptr == '\n' || *endptr == '\r')
					++endptr;
				if (*endptr != '\0') return false;
				outVal = static_cast<int>(val);
				return true;
			}
		};

		template<>
		struct FromString<float> {
			static bool convert(const std::string& str, float& outVal) {
				char* endptr = NULL;
				float val = static_cast<float>(strtod(str.c_str(), &endptr));
				if (endptr == str.c_str()) return false;
				while (*endptr == ' ' || *endptr == '\t' || *endptr == '\n' || *endptr == '\r')
					++endptr;
				if (*endptr != '\0') return false;
				outVal = val;
				return true;
			}
		};

		template<>
		struct FromString<bool> {
			static bool convert(const std::string& str, bool& outVal) {
				if (str == "true" || str == "1") {
					outVal = true;
					return true;
				}
				if (str == "false" || str == "0") {
					outVal = false;
					return true;
				}
				return false;
			}
		};

		template<typename T>
		bool get_value(const JsonObject& obj, const std::string& key, T& outVal, const T& defaultVal) {
			JsonObject::const_iterator it = obj.find(key);
			if (it == obj.end()) {
				outVal = defaultVal;
				return false;
			}

			if (!FromString<T>::convert(it->second, outVal)) {
				outVal = defaultVal;
				return false;
			}

			return true;
		}


		extern JsonObject globObj;
	}
}
