# %% setup ----------------------------------------------------------------------------------------------------
from math import fabs
import sys
import os
import subprocess
import numpy as np
import tensorflow as tf
import IPython
import IPython.display
import matplotlib as mpl
import matplotlib.pyplot as plt
# the dir of this program
test_executable_dir = os.getcwd()
# trainer's path relative to this executable 
trainer_relative_path = "../../trainer.py"

trainer_full_path = os.path.join(test_executable_dir, trainer_relative_path)


test_data_dir = test_executable_dir
data_path = os.path.join(test_data_dir, 'simpleSamples.csv')
tf_model_target_path = os.path.join(test_data_dir, 'tfModelDir')
input_size = 16

# %% load into 'model' the model that we created and saved---------------------------------------------------------------------------------------------------------

print("loading model from " + tf_model_target_path)
model = tf.keras.models.load_model(tf_model_target_path)
# %% visualize model: save to image file ---------------------------------------------------------------------------------------------------------
from keras.utils import plot_model
plot_model(model, to_file='model.jpg')

# %%  visualize model: in notebook ---
from IPython.display import SVG
from keras.utils import model_to_dot

SVG(model_to_dot(model).create(prog='dot', format='svg'))

# %%  visualize model: top layer weights---
top_layer = model.layers[0]
import matplotlib.pyplot as plt
plt.imshow(top_layer.get_weights()[0][:, :, :, 0].squeeze(), cmap='gray')
# %%  use keras.vis package ---
from vis.visualization import visualize_activation
from vis.utils import utils
from keras import activations
# %%  use keras.vis package ---
# %matplotlib inline
plt.rcParams['figure.figsize'] = (18, 6)

# we can specify this as -1 since it corresponds to the last layer.
layer_idx = utils.find_layer_idx(model, -1)

# Swap softmax with linear
model.layers[layer_idx].activation = activations.linear
model = utils.apply_modifications(model)

# This is the output node we want to maximize.
filter_idx = 0
img = visualize_activation(model, layer_idx, filter_indices=filter_idx)
plt.imshow(img[..., 0])
# %% 

from tensorflow import keras
from tensorflow.keras import utils
#from tensorflow.keras.utils import vis_utils
from tensorflow.keras.utils import plot_model
plot_model(
    model, 
    to_file='model.png', 
    show_shapes=False, 
    show_dtype=False,
    show_layer_names=True, 
    rankdir='TB', 
    expand_nested=False, 
    dpi=96)
# %% ---------------------------------------------------------------------------------------------------------
with open(data_path) as data_stream:
    for line in data_stream:
        line = line.rstrip()
        numbers = line.split(',')
        sample = numbers[:input_size]
        sample = [float(str) for str in sample]
        labels = numbers[input_size:]
        labels = [float(str) for str in labels]
        # model.predict expects batches and returns a list of predictions
        # we only feed one sample at a time so we only get one prediction
        print("metrics defined on the model: " + str(model.metrics_names))
        accuracy = model.evaluate(
                x= np.array([sample])
                , y= np.array([labels])
                , batch_size= 1
                )[1]
        print("categorical accuracy of the model on the sample: " + str(accuracy))
        min_to_pass = float(0.99)
        if accuracy < min_to_pass:
            print("[FAIL] not accurate enough")
            print("actual accuracy: " + str(accuracy))
            print("should be higher than " + str(min_to_pass))
            print("relevant sample: " + str(sample))
            print("relevant label: " + str(labels))
            exit(code=1)
print("[PASS]")