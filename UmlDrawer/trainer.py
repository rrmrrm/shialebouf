from math import fabs
import sys
import os

from tensorflow.python.keras import regularizers
from tensorflow.python.keras.layers.core import Dropout

#sys.stdout = open("tensorFlowTrain_output.log",'w')
sys.stderr = open("tensorFlowTrain_err.log",'w')

#https://www.tensorflow.org/guide/keras/train_and_evaluate/


inputFileName = 'trainingData.csv'
outputFileName = 'trainedNetwork.txt'

retrain = False
# áttanításkor a betöltött súlyok változatlanok maradjanak-e:
freezeKeptWights = True
saveModelPath = 'savedTfModel'
loadModelPath = 'savedTfModel'
# TODO: update help
if len(sys.argv) == 2:
    if sys.argv[1] == "-h" or sys.argv[1] == "--help" or sys.argv[1] == "/h" or sys.argv[1] == "/help":
        print("give input size, output size, the epoch count, testData ratio and batch size, inputfile's name, and the outputfile's name as argument. " + 
            "the program a traines a network with given input and output size on the inputfile('trainingData.csv' by default), " + 
            "and saves it to the given outputfile(which defaults to 'trainedNetwork.txt' if not given).")
        print("usage:", file=sys.stderr, flush=True)
        print("command <inputSize> <expOutputSize> <epochs_num> <testDataRatio> <batchSize> [<inputFileName ='trainingData.csv'> [<outputFileName ='trainedNetwork.txt'>]]")
        exit()
if len(sys.argv) < 6:
    print("ERROR: no input size or output size or epoch count or testDataRation or batch size given as argument!")
    exit()
#a tanítómintaként használt képek pixeleinek száma:
inputSize = int(sys.argv[1])
expOutputSize = int(sys.argv[2])
epochs_num = int(sys.argv[3])
#az adatsor pár eleme test mintaként lesz használva, a testDataRation arányban:
testDataRatio = float(sys.argv[4])
batchSize= int(sys.argv[5])
if len(sys.argv) > 6:
    inputFileName = sys.argv[6]
if len(sys.argv) > 7:
    outputFileName = sys.argv[7]
if len(sys.argv) > 8:
    saveModelPath = sys.argv[8]
if len(sys.argv) > 9:
    if sys.argv[9] != "0":
        retrain = True
    else:
        retrain = False
if len(sys.argv) > 10:
    loadModelPath = sys.argv[10]
import pandas as pd
import numpy as np
import tensorflow as tf
from tensorflow.keras import layers
from tensorflow.keras.layers.experimental import preprocessing


#np.set_printoptions(precision=5, suppress=True)
names_array = np.empty(0)
print(names_array)

for i in range(0, inputSize):
    names_array = np.append(names_array, "cell_"+str(i))
for i in range(0, expOutputSize):
    names_array = np.append(names_array, "out_"+str(i))
#print(names_array)
#a names_array arra kell, hogy a train nevű DataFramenek legyenek oszlopnevei
train = pd.read_csv(
    inputFileName, header=None, names=names_array)


#print(train.head())
features = train.copy()

#megkeverem az adatokat(ez a tanítást segíti, és enélkül a test adaok tömbjébe nem feltétlen jutna mindegyik osztályhoz-hoz tartozó mintából):
#forrás: https://stackoverflow.com/questions/29576430/shuffle-dataframe-rows
print("shuffling data")
features = features.sample(frac=1).reset_index(drop=True)

#labels csak az elvárt outputokat fogja tartalmazni, features pedig a bemeneteket:
labels = []
#labels = np.delete(labels, 0)
#a labels-be kerülnek a features-ből az elvárt kimenetekhez tartozó oszlopok:
for i in range(0, expOutputSize):
    outputCol = features.pop("out_"+str(i))
    labels.append(outputCol)
labels = np.array(labels)
#az append függvény a feature oszlopait sorokként töltötte bele a labels-be, ezért a labels-t transzponálni kell:
labels = np.transpose(labels)
features = np.array(features)


#kettébontom az adathalmazt tanító és teszt mintákra testDataRatio arányban:
cut_ind = int( len(features)*testDataRatio )
trainingFeatures = features[cut_ind : ]
trainingLabels = labels[cut_ind : ]
testFeatures= features[ : cut_ind]
testLabels = labels[ : cut_ind]

# tudnunk kell, hogy van-e teszt data, mert ha nincs, akkor nem lesz értelmezve validációs hiba (teszt-hiba)
has_test_data = True
if len(testFeatures) == 0:
    has_test_data = False
print("trainingData size:", len(trainingFeatures))
print("testData size:", len(testFeatures))

models = []
#if retrain:
#    print("loading model from " + loadModelPath)
#    modeltoRetrain = tf.keras.models.load_model(loadModelPath)
#    models = [modeltoRetrain]

    
    

if retrain:
    print("loading model from " + loadModelPath)
    modeltoRetrain = tf.keras.models.load_model(loadModelPath)
    
    layersList = [l for l in modeltoRetrain.layers]
    layersToKeep = len(layersList) -1
    assert(layersToKeep > 0)
    assert(layersToKeep <= len(layersList))
    
    # layerSizesToReplaceRest: az új létrehozandó rétegek méretei.
    # Az utolsó elem felül lesz írva(a 2 méretű) expOutputSize-ra, így az nem fogja meghatározni semelyik réteg méretét,
    #  de layerSizesToReplaceRest minden egyes eleme egy új réteget jelent
    layerSizesToReplaceRest = [15,2]
    assert(len(layerSizesToReplaceRest) > 0)
    
    # opcionális: a meghagyott rétegek tanítását megakadályozom:
    if freezeKeptWights:
        print("freezing loaded weights before retraining.")
        for i in range(0, layersToKeep):
            layersList[i].trainable = False
    else:
        print("un-freezing loaded weights before retraining.")
        for i in range(0, layersToKeep):
            layersList[i].trainable = True
    modeltoRetrain.summary()

    # a betöltött háló rétegeinek méretét egy listában tárolom, az elemei határozzák majd meg az új rétegek számát és méretét:
    layersOutputShapes = [l.output_shape for l in layersList]
    #az inputlayer speciális:
    layersOutputSizes = [inputSize]
    # a többi réteg méretét is megadjuk:
    for i in range(1, layersToKeep):
        layersOutputSizes.append(layersOutputShapes[i][1])
    print("a meghagyott rétegek méretei: ")
    print(layersOutputSizes)

    # itt adom hozzá layersOutputSizes-hez az új létrehozandó rétegek méretét.
    # Az utolsó elem felül lesz írva így az nem fogja meghatározni semelyik réteg méretét,
    #  de minden egyes hozzáadott elem egy új réteget jelent.(ld. lejjebb)
    for s in layerSizesToReplaceRest:
        layersOutputSizes.append(s)

    # az utolsó réteg perceptronjainak a számát frissítjük, hogy a háló képes legyen a megfelelő számú alakzat megkülönböztetésére:
    layersOutputSizes[len(layersOutputSizes)-1] = expOutputSize
    print("new layersOutputSizes: ")
    print(layersOutputSizes)
    # az utolsó meghagyott réteg output-ját átadjuk az első új rétegnek:
    x = modeltoRetrain.layers[layersToKeep-1].output
    for i in range(layersToKeep, len(layersOutputSizes)):
        print(layersOutputSizes[i])
        x = layers.Dense(units=layersOutputSizes[i], activation='sigmoid', name="dense_"+str(i)+"replaced")(x)

    result_model = tf.keras.Model(inputs=layersList[0].input, outputs=x)
    print("compiling model before retraining...")
    lossFunc = tf.keras.losses.CategoricalCrossentropy(from_logits=True)
    metricsFunc = tf.keras.metrics.CategoricalAccuracy()
    result_model.compile( 
            optimizer = tf.optimizers.Adam(),
            loss = lossFunc, 
            metrics = metricsFunc
        )

   
    result_model.summary()
    
    models = [result_model]
else:
    print("creating model")
    # TODO: remove mosels list, replace it with its contect which is a single model anyway

    #több modellt próblok ki: regularizáció, dropout, rétegSzám-és szélességek módosításával
    models = [
        tf.keras.Sequential([
            tf.keras.Input(inputSize),
            layers.Dense(80, activation='sigmoid', activity_regularizer=regularizers.l2(0.005)),
            layers.Dropout(0.005),
            layers.Dense(30, activation='sigmoid', activity_regularizer=regularizers.l2(0.0005)),
            layers.Dropout(0.005),
            layers.Dense(18, activation='sigmoid', activity_regularizer=regularizers.l2(0.005)),
            layers.Dense(expOutputSize, activation='sigmoid')
        ]),
    ]   
    print("compiling model")

    # CCE = CategoricalCrossentropy
    # from_logits=True: azt jelezzük ezzel, hogy a az utolsó nem softmax() réteg, így a kimenet 'raw logit' okat tartalmaz. Ez információ alapján a CCE loss függvény először a softmax() függvényt végrehajtja a predikciókon, és csak ezután számol hibát.
    # több helyen írják, hogy így stabilabb a hibaszámítás, mintha az utolsó rétegbe tennénk a softmxot.
    # https://stackoverflow.com/a/57304538
    # CCE implementációja: https://github.com/keras-team/keras/blob/985521ee7050df39f9c06f53b54e17927bd1e6ea/keras/backend/numpy_backend.py#L333
    # kérdés: megéri-e egyyáltalán BÁRHOL softmax-ot alkalmazni? 
    lossFunc = tf.keras.losses.CategoricalCrossentropy(from_logits=True)
    metricsFunc = tf.keras.metrics.CategoricalAccuracy()
    #metricsFunc = tf.keras.metrics.MeanSquaredError()
    #lossFunc = tf.losses.MeanSquaredError()
    for m in models:
        m.compile( 
            optimizer = tf.optimizers.Adam(),
            loss = lossFunc, 
            metrics = metricsFunc
        )

#tanítás:
print("training model:")
early_stopping = tf.keras.callbacks.EarlyStopping(
    monitor='val_loss' if has_test_data else 'loss', # as mentioned in the start, there is no validational loss, if there is no validationaldata(testdata)
    patience = 200,
    mode = 'min',
    restore_best_weights=True
)
def fit(m):
    history = m.fit(
        trainingFeatures, 
        trainingLabels,
        validation_data=(testFeatures, testLabels),
        batch_size=batchSize, 
        epochs=epochs_num,
        callbacks = [early_stopping]
    )
    return history


print("a modell tanítása:")
fit(models[0])
#tesztelés:
print("testing models:")
models_and_acc = []
for m in models:
    print("metrics defined on the model: " + str(m.metrics_names))
    if has_test_data:
        testResult = m.evaluate(testFeatures, testLabels, batch_size=1)
    else:
        testResult = m.evaluate(trainingFeatures, trainingLabels, batch_size=1)
    print("test results:", testResult)

    new_dict = {'model':m, 'accuracy':testResult[1]}
    print("model and accuracy:" + str(new_dict))
    models_and_acc.append(new_dict)

print("------------------------------------------------------------------------------------------")
#legjobb betanított modell kiválasztása a mért accuracy értéke alapján:
best_m_acc = max( models_and_acc, key=(lambda ma : ma['accuracy']) )
print("best model, and its accuracy: " +str(best_m_acc))
model = best_m_acc['model']

# mentem a hálót. így újra lehet azt tanítani és  a python kiértékelő program be tudja tölteni
print("saving most accurate model to " + saveModelPath)
model.save(saveModelPath)

sys.stdout.close()
sys.stderr.close()
