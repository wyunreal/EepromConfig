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
    EepromConfigReader(int configVersion, long configLocation)
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

template <class T>
struct SetItem
{
    byte isPresent;
    T item;
};

template <class T>
class EepromCollectionReader;

template <class T>
class EepromCollectionIterator
{
    public:
    bool next()
    {
        for (int i = currentIndex + 1; i < reader->maxSize; i++) {
            currentItem = reader->getItemAtIndex(i);
            if (currentItem.isPresent) {
                currentIndex = i;
                return true;
            }
        }
        currentIndex = reader->maxSize;
        return false;
    }

    T current()
    {
        return currentItem.item;
    }

    private:
    SetItem<T> currentItem;
    int currentIndex;
    EepromCollectionReader<T>* reader;

    friend class EepromCollectionReader<T>;
};

template <class T>
class EepromCollectionReader
{
public:
    EepromCollectionReader(byte configVersion, long collectionLocation, int collectionMaxSize)
    {
        startAddress = collectionLocation;
        maxSize = collectionMaxSize;
        
        byte readVersion = EEPROM.get(startAddress, readVersion);
        if (readVersion != configVersion) {
            SetItem<T> slotItem;
            slotItem.isPresent = 0;
            EEPROM.put(startAddress, configVersion);
            for (int slot = 0; slot < maxSize; slot++) {
                EEPROM.put(startAddress + sizeof(byte) + sizeof(slotItem) * slot, slotItem);
            }
        }
    }
    bool addItem(T item)
    {
        SetItem<T> slotItem;
        for (int slot = 0; slot < maxSize; slot++) {
            EEPROM.get(startAddress + sizeof(byte) + sizeof(slotItem) * slot, slotItem);
            if (!slotItem.isPresent) {
                slotItem.item = item;
                slotItem.isPresent = 1;
                EEPROM.put(startAddress + sizeof(byte) + sizeof(slotItem) * slot, slotItem);
                return true;
            }
        }
        return false;
    }
    void setItemAtIndex(T item, int index)
    {
        SetItem<T> slotItem;
        slotItem.isPresent = 1;
        slotItem.item = item;
        EEPROM.put(startAddress + sizeof(byte) + sizeof(slotItem) * index, slotItem);
    }
    bool removeItem(T item)
    {
        SetItem<T> slotItem;
        for (int slot = 0; slot < maxSize; slot++) {
            EEPROM.get(startAddress + sizeof(byte) + sizeof(slotItem) * slot, slotItem);
            if (slotItem.isPresent && slotItem.item == item) {
                slotItem.isPresent = 0;
                EEPROM.put(startAddress + sizeof(byte) + sizeof(slotItem) * slot, slotItem);
                return true;
            }
        }
        return false;
    }

    SetItem<T> getItemAtIndex(int index)
    {
        SetItem<T> slotItem; 
        EEPROM.get(startAddress + sizeof(byte) + sizeof(slotItem) * index, slotItem);
        return slotItem;
    }

    EepromCollectionIterator<T> *iterate()
    {
        iterator.currentIndex = -1;
        iterator.reader = this;
        return &iterator;
    }

    friend class EepromCollectionIterator<T>;

private:
    long startAddress;
    int maxSize;
    EepromCollectionIterator<T> iterator;
};

#endif
