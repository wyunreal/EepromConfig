#ifndef EEPROM_CONFIG_H
#define EEPROM_CONFIG_H

#include <Arduino.h>
#include <EEPROM.h>

template <class T>
struct ConfigData
{
    byte version;
    T data;
};

template <class T>
class EepromConfigReader
{
public:
    EepromConfigReader(int configVersion, long configLocation) : config()
    {
        codeVersion = configVersion;
        startAddress = configLocation;
        readConfig();
    }
    void writeConfig()
    {
        EEPROM.put(startAddress, config.version);
        EEPROM.put(startAddress + sizeof(config.version), config.data);
    }
    ConfigData<T> *getConfig()
    {
        return &config;
    }

private:
    ConfigData<T> *readConfig()
    {
        EEPROM.get(startAddress, config.version);
        if (config.version == codeVersion)
        {
            EEPROM.get(startAddress + sizeof(config.version), config.data);
        }
        else
        {
            config.version = codeVersion;
            writeConfig();
        }
        return &config;
    }
    ConfigData<T> config;
    long startAddress;
    int codeVersion;
};

#endif