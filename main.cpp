#include <vector>
#include <string>
#include <map>
#include "json11.hpp" // Asegúrate de tener esta biblioteca en tu sistema
#include "jsonlib.hpp" // No estoy seguro de qué biblioteca es esta, asegúrate de tenerla en tu sistema
using namespace std;

enum variant_type { Symbol, Number, List, Proc, Lambda, Cadena };

struct Entorno;

class Variant {
public:
    typedef Variant(*proc_type) ( const vector<Variant>& );
    typedef vector<Variant>::const_iterator iter;
    typedef map<string, Variant> map;

    variant_type type;
    string val;
    vector<Variant> list;
    proc_type proc;
    Entorno* env;

    Variant(variant_type type = Symbol) : type(type) , env(0), proc(0) { }
    Variant(variant_type type, const string& val) : type(type), val(val) , env(0) , proc(0) { }
    Variant(proc_type proc) : type(Proc), proc(proc) , env(0) { }

    string to_string();
    string to_json_string();
    static Variant from_json_string(string json);
    static Variant parse_json(jsonlib::Json job);  // Asegúrate de que esta es la definición correcta de la función
};
string Variant::to_string() {
    switch (type) {
        case Symbol:
            return val;
        case Number:
            return std::to_string(std::stod(val)); // Convertir el valor numérico a string
        case List: {
            string result = "(";
            for (const auto& elem : list) {
                result += elem.to_string() + " ";
            }
            if (!list.empty()) {
                result.pop_back(); // Eliminar el espacio extra al final
            }
            result += ")";
            return result;
        }
        case Proc:
            return "Proc";
        case Lambda:
            return "Lambda";
        case Cadena:
            return val; // Devolver el valor de la cadena
        default:
            return "UnknownType";
    }
}

string Variant::to_json_string() {
    switch (type) {
        case Symbol:
        case Cadena:
            return "\"" + val + "\"";
        case Number:
            return val;
        case List: {
            string result = "[";
            for (const auto& elem : list) {
                result += elem.to_json_string() + ",";
            }
            if (!list.empty()) {
                result.pop_back(); // Eliminar la coma extra al final
            }
            result += "]";
            return result;
        }
        case Proc:
            return "\"Proc\"";
        case Lambda:
            return "\"Lambda\"";
        default:
            return "\"UnknownType\"";
    }
}

Variant Variant::from_json_string(std::string sjson) {
    string err;
    json11::Json json = json11::Json::parse(sjson, err);
    if (!err.empty()) {
        // Manejar el error de análisis JSON
        return Variant();
    }
    return parse_json(jsonlib::Json(json)); // Convertir json11::Json a jsonlib::Json y llamar a parse_json
}

Variant Variant::parse_json(jsonlib::Json job) {
    if (job.is_number()) {
        return Variant(Number, job.to_string());
    } else if (job.is_string()) {
        return Variant(Cadena, job.to_string());
    } else if (job.is_array()) {
        Variant result(List);
        for (const auto& elem : job.array_items()) {
            result.list.push_back(parse_json(elem));
        }
        return result;
    } else if (job.is_object()) {
        // Puedes manejar objetos JSON según tus necesidades
    }

    // Por defecto, devolver una instancia vacía
    return Variant();
}

int main() {
    // Ejemplo de uso
    Variant v1(Symbol, "hello");
    Variant v2(Number, "42.0");
    Variant v3(List, {v1, v2});
    Variant v4(proc_type([](const std::vector<Variant>& args) {
        return Variant(Number, "84.0");
    }));

    cout << v1.to_string() << endl;
    cout << v2.to_string() << endl;
    cout << v3.to_string() << endl;
    cout << v4.to_string() << endl;

    string json_str = v3.to_json_string();
    cout << json_str << endl;

    Variant v5 = Variant::from_json_string(json_str);
    cout << v5.to_string() << endl;

    return 0;
}