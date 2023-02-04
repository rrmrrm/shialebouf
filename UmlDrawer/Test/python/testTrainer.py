from math import fabs
import sys
import os
import subprocess
import numpy as np
import tensorflow as tf
# the dir of this program
test_executable_dir = os.getcwd()
# trainer's path relative to this executable 
#trainer_relative_path = "../../trainer.py"

#trainer_full_path = os.path.join(test_executable_dir, trainer_relative_path)

#evaluat model loaded from PyModel directory  on small samples
def test1():
    print("run test1 ...")
    # test_data_path: directory of testdatas
    #test_data_dir = test_executable_dir
    # load data which is (drawing, label) pairs
    data_path = "drawingsInCsvForTest/drawings.csv"
    #data_path = os.path.join(test_data_dir, 'simpleSamples.csv')
    #tf_model_target_path = os.path.join(test_data_dir, 'tfModelDir')
    #print("starting trainer")
    #input_size = 16
    input_size = 30 * 30
    #trainer_args = [
    #    input_size
    #    , 4
    #    , 100
    #    , 0 # no test data
    #    , 1
    #    , data_path
    #    , os.path.join(test_data_dir, 'testNetwork.txt')
    #    , tf_model_target_path
    #    , 0
    #    , ''
    #]
    #trainer_args = [str(arg) for arg in trainer_args]
    #python_executable = 'python3' # sys.executable might be better
    
    #cmd = [python_executable] + ' ' + trrainer_full_path] + trainer_args
    #with subprocess.Popen(cmd, stdout=subprocess.PIPE) as proc:
    #    print(proc.stdout.read())
    #print("trainer finished ")

    # load into 'model' the model that we created and saved
    #print("loading model from " + tf_model_target_path)
    #model = tf.keras.models.load_model(tf_model_target_path) 
    tf_model_loadPath = "PyModel"
    model = tf.keras.models.load_model(tf_model_loadPath)
    print("loading model from " + tf_model_loadPath)
    with open(data_path) as data_stream:
        ind = 0
        rightGuesses = 0
        for line in data_stream:
            ind += 1
            print("constructiong. " + str(ind) + ". input and label")
            line = line.rstrip()
            numbers = line.split(',')
            sample = numbers[:input_size]
            sample = [float(str) for str in sample]
            labels = numbers[input_size:]
            labels = [float(str) for str in labels]
            # model.predict expects batches and returns a list of predictions
            # we only feed one sample at a time so we only get one prediction
            #print("metrics defined on the model: " + str(model.metrics_names)) 
            print("calculating loss, and accuracy of model:")
            accuracy = model.evaluate(
                    x= np.array([sample])
                    , y= np.array([labels])
                    , batch_size= 1
                    )[1]
            #print("categorical accuracy of the model on the sample: " + str(accuracy))
            print("labels(expected output vector for input): " + str(labels))
            
            prediction = model.predict(np.array([sample]))[0]
            print("prediction by model(actual output vector): " + str(prediction))
            maxp_i = 0
            for i in range(0, len(prediction)):
                if prediction[i] > prediction[maxp_i]:
                    maxp_i = i
            print("so the model thinks the sample belongs to category " + str(maxp_i+1))
            eps = 0.001
            if(labels[maxp_i] >= 1.0-eps):
                print("the model was right")
                rightGuesses += 1
            else:
                print("the model was wrong")
            
            print("\n")
            #min_to_pass = float(0.99)
            #if accuracy < min_to_pass:
            #    print("[FAIL] not accurate enough")
            #    print("actual accuracy: " + str(accuracy))
            #    print("should be higher than " + str(min_to_pass))
            #    print("relevant sample: " + str(sample))
            #    print("relevant label: " + str(labels))
            #    exit(code=1)
    #print("[PASS]")
    if ind > 0:
        print("the model guessed" + str(rightGuesses) + " right out of " + str(ind) + ". (guess rate is " + str(100*rightGuesses/ind) + "%)")
test1()
