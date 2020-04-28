#include "EepromConfig.h"

struct Config
{
    Config() { intValue = 0; }
    int intValue;
};

void setup()
{
    Serial.begin(9600);

    EepromConfigReader<Config> configReader(2, 0);

    Serial.println("Read value: ");
    Serial.print("Version: ");
    Serial.println(configReader.getConfig()->version);

    Serial.print("Data: ");
    Serial.println(configReader.getConfig()->data.intValue);

    Serial.println("Writting data");
    configReader.getConfig()->data.intValue = 7;
    configReader.writeConfig();
}

void loop()
{
}