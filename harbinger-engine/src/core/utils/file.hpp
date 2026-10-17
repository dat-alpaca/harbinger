#pragma once
#include <fstream>
#include <sstream>
#include <filesystem>
#include <nlohmann/json.hpp>

#include "core/containers/containers.hpp"
#include "core/logging/logger.hpp"
#include "core/memory/memory_tag.hpp"

namespace har
{
    inline har_string read_file_contents(const std::filesystem::path& filepath)
    {
        if (!std::filesystem::exists(filepath))
		{
            HAR_LOG_ERROR("Filepath does not exist: {}", filepath.string().c_str());
			return {};
		}

        std::ifstream file(filepath);
        if (!file.is_open())
			HAR_LOG_ERROR("Failed to load file: {}", filepath.string().c_str());

        std::stringstream ss;

        ss << file.rdbuf();
        return har_string { ss.str() };
    }

    template<memory_tag tag>
    inline har_vector<tag, u8> read_file_binary(const std::filesystem::path& filepath)
	{
		if (!std::filesystem::exists(filepath))
		{
            HAR_LOG_ERROR("Filepath does not exist: {}", filepath.string().c_str());
			return {};
		}

		std::ifstream file(filepath, std::ios::binary | std::ios::ate);
		if (!file.is_open())
			HAR_LOG_ERROR("Failed to load file: {}", filepath.string().c_str());

		u64 fileSize = static_cast<u64>(file.tellg());
		har_vector<tag, u8> data(fileSize);

		file.seekg(0);
		file.read(reinterpret_cast<char*>(data.data()), fileSize);

		file.close();
		return data;
	}

	inline nlohmann::json read_json(const std::filesystem::path& filepath)
	{
		std::ifstream file(filepath);
		return nlohmann::json::parse(file);
	}

	template <typename T>
	inline std::optional<T> parse_json(const std::filesystem::path& filepath)
	{
		nlohmann::json jsonObject;
		try
		{
			jsonObject = read_json(filepath);
		}
		catch (nlohmann::json::exception& e)
		{
			HAR_LOG_ERROR("Failed to parse json file at: {}. {}", filepath.string(), e.what());
			return std::nullopt;
		}

		T result;
		try
		{
			result = jsonObject.get<T>();
		}
		catch (nlohmann::json::exception& e)
		{
			HAR_LOG_ERROR("Failed to get element in json file at: {}. {}", filepath.string(), e.what());
			return std::nullopt;
		}

		return result;
	}

	inline void write_json(const std::filesystem::path& filepath, const nlohmann::json& data)
	{
		std::ofstream file(filepath);
		file
            << std::setfill(' ') 
            << std::setw(4)
            << std::setprecision(17)
            << data;
	}
}