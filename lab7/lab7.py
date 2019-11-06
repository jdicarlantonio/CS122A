import spidev
import time

_numValues = 0
_mean = 0
_s = 0

def createSPI(device):
    spi = spidev.SpiDev()
    spi.open(0, device)
    spi.max_speed_hz = 1000000

    spi.mode = 0
    return spi

def WelfordsAlgorithm(newLightValue):
    global _numValues
    global _mean
    global _s

    _numValues += 1
    if _numValues == 1:
        _mean = newLightValue
        _s = 0
    else:
        _oldMean = _mean
        _mean = _oldMean + (newLightValue - _oldMean) / _numValues
        _s = _s + ((newLightValue - _oldMean) * (newLightValue - _mean))
    return _mean

if __name__ == '__main__':
    try:

        spi0 = createSPI(0)
        spi1 = createSPI(1)

        average = [0]
        while True:

            newLightValue =  spi0.xfer([0x10])
            average[0] = int(WelfordsAlgorithm(newLightValue[0]))
           
            #spi0.writebytes([0x10])
            spi1.writebytes([0x40])
            spi1.writebytes(average)
            time.sleep(1)
            spi1.writebytes([0x20])
            spi1.writebytes(newLightValue)
            print("average:")
            print(average)
            print("value:")
            print(newLightValue)
            time.sleep(1)

            #time.sleep(0.5)

    except KeyboardInterrupt:
        spi0.close()
        spi1.close()
        exit()
