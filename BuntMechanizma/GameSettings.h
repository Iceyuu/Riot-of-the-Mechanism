// GameSettings.h
#pragma once
#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

#include <fstream>

struct GameSettings {
    float sfxVolume = 100.f;
    float bgmVolume = 100.f;
    bool skipStory = false;

    void save(const std::string& filename = "settings.dat") {
        std::ofstream file(filename);
        if (file.is_open()) {
            file << sfxVolume << " " << bgmVolume << " " << skipStory;
            file.close();
        }
    }

    void load(const std::string& filename = "settings.dat") {
        std::ifstream file(filename);
        if (file.is_open()) {
            file >> sfxVolume >> bgmVolume >> skipStory;
            file.close();
        }
    }
};

#endif