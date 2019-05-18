#pragma once
#include <string>
#include <random>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <algorithm>

void print(const std::vector<std::string>& path)
{
    std::string s = "/";
    for(const auto& e : path)
        s += e + "/";

    std::cerr << s << std::endl;
}

struct Random
{
        std::mt19937 gen;

        Random():
            gen(0)
        {

        }

        // Found on stackoverflow
        std::string random_string( size_t length )
        {
            auto randchar = []() -> char
            {
                    static const char charset[] =
                    "0123456789"
                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                    "abcdefghijklmnopqrstuvwxyz";
                    const size_t max_index = (sizeof(charset) - 1);
                    return charset[ rand() % max_index ]; };
            std::string str(length,0);
            std::generate_n( str.begin(), length, randchar );
            return str;
        }

        int getRandomInt()
        {
            using namespace std;
            static uniform_int_distribution<int>
                    dist(numeric_limits<int>::lowest(),
                         numeric_limits<int>::max());

            return dist(gen);
        }

        uint32_t getRandomUInt()
        {
            using namespace std;
            static uniform_int_distribution<uint32_t>
                    dist(numeric_limits<uint32_t>::lowest(),
                         numeric_limits<uint32_t>::max());

            return dist(gen);
        }

        float getRandomFloat()
        {
            using namespace std;
            static uniform_real_distribution<float>
                    dist(numeric_limits<float>::lowest(),
                         numeric_limits<float>::max());

            return dist(gen);
        }


        std::string getRandomString()
        {
            using namespace std;
            static uniform_int_distribution<int>
                    dist(1,
                         15);

            return random_string(dist(gen));
        }


        std::string getRandomAddress()
        {
          std::string str;
          // TODO average depth from stats
          for(int i = 0; i < std::abs(rand()) % 10; i++)
          {
            str.append("/");
            str.append(random_string(1 + rand() % 20));
          }
          return str;
        }

        std::vector<std::vector<std::string>> getRandomAddresses(int num)
        {
            std::vector<std::vector<std::string>> dev;
            dev.push_back({"/"});
            for(int i = 0; i < num; i++)
            {
                int num = rand() % dev.size();
                auto randompath = dev[num];

                randompath.push_back(random_string(1 + rand() % 20));

                dev.push_back(randompath);
            }

            // Remove "/" at the beginning of each address
            for(std::size_t i = 0; i < dev.size(); i++)
            {
                dev[i].erase(dev[i].begin());
            }

            // Remove the root address
            dev.erase(dev.begin());
            return dev;
        }

};
