#ifndef JSONHELPER_HPP
#define JSONHELPER_HPP

#include <string>
#include <dist/json/json.h>

/// Set of useful functions for manipulating json.
/// This class is not meant to be instanciated, because it has only static methods.
class JsonHelper
{
	public:
		/// Creates a json value from a file.
		/// \param filePath Path of the file to load.
		/// \result The resulting json value represented in the file.
		/// \see saveToFile
		static Json::Value loadFromFile(const std::string& filePath);

		/// Saves a json value to a file.
		/// The file is truncated if it already exists.
		/// This operation is very easy, but is provided for consistency
		/// with \a loadFromFile.
		/// \param value The json value to save.
		/// \param filePath Path of the file.
		/// \see loadFromFile
		static void saveToFile(const Json::Value& value, const std::string& filePath);

		/// Check if \a value is conformant to \a model.
		/// If not, raise an exception.
		/// The value is not const because some of its childs can have default value,
		/// so this function set missing values to default (effectively modifying the value).
		/// \param value Value to check.
		/// \param model Description model.
		static void parse(Json::Value& value, const Json::Value& model);

		/// Merges two Json values in one.
		/// Every member of the right value will be copied into the left value.
		/// \param left A json value.
		/// \param right Another json value
		/// \throw std::invalid_argument if the merging is not possible
		static void merge(Json::Value& left, const Json::Value& right);

		/// Checks if the data inside the left value is too in the right value.
		/// If this method returns true, then merging left and right will result in right.
		/// \param left A json value.
		/// \param right Another json value.
		/// \return true if left is a subset of right, false otherwise.
		static bool isSubset(const Json::Value& left, const Json::Value& right);

		/// Recursively checks if two values are strictly equal.
		/// \param left A json value.
		/// \param right Another json value.
		/// \return true if left is strictly equal to the right, false otherwise.
		static bool isEqual(const Json::Value& left, const Json::Value& right);

		/// Strip the data in left value that is also present in right value.
		/// This can be viewed as a set substraction.
		/// \pre isSubset(right, left).
		/// \post isEqual(merge(<left after call>, right), <left before call>).
		/// \param left A json value.
		/// \param right Another json value.
		static void substract(Json::Value& left, const Json::Value& right);

	private:
		/// Return a string depending of the given \a type.
		/// \param type Type to encode in a string.
		/// \return A formated string.
		static std::string typeToStr(Json::ValueType type);

		/// Return a Json::ValueType from the the given \a string.
		/// \param str String to decode.
		/// \return A Json type.
		static Json::ValueType strToType(const std::string& str);

		/// Check if every element in \a objects corresponds to one element in \a valuesTypes, throw an exception otherwise.
		/// If all elements in the object don't have the right type, an exception is raised.
		/// \param object A Json value containing the data, it must be an object.
		/// \param name The name of the object.
		/// \param valuesTypes The mapping between elements names and their types.
		static void parseObject(const Json::Value& object, const std::string& name, std::map<std::string, Json::ValueType> valuesTypes);

		/// Check if every element listed in \a valuesTypes exists in \a object and has the right type.
		/// If all elements are not found in the object, an exception is raised.
		/// \param object A Json value containing the data, it must be an object.
		/// \param name The name of the object.
		/// \param valuesTypes The mapping between elements names and their types.
		static void requireValues(const Json::Value& object, const std::string& name, std::map<std::string, Json::ValueType> valuesTypes);

		/// Check if every element in \a value have the type \a type.
		/// If all elements in the object don't have the right type, an exception is raised.
		/// \param object A Json value containing the data, it must be an object.
		/// \param name The name of the object.
		/// \param type The expected type of every element in \a object.
		static void parseObject(const Json::Value& object, const std::string& name, Json::ValueType type);

		/// Check if \a value corresponds to one element in \a values, throw an exception otherwise.
		/// \param value A Json value.
		/// \param name The name of the value.
		/// \param values The list of every possible value of \a value.
		static void parseValue(const Json::Value& value, const std::string& name, std::vector<Json::Value> values);

		/// Check if \a value have the type \a type.
		/// If value don't have the right type, an exception is raised.
		/// \param value A Json value.
		/// \param name The name of the value.
		/// \param type The expected type of \a value.
		static void parseValue(const Json::Value& value, const std::string& name, Json::ValueType type);

		/// Check if every element in \a array corresponds to one element in \a values, throw an exception otherwise.
		/// \param array A Json value containing the data, it must be an array.
		/// \param name The name of the array.
		/// \param values The elements expecteds in \a array.
		static void parseArray(const Json::Value& array, const std::string& name, std::vector<Json::Value> values);

		/// Check if every element in \a value have the type \a type.
		/// If all elements in the array don't have the right type, an exception is raised.
		/// \param array A Json value containing the data, it must be an array.
		/// \param name The name of the array.
		/// \param type The expected type of every element in \a array.
		static void parseArray(const Json::Value& array, const std::string& name, Json::ValueType type);

		/// Check if \a value is conformant to \a model.
		/// If not, raise an exception.
		/// The value is not const because some of its childs can have default value,
		/// so this function set missing values to default (effectively modifying the value).
		/// This is the concrete recursive function called by parse
		/// \param value Value to check.
		/// \param model Description model.
		/// \param valueName The name of the value.
		/// \param modelName The name of the model value.
		static void parseImpl(Json::Value& value, const Json::Value& model, const std::string& valueName, const std::string& modelName);
};

#endif//JSONHELPER_HPP
