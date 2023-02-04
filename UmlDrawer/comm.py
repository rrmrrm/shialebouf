#import pandas as pd
import os
import numpy as np
import tensorflow as tf
import time
import sys

print("active directory before:")
print(os.path.dirname(sys.argv[0]))
os.chdir(os.path.dirname(sys.argv[0]))
print("changed ACTIVE DIRECTORY to:")
print(os.path.dirname(sys.argv[0]))
print("files in ACTIVE DIRECTORY:")
print(os.listdir())

loadModelPath = "PyModel"
inStream =  open("toPy.txt", "r")
outStream =  open("toCpp.txt","w") # "wa" ?

print("loading model from " + loadModelPath)
model = tf.keras.models.load_model(loadModelPath)
try_indefinitely = True
try_times_initial = 3000
try_times = try_times_initial
buf = ""
data_end_token = "_COMPLETE_"
while(True):
    line = inStream.readline()
    if(not try_indefinitely and try_times <= 0):
        print("no tries left. fail")
        inStream.close()
        outStream.close()
        exit()
    # if the other process didn't write to the file, we try again
    if(line == ""):
        print(".")
        time.sleep(1.0)
        try_times -= 1
        continue
    # reset try_times on succesful read 
    try_times = try_times_initial
    # append what we read to buf
    buf += line
    if(line.count(data_end_token) == 0):
        print("read parial data, len(data): " + str(len(line))+ ". Characters buffered: " + str(len(buf)))
        continue
    # the data is complete, so we cut the data from buf and proceed.
    (line, buf) = buf.split( sep=data_end_token, maxsplit=1)
    #val_cnt = float(line)
    #line = inStream.readline()
    val_strs = line.split(',')
    vals = [float(str) for str in val_strs]
    prediction = model.predict(np.array([vals]))[0]
    print("test prediction:", prediction)
    print("sending prediction to outStream")
    for v in prediction:
        outStream.write(str(v)+" ")
    outStream.write("\n")
    outStream.flush()
inStream.close()
outStream.close()