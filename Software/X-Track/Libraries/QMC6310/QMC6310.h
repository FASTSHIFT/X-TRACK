#ifndef __QMC6310_H__

#define __QMC6310_H__

#include "Arduino.h"

#define DO_SELF_TEST_ON_INIT 1
#define USE_CON_MODE_AS_DEFAULT 0

#define QMC6310U

#ifdef QMC6310U
#define QMC6310_ADDR 0x1C
#endif

#ifdef QMC6310N
#define QMC6310_ADDR 0x3C
#endif

class QMC6310
{
public:
   template <typename T> struct vector
    {
        T x, y, z;
    };

    typedef enum QMC6310_MODE
    {
        QMC6310_SUSPEND_MODE     = 0b00000000,
        QMC6310_NORMAL_MODE      = 0b00000001,
        QMC6310_SINGLE_MODE      = 0b00000010,
        QMC6310_CONTINUOUS_MODE  = 0b00000011,
    } QMC6310_MODE_t;

    typedef enum QMC6310_ODR
    {
        QMC6310_ODR_10_HZ  = 0b00000000,
        QMC6310_ODR_50_HZ  = 0b00000100,
        QMC6310_ODR_100_HZ = 0b00001000,
        QMC6310_ODR_200_HZ = 0b00001100,
    } QMC6310_ODR_t;

    vector<int16_t> m;

    uint8_t last_status;

    QMC6310(void);
    ~QMC6310() {}

    bool init(uint8_t addr = QMC6310_ADDR);
    void enableDefault(void);

    bool read(void);

    void setTimeout(uint16_t timeout);
    uint16_t getTimeout(void);
    bool timeoutOccurred(void);


    // vector functions
    template <typename Ta, typename Tb, typename To> static void vector_cross(const vector<Ta> *a, const vector<Tb> *b, vector<To> *out);
    template <typename Ta, typename Tb> static float vector_dot(const vector<Ta> *a, const vector<Tb> *b);
    static void vector_normalize(vector<float> *a);

private:
    uint8_t address;
    
    uint16_t io_timeout;
    bool did_timeout;

    void WriteReg(uint8_t reg, uint8_t data);
    uint8_t ReadReg(uint8_t reg);

};

template <typename Ta, typename Tb, typename To> void QMC6310::vector_cross(const vector<Ta> *a, const vector<Tb> *b, vector<To> *out)
{
    out->x = (a->y * b->z) - (a->z * b->y);
    out->y = (a->z * b->x) - (a->x * b->z);
    out->z = (a->x * b->y) - (a->y * b->x);
}

template <typename Ta, typename Tb> float QMC6310::vector_dot(const vector<Ta> *a, const vector<Tb> *b)
{
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

#endif
