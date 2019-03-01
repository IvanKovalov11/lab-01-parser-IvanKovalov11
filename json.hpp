#include <iostream>
#include <any>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <fstream>

class Json
{
private:
	std::unordered_map<std::string, std::any> map;
	std::vector<std::any> array;

	enum type { object, Array, null };

	type type_;

	void key_(std::istringstream& stream, std::string& key) {
		char c;
		while (stream >> c && c != '"') {
			if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) throw std::logic_error("input_error");
			else key.push_back(c);
		}
	};
	void value_(std::istringstream& stream, std::any& value, bool& success) {
		char c;
		if (!(stream >> c)) throw std::logic_error("input_error");
		std::string val;
		switch (c)
		{
		case 'f':
		{
			while (stream >> c && c != ',' && c != '}' && c != ']') {
				val.push_back(c);
			}
			if (val == "alse") {
				value = false;
			}
			else {
				throw std::logic_error("input_error");
			}
			if (c == '}' || c == ']') {
				success = false;
			}
			else if (c != ',') {
				throw std::logic_error("input_error");
			}
			break;
		}
		case 't':
		{
			while (stream >> c && c != ',' && c != '}' && c != ']') {
				val.push_back(c);
			}
			if (val == "rue") {
				value = true;
			}
			else {
				throw std::logic_error("input_error");
			}
			if (c == '}' || c == ']') {
				success = false;
			}
			else if (c != ',') {
				throw std::logic_error("input_error");
			}
			break;
		}
		case 'n':
		{
			while (stream >> c && c != ',' && c != '}' && c != ']') {
				val.push_back(c);
			}
			if (val == "ull") {
				value = null;
			}
			else {
				throw std::logic_error("input_error");
			}
			if (c == '}' || c == ']') {
				success = false;
			}
			else if (c != ',') {
				throw std::logic_error("input_error");
			}
			break;
		}
		case '"':
		{
			while (stream >> c && c != '"') {
				if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) throw std::logic_error("input_error");
				else val.push_back(c);
			}
			value = val;
			stream >> c;
			if (c == '}' || c == ']') {
				success = false;
			}
			else if (c != ',') {
				throw std::logic_error("input_error");
			}
			break;
		}
		case '[':
		{
			Json J;
			J.array = J.parse_arr(stream);
			J.type_ = Array;
			value = J;
			if (stream >> c) {
				if (c == '}' || c == ']') {
					success = false;
				}
				else if (c != ',') {
					throw std::logic_error("input_error");
				}
			}
			break;
		}
		case '{':
		{
			Json J;
			J = pars_obj(stream);
			J.type_ = object;
			value = J;
			stream >> c;
			if (c == '}' || c == ']')
				success = false;
			else if (c != ',') throw std::logic_error("input_error");
			break;
		}
		default:
		{
			if (c == ']')
			{
				success = false;
				break;
			}
			if (isdigit(c)) {
				val.push_back(c);
			}
			else {
				throw std::logic_error("input_error");
			}
			bool ISdouble = false;
			while (stream >> c && c != ',' && c != '}' && c != ']') {
				if (c == '.') {
					if (ISdouble) throw std::logic_error("input_error");
					else {
						ISdouble = true;
						val.push_back(c);
					}
				}
				else if (isdigit(c)) val.push_back(c);
			}
			std::istringstream str(val);
			if (ISdouble) {
				double result = 0;
				str >> result;
				value = result;
			}
			else {
				int result = 0;
				str >> result;
				value = result;
			}
			if (c == '}' || c == ']') {
				success = false;
			}
			else if (c != ',') {
				throw std::logic_error("input_error");
			}
			break;
		}
		}
	};
	std::vector<std::any> parse_arr(std::istringstream& stream) {
		std::any value;
		std::vector<std::any> vect;
		bool success = true;
		while (success)
		{
			value_(stream, value, success);
			vect.push_back(value);
			value.reset();
		}
		return vect;
	};
	Json pars_obj(std::istringstream& stream) {
		Json data;
		bool success = true;
		char c;
		while (success) {
			if (stream >> c && c != '"') {
				if (c == '}') break;
				else throw std::logic_error("input_error");
			}
			std::string key;
			key_(stream, key);
			if (!(stream >> c && c == ':')) throw std::logic_error("input_error");
			std::any value;
			value_(stream, value, success);
			data.map.insert(std::pair<std::string, std::any>(key, value));
		}
		return data;
	};

public:

	Json() { type_ = null; }

	// Конструктор из строки, содержащей Json-данные.
	Json(const std::string& s) {
		std::string file_symbols;
		for (unsigned i = 1; i < 5; ++i) {
			file_symbols.push_back(s[s.size() - i]);
		}
		Json J;
		if (file_symbols == "txt.") J = parseFile(s);
		else J = parse(s);
		this->array = J.array;
		this->map = J.map;
		this->type_ = J.type_;
	};

	// Метод возвращает true, если данный экземпляр содержит в себе JSON-массив. Иначе false.
	bool is_array() const {
		if (type_ == Array) return true;
		else return false;
	};

	// Метод возвращает true, если данный экземпляр содержит в себе JSON-объект. Иначе false.
	bool is_object() const {
		if (type_ == object) return true;
		else return false;
	};

	// Метод возвращает true, если данный экземпляр не содержит ничего, т.е. null. Иначе false.
	bool is_null() const {
		if (type_ == null) return true;
		else return false;
	};

	// Метод возвращает значение по ключу key, если экземпляр является JSON-объектом.
	// Значение может иметь один из следующих типов: Json, std::string, double, bool или быть пустым.
	// Если экземпляр является JSON-массивом, генерируется исключение.
	const std::any& operator[](const std::string& key) const {
		return map.at(key);
	};

	// Метод возвращает значение по индексу index, если экземпляр является JSON-массивом.
	// Значение может иметь один из следующих типов: Json, std::string, double, bool или быть пустым.
	// Если экземпляр является JSON-объектом, генерируется исключение.
	const std::any& operator[](int index) const {
		return array.at(index);
	};

	// Метод возвращает объект класса Json из строки, содержащей Json-данные.
	Json parse(const std::string& s) {
		std::istringstream stream(s);
		char c;
		stream >> c;
		switch (c)
		{
		case('{'): {
			Json obj;
			obj = obj.pars_obj(stream);
			obj.type_ = object;
			return obj;
		}
		case('['): {
			Json arr;
			arr.array = arr.parse_arr(stream);
			arr.type_ = Array;
			return arr;
		}
		default:
			throw std::logic_error("input_error");
			break;
		}
	};

	// Метод возвращает объекта класса Json из файла, содержащего Json-данные в текстовом формате.
	Json parseFile(const std::string& path_to_file) {
		std::ifstream file(path_to_file);
		if (!file.is_open())
			throw std::logic_error("input_error");
		else
		{
			std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();
			return parse(data);
		}
	};

};
