import spidev
import time
import requests
from requests.exceptions import HTTPError

def server_sendLightData(newData):
    try:
        r = requests.get(url="http://pi-wireless:8000/submitLightValue/{}".format(newData[0]))
        data = r.json()
        average = data['Average']
        print(data)
        return average
        
    except requests.exceptions.RequestException as e:
        print(e)
        sys.exit(1)

        

def createSPI(device):
    spi = spidev.SpiDev()
    spi.open(0, device)
    spi.max_speed_hz = 1000000

    spi.mode = 0
    return spi


def WelfordsAlgorithm(newLightValue):
    _numValues = 0
    _mean = 0
    _s = 0
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

        while True:
            newLightValue = spi0.readbytes(1)
            average = server_sendLightData(newLightValue)
            #average = WelfordsAlgorithm(newLightValue)

            spi1.writebytes([int(average)])
           # print("value:")
           # print(average)
            time.sleep(1)

            #time.sleep(0.5)

    except KeyboardInterrupt:
        spi0.close()
        spi1.close()
        exit()
