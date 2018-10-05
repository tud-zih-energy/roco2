from sklearn import linear_model
import numpy as np


def linear(experiments, x_pattern, y_pattern):
    x_fit, y_fit = zip(*((ex[x_pattern], ex[y_pattern]) for ex in experiments))

    model = linear_model.LinearRegression()

    x_fit = np.array(x_fit).reshape(len(x_fit), 1)
    model.fit(np.array(x_fit), np.array(y_fit))

    def predict(x):
        x = x.reshape(len(x), 1)
        return model.predict(x)

    return predict
