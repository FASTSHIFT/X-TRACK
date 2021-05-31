#ifndef __STORAGE_SERVICE_H
#define __STORAGE_SERVICE_H

#include <stdint.h>
#include <vector>

class StorageService {
public:
    typedef enum {
        TYPE_UNKNOW,
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_DOUBLE,
        TYPE_STRING
    }DataType_t;

public:
    StorageService(const char* filepath);
    ~StorageService();

    bool Add(const char* key, void* value, uint16_t size, DataType_t type);
    bool Remove(const char* key);
    bool SaveFile();
    bool LoadFile();

private:
    typedef struct {
        const char* key;
        void* value;
        uint16_t size;
        DataType_t type;
    }Node_t;

private:
    const char* FilePath;
    std::vector<Node_t*> NodePool;

private:
    Node_t* SearchNode(const char* key);
};

#endif
