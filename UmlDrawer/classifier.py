#import pandas as pd
import os
import numpy as np
import tensorflow as tf
import time
import sys

path = os.path.abspath(os.getcwd())
print("active directory: "+str(path))
print("files in ACTIVE DIRECTORY:")
print(os.listdir())

loadModelPath = os.path.join(path, "PyModel")
inStream =  open("toPy.txt", "r")
outStream =  open("toCpp.txt","w") # "wa" ?

print("loading model from " + loadModelPath)
model = None 
try:
    model = tf.keras.models.load_model(loadModelPath)
except:
    print("ERROR: couldn't find tensorflow model at" + loadModelPath)
    sys.exit(1)

try_indefinitely = True
try_times_initial = 3000
try_times = try_times_initial
buf = ""
data_end_token = "_COMPLETE_"
session_end_token = "_DO_SHUTDOWN_"
classifier_started_token = "_CLASSIFIER_STARTED_"
# tell the painter, we are ready 
print("writing '" + classifier_started_token + "' message to painter componenet");
outStream.write(classifier_started_token + "\n")
outStream.flush()
print("wrote")

# start accepting drawings
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
    if(buf.count(session_end_token) > 0):
        print("read '" + session_end_token + "', which means it's time to stop. GG")
        inStream.close()
        outStream.close()
        sys.exit(0)
    if(line.count(data_end_token) == 0):
        print("read parial data, len(data): " + str(len(line))+ ". Characters buffered: " + str(len(buf)))
        continue
    # the data is complete, so we cut the complete data from buf and proceed.
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
