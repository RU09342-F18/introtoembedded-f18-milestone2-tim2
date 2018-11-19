import time
import serial
import sys

ser = serial.Serial(port='/dev/ttyACM1', baudrate=115200)

counter = 0;
pwm = 0;

while counter < int(sys.argv[1]):
    counter = counter + 1
    if counter % 5000 == 0:
        pwm += 1000;
        if pwm > 9999:
            pwm = 9999
        elif pwm == 9999:
            break;

def readline():
    while 1:
        string = b""
        while 1:
            in_byte = ser.read(1)
            if in_byte == b'\n':
                break
            else:
                string += in_byte

        try:
            return int(string[-6:])
        except ValueError:
            pass



while 1:
    temp = readline()
    #temp = int(ser.readline())

    counter = counter + 1
    if counter % 5000 == 0:
        pwm += 1000;
        if pwm > 9999:
            pwm = 9999
        elif pwm == 9999:
            break;

    pwm_cmd = 'S{:04d}'.format(pwm).encode('ascii')

    ser.write(pwm_cmd)

    ser.flush()

    print("{} {} {}".format(counter, pwm, temp))
    sys.stdout.flush()

ser.close()
