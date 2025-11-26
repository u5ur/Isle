#pragma once
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

namespace Isle
{
    class ScopedTimer
    {
    public:
        ScopedTimer(const std::string& name);
        ~ScopedTimer();

    private:
        std::string m_Name;
        std::chrono::high_resolution_clock::time_point m_StartTime;
    };

    class Profiler
    {
    public:
        static void BeginSection(const std::string& name);
        static void EndSection(const std::string& name);
        static void PrintResults();
        static void Clear();

    private:
        struct TimingData
        {
            double totalMs = 0.0;
            int callCount = 0;
            std::chrono::high_resolution_clock::time_point startTime;
        };

        static std::unordered_map<std::string, TimingData> s_Timings;
        static std::vector<std::pair<std::string, double>> s_OrderedResults;
    };

    inline ScopedTimer::ScopedTimer(const std::string& name)
        : m_Name(name)
    {
        m_StartTime = std::chrono::high_resolution_clock::now();
    }

    inline ScopedTimer::~ScopedTimer()
    {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - m_StartTime);
        double ms = duration.count() / 1000.0;

        ISLE_LOG("[PROFILE] %s: %.2fms\n", m_Name.c_str(), ms);
    }

    inline std::unordered_map<std::string, Profiler::TimingData> Profiler::s_Timings;
    inline std::vector<std::pair<std::string, double>> Profiler::s_OrderedResults;

    inline void Profiler::BeginSection(const std::string& name)
    {
        auto& data = s_Timings[name];
        data.startTime = std::chrono::high_resolution_clock::now();
    }

    inline void Profiler::EndSection(const std::string& name)
    {
        auto it = s_Timings.find(name);
        if (it == s_Timings.end())
            return;

        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - it->second.startTime);
        double ms = duration.count() / 1000.0;

        it->second.totalMs += ms;
        it->second.callCount++;
    }

    inline void Profiler::PrintResults()
    {
        s_OrderedResults.clear();

        for (const auto& [name, data] : s_Timings)
        {
            s_OrderedResults.push_back({ name, data.totalMs });
        }

        std::sort(s_OrderedResults.begin(), s_OrderedResults.end(),
            [](const auto& a, const auto& b) { return a.second > b.second; });

        ISLE_LOG("=== Profiler Results ===\n");
        for (const auto& [name, ms] : s_OrderedResults)
        {
            const auto& data = s_Timings[name];
            ISLE_LOG("%s: %.2fms (calls: %d, avg: %.2fms)\n",
                name.c_str(), ms, data.callCount, ms / data.callCount);
        }
        ISLE_LOG("========================\n");
    }

    inline void Profiler::Clear()
    {
        s_Timings.clear();
        s_OrderedResults.clear();
    }
}