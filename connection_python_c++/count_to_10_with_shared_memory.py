import mmap
import time

if __name__ == '__main__':
    count = 0
    error_count = 0
    condition = 0
    shm = mmap.mmap(0, 512, "Local\\Test")
    while True:
        print("condion = ", condition)
        if condition == 0:
            shm.seek(0)
            shm.write(bytes(" ", 'UTF-8'))
            shm.write(bytes(str(count), 'UTF-8'))
            print("count = ", count)
            count = int(count) + 2
            condition = 1
        elif condition == 1:
            print(shm.readline())
            print("shm[1] = ", shm[0:6])
            if shm[0:4] == b'next':
                condition = 0
            elif shm[0:6] == b'return':
                count = 0
                condition = 0
        time.sleep(1)
